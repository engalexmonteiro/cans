/*
 * position.h
 *
 *  Created on: May 9, 2014
 *      Author: alex
 */

#ifndef POSITION_H_
#define POSITION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gps.h>
#include <math.h>

int getGPSData(struct gps_data_t *gpsdata);

void debugDump(struct gps_data_t *gpsdata);

void print_position(struct gps_data_t *gpsdata);

void print_all_position(int sample, struct gps_data_t *gpsdata);

void fprint_position(FILE *file, struct gps_data_t *gpsdata);

void fprint_all_position(int sample, struct gps_data_t *gpsdata,char* namefile);

#endif /* POSITION_H_ */
