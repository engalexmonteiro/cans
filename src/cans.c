/*
 ============================================================================
 Name        : CANS.c Name        : CANS.c

 Author      : Alex Monteiro
 Version     :
 Copyright   : Your copyright notice
 Description : Context Aware Network Selection, CANS
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <sys/times.h>
#include "mdevice.h"

#include <pthread.h>
//#include <oping.h>
#include <semaphore.h>

/*
 * triploS.c
 *
 *  Created on: 05/03/2013
 *      Author: Alex Monteiro
 */

/*Dependences
 * libiw-dev: 		  	-liw
 * bluetooth-dev:		-lbluetooth
 * libacpi-dev			-lacpi
 * liboping-dev 		-loping
 * libxext-dev			-lXext
 * libxcb-dpms0-dev		-lxcb-dpms
 * libgps				-lgps
 * lib					-pthreads
 * libnm-glib-dev
 * libnm-util-dev
 * libnm-dev
 */


#include <iwlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

//Samples Files
#define WMAN "ppp0"
#define WLAN "wlan0"
#define WPAN "bnep0"

#define INTMIN 5
#define INTMAX 5

#define BWIFACE "bwifaces.dat"
#define WALKSPEEDMAN 1.39

#define TIMEFORENABLE 	300
#define TIMEFORDISABLE 	180

//Global variables
static int amostras = 0;
static int sample_time = 1;
static int saveinfile = 0;
static int vh_print = 0;
static int hh_wifi_print = 0;
static int hh_blue_print = 0;
static int mng3g_print = 0;
static int monitor = 0;
static int exec_bg = 0;
static int print = 0;
static int debug = 0;
static int nosafe = 0;

struct option longoptions[] = {
        {"vh", no_argument, NULL,1},
        {"hh_wifi", no_argument, NULL,2},
        {"hh_blue", no_argument, NULL,3},
        {"mng3g", no_argument, NULL,4},
        {"nosafe", no_argument, NULL,5},
        {"file", no_argument, NULL, 'f'},
        {"monitor", no_argument, NULL, 'm'},
        {"samples", required_argument, NULL, 'n'},
        {"interval", required_argument, NULL, 'i'},
        {"help", no_argument, NULL,'h'},
        {"version", no_argument, NULL,'v'},
        {"daemon", no_argument, NULL,'d'},
        {"debug", no_argument, NULL,'D'},
        {0, 0, 0, 0}
};

int help_info(){

	printf("\nMESISC - MEchanism Smart Interfaces Selection based in Context\n");
	printf("\nsmsic [OPTION] [ARGUMENT]\n");
	printf("OPTIONS:\n\t\t-n [value]\t: number of samples");
	printf("\n\t\t-i [time]\t: interval between the samples in seconds");
	printf("\n\t\t-f\t\t: Save in txt file");
	printf("\n\t\t-m\t\t: Execute in monitor mode (show only information about bandwith and power consume)");
	printf("\n\t\t-p\t\t: Print execution in monitor mode");
	printf("\n\t\t-d\t\t: Execute as daemon");
	printf("\n\t\t1 or --vh\t: Show Process Selection Interfaces");
	printf("\n\t\t2 or --hh_wifi\t: Show Horizontal Selection and handover for wifi networks");
	printf("\n\t\t3 or --hh_blue\t: Show Horizontal Selection and handover bluetooth networks");
	printf("\n\t\t4 or --mng3g\t: Show Manager 3G interface for broadband networks");
	printf("\n\t\t5 or --nosafe\t: Wi-Fi Selection include open unknow networks");
	printf("\n\t\t-h\t\t: Show this help menu\n\n");
	return 0;
};

static float speed = 0;

static mobile_device md;

