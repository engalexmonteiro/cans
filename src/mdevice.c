/*
 * mdevice.c
 *
 *  Created on: Mar 13, 2014
 *      Author: alex
 */

#include "mdevice.h"
#include "ifaces/ifaces.h"
#include <dirent.h>

int get_context(struct mobile_device_t *md){

	unsigned int current_bw_usage = md->host_ifaces.sum_if.tot_bw_total_whole;


	//If the device is stop and conected AC power
	if((md->emd.ac) && (md->gpsdata.fix.speed <= WALKSPEEDMAN) ){
			md->current_context = TROUGHPUT;
			return TROUGHPUT;
	}

	//If the device is charger in car
	if((md->emd.ac) && (md->gpsdata.fix.speed > WALKSPEEDMAN) ){
				md->current_context = COVERAGE;
				return COVERAGE;
	}

	//If the device is battery is less 30%
	if(md->emd.percentage < 30 && !md->emd.ac){
		md->current_context = POWERSAVE;
		return POWERSAVE;
	}

	if(md->gpsdata.fix.speed == 0){
		if(md->in_use){
				md->current_context = TROUGHPUT;
				return TROUGHPUT;}
		else{
			if(current_bw_usage <= 1e2){
				md->current_context = POWERSAVE;
				return POWERSAVE;
			}
			if( current_bw_usage > 1e2){
				md->current_context = TROUGHPUT;
				return TROUGHPUT;
			}
		}
	}

	if((md->gpsdata.fix.speed > 0) && (md->gpsdata.fix.speed <= WALKSPEEDMAN)){
		if(md->in_use){
			md->current_context = TROUGHPUT;
			return TROUGHPUT;
		}
		else{
			md->current_context = COVERAGE;
			return COVERAGE;
		}
	}

	if(md->gpsdata.fix.speed > WALKSPEEDMAN){
		md->current_context = COVERAGE;
		return COVERAGE;
	}

	md->current_context = TROUGHPUT;
	return TROUGHPUT;
}

int select_interface(struct mobile_device_t *md){

	switch(md->current_context){
			case COVERAGE:
							if(md->iface3g.connected)
								return IF_3G;
							if(md->wifi.connected)
								return IF_WIFI;
							if(md->bluetooth.connected)
								return IF_BLUETOOTH;
							break;
			case POWERSAVE:
							if(md->bluetooth.connected)
								return IF_BLUETOOTH;
							if(md->wifi.connected)
								return IF_WIFI;
							if(md->iface3g.connected)
								return IF_3G;
							break;
			case TROUGHPUT:
							if(md->wifi.connected)
									return IF_WIFI;
							if(md->iface3g.connected)
									return IF_3G;
							if(md->bluetooth.connected)
									return IF_BLUETOOTH;
							break;
			default:
							return -1;
	}

	return -1;
}


const char* context_to_string(int context){
		switch(context){
				case TROUGHPUT: return "TROUGHPUT"; break;
				case POWERSAVE: return "POWERSAVE"; break;
				case COVERAGE: return "COVERAGE"; break;
				default:  return "NOT IDENTIFY"; break;
			}

		return "NOT IDENTIFY";
}

const char* bestiface_to_string(int bestiface){
		switch(bestiface){
				case IF_BLUETOOTH: return "BLUETOOTH"; break;
				case IF_WIFI: return "WIFI"; break;
				case IF_3G: return "3G"; break;
				default: return "NOT IDENTIFY"; break;
				}

		return "NOT IDENTIFY";
}


int found_net(char *net){

        DIR *dir = NULL;
        int ok;
        struct dirent *drnt = NULL;

        dir = opendir("/etc/NetworkManager/system-connections/");

        if(dir)
        {
                while(drnt = readdir(dir))
                {
            		ok = !strcmp(drnt->d_name,net);

            		if(ok){
            			closedir(dir);
            			return 0;
            		}

                }

                closedir(dir);
        }

        return 1;
}


int get_info_ifaces(struct mobile_device_t *md){

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
		get_iface_bluetooth(&md->bluetooth,(GPtrArray *)devices);

		get_iface_wifi(&md->wifi,(GPtrArray *)devices);
		md->wifi.hardware_enable = nm_client_wireless_hardware_get_enabled(client);

		get_iface_3g(&md->iface3g,(GPtrArray *)devices);
		md->iface3g.hardware_enable = nm_client_wwan_hardware_get_enabled(client);
	}

	g_object_unref (client);

	return 0;
}


