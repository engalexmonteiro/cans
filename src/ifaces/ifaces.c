#include "ifaces.h"
#include <NetworkManager.h>
#include <arpa/inet.h>
#include <string.h>

/* * Modern libnm handles all D-Bus communication internally.
 * We no longer need manual GHashTable connections or DBusGProxy.
 */

static void
get_info_iface(struct iface_t *iface, gpointer data)
{
    NMDevice *device = NM_DEVICE(data);
    NMDeviceState state;
    NMActiveConnection *active;

    // 1. Connection Info
    active = nm_device_get_active_connection(device);
    iface->active = active;

    if (active) {
        iface->is_default = nm_active_connection_get_default(active);
        // In modern libnm, NMRemoteConnection implements the NMConnection interface
        NMRemoteConnection *remote_con = nm_active_connection_get_connection(active);
        iface->connection = NM_CONNECTION(remote_con);

        if (iface->connection) {
            iface->s_con = nm_connection_get_setting_connection(iface->connection);
        }
    }

    // 2. Basic Device Info
    strncpy(iface->ifname, nm_device_get_iface(device), sizeof(iface->ifname) - 1);
    state = nm_device_get_state(device);
    iface->connected = (state == NM_DEVICE_STATE_ACTIVATED) ? 1 : 0;

    // Hardware Address (Generic for all types)
    const char *hw_addr = nm_device_get_hw_address(device);
    if (hw_addr) {
        strncpy(iface->mac_address, hw_addr, sizeof(iface->mac_address) - 1);
    }

    // 3. Type-Specific Speed and Identification
    iface->speed = 0;
    if (NM_IS_DEVICE_ETHERNET(device)) {
        if (strcmp(iface->ifname, "bnep0") == 0) {
            iface->type = BLUETOOTH;
        } else {
            iface->type = ETHERNET;
        }
        iface->speed = nm_device_ethernet_get_speed(NM_DEVICE_ETHERNET(device));
    } else if (NM_IS_DEVICE_WIFI(device)) {
        iface->type = WIFI;
        // Bitrate is in Kbps, commonly converted to Mbps for display
        iface->speed = nm_device_wifi_get_bitrate(NM_DEVICE_WIFI(device)) / 1000;
    } else if (NM_IS_DEVICE_MODEM(device)) {
        iface->type = IFACE3G;
    }

    // 4. IP Configuration (The Modern IPv4/IPv6 Unified API)
    if (state == NM_DEVICE_STATE_ACTIVATED) {
        NMIPConfig *cfg4 = nm_device_get_ip4_config(device);
        if (cfg4) {
            // Get Addresses from GPtrArray
            GPtrArray *addresses = nm_ip_config_get_addresses(cfg4);
            if (addresses && addresses->len > 0) {
                NMIPAddress *addr = g_ptr_array_index(addresses, 0);

                // IP Address
                strncpy(iface->ip_address, nm_ip_address_get_address(addr), sizeof(iface->ip_address) - 1);

                // Netmask Calculation from Prefix
                guint32 prefix = nm_ip_address_get_prefix(addr);
                struct in_addr netmask;
                netmask.s_addr = htonl(0xFFFFFFFF << (32 - prefix));
                strncpy(iface->net_mask, inet_ntoa(netmask), sizeof(iface->net_mask) - 1);

                // Gateway
                const char *gw = nm_ip_config_get_gateway(cfg4);
                if (gw) strncpy(iface->gw_address, gw, sizeof(iface->gw_address) - 1);
            }

            // Nameservers (Returns const char * const *)
            const char * const *nameservers = nm_ip_config_get_nameservers(cfg4);
            if (nameservers) {
                if (nameservers[0]) strncpy(iface->dns1_address, nameservers[0], sizeof(iface->dns1_address) - 1);
                if (nameservers[1]) strncpy(iface->dns2_address, nameservers[1], sizeof(iface->dns2_address) - 1);
            }
        }
    }
}

// Global Client Helper to reduce boilerplate
static NMClient *get_client() {
    return nm_client_new(NULL, NULL);
}

// --- Interface Selection Logic ---

static int get_device_by_type(struct iface_t *iface, GType device_type, const char *exclude_name, const char *match_name) {
    NMClient *client = get_client();
    if (!client) return -1;

    const GPtrArray *devices = nm_client_get_devices(client);
    iface->exist = 0;

    for (guint i = 0; i < devices->len; i++) {
        NMDevice *device = g_ptr_array_index(devices, i);
        const char *name = nm_device_get_iface(device);

        if (g_type_is_a(G_OBJECT_TYPE(device), device_type)) {
            if (exclude_name && strcmp(name, exclude_name) == 0) continue;
            if (match_name && strcmp(name, match_name) != 0) continue;

            get_info_iface(iface, device);
            iface->exist = 1;
            break;
        }
    }
    g_object_unref(client);
    return 0;
}

