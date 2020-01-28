/*
 * midiscovery.h
 *
 *  Created on: 05/03/2013
 *      Author: alex
 */

#ifndef MIDISCOVERY_H_
#define MIDISCOVERY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
// Librarys Bluetooth and Wi-fi
#include <iwlib.h>
#include <time.h>
#include <sys/time.h>

#include "multinterfaces.h"

#define SIZEQUERY 150
#define OVERLAPFACTOR "fscanais.txt" // this file store the overlap factor of the channels
#define WIFINETWORKS "varredura.txt" // this file store the wifi network detected

void init_wifi_info(wireless_scan_mi_list* wifinet);

int add_ap_wifilist(wireless_scan_mi_list* list,wireless_scan_mi *current_device);

int update_statistics(wireless_scan_mi* device);

wireless_scan_mi* find_wifinet(wireless_scan_mi_list* list, wireless_scan_mi* wifinet);

int iw_scan_wifi(wireless_scan_mi_list* list);

int iw_scan_wifiv2(char *ifname, wireless_scan_mi_list* list);

int iw_scan_wifiv3(char *ifname, wireless_scan_mi_list* list);

int score_net_snr(wireless_scan_mi_list* list);

int print_scan_mi(wireless_scan_mi_list* list);

int print_classi_network(wireless_scan_mi_list* list);

int fprint_scan_mi(int itr, wireless_scan_mi_list* list);

int fprint_olf(int itr, wireless_scan_mi_list* list, char* filename);

int fprint_best_channels(int itr, wireless_scan_mi_list* list, char* filename);

int get_mywifi_info(char* ifname, wireless_scan_mi* wifinet);

int get_rssi_info(char* ifname, wireless_scan_mi* wifinet);

void fprintf_mywifi_info(FILE * 	 file,wireless_scan_mi* wifinet);

int fprint_current_net(int itr, wireless_scan_mi* wifinet, char* filename);

void limpar_scan_mi(wireless_scan_mi_list* list);


#endif /* MIDISCOVERY_H_ */