int monitor_mode(mobile_device *md){

	struct tm *temp;
	struct timeval start,end;
	double time_sleep;

	for(md->itr.cont = 0; md->itr.cont < amostras ; md->itr.cont++){

		gettimeofday(&start, NULL);

		md->itr.c_time = start;

		gathering_infoctx();

		//md->bw_total = md->host_ifaces.sum_if.tot_bw_total_whole + md->host_ifaces.sum_if.tot_bw_total_part/10e2;

		if(saveinfile){
				file_for_bw(md->itr.cont,&md->host_ifaces, WLAN,"bw_wifi.dat");
				file_for_bw(md->itr.cont,&md->host_ifaces, WPAN,"bw_bluetooth.dat");
				file_for_bw(md->itr.cont,&md->host_ifaces, WMAN,"bw_3g.dat");

				fprint_all_info_energy(md->itr.cont,&md->emd,"conspwr.dat");
		}

		if(print){
			temp = localtime(&md->itr.c_time.tv_sec);

			if(md->itr.cont == 0){
				printf("#Date\t\tTime\t\tSample\tLati.\tLong.\tSpeed\tEnergy\tAC\tUse\tBandwith\n");
			}

			if(md->itr.cont > 0) /*\t%7lu.%03u*/
				printf("%02d/%02d/%02d\t%02d:%02d:%02d\t%02d\t%.3f\t%.3f\t%.3f\t%d\t%s\t%s\t%.3f\n",
					temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
					temp->tm_hour,temp->tm_min,temp->tm_sec,
					md->itr.cont,
					(float)md->gpsdata.fix.latitude,
					(float)md->gpsdata.fix.longitude,
					(float)md->gpsdata.fix.speed,
					md->emd.percentage,
					md->emd.ac ? "on" : "off",
					md->in_use ? "on" : "off",
					md->bw_total);
		}

		md->host_ifaces.first_pass = 0;

		gettimeofday(&end, NULL);

		time_sleep = sample_time*10e5 - (end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/10e5)*10e5;

		if(time_sleep > 0)
			usleep(time_sleep);

	}

	return 0;
}

int execute_selection(mobile_device *md){

	char command[100];
	char s_iface[10];
	char gw[50];

	switch(md->bestinterface){
		case IF_BLUETOOTH:	strcpy(s_iface,"bnep0");
							strcpy(gw,md->bluetooth.gw_address);
							break;
		case IF_WIFI:		strcpy(s_iface,"wlan0");
							strcpy(gw,md->wifi.gw_address);
							break;
		case IF_3G:			strcpy(s_iface,"ppp0");
							strcpy(gw,md->iface3g.gw_address);
							break;
		default:
							return -1;
							break;
	}

	//printf("route del default\n");
	system("route del default");
	sprintf(command,"route add default gw %s %s",gw,s_iface);
	//printf("%s\n",command);
	system(command);

	return 0;
}

int scenarios(){

	if(md.itr.cont == 0){ //User stoped in use
		system("xset dpms force on");
		speed = 0;
	}


	if(md.itr.cont == 300){ //User stoped not use
		system("xset dpms force off");
		speed = 0;

	}

	if(md.itr.cont == 600){	//User walking  in use
		speed = 3;
		system("xset dpms force on");
	}

	if(md.itr.cont == 900){
		speed = 3;
		system("xset dpms force off");
	}

	if(md.itr.cont == 1200){
		speed = 10;
		system("xset dpms force on");
	}

	if(md.itr.cont == 1800){
		speed = 10;
		system("xset dpms force off");

	}

	if(md.itr.cont == 2400){
		speed = 3;
		system("xset dpms force on");
	}

	if(md.itr.cont == 2700){
		speed = 3;
		system("xset dpms force off");
	}

	if(md.itr.cont == 3000){
		speed = 0;
		system("xset dpms force on");
	}

	if(md.itr.cont == 3300){
		speed = 0;
		system("xset dpms force off");
	}

	return 0;
}


int gathering_infoctx(){

	    struct timeval start;

		gettimeofday(&start, NULL);

		md.itr.c_time = start;

		if(debug)
			printf("%d - 1 Thread Gathering Information Context - Start ...",md.itr.cont);

		init_dm_info(&md);

		get_dm_info(&md);

		md.current_context = get_context(&md);

		md.bestinterface = select_interface(&md);

		if(saveinfile){
			file_for_bw(md.itr.cont,&md.host_ifaces, "wlan0","bw_wifi.dat");
			file_for_bw(md.itr.cont,&md.host_ifaces, "bnep0","bw_bluetooth.dat");
			file_for_bw(md.itr.cont,&md.host_ifaces, "ppp0","bw_3g.dat");
			fprint_all_info_energy(md.itr.cont,&md.emd,"conspwr.dat");
			fprint_dm_info(&md,"dminfo.dat");
		}

		if(vh_print)
			print_dm_logger(&md);

		md.host_ifaces.first_pass = 0;

		if(debug)
			printf(" end\n");

		return 0;
}