int score_mywifi_info(struct mobile_device_t *md){


	if(md->current_wifinet.connected){
		if(md->current_wifinet.nivel != -100){
			md->current_wifinet.sum_level = md->current_wifinet.sum_level + (float)md->current_wifinet.nivel;
			md->current_wifinet.cont_level++;
			md->current_wifinet.avg_level = (float)(md->current_wifinet.sum_level/md->current_wifinet.cont_level);
		}

		md->current_wifinet.factor_overlap = (md->ap_wifi_neigbor.fator_sobreposicao[md->current_wifinet.canal-1] - (100 + md->current_wifinet.nivel));

		md->current_wifinet.sum_olf = md->current_wifinet.sum_olf + md->current_wifinet.factor_overlap;
		md->current_wifinet.cont_olf++;
		md->current_wifinet.avg_olf = md->current_wifinet.sum_olf/md->current_wifinet.cont_olf;

		md->current_wifinet.score = (float)(md->current_wifinet.qualidade/100 + (float)(md->current_wifinet.nivel-(-100))/65 + (float)(md->ap_wifi_neigbor.max_olf - md->current_wifinet.factor_overlap)/md->ap_wifi_neigbor.max_olf);

		md->current_wifinet.sum_score = md->current_wifinet.sum_score + md->current_wifinet.score;
		md->current_wifinet.cont_score++;
		md->current_wifinet.avg_score = md->current_wifinet.sum_score/md->current_wifinet.cont_score;
	}
	else{
		md->current_wifinet.factor_overlap = 0;
		md->current_wifinet.sum_olf = 0;
		md->current_wifinet.cont_olf = 0;
		md->current_wifinet.avg_olf = 0;

		md->current_wifinet.score = 0;

		md->current_wifinet.sum_score = 0;
		md->current_wifinet.cont_score = 0;
		md->current_wifinet.avg_score = 0;
	}

	return 0;
}


int get_dm_info(struct mobile_device_t *md){

			//memset(&md->gpsdata,0,sizeof(struct gps_data_t));

			getGPSData(&md->gpsdata);

			read_bw_interfaces(&md->host_ifaces);

			md->bw_total = md->host_ifaces.sum_if.tot_bw_total_whole + md->host_ifaces.sum_if.tot_bw_total_part/10e2;

			get_info_energy(&md->emd);

			md->in_use = user_utilization();

			//get_info_bluetooth(&md->bluetooth);
			//get_info_wifi(&md->wifi);
			//get_info_iface3g(&md->iface3g);

			get_info_ifaces(md);

			gettimeofday(&md->itr.c_time,NULL);

			return 0;
}

int print_dm_info(struct mobile_device_t *md){

			printf("\nCurrent Wifi Network\n");
			if( md->current_wifinet.connected){
				printf("ESSID:%s\nIP Address: %s\nService: %.2f ms\nMAC Address:%s\nFrequence:%.3f GHz\nQuality: %.2f %%\nSignal:%d dBm\nChannel: %d\nMax bitrate: %d Mbps\n",
						md->current_wifinet.essid,
						md->current_wifinet.ip_address,
						md->current_wifinet.service ? (float)md->current_wifinet.service/1000 : -1,
						md->current_wifinet.mac_address,
						md->current_wifinet.frequencia / 1e9,
						md->current_wifinet.qualidade,
						md->current_wifinet.nivel,
						md->current_wifinet.canal,
						(int)(md->current_wifinet.maxbitrate / 1e6));
			}else
				printf("\nInterface Not connected\n");

			printf("\nCurrent Bluetooth Network\n");
			if(md->current_bluenet.connected){
				printf("Name:%s\nIP Address: %s\nService: %.2f ms\nMAC Address: %s\nQuality: %.2f %%\nSignal: %d dBm\nGood Channels: %d\nBad Channels: %d\n",
						md->current_bluenet.essid,
						md->current_bluenet.ip_address,
						md->current_bluenet.service ? (float)md->current_bluenet.service/1000 : -1,
						md->current_bluenet.mac_address,
						md->current_bluenet.qualidade,
						md->current_bluenet.nivel,
						md->current_bluenet.good_channels,
						md->current_bluenet.bad_channels);
			}else
				printf("\nInterface Not connected\n");

			printf("\nPower Status\n");
			printf("Power cable: %s\nPercentage: %d\nLoad: %d mAh\nRate: %d mA\nVoltage: %d V\nCurrent Load: %d mAh\n",
					(md->emd.ac) ? "on" : "off",
					md->emd.percentage,
					md->emd.last_full_cap,
					md->emd.present_rate,
					md->emd.present_voltage/1000,
					md->emd.remaining_cap);


			printf("\nResult\n");
			printf("Context: ");
			switch(md->current_context){
				case TROUGHPUT: printf("TROUGHPUT\n"); break;
				case POWERSAVE: printf("POWERSAVE\n"); break;
				case COVERAGE: printf("COVERAGE\n"); break;
				default: printf("NOT IDENTIFY\n"); break;
			}

			printf("Current Best interface: ");
						switch(md->bestinterface){
							case IF_BLUETOOTH: printf("BLUETOOTH\n"); break;
							case IF_WIFI: printf("WIFI\n"); break;
							case IF_3G: printf("3G\n"); break;
							default: printf("NOT IDENTIFY\n"); break;
			}

			return 0;
}

