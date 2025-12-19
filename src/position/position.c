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
		int rc;

		// 1. Conectar ao GPSd
		// O quarto argumento é para o nome do dispositivo, geralmente NULL para localhost
		if (gps_open("localhost", DEFAULT_GPSD_PORT, gpsdata) < 0) {
			fprintf(stderr, "Erro: Não foi possível conectar ao GPSd\n");
			return -1;
		}

		// 2. Registrar para atualizações
		gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);

		// 3. Loop de leitura
		while (cont < TRYS) {
			// Nas versões atuais, verificamos se há dados esperando com timeout (microssegundos)
			// O timeout aqui é de 500ms (500000 microsegundos)
			if (gps_waiting(gpsdata, 500000)) {

				// gps_read agora retorna o número de bytes lidos ou -1
				if ((rc = gps_read(gpsdata, NULL, 0)) == -1) {
					fprintf(stderr, "Erro na leitura do GPSd\n");
					break;
				}

				// Verificamos se temos um fix válido (status > STATUS_NO_FIX)
				// Em versões recentes, status fica dentro da estrutura fix
				if (gpsdata->fix.status >= STATUS_GPS && gpsdata->fix.mode >= MODE_2D) {

					// Verificação de segurança para NaN usando isnan() da math.h
					if (isnan(gpsdata->fix.latitude) || isnan(gpsdata->fix.longitude)) {
						// Sem fix real ainda
						continue;
					}

					// Fix legítimo encontrado!
					// Podemos sair do loop
					goto success;
				}
			}

			cont++;
		}

		// Se chegou aqui, falhou em obter o fix
		gps_stream(gpsdata, WATCH_DISABLE, NULL);
		gps_close(gpsdata);
		return -1;

		success:
		// Limpeza antes de retornar sucesso
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