int execution(int i){

	static int previus_iface, cont_iface = 0;

	if(md.itr.cont == 0)
		previus_iface = md.bestinterface;

	if(previus_iface != md.bestinterface){

		cont_iface++;

		if(cont_iface == 5){
			previus_iface = md.bestinterface;
			cont_iface = 0;
			execute_selection(&md);
		}

	}else{
		cont_iface = 0;
	}

	return 0;
}

int manager_iface3g(int i){

	static int b3g_enable=0;
	static struct timeval start, end;
	struct tm *temp;
	static double time_sleep;
	FILE *file_3g;


	if(saveinfile){

		if(i == 0)
			file_3g = fopen("hzh_3g.dat","w");
		else
			file_3g = fopen("hzh_3g.dat","a");

		if(file_3g == NULL){
			fprintf(stderr,"Fail Open manager interface 3G file error\n");
			return -1;
		}

	}

	gettimeofday(&start,NULL);
	temp = localtime(&start.tv_sec);


	if(debug)
		printf("%d - 4 Thread Manager 3g start... ",i);

	if(md.iface3g.hardware_enable){

			if(md.wifi.connected || md.bluetooth.connected){
				if(md.iface3g.connected || b3g_enable){
					system("/usr/bin/nmcli con down id \"Oi Default\" --nowait");
					//enable_iface_3g(0);
					b3g_enable = 0;
				}

			}else{
				if(!md.iface3g.connected || !b3g_enable){
					//enable_iface_3g(1);
					system("/usr/bin/nmcli con up id \"Oi Default\" --nowait");
					b3g_enable = 1;
				}
			}


		}


		if(mng3g_print){

				if(i == 0)
					printf("#Date\t\tTime\tS\tHW 3G\tWifi\tBlue\tCnc3g\tIf3g\n");

				printf("%02d/%02d/%d\t%02d:%02d:%02d %d\t%d\t%d\t%d\t%d\t%d\n",
							temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
							temp->tm_hour,temp->tm_min,temp->tm_sec,
							i,
							md.iface3g.hardware_enable,
							md.wifi.connected,
							md.bluetooth.connected,
							md.iface3g.connected,
							b3g_enable);
		}

		if(saveinfile){

				if(i == 0)
					fprintf(file_3g,"#Date\t\tTime\tS\t3gEn\n");

				fprintf(file_3g,"%02d/%02d/%d\t%02d:%02d:%02d %d\t%d\n",
							temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
							temp->tm_hour,temp->tm_min,temp->tm_sec,
							i,
							b3g_enable);

		}

		if(debug)
			printf(" end\n");

		gettimeofday(&end,NULL);


	if(saveinfile)
		fclose(file_3g);

	return 0;
}






int select_wifi_networks(struct mobile_device_t *md){

	wireless_scan_mi* current_device = NULL;
	wireless_scan_mi* wifinet_found = NULL;
	wireless_scan_mi* best_wifi_ap = NULL;
	int first_time = 1;

	if (md->itr.cont == 0){
		memcpy(&md->previus_best_ap,&md->current_wifinet,sizeof(wireless_scan_mi));
		memcpy(&md->best_ap,&md->current_wifinet,sizeof(wireless_scan_mi));
	}

	if(debug){
		printf("\n%f",md->current_wifinet.score);
		printf("\nPrevius Best AP                -  Best AP\n");
		printf("%s %f ",md->previus_best_ap.essid,md->previus_best_ap.avg_score);
		printf("%f %s \n",md->best_ap.avg_score,md->best_ap.essid);
	}

	current_device = md->ap_wifi_neigbor.head_list;

	if(md->ap_wifi_neigbor.head_list != NULL){

	while(current_device != NULL){

		if((current_device->score > md->current_wifinet.score) && !found_net(current_device->essid)){

			if(first_time){
				best_wifi_ap = current_device;
				first_time = 0;
			}

			wifinet_found = find_wifinet(&md->wifinet_selected,current_device);

			if(wifinet_found != NULL){
				wifinet_found->sum_score = wifinet_found->sum_score + current_device->score;
				wifinet_found->cont_score++;
				wifinet_found->avg_score = wifinet_found->sum_score/wifinet_found->cont_score;
				wifinet_found->score = current_device->score;
			}
			else
				add_ap_wifilist(&md->wifinet_selected,current_device);

			if(debug) printf("\nBest AP < Current_device\n %s %f < %f %s\n",
					best_wifi_ap->essid,
					best_wifi_ap->avg_score,
					current_device->avg_score,
					current_device->essid);

			if(best_wifi_ap->avg_score < current_device->avg_score)
				best_wifi_ap = current_device;

		}

		current_device = current_device->next;

	}

	if(best_wifi_ap->avg_score >= md->current_wifinet.avg_score){

		if(!strcmp(best_wifi_ap->mac_address,md->current_wifinet.mac_address)){
			memcpy(&md->best_ap,&md->current_wifinet,sizeof(wireless_scan_mi));
			md->cont_best_ap = 0;
		}else{
			if(!strcmp(best_wifi_ap->mac_address,md->previus_best_ap.mac_address)){
				memcpy(&md->best_ap,best_wifi_ap,sizeof(wireless_scan_mi));
				md->cont_best_ap++;
			}else{
				memcpy(&md->previus_best_ap,&md->best_ap,sizeof(wireless_scan_mi));
				memcpy(&md->best_ap,best_wifi_ap,sizeof(wireless_scan_mi));
				md->cont_best_ap = 0;
			}
		}
	}

	}

	return 0;
}