void init_dm_info(struct mobile_device_t *md){

	md->current_wifinet.connected = 0;
	md->current_wifinet.nivel = -100;
	md->current_wifinet.service = 0;

}



int print_dm_logger(struct mobile_device_t *md){

			struct tm *temp;

			temp = localtime(&md->itr.c_time.tv_sec);

			if(md->itr.cont == 0){
				printf("#Date\t\tTime\t\tSample\tSpeed\tEnergy\tAC\tUse\tBandwith\tContext\tBest Interface\n");
			}

			if(md->itr.cont > 0) /*\t%7lu.%03u*/
				printf("%02d/%02d/%02d\t%02d:%02d:%02d\t%02d\t%.5f\t%d\t%s\t%d\t%7lu.%03u\t%7lu.%03u\t%7lu.%03u\t%s\t%s\n",
						temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
						temp->tm_hour,temp->tm_min,temp->tm_sec,
						md->itr.cont,
						md->gpsdata.fix.speed,
						md->emd.percentage,
						md->emd.ac ? "On" : "Off",
						md->in_use,
						md->host_ifaces.sum_if.tx_bw_total_whole,
						md->host_ifaces.sum_if.tx_bw_total_part,
						md->host_ifaces.sum_if.rx_bw_total_whole,
						md->host_ifaces.sum_if.rx_bw_total_part,
						md->host_ifaces.sum_if.tot_bw_total_whole,
						md->host_ifaces.sum_if.tot_bw_total_part,
						context_to_string(md->current_context),
						bestiface_to_string((unsigned long)md->bestinterface));

			return 0;
}

int fprint_dm_info(struct mobile_device_t *md, char *name_file){

			struct tm *temp;
			FILE *arquivo;

			temp = localtime(&md->itr.c_time.tv_sec);

			if(md->itr.cont == 0){
				arquivo = fopen(name_file,"w");
				fprintf(arquivo,"#Date\t\tTime\tSample\tTime\tLatitu.\tLongit.\tSpeed\tEnergy\tAC\tUse\tBandwith\tContext\tBest Interface\n");
			}else
				arquivo = fopen(name_file,"a");

			if(md->itr.cont > 0) /*\t%7lu.%03u*/
				fprintf(arquivo,"%02d/%02d/%02d\t%02d:%02d:%02d\t%02d\t%.5f\t%.5f\t%.5f\t%d\t%d\t%d\t%7lu.%03u\t%d\t%d\n",
						temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
						temp->tm_hour,temp->tm_min,temp->tm_sec,
						md->itr.cont,
						md->gpsdata.fix.latitude,
						md->gpsdata.fix.longitude,
						md->gpsdata.fix.speed,
						md->emd.percentage,
						md->emd.ac,
						md->in_use,
						md->host_ifaces.sum_if.tot_bw_total_whole,
						md->host_ifaces.sum_if.tot_bw_total_part,
						md->current_context,
						md->bestinterface);

			fclose(arquivo);

			return 0;
}



