/*
 * teste.c
 *
 *  Created on: Jul 21, 2014
 *      Author: root
 */


#include "ifaces.h"

#define DBUS_TYPE_G_MAP_OF_VARIANT (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE))
#define DBUS_TYPE_G_MAP_OF_MAP_OF_VARIANT (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, DBUS_TYPE_G_MAP_OF_VARIANT))
#define DBUS_TYPE_G_ARRAY_OF_OBJECT_PATH (dbus_g_type_get_collection ("GPtrArray", dbus_g_object_path_get_g_type ()))

static GHashTable *connections = NULL;


static gchar *
ip4_address_as_string (guint32 ip)
{
struct in_addr tmp_addr;
char buf[INET_ADDRSTRLEN+1];

memset (&buf, '\0', sizeof (buf));
tmp_addr.s_addr = ip;

if (inet_ntop (AF_INET, &tmp_addr, buf, INET_ADDRSTRLEN)) {
return g_strdup (buf);
} else {
g_warning ("%s: error converting IP4 address 0x%X",
__func__, ntohl (tmp_addr.s_addr));
return NULL;
}
}


static NMConnection *
get_connection_for_active (NMActiveConnection *active)
{
	const char *path;

g_return_val_if_fail (active != NULL, NULL);

path = nm_active_connection_get_connection (active);
g_return_val_if_fail (path != NULL, NULL);

return (NMConnection *) g_hash_table_lookup (connections, path);
}

static void
get_one_connection (DBusGConnection *bus,
                    const char *path,
                    GHashTable *table)
{
DBusGProxy *proxy;
NMConnection *connection = NULL;
GError *error = NULL;
GHashTable *settings = NULL;

g_return_if_fail (bus != NULL);
g_return_if_fail (path != NULL);
g_return_if_fail (table != NULL);

proxy = dbus_g_proxy_new_for_name (bus, NM_DBUS_SERVICE,
path, NM_DBUS_INTERFACE_SETTINGS_CONNECTION);
if (!proxy)
return;

if (!dbus_g_proxy_call (proxy, "GetSettings", &error,
G_TYPE_INVALID,
DBUS_TYPE_G_MAP_OF_MAP_OF_VARIANT, &settings,
G_TYPE_INVALID)) {
g_warning ("error: cannot retrieve connection: %s", error ? error->message : "(unknown)");
goto out;
}

connection = nm_connection_new_from_hash (settings, &error);
g_hash_table_destroy (settings);

if (!connection) {
g_warning ("error: invalid connection: '%s' / '%s' invalid: %d",
error ? g_type_name (nm_connection_lookup_setting_type_by_quark (error->domain)) : "(unknown)",
error ? error->message : "(unknown)",
error ? error->code : -1);
goto out;
}

nm_connection_set_path (connection, path);
g_hash_table_insert (table, g_strdup (path), g_object_ref (connection));

out:
g_clear_error (&error);
if (connection)
g_object_unref (connection);
g_object_unref (proxy);
}


gboolean
get_all_connections (void)
{
		GError *error = NULL;
		DBusGConnection *bus;
		DBusGProxy *proxy = NULL;
		GPtrArray *paths = NULL;
		int i;
		gboolean sucess = FALSE;

		bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);
		if (error || !bus) {
			g_warning ("error: could not connect to dbus");
			goto out;
		}

		proxy = dbus_g_proxy_new_for_name (bus,
								NM_DBUS_SERVICE,
								NM_DBUS_PATH_SETTINGS,
								NM_DBUS_INTERFACE_SETTINGS);
		if (!proxy) {
			g_warning ("error: failed to create DBus proxy for settings service");
			goto out;
		}

		if (!dbus_g_proxy_call (proxy, "ListConnections", &error,
                                G_TYPE_INVALID,
                                DBUS_TYPE_G_ARRAY_OF_OBJECT_PATH, &paths,
                                G_TYPE_INVALID)) {
			/* No connections or settings service may not be running */
			g_clear_error (&error);
			goto out;
		}

		connections = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);

		for (i = 0; paths && (i < paths->len); i++)
			get_one_connection (bus, g_ptr_array_index (paths, i), connections);

		sucess = TRUE;

		out:
		if (bus)
			dbus_g_connection_unref (bus);
		if (proxy)
			g_object_unref (proxy);

			return sucess;
}