int horizontal_handover_bluetooth(int i){

	static float speed_dm = 0;
	static int time_for_disable = 0, time_for_enable = 0;
	int 	blueth_enable;
	int     control_process;
	struct timeval start, end;
	double time_sleep;
	struct tm *temp;

	FILE *file_blue;

	if(saveinfile){

		if( i == 0)
			file_blue = fopen("hzh_bluetooth.dat","w");
		else
			file_blue = fopen("hzh_bluetooth.dat","a");

		if(file_blue == NULL){
			fprintf(stderr,"Error open bluetooth register file\n");
			return -1;
		}

	}

	gettimeofday(&start,NULL);
	temp = localtime(&start.tv_sec);

	speed_dm = speed;
	blueth_enable;

	if(speed_dm <= 0.5){

				time_for_disable = 0;
				time_for_enable++;

				if(time_for_enable >= 300 || blueth_enable){
						if(!blueth_enable){
							system("/usr/sbin/hciconfig hci0 up");
							system("/usr/bin/nmcli con up id \"AlexETSS-0 Network\" --nowait");
						}
				}

	}else{

				time_for_disable++;
				time_for_enable = 0;

				if(time_for_disable >= 180){

					if(blueth_enable){
						system("/usr/bin/nmcli dev disconnect iface bnep0 --nowait");
						system("/usr/sbin/hciconfig hci0 down");
					}

				}

		}

		if(saveinfile){
			fprint_current_net(i,&md.current_wifinet, "currentwifnet.dat");

			if(i == 0)
				fprintf(file_blue,"#Date\t\tTime\tS\tWiEn\tStop\tStart\tRSSI\tAvgRSSI\tOF\tOFAvg\tScore\tScoreAvg\tTR\tTRAvg\tKC\tCONT\tSpeed\tItv\n");

			fprintf(file_blue,"%02d/%02d/%d\t%02d:%02d:%02d %02d\t%d\t%d\t%d\t",
						temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
						temp->tm_hour,temp->tm_min,temp->tm_sec,
						i,
						blueth_enable,
						time_for_disable,
						time_for_enable);

			if(!blueth_enable)
				fprintf(file_blue,"\n");
			else
				fprintf(file_blue,"%d\t%.2f\t%d\t%.2f\t%d\t%.2f\n",
					md.current_bluenet.nivel,
					md.current_bluenet.avg_level,
					md.current_bluenet.service,
					md.current_bluenet.avg_service,
					md.current_bluenet.cont,
					speed_dm);
		}


		gettimeofday(&end, NULL);


	if(saveinfile)
		fclose(file_blue);

	return 0;

}


