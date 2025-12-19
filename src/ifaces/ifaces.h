/*
 * ifaces.h
 *
 *  Created on: Jul 22, 2014
 *      Author: alex
 */

#ifndef IFACES_H_
#define IFACES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <glib.h>
#include <NetworkManager.h>


enum ifacetype{
	ETHERNET,
	WIFI,
	BLUETOOTH,
	IFACE3G
};

typedef struct iface_t{

	  int 		type;					/* Bluetooth or Wi-fi 	*/
	  int 		is_default;
	  int		exist;
	  unsigned int speed;

	  NMDevice 				*device;
	  NMActiveConnection 	*active;
	  NMConnection 			*connection;
	  NMSettingConnection 	*s_con;


	  int 		hardware_enable;
	  char		ifname[10];
	  char     ip_address[50];   		/* IP Address 			*/
	  char		mac_address[50];		/* Access point address */
	  char		net_mask[50];			/* Access point address */
	  char		gw_address[50];			/* Access point address */
	  char 		dns1_address[50];
	  char 		dns2_address[50];
	  char		protocol[50];			/* Access point address */
	  int 		connected;				/* connected 			*/


} iface;


int get_info_bluetooth(struct iface_t *bluetooth);
int get_info_wifi(struct iface_t *wifi);
int get_info_iface3g(struct iface_t *iface3g);

int enable_iface_all(int enable);
int enable_iface_wifi(int enable);
int enable_iface_3g(int enable);

int get_enabled_iface_wifi();

int get_iface_wifi(struct iface_t *wifi, GPtrArray *devices);
int get_iface_bluetooth(struct iface_t *wifi, GPtrArray *devices);
int get_iface_3g(struct iface_t *wifi, GPtrArray *devices);

gboolean get_all_connections(void);

int print_info_iface(struct iface_t *iface);

#endif /* IFACES_H_ */
