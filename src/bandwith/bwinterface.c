/*
 * bwinterface.c
 *
 *  Created on: Jul 22, 2013
 *      Author: alex
 */

#include "bwinterface.h"

static int print_bytes = 0;

bool_t
do_total(total_interface_t * total_if, interface_t * interface, int first_pass)
{
	total_if->rx_bw_total_whole += (*interface)->rx_rate_whole;
	total_if->rx_bw_total_part += (*interface)->rx_rate_part;

	if (total_if->rx_bw_total_part > 1000) {
		total_if->rx_bw_total_whole++;
		total_if->rx_bw_total_part -= 1000;
	}
	total_if->tx_bw_total_whole += (*interface)->tx_rate_whole;
	total_if->tx_bw_total_part += (*interface)->tx_rate_part;

	if (total_if->tx_bw_total_part > 1000) {
		total_if->tx_bw_total_whole++;
		total_if->tx_bw_total_part -= 1000;
	}
	total_if->tot_bw_total_whole = total_if->rx_bw_total_whole + total_if->tx_bw_total_whole;
	total_if->tot_bw_total_part = total_if->rx_bw_total_part + total_if->tx_bw_total_part;

	if (total_if->tot_bw_total_part > 1000) {
		total_if->tot_bw_total_whole++;
		total_if->tot_bw_total_part -= 1000;
	}
	return(TRUE);
}

bool_t
print_total(char * method, total_interface_t * total_if)
{
	printf("%12s     %8lu.%03u    %8lu.%03u    %8lu.%03u\n", method,
		   total_if->rx_bw_total_whole,  total_if->rx_bw_total_part,
		   total_if->tx_bw_total_whole,  total_if->tx_bw_total_part,
		   total_if->tot_bw_total_whole, total_if->tot_bw_total_part);
    return(TRUE);
}

bool_t
print_uptime(float * uptime)
{
	unsigned int tmp = 0, hour = 0, min = 0, sec = 0;
	char hostname[30] = "";

	tmp = (uint)(*uptime);
	sec = tmp % 60;
	tmp = (tmp - sec) / 60;
	min = tmp % 60;
	tmp = (tmp - min) / 60;
	hour = tmp % 24;
	tmp = (tmp - hour) / 24;

	if (gethostname(hostname, strlen(hostname)) != 0)
		strcpy(hostname, "System");

	printf("\n%s uptime: %u day%c %u hour%c %u minutes and %u seconds\n",
		   hostname, tmp, (tmp==1?' ':'s'), hour, (hour==1?' ':'s'), min, sec);
    return(TRUE);
}

bool_t
print_interface(interface_t * interface)
{
	printf("%12s      %7lu.%03u     %7lu.%03u     %7lu.%03u\n", (*interface)->name,
		   (*interface)->rx_rate_whole,  (*interface)->rx_rate_part,
		   (*interface)->tx_rate_whole,  (*interface)->tx_rate_part,
		   (*interface)->tot_rate_whole, (*interface)->tot_rate_part );
	return(TRUE);
}

bool_t
print_max(interface_t * interface)
{
	ulong max_whole = (*interface)->rx_max_whole + (*interface)->tx_max_whole;
	ulong max_part  = (*interface)->rx_max_part + (*interface)->tx_max_part;

	if (max_part >= 1000) {
		max_whole++;
		max_part -= 1000;
	}
	printf("        max:      %7lu.%03lu     %7lu.%03lu     %7lu.%03lu \n",
		   (*interface)->rx_max_whole, (*interface)->rx_max_part,
		   (*interface)->tx_max_whole, (*interface)->tx_max_part,
		   max_whole, max_part);
	return(TRUE);
}


bool_t
print_ut_ave(interface_t * interface)
{
	printf("       aver:          %7.3f         %7.3f         %7.3f\n",
		   (*interface)->rx_average, (*interface)->tx_average ,
		   (*interface)->rx_average + (*interface)->tx_average );
	return(TRUE);
}

bool_t
fprint_interface(FILE *arquivo, interface_t * interface)
{
	fprintf(arquivo, "%12s %7lu.%03u %7lu.%03u %7lu.%03u ", (*interface)->name,
		   (*interface)->rx_rate_whole,  (*interface)->rx_rate_part,
		   (*interface)->tx_rate_whole,  (*interface)->tx_rate_part,
		   (*interface)->tot_rate_whole, (*interface)->tot_rate_part );
	return(TRUE);
}