int horizontal_handover_wifi(int i){

	static int interval = 15, wifi_enable;
	static int time_for_enable = 0, time_for_disable = 0;
	static float speed_dm = 0; //
	char command[100];
	int keep_connection;
	struct timeval start;
	struct tm *temp;
	FILE *file_wifi;


	gettimeofday(&start,NULL);
	temp = localtime(&start.tv_sec);

	wifi_enable = get_enabled_iface_wifi();

	if(speed_dm <= WALKSPEEDMAN){

		time_for_disable = 0;
		time_for_enable++;

		if(time_for_enable >= 10 || wifi_enable){

			if(!wifi_enable)
				enable_iface_wifi(1);
			else
			{
					md.current_wifinet.nivel = get_rssi_info(WLAN,&md.current_wifinet);

					if(md.current_wifinet.connected){

						if(md.current_wifinet.nivel < -55)
							interval = INTMIN;
						else
							interval = INTMAX;

						if(!(i%interval)){

							iw_scan_wifiv3(WLAN,&md.ap_wifi_neigbor);

						}

						get_mywifi_info(WLAN,&md.current_wifinet);

						score_mywifi_info(&md);

						if(!(i%interval))
							select_wifi_networks(&md);

						if(md.wifi.connected){
							strcpy(md.current_wifinet.ip_address,md.wifi.ip_address);
							md.current_wifinet.service = 0;
						}

						if((md.current_wifinet.nivel > -61) && (md.cont_best_ap < 4)){
							md.current_wifinet.cont = 0;
							keep_connection = 1;
						}
						else{

							keep_connection = 0;

							md.current_wifinet.cont = md.current_wifinet.cont + 1;

							if(md.current_wifinet.cont > 5){

								if(!nosafe){
									if(!found_net(md.best_ap.essid)){
										snprintf(command,100,"/usr/bin/nmcli con up id \"%s\"",md.best_ap.essid);
										system(command);
										md.current_wifinet.cont = 0;
									}
								}
								else{
									if(!found_net(md.best_ap.essid))
										snprintf(command,100,"/usr/bin/nmcli con up id \"%s\" --nowait",md.best_ap.essid);
									else
										snprintf(command,100,"/usr/bin/nmcli dev wifi connect \"%s\" --nowait",md.best_ap.essid);

									system(command);
									md.current_wifinet.cont = 0;
								}
									//snprintf(command,100,"/usr/bin/nmcli dev wifi connect \"%s\" --nowait",md.best_ap.essid);

//							printf("\t%s\n",command);

							}
						}

						//update_statistics(&md.current_wifinet);

					}else{
						iw_scan_wifiv3("wlan0",&md.ap_wifi_neigbor);
						select_wifi_networks(&md);

						if(!nosafe){
							if(!found_net(md.best_ap.essid) && strcmp(md.best_ap.essid,md.current_wifinet.essid)){
								snprintf(command,100,"/usr/bin/nmcli con up id \"%s\"",md.best_ap.essid);
								system(command);
								md.current_wifinet.cont = 0;
							}
						}else{
							if(!found_net(md.best_ap.essid))
								snprintf(command,100,"/usr/bin/nmcli con up id \"%s\" --nowait",md.best_ap.essid);
							else
								snprintf(command,100,"/usr/bin/nmcli dev wifi connect \"%s\" --nowait",md.best_ap.essid);

							system(command);
							md.current_wifinet.cont = 0;
						}
					}
	     		}

     		}

		}else{
			time_for_disable++;
			time_for_enable = 0;

			if(time_for_disable >= 10){
				if(wifi_enable)
					enable_iface_wifi(0);
			}
		}


		if(hh_wifi_print){

					if(i == 0)
						printf("#Date\t\tTime\tS\tSpeed\tWiEn\tStop\tStart\tRSSI\tAvg\tOF\tAvg\tScore\tAvg\tBestNet\tCC\tKC\tCONT\tItv\n");

					printf("%02d/%02d/%d\t%02d:%02d:%02d %02d\t%.2f\t%d\t%d\t%d\t",
										temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
										temp->tm_hour,temp->tm_min,temp->tm_sec,
										i,
										speed_dm,
										wifi_enable,
										time_for_disable,
										time_for_enable);

					if(!wifi_enable)
						printf("\n");
					else
						printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%d\t%d\t%d\n", //%d\t%.2f\t
										md.current_wifinet.nivel,
										md.current_wifinet.avg_level,
										md.current_wifinet.factor_overlap,
										md.current_wifinet.avg_olf,
										md.current_wifinet.score,
										md.current_wifinet.avg_score,
										md.best_ap.avg_score,
										md.cont_best_ap,
//										md.current_wifinet.service,
//										md.current_wifinet.avg_service,
										keep_connection,
										md.current_wifinet.cont,
										interval);
				}

				if(saveinfile){

					if(i == 0 )
						file_wifi = fopen("hzh_wifi.dat","w");
					else
						file_wifi = fopen("hzh_wifi.dat","a");

					if(file_wifi == NULL){
						fprintf(stderr,"Error open wifi manager file");
						return -1;
					}

					if(!(i%interval)){

						fprint_scan_mi(i,&md.ap_wifi_neigbor);
						fprint_olf(i, &md.ap_wifi_neigbor, "channeloverlap.dat");
						fprint_best_channels(i, &md.ap_wifi_neigbor, "bestchannel.dat");
					}


					fprint_current_net(i,&md.current_wifinet, "currentwifnet.dat");

					if(i == 0)
						fprintf(file_wifi,"#Date\t\tTime\tS\tSpeed\tWiEn\tStop\tStart\tRSSI\tAvg\tOF\tAvg\tScore\tAvg\tBestNet\tCC\tKC\tCONT\tItv\n");

					fprintf(file_wifi,"%02d/%02d/%d\t%02d:%02d:%02d %02d\t%.2f\t%d\t%d\t%d\t",
							temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
							temp->tm_hour,temp->tm_min,temp->tm_sec,
							i,
							speed_dm,
							wifi_enable,
							time_for_disable,
							time_for_enable);


					if(!wifi_enable)
						fprintf(file_wifi,"\n");
					else
						fprintf(file_wifi,"%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%d\t%d\t%d\n", //%d\t%.2f\t
								md.current_wifinet.nivel,
								md.current_wifinet.avg_level,
								md.current_wifinet.factor_overlap,
								md.current_wifinet.avg_olf,
								md.current_wifinet.score,
								md.current_wifinet.avg_score,
								md.best_ap.avg_score,
								md.cont_best_ap,
//										md.current_wifinet.service,
//										md.current_wifinet.avg_service,
								keep_connection,
								md.current_wifinet.cont,
								interval);

					fclose(file_wifi);
				}



		return 0;
}

