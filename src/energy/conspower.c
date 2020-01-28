/*
 ============================================================================
 Name        : TestACPI.c
 Author      : Alex Monteiro
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "conspower.h"

int get_info_energy(energy_md * emd) {

	global_t *global = malloc (sizeof (global_t));

	if(check_acpi_support() == SUCCESS){

        init_acpi_batt(global);
        init_acpi_acadapt(global);

        read_acpi_acstate(global);

		if(!global->adapt.ac_state){
			emd->ac = 1;
		}else{
			emd->ac = 0;
		}

		read_acpi_batt(global->batt_count-1);

		emd->charge_time = batteries[global->batt_count-1].charge_time;
		emd->design_cap = batteries[global->batt_count-1].design_cap;
		emd->design_voltage = batteries[global->batt_count-1].design_voltage / 1000;
		emd->last_full_cap = batteries[global->batt_count-1].last_full_cap;
		emd->percentage = batteries[global->batt_count-1].percentage;
		emd->present = batteries[global->batt_count-1].present;
		emd->present_rate = batteries[global->batt_count-1].present_rate;
		emd->present_voltage = batteries[global->batt_count-1].present_voltage;
		emd->remaining_cap = batteries[global->batt_count-1].remaining_cap;

		gettimeofday(&emd->time,NULL);

	 }

	  return EXIT_SUCCESS;
}

int print_info_energy(energy_md * emd) {

		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
		emd->ac,
		emd->charge_time,
		emd->design_cap,
		emd->design_voltage,
		emd->last_full_cap,
		emd->percentage,
		emd->present,
		emd->present_rate,
		emd->present_voltage,
		emd->remaining_cap);

	  return 0;
}


int print_all_info_energy(int itr, energy_md * emd) {

		struct tm *temp;

		temp = localtime(&emd->time.tv_sec);

		if (itr == 0)
			printf("#Date\t\tHour\tS\tAC\tMin\tmAh\tV\tmAh\t%%\tOn\tRate\tV\tMin\n");

		printf("%02d/%02d/%02d\t%02d:%02d:%02d %02d\t",
				temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
				temp->tm_hour,temp->tm_min,temp->tm_sec,
				itr);

		print_info_energy(emd);

	  return 0;
}

int fprint_info_energy(FILE *file, energy_md * emd) {

		fprintf(file,"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
		emd->ac,
		emd->charge_time,
		emd->design_cap,
		emd->design_voltage,
		emd->last_full_cap,
		emd->percentage,
		emd->present,
		emd->present_rate,
		emd->present_voltage,
		emd->remaining_cap);

	  return 0;
}


int fprint_all_info_energy(int itr, energy_md * emd, char* namefile) {

		FILE *file;
		struct tm *temp;

		temp = localtime(&emd->time.tv_sec);

		if (itr == 0){
			file = fopen(namefile,"w");
			fprintf(file,"#Date\t\tHour\tS\tAC\tMin\tmAh\tV\tmAh\t%%\tOn\tRate\tV\tMin\n");
		}else
			file = fopen(namefile,"a");

		fprintf(file,"%02d/%02d/%02d\t%02d:%02d:%02d %02d\t",
						temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
						temp->tm_hour,temp->tm_min,temp->tm_sec,
						itr);

		fprint_info_energy(file,emd);

		fclose(file);

	  return 0;
}

int teste(int n){

	energy_md * emd=NULL;
	int i;

	for(i=0; i< n; i++){
		get_info_energy(emd);
		print_all_info_energy(i,emd);
		fprint_all_info_energy(i,emd,"powerconsume");
		sleep(1);
	}

	return 0;
}