bool_t
fprint_max(FILE *arquivo, interface_t * interface)
{
	ulong max_whole = (*interface)->rx_max_whole + (*interface)->tx_max_whole;
	ulong max_part  = (*interface)->rx_max_part + (*interface)->tx_max_part;

	if (max_part >= 1000) {
		max_whole++;
		max_part -= 1000;
	}
	fprintf(arquivo ,"%7lu.%03lu %7lu.%03lu %7lu.%03lu ",
		   (*interface)->rx_max_whole, (*interface)->rx_max_part,
		   (*interface)->tx_max_whole, (*interface)->tx_max_part,
		   max_whole, max_part);
	return(TRUE);
}


bool_t
fprint_ut_ave(FILE *arquivo, interface_t * interface)
{
	fprintf(arquivo, "%7.3f %7.3f %7.3f",
		   (*interface)->rx_average, (*interface)->tx_average ,
		   (*interface)->rx_average + (*interface)->tx_average );
	return(TRUE);
}

void
initialize_total(total_interface_t * total_if)
{
	total_if->rx_bw_total_whole = total_if->tx_bw_total_whole = 0;
	total_if->tot_bw_total_whole = total_if->rx_bw_total_part = 0;
	total_if->tx_bw_total_part = total_if->tot_bw_total_part  = 0;
}

bool_t
count_average(interface_t * interface, float * uptime)
{
	(*interface)->rx_average = (float)((*interface)->rx_bytes_new / *uptime / 1024);
	(*interface)->tx_average = (float)((*interface)->tx_bytes_new / *uptime / 1024);
	return(TRUE);
}

unsigned long
bwm_calc_remainder(unsigned long num, unsigned long den)
{
	unsigned long long n = num, d = den;
	return (((n - (n / d) * d) * 1000) / d);
}

bool_t
do_interface(char * buffer, interface_t * interface, int first_pass)
{
	char * pbuffer = NULL;
	int    field_number = 0;
	unsigned long int conv_field = 0;

	pbuffer = buffer;
	pbuffer = strtok(pbuffer, " :");

	if (first_pass) {
		if ((*interface = (interface_t) malloc(sizeof(struct interface))) == NULL)
			return(FALSE);
		strncpy((*interface)->name, pbuffer, 11);
	}
	(*interface)->time_old = (*interface)->time_new;
	gettimeofday(&((*interface)->time_new), NULL);

	(*interface)->time_diff_ms = (ulong)
		((*interface)->time_new.tv_sec * 1000 + (*interface)->time_new.tv_usec / 1000) -
		((*interface)->time_old.tv_sec * 1000 + (*interface)->time_old.tv_usec / 1000);

	field_number = 0;

	while ((pbuffer = strtok(NULL, " :") ) != NULL) {
		conv_field = strtoul(pbuffer, NULL, 10);
		field_number++;

		switch (field_number) {
			case 1:
				(*interface)->rx_bytes_old = (*interface)->rx_bytes_new;
				if (print_bytes == 0)
					(*interface)->rx_bytes_new = conv_field * 8;
				else
					(*interface)->rx_bytes_new = conv_field;
				(*interface)->rx_kbytes_dif = ( (*interface)->rx_bytes_new -
												(*interface)->rx_bytes_old) * 1000 / 1024;
				(*interface)->rx_rate_whole = (*interface)->rx_kbytes_dif /
					(*interface)->time_diff_ms;
				(*interface)->rx_rate_part = bwm_calc_remainder((*interface)->rx_kbytes_dif,
																(*interface)->time_diff_ms);

				if ((*interface)->rx_rate_whole >= (*interface)->rx_max_whole) {
					if (((*interface)->rx_rate_part > (*interface)->rx_max_part) ||
						((*interface)->rx_rate_whole >= (*interface)->rx_max_whole)) {
						(*interface)->rx_max_part  = (*interface)->rx_rate_part;
						(*interface)->rx_max_whole = (*interface)->rx_rate_whole;
					}
				}
				break;

			case 9:
				(*interface)->tx_bytes_old = (*interface)->tx_bytes_new;
				if (print_bytes == 0)
					(*interface)->tx_bytes_new = conv_field * 8;
				else
					(*interface)->tx_bytes_new = conv_field;
				(*interface)->tx_kbytes_dif = ((*interface)->tx_bytes_new -
											   (*interface)->tx_bytes_old) * 1000 / 1024;
				(*interface)->tx_rate_whole = (*interface)->tx_kbytes_dif /
					(*interface)->time_diff_ms;
				(*interface)->tx_rate_part = bwm_calc_remainder((*interface)->tx_kbytes_dif,
																(*interface)->time_diff_ms);

				if ((*interface)->tx_rate_whole >= (*interface)->tx_max_whole) {
					if (((*interface)->tx_rate_part > (*interface)->tx_max_part) ||
						((*interface)->tx_rate_whole >= (*interface)->tx_max_whole) ) {
						(*interface)->tx_max_part  = (*interface)->tx_rate_part;
						(*interface)->tx_max_whole = (*interface)->tx_rate_whole;
					}
				}
				(*interface)->tot_rate_whole = (*interface)->rx_rate_whole +
					(*interface)->tx_rate_whole;
				(*interface)->tot_rate_part = (*interface)->rx_rate_part +
					(*interface)->tx_rate_part;

				if ((*interface)->tot_rate_part > 1000) {
					(*interface)->tot_rate_whole++;
					(*interface)->tot_rate_part -= 1000;
				}
				break;
		}
	}
	return(TRUE);
}

