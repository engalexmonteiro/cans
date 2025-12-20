/*
 * bwinterface.h
 *
 *  Created on: Jul 5, 2013
 *      Author: alex
 */

#ifndef BWINTERFACE_H_
#define BWINTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#ifndef TIOSCGWINSZ
#include <sys/ioctl.h>
#endif

/*
 * 2.2 kernel supports only 16 network cards, and since there are not
 * that many 2.4 linuxes running more than 16 interfaces..
 */
#define MAX_INTERFACES 16
#define INTERFACE_NAME 16

#define DEVFILE "/proc/net/dev"
#define UPTIME  "/proc/uptime"

typedef enum {
    TRUE  = 1,
    FALSE = 0
} bool_t;

typedef struct interface {
    char name[16];
    ulong tx_rate_whole, rx_rate_whole, tot_rate_whole;
    ulong tx_bytes_old, rx_bytes_old, tx_bytes_new;
    ulong rx_bytes_new, tx_kbytes_dif, rx_kbytes_dif;
    ulong time_diff_ms;
    unsigned tx_rate_part, rx_rate_part, tot_rate_part;
    struct timeval time_old, time_new;
    float rx_average, tx_average, total_average;
    ulong rx_max_whole, rx_max_part, tx_max_whole, tx_max_part;
} * interface_t;

typedef struct total_interface {
    ulong rx_bw_total_whole, tx_bw_total_whole, tot_bw_total_whole;
    unsigned rx_bw_total_part, tx_bw_total_part, tot_bw_total_part;
} total_interface_t;

typedef struct host_interfaces {
	struct timeval gth_time;
	//const time_t gth_time;
	char host[50];
	int  number_iface;
	int first_pass;
	float uptime;
	interface_t interface[MAX_INTERFACES];
	total_interface_t sum_if;
} host_interfaces_t;

void   initialize_total(total_interface_t *);


bool_t do_total(total_interface_t *, interface_t *, int);
bool_t do_interface(char *, interface_t *, int first_pass);
bool_t count_average(interface_t *, float *);
ulong bwm_calc_remainder(ulong, ulong);

bool_t print_interface(interface_t * interface);

int read_bw_interfaces(host_interfaces_t* host_ifaces);
void print_bw_interfaces(host_interfaces_t* host_ifaces);
int  fprint_bw_interfaces(FILE *arquivo, host_interfaces_t* host_ifaces);
int fprint_bw_interface(FILE *arquivo, char *iface, host_interfaces_t* host_ifaces);
int file_for_bw(int i, host_interfaces_t *host_ifaces, char* iface, char *name_file);

void  print_help(char *);



#ifdef __GNUC__
#define fatal(...)\
{\
    fprintf(stderr, "FATAL ERROR OCCURED: ");\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, ": %s\n", strerror(errno));\
    exit(-1);\
}

#else /* __GNUC__ */
void
fatal(const char * fmt, ...)
{
    va_list list;

    fprintf(stderr, "FATAL ERROR OCCURED: ");
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    fprintf(stderr, ": %s\n", strerror(errno));
    exit(-1);
}

#endif /* __GNUC__ */

#endif /* BWINTERFACE_H_ */