int main(int argc,char **argv){

	int option;

	memset(&md,0,sizeof(mobile_device));

	md.host_ifaces.first_pass = 1;

	while( (option=getopt_long(argc, argv, "mn:fi:hdDp",longoptions,NULL)) != -1 ){
		switch(option){
			case 'm' : monitor = 1;
						break;
			case 'n' : amostras = atoi(optarg);
				   	    break;
			case 'f' : saveinfile = 1;
	    		   	    printf("\nEsta funcao salva no arquivo\n");
	    		   	    break;
			case 'i' : sample_time = atoi(optarg);
				   	    break;
			case 'h' : help_info();
					    return 0;
				   	    break;
			case 'd' :  exec_bg = 1;
						break;
			case 'D' :  debug = 1;
						break;
			case 'p' :  print = 1;
						break;
			case 1 : 	vh_print = 1;
					    break;
			case 2 : 	hh_wifi_print = 1;
					    break;
			case 3 : 	hh_blue_print = 1;
						break;
			case 4 : 	mng3g_print = 1;
						break;
			default:  amostras = 0;
					   vh_print = 1;
				 	   break;
		}
	}


	if(amostras == 0){
		printf("You need put a sample value equal or above \"1\" or put \"-1\" for infinite\n");
		help_info();
		return -1;
	}

	if(exec_bg)
		daemon(0, 0);

	if(monitor){
		monitor_mode(&md);
		return 0;
	}

	struct timeval start, end;
	double time_sleep;

	for(md.itr.cont = 0; md.itr.cont != amostras ; md.itr.cont++){

		gettimeofday(&start, NULL);

		gathering_infoctx();

		horizontal_handover_wifi(md.itr.cont);

		horizontal_handover_bluetooth(md.itr.cont);

		manager_iface3g(md.itr.cont);

		execution(md.itr.cont);

		gettimeofday(&end, NULL);

		time_sleep = sample_time*1e6 - (end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1e6)*1e6;

		if(time_sleep > 0)
			usleep(time_sleep);

	}

	return 0;
}