int read_bw_interfaces(host_interfaces_t* host_ifaces){

		int i = 0, j = 0;

		char buffer[256];
		FILE * fp = NULL;

		gethostname(host_ifaces->host, sizeof(host_ifaces->host));

		initialize_total(&host_ifaces->sum_if);

		gettimeofday(&host_ifaces->gth_time,NULL);

		if ((fp = fopen(UPTIME, "r")) == NULL)
			fatal("fopen failed in %s", __FUNCTION__);
		fscanf(fp, "%f", &host_ifaces->uptime);
		fclose(fp);


		if ((fp = fopen(DEVFILE, "r")) == NULL)
			fatal("fopen() failed  in %s", __FUNCTION__);
		fgets(buffer, 255, fp); fgets(buffer, 255, fp);

		i = 0;

		while (fgets(buffer, 255, fp) != NULL && i < MAX_INTERFACES - 1) {
			//printf("%s",buffer);
			if (!do_interface(buffer, &host_ifaces->interface[i], host_ifaces->first_pass))
				printf("erro ao ler interface");
			count_average(&host_ifaces->interface[i], &host_ifaces->uptime);
			i++;
		}
		fclose(fp);

		host_ifaces->number_iface = i;

		//printf("numero de interfaces: %d\n",host_ifaces->number_iface);

		for (j = 0; j < i; j++)
					if (!do_total(&host_ifaces->sum_if, &host_ifaces->interface[j], host_ifaces->first_pass))
						fatal("do_total failed in %s", __FUNCTION__);


		return 0;
}


void print_bw_interfaces(host_interfaces_t* host_ifaces){

	int j;

	if (!host_ifaces->first_pass) {
						for (j = 0; j < host_ifaces->number_iface; j++) {
							print_interface(&host_ifaces->interface[j]);
							print_max(&host_ifaces->interface[j]);
							print_ut_ave(&host_ifaces->interface[j]);
							printf("\n");
						}
						printf("\n");
						print_total("All", &host_ifaces->sum_if);
						print_uptime(&host_ifaces->uptime);
					    printf("\n");
	}

}

int fprint_bw_interfaces(FILE *arquivo,host_interfaces_t* host_ifaces){

	int j;

	if (!host_ifaces->first_pass) {
						for (j = 0; j < host_ifaces->number_iface; j++) {
							fprint_interface(arquivo, &host_ifaces->interface[j]);
							fprint_max(arquivo, &host_ifaces->interface[j]);
							fprint_ut_ave(arquivo, &host_ifaces->interface[j]);
							fprintf(arquivo, "\n");
						}
	}

	return 0;
}

int fprint_bw_interface(FILE *arquivo, char *iface, host_interfaces_t* host_ifaces){

	int j;

	if (!host_ifaces->first_pass) {
						for (j = 0; j < host_ifaces->number_iface; j++) {

							if(!strcmp(iface,host_ifaces->interface[j]->name)){
								fprint_interface(arquivo, &host_ifaces->interface[j]);
								fprint_max(arquivo, &host_ifaces->interface[j]);
								fprint_ut_ave(arquivo, &host_ifaces->interface[j]);
								break;
							}

						}

	}

	return 0;
}

int file_for_bw(int i, host_interfaces_t* host_ifaces, char* iface, char *name_file){

	FILE *arquivo;
	struct tm *temp;

	temp = localtime(&host_ifaces->gth_time);

	if(i == 0){
		arquivo = fopen(name_file,"w");
		fprintf(arquivo,"#Bandwith\n#Date\t\tHour\t\tSample\tIface\tRx\t\tTx\tTotal\t\tMax.Rx\tMax.Tx\tMax.Total\tAv Rx\tAv Tx\tAv Total\t\n");
	}else
		arquivo = fopen(name_file,"a");

	if(i > 0)
		fprintf(arquivo,"%02d/%02d/%02d\t%02d:%02d:%02d\t%d",
								temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
								temp->tm_hour,temp->tm_min,temp->tm_sec,
								i);

	fprint_bw_interface(arquivo,iface,host_ifaces);

	if(i > 0)
		fprintf(arquivo, "\n");

	fclose(arquivo);

	return 0;
}
