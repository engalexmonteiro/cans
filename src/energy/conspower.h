/*
 * conspower.h
 *
 *  Created on: Mar 13, 2014
 *      Author: alex
 */

#ifndef CONSPOWER_H_
#define CONSPOWER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <libacpi.h>

typedef struct energy_md {

		int ac;
		int present;                 /**< battery slot is currently used by a battery or not? 0 if not, 1 if yes */
		int design_cap;              /**< assuming capacity in mAh*/
		int last_full_cap;           /**< last full capacity when the battery was fully charged */
		int design_voltage;          /**< design voltage in mV */
		int present_rate;            /**< present rate consuming the battery */
		int remaining_cap;           /**< remaining capacity, used to calculate percentage */
		int present_voltage;         /**< present voltage */
		int percentage;              /**< remaining battery percentage */
		int charge_time;             /**< remaining time to fully charge the battery in minutes */
		int remaining_time;          /**< remaining battery life time in minutes */
		struct timeval time;
} energy_md;

int get_info_energy(energy_md * emd);

int print_info_energy(energy_md * emd);

int print_all_info_energy(int itr, energy_md * emd);

int fprint_info_energy(FILE *file, energy_md * emd);

int fprint_all_info_energy(int itr, energy_md * emd,char* namefile);

int teste(int n);

#endif /* CONSPOWER_H_ */
