/*
 * mdevice.h
 *
 *  Created on: Mar 12, 2014
 *      Author: Alex Monteiro
 */

#ifndef MDEVICE_H_
#define MDEVICE_H_

#include "bandwith/bwinterface.h"
#include "energy/conspower.h"
#include "use/use.h"
#include "position/position.h"
#include "ifaces/ifaces.h"

#include "netrf/multinterfaces.h"
#include "netrf/mod802151.h"
#include "netrf/mod80211.h"
#include "ifaces/ifaces.h"
#include "network/network.h"
#include <gps.h>
#include <sys/time.h>

#define SIZEQUERY 150
#define IFNAME "wlan0"

#define INT_WIFI 15
#define INT_BLUE 30

#define WALKSPEEDMAN 1.39

enum ifaces{
	IF_BLUETOOTH,
	IF_WIFI,
	IF_3G
};

enum context{
	TROUGHPUT,
	POWERSAVE,
	COVERAGE
};


typedef struct index_t{
	struct timeval c_time;
	int cont;
} idx;


typedef struct ifaces_selected{
	int order_bluetooth;
	int order_wifi;
	int order_3g;

} ifaces_selected_t;

typedef struct mobile_device_t
{

  idx itr;

  /* Device identification */
  char hostname[50];

  int current_context;

  int bestinterface;

  //User Utilization
  int in_use;

  //Position and movement
  struct gps_data_t gpsdata;

  float bw_total;

  //Power Status
  energy_md 					emd;

  //Access Points Neighboors
  wireless_scan_mi_list 		historic_wifi;

  wireless_scan_mi_list			wifinet_selected;
  wireless_scan_mi 				previus_best_ap;
  wireless_scan_mi 				best_ap;
  int			 				cont_best_ap;

  wireless_scan_mi_list 		ap_wifi_neigbor;
  wireless_scan_mi_list 		ap_blue_neigbor;

  //Interfaces
  iface							wifi;
  iface							bluetooth;
  iface							iface3g;

  wireless_scan_mi 				current_wifinet;
  wireless_scan_mi 				current_bluenet;

  int 						    if3g_connected;

  //Bandwith
  host_interfaces_t 			host_ifaces;

  ifaces_selected_t				order_prefer;

} mobile_device;

int found_net(char *net);

int score_mywifi_info(struct mobile_device_t *md);

int get_dm_info(struct mobile_device_t *md);

void init_dm_info(struct mobile_device_t *md);

int print_dm_info(struct mobile_device_t *md);

int print_dm_logger(struct mobile_device_t *md);

int fprint_dm_info(struct mobile_device_t *md, char *name_file);

const char* bestiface_to_string(int bestiface);

const char* context_to_string(int context);

int select_interface(struct mobile_device_t *md);

int get_context(struct mobile_device_t *md);

#endif /* MDEVICE_H_ */