static void
get_info_iface (struct iface_t *iface, gpointer data)
{
		NMDevice *device = NM_DEVICE (data);
		char *tmp;
		NMDeviceState state;
		const GArray *array;
		NMActiveConnection *active;


		active = nm_device_get_active_connection (device);
		iface->active = active;

		if (active) {
			NMConnection *connection;
			NMSettingConnection *s_con;

			iface->is_default = nm_active_connection_get_default (active);

			connection = get_connection_for_active (active);
			iface->connection = connection;

			if (connection) {
				s_con = nm_connection_get_setting_connection (connection);
				iface->s_con = s_con;


			}
		}

		strcpy(iface->ifname,nm_device_get_iface (device));

		state = nm_device_get_state (device);


		if(state == NM_DEVICE_STATE_ACTIVATED)
			iface->connected = 1;
		else
			iface->connected = 0;

		tmp = NULL;
		iface->speed = 0;
		if (NM_IS_DEVICE_ETHERNET (device) && strcmp(iface->ifname,"bnep0")){
					iface->type = ETHERNET;
					tmp = g_strdup (nm_device_ethernet_get_hw_address (NM_DEVICE_ETHERNET (device)));
					strcpy(iface->mac_address,tmp);
					g_free (tmp);
					iface->speed = nm_device_ethernet_get_speed (NM_DEVICE_ETHERNET (device));
		}

		if (NM_IS_DEVICE_ETHERNET (device) && !strcmp(iface->ifname,"bnep0")){
			iface->type = BLUETOOTH;
			tmp = g_strdup (nm_device_ethernet_get_hw_address (NM_DEVICE_ETHERNET (device)));
			strcpy(iface->mac_address,tmp);
			g_free (tmp);
			iface->speed = nm_device_ethernet_get_speed (NM_DEVICE_ETHERNET (device));

		}

		if(NM_IS_DEVICE_WIFI(device)){
			iface->type = WIFI;
			tmp = g_strdup (nm_device_wifi_get_hw_address(NM_DEVICE_WIFI (device)));
			strcpy(iface->mac_address,tmp);
			g_free (tmp);
			iface->speed = nm_device_wifi_get_bitrate (NM_DEVICE_WIFI (device));
			//iface->speed /= 1000;

		}

		if (NM_IS_DEVICE_MODEM (device)){
			iface->type = IFACE3G;

		}

		if (state == NM_DEVICE_STATE_ACTIVATED) {
			NMIPConfig *cfg4 = nm_device_get_ip4_config (device);
			GSList *iter;

			if (cfg4) {

				for (iter = (GSList *) nm_ip4_config_get_addresses (cfg4); iter; iter = g_slist_next (iter)) {
					NMIPAddress *addr = (NMIPAddress *) iter->data;
					guint32 prefix = nm_ip4_address_get_prefix (addr);
					char *tmp2;

					tmp = ip4_address_as_string (nm_ip4_address_get_address (addr));
					strcpy(iface->ip_address,tmp);
					g_free (tmp);

					tmp2 = ip4_address_as_string (nm_utils_ip4_prefix_to_netmask (prefix));
					strcpy(iface->net_mask,tmp2);
					g_free (tmp2);

					tmp = ip4_address_as_string (nm_ip4_address_get_gateway (addr));
					strcpy(iface->gw_address,tmp2);
					g_free (tmp);
				}

				array = nm_ip4_config_get_nameservers (cfg4);

				if (array) {

					if(array->len < 2){
						tmp = ip4_address_as_string (g_array_index (array, guint32,0));
						strcpy(iface->dns1_address,tmp);
						g_free (tmp);
					}else{
						if((array->len < 3)){
								tmp = ip4_address_as_string (g_array_index (array, guint32,0));
								strcpy(iface->dns1_address,tmp);
								g_free (tmp);
								tmp = ip4_address_as_string (g_array_index (array, guint32,1));
								strcpy(iface->dns2_address,tmp);
								g_free (tmp);

						}
					}
				}
			}
		}

}

int get_iface_ethernet(struct iface_t *ethernet, GPtrArray *devices){

	int i;
	NMDevice *device;

	ethernet->exist = 0;

	for (i = 0; i < devices->len; i++){

			device = NM_DEVICE (devices->pdata[i]);

			if (NM_IS_DEVICE_ETHERNET (device) && strcmp(nm_device_get_iface (device),"bnep0")){
				get_info_iface (ethernet,device);
				ethernet->device = device;
				ethernet->exist = 1;
			}
	}

	return 0;
}



