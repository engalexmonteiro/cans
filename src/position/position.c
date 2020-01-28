/*
 * position.c
 *
 *  Created on: May 9, 2014
 *      Author: alex
 */
#include "position.h"

#define TRYS 20

int getGPSData(struct gps_data_t *gpsdata){
	int cont = 0;

    //connect to GPSd
    if(gps_open("localhost", DEFAULT_GPSD_PORT, gpsdata)<0){
        fprintf(stderr,"Could not connect to GPSd\n");
        return(-1);
    }

    //register for updates
    gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);


    //fprintf(stderr,"Waiting for gps lock.");
    //when status is >0, you have data.
    while(gpsdata->status == 0 && cont < TRYS){
        //block for up to .5 seconds
        if (gps_waiting(gpsdata, 50000)){
            //dunno when this would happen but its an error
            if(gps_read(gpsdata)==-1){
                fprintf(stderr,"GPSd Error\n");
                gps_stream(gpsdata, WATCH_DISABLE, NULL);
                gps_close(gpsdata);
                return(-1);
                break;
            }
            else{
                //status>0 means you have data
                if(gpsdata->status>0){
                    //sometimes if your GPS doesnt have a fix, it sends you data anyways
                    //the values for the fix are NaN. this is a clever way to check for NaN.
                    if(gpsdata->fix.longitude!=gpsdata->fix.longitude || gpsdata->fix.altitude!=gpsdata->fix.altitude){
                        //fprintf(stderr,"Could not get a GPS fix.\n");
                        gps_stream(gpsdata, WATCH_DISABLE, NULL);
                        gpsdata->fix.altitude = 0;
                        gpsdata->fix.latitude = 0;
                        gpsdata->fix.speed = 0;
                        gps_close(gpsdata);
                        return(-1);
                    }
                    //otherwise you have a legitimate fix!
                    //else
                        //fprintf(stderr,"\n");
                }
                //if you don't have any data yet, keep waiting for it.
                //else
                    //fprintf(stderr,".");
            }
        }
        //apparently gps_stream disables itself after a few seconds.. in this case, gps_waiting returns false.
        //we want to re-register for updates and keep looping! we dont have a fix yet.
        else
            gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);

        cont++;
        //just a sleep for good measure.
//        sleep(1);
    }
    //cleanup
    gps_stream(gpsdata, WATCH_DISABLE, NULL);
    gps_close(gpsdata);

    return 0;
}


void debugDump(struct gps_data_t *gpsdata){
    fprintf(stderr,"Longitude: %lf\nLatitude: %lf\nAltitude: %lf\nSpeed: %lf m/s\nAccuracy: %lf\n\n",
                gpsdata->fix.latitude, gpsdata->fix.longitude, gpsdata->fix.altitude,gpsdata->fix.speed,
                (gpsdata->fix.epx>gpsdata->fix.epy)?gpsdata->fix.epx:gpsdata->fix.epy);
}

void print_position(struct gps_data_t *gpsdata){
    printf("%lf\t%lf\t%lf\t%lf\t%lf\n",
                gpsdata->fix.latitude, gpsdata->fix.longitude, gpsdata->fix.altitude,gpsdata->fix.speed,
                (gpsdata->fix.epx>gpsdata->fix.epy)?gpsdata->fix.epx:gpsdata->fix.epy);
}


void print_all_position(int sample, struct gps_data_t *gpsdata){

	struct timeval time;
	struct tm *temp;

	gettimeofday(&time,NULL);

	temp = localtime(&time.tv_sec);

	if(sample == 0)
		printf("#Date\t\tHora\t\tSample\tLongitude\tLatitude\tAltitude\tSpeedm/s\tAccuracy\n");


	printf("%d/%d/%d\t%d:%d:%d\t%d\t",
			temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
			temp->tm_hour,temp->tm_min,temp->tm_sec,
			sample);

	print_position(gpsdata);
}

void fprint_position(FILE *file , struct gps_data_t *gpsdata){
    fprintf(file,"%lf\t%lf\t%lf\t%lf\t%lf\n",
                gpsdata->fix.latitude, gpsdata->fix.longitude, gpsdata->fix.altitude,gpsdata->fix.speed,
                (gpsdata->fix.epx>gpsdata->fix.epy)?gpsdata->fix.epx:gpsdata->fix.epy);
}

void fprint_all_position(int sample, struct gps_data_t *gpsdata,char* namefile) {

	FILE *file;
	struct timeval time;
	struct tm *temp;

	if (sample == 0){
		file = fopen(namefile,"w");
		fprintf(file,"#Date\t\tHora\t\tSample\tLongitude\tLatitude\tAltitude\tSpeedm/s\tAccuracy\n");
	}else
		file = fopen(namefile,"a");

	gettimeofday(&time,NULL);

	temp = localtime(&time.tv_sec);


	fprintf(file,"%d/%d/%d\t%d:%d:%d\t%d\t",
			temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
			temp->tm_hour,temp->tm_min,temp->tm_sec,
			sample);

	fprint_position(file,gpsdata);

	fclose(file);
}