int get_info_ethernet(struct iface_t *ethernet) {
    return get_device_by_type(ethernet, NM_TYPE_DEVICE_ETHERNET, "bnep0", NULL);
}

int get_info_bluetooth(struct iface_t *bluetooth) {
    // Note: You previously hardcoded a MAC/Name here. Update if necessary.
    return get_device_by_type(bluetooth, NM_TYPE_DEVICE_ETHERNET, NULL, "20:12:11:15:32:4C");
}

int get_info_wifi(struct iface_t *wifi) {
    return get_device_by_type(wifi, NM_TYPE_DEVICE_WIFI, NULL, NULL);
}

int get_info_iface3g(struct iface_t *iface3g) {
    return get_device_by_type(iface3g, NM_TYPE_DEVICE_MODEM, NULL, NULL);
}

// --- Control Functions ---

int enable_iface_all(int enable) {
    NMClient *client = get_client();
    if (client) {
        nm_client_networking_set_enabled(client, enable, NULL);
        g_object_unref(client);
    }
    return 0;
}

int get_enabled_iface_wifi() {
    NMClient *client = get_client();
    if (!client) return 0;
    gboolean state = nm_client_wireless_get_enabled(client);
    g_object_unref(client);
    return state;
}

int enable_iface_wifi(int enable) {
    NMClient *client = get_client();
    if (client) {
        nm_client_wireless_set_enabled(client, enable);
        g_object_unref(client);
    }
    return 0;
}

int enable_iface_3g(int enable) {
    NMClient *client = get_client();
    if (client) {
        nm_client_wwan_set_enabled(client, enable);
        g_object_unref(client);
    }
    return 0;
}

int print_info_iface(struct iface_t *iface) {
    if (iface->exist) {
        printf("%d\t%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\n",
                iface->type, iface->ifname, iface->ip_address,
                iface->net_mask, iface->gw_address, iface->dns1_address,
                iface->dns2_address, iface->connected, iface->is_default,
                iface->speed, iface->mac_address);
    }
    return 0;
}


gboolean
get_all_connections(void)
{
    NMClient *client = nm_client_new(NULL, NULL);
    if (!client) {
        g_warning("Error: Could not create NMClient to retrieve connections");
        return FALSE;
    }

    // nm_client_get_connections returns a const GPtrArray of NMRemoteConnection*
    const GPtrArray *connections_list = nm_client_get_connections(client);

    if (!connections_list || connections_list->len == 0) {
        g_warning("No connections found in NetworkManager");
        g_object_unref(client);
        return FALSE;
    }

    // Logic: In modern libnm, you don't need a global GHashTable 'connections'
    // because the 'active connection' object (NMActiveConnection) already
    // provides a direct pointer to the NMConnection object.

    g_object_unref(client);
    return TRUE;
}

int get_iface_bluetooth(struct iface_t *bluetooth, GPtrArray *devices) {
    bluetooth->exist = 0;

    for (guint i = 0; i < devices->len; i++) {
        NMDevice *device = NM_DEVICE(g_ptr_array_index(devices, i));
        const char *iface_name = nm_device_get_iface(device);

        // Matches by your specific Bluetooth interface name/MAC
        if (iface_name && strcmp(iface_name, "20:12:11:15:32:4C") == 0) {
            get_info_iface(bluetooth, device);
            bluetooth->device = device;
            bluetooth->exist = 1;
            break;
        }
    }
    return 0;
}

int get_iface_wifi(struct iface_t *wifi, GPtrArray *devices) {
    wifi->exist = 0;

    for (guint i = 0; i < devices->len; i++) {
        NMDevice *device = NM_DEVICE(g_ptr_array_index(devices, i));

        if (NM_IS_DEVICE_WIFI(device)) {
            get_info_iface(wifi, device);
            wifi->device = device;
            wifi->exist = 1;
            break;
        }
    }
    return 0;
}

int get_iface_3g(struct iface_t *iface3g, GPtrArray *devices) {
    iface3g->exist = 0;

    for (guint i = 0; i < devices->len; i++) {
        NMDevice *device = NM_DEVICE(g_ptr_array_index(devices, i));

        if (NM_IS_DEVICE_MODEM(device)) {
            get_info_iface(iface3g, device);
            iface3g->device = device;
            iface3g->exist = 1;
            break;
        }
    }
    return 0;
}