int get_iface_bluetooth(struct iface_t *bluetooth, GPtrArray *devices){

	int i;
	NMDevice *device;

	bluetooth->exist = 0;

	for (i = 0; i < devices->len; i++){

			device = NM_DEVICE (devices->pdata[i]);

			//printf("%s == %s --> %d %d\n",nm_device_get_iface (device),"20:12:11:15:32:4C",!strcmp(nm_device_get_iface (device),"20:12:11:15:32:4C"),NM_IS_DEVICE_ETHERNET (device));

			if (!strcmp(nm_device_get_iface (device),"20:12:11:15:32:4C")){
				//printf("bluetooth \n");

				get_info_iface (bluetooth,device);
				bluetooth->device = device;
				bluetooth->exist = 1;
			}
	}

	return 0;
}

int get_iface_wifi(struct iface_t *wifi, GPtrArray *devices){

	int i;
	NMDevice *device;

	wifi->exist = 0;

	for (i = 0; i < devices->len; i++){

			device = NM_DEVICE (devices->pdata[i]);

			if (NM_IS_DEVICE_WIFI(device)){
				get_info_iface (wifi,device);
				wifi->exist = 1;
				wifi->device = device;
			}
	}

	return 0;
}

int get_iface_3g(struct iface_t *iface3g, GPtrArray *devices){

	int i;
	NMDevice *device;

	iface3g->exist = 0;

	for (i = 0; i < devices->len; i++){

			device = NM_DEVICE (devices->pdata[i]);

			if (NM_IS_DEVICE_MODEM (device)){
				get_info_iface (iface3g,device);
				iface3g->exist = 1;
				iface3g->device = device;
			}
	}

	return 0;
}

int get_info_ethernet(struct iface_t *ethernet){

	const GPtrArray *devices;
	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	if (!client) {
		 exit (1);
	}

	if (!get_all_connections ())
		exit (1);

	devices = nm_client_get_devices (client);

	if (devices){
		get_iface_ethernet(ethernet,(GPtrArray *)devices);
	}

	g_object_unref (client);

	return 0;
}


int get_info_bluetooth(struct iface_t *bluetooth){

	const GPtrArray *devices;
	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	if (!client) {
		 exit (1);
	}

	if (!get_all_connections ())
			exit (1);

	devices = nm_client_get_devices (client);

	if (devices){
		get_iface_bluetooth(bluetooth,(GPtrArray *)devices);
	}

	g_object_unref (client);

	return 0;
}

int get_info_wifi(struct iface_t *wifi){

	const GPtrArray *devices;
	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	if (!client) {
		 exit (1);
	}

	if (!get_all_connections ())
		 exit (1);

	devices = nm_client_get_devices (client);

	if (devices){
		get_iface_wifi(wifi,(GPtrArray *)devices);
	}

	g_object_unref (client);

	return 0;
}

int get_info_iface3g(struct iface_t *iface3g){

	const GPtrArray *devices;
	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	if (!client) {
		 exit (1);
	}

	if (!get_all_connections ())
		exit (1);

	devices = nm_client_get_devices (client);

	if (devices){
		get_iface_3g(iface3g,(GPtrArray *)devices);
	}

	g_object_unref (client);

	return 0;
}


int print_info_iface(struct iface_t *iface){

		if(iface->exist)
			printf("%d\t%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\n",
					iface->type,
					iface->ifname,
					iface->ip_address,
					iface->net_mask,		/* Access point address */
					iface->gw_address,
					iface->dns1_address,
					iface->dns2_address,
					iface->connected,
					iface->is_default,
					iface->speed,
					iface->mac_address);

		return 0;
}

int enable_iface_all(int enable){

	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	nm_client_networking_set_enabled (client,enable,NULL);

	g_object_unref (client);

	return 0;
}

int get_enabled_iface_wifi(){

	NMClient *client;
	int state;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	state = nm_client_wireless_get_enabled (client);

	g_object_unref (client);

	return state;
}

int enable_iface_wifi(int enable){

	const GPtrArray *devices;
	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	nm_client_wireless_set_enabled (client,enable);

	g_object_unref (client);

	return 0;
}


int enable_iface_3g(int enable){

	const GPtrArray *devices;
	NMClient *client;

	g_type_init ();

	client = nm_client_new(NULL,NULL);

	nm_client_wwan_set_enabled (client,enable);

	g_object_unref (client);

	return 0;
}
