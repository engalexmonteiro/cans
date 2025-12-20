/*
 * classifi.c
 *
 *  Created on: 20/02/2013
 *      Author: Alex Monteiro
 *      email:  eng.alexmonteiro@gmail.com
 */

#include "mod80211.h"
#include <iwlib.h>
#include <sys/stat.h>

static const char *wi_type_device[2] = {"802.11", "802.15.1"};

// Tabela de sobreposi����o de canais 802.11b
static float mtsb[CANAL][CANAL] = {

		/* Channel	  1		2	3	  4	   5	6	7	  8    9  10	11	12	  13    14 */
		/* 1 */		{1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00},
		/* 2 */		{0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00},
		/* 3 */		{0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00,0.00,0.00,0.00,0.00},
		/* 4 */		{0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00,0.00,0.00,0.00},
		/* 5 */		{0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00,0.00,0.00},
		/* 6 */		{0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00,0.00},
		/* 7 */		{0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00,0.00},
		/* 8 */		{0.00,0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00,0.00},
		/* 9 */		{0.00,0.00,0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.09,0.00},
		/* 10 */	{0.00,0.00,0.00,0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.31,0.00},
		/* 11 */	{0.00,0.00,0.00,0.00,0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.54,0.00},
		/* 12 */	{0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.77,0.22},
		/* 13 */	{0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.09,0.31,0.54,0.77,1.00,0.45},
		/* 14 */	{0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.22,0.45,1.00}
};


// Fun����o de c��lculo do fator de sobre posi����o
void calcula_FatorSbr(int* utilizacao, float* fator_sobreposicao){

	int canal , canal_sbr;

	for(canal=0; canal<CANAL; canal++){
		for(canal_sbr=0; canal_sbr<CANAL; canal_sbr++){

			fator_sobreposicao[canal] = fator_sobreposicao[canal] + (float)utilizacao[canal_sbr]*mtsb[canal][canal_sbr];

		}
	}
}

int verify_best_channels(int* best_channels,float* fator_sobreposicao){
	int i;

	float menor_fator = fator_sobreposicao[0];

	for(i = 1; i < CANAL-1 ; i++){
		if(menor_fator > fator_sobreposicao[i])
			menor_fator = fator_sobreposicao[i];
	}

	for(i = 0; i < CANAL ; i++){
		if(menor_fator >= fator_sobreposicao[i])
			best_channels[i] = 1;
		else
			best_channels[i] = 0;
	}

	return 0;
}


int freq2canal(double	freq)
{
	int frequencia;

	frequencia = (int)(freq/1000000);

	switch(frequencia){
	case 2412: return 1;
	case 2417: return 2;
	case 2422: return 3;
	case 2427: return 4;
	case 2432: return 5;
	case 2437: return 6;
	case 2442: return 7;
	case 2447: return 8;
	case 2452: return 9;
	case 2457: return 10;
	case 2462: return 11;
	case 2467: return 12;
	case 2472: return 13;
	case 2484: return 14;
	default:  return 0;
	}
}


void verifica_Canais(int* utilizacao, wireless_scan_head* context){


	wireless_scan* celula_atual;
	celula_atual = context->result;
	int canal;

	do{
		if(celula_atual == NULL){
			break;
		}

		//Converte frequencia em canal
		canal = freq2canal(celula_atual->b.freq);

		// printf("Frequencia: %g, Canal: %d\n",celula_atual->b.freq,canal); //Caso precise verificar

		//Preenche o vetor de utilizacao de canal
		utilizacao[canal-1]++;

		celula_atual = celula_atual->next;

	}while(celula_atual != NULL);

}


void verifica_Canaisv2(int* utilizacao, wireless_scan_head* context){


	wireless_scan* celula_atual;
	celula_atual = context->result;
	int canal;
	int rssi;

	do{
		if(celula_atual == NULL){
			break;
		}

		//Converte frequencia em canal
		canal = freq2canal(celula_atual->b.freq);
		if (celula_atual->stats.qual.level > 64)
			rssi  = celula_atual->stats.qual.level - 0x100;
		else
			rssi  = celula_atual->stats.qual.level;

		//Preenche o vetor de utilizacao de canal
		utilizacao[canal-1] = utilizacao[canal-1] + (100 + rssi);

		celula_atual = celula_atual->next;

	}while(celula_atual != NULL);


}

float fatordesobreposicao(int diferenca){

	switch(diferenca){
	case 0 : return 1;
	case 1 : return 0.7272;
	case 2 : return 0.2714;
	case 3 : return 0.0375;
	case 4 : return 0.0054;
	case 5 : return 0.0008;
	case 6 : return 0.0002;
	default: return 0;
	}
}


int calcula_snr(wireless_scan_mi* rede1, wireless_scan_mi* rede2){
	int diferenca;
	float fs;
	float snr;
	float interferencia;
	float lolf;

	diferenca = abs(rede1->canal - rede2->canal);

	fs = fatordesobreposicao(diferenca);

	lolf = 20*log(fs);

	interferencia = rede2->nivel + lolf;

	snr = (rede1->nivel-2) - interferencia;

	return snr;
}

int score_net_snr(wireless_scan_mi_list* list){

	wireless_scan_mi* celula_atual;
	wireless_scan_mi* celula_comparacao;
	//wireless_scan_mi* ;
	float max_ovf;

	int snr, control = 1;

	celula_atual = list->head_list;

	while(celula_atual != NULL){

		celula_atual->score_snr = 0;

		celula_comparacao = list->head_list;

		if(control)
			max_ovf = celula_atual->factor_overlap;

		while(celula_comparacao != NULL){

			if(max_ovf < celula_comparacao->factor_overlap)
				max_ovf = celula_comparacao->factor_overlap;

			if (strcmp(celula_atual->mac_address,celula_comparacao->mac_address)){

				snr = calcula_snr(celula_atual, celula_comparacao);

				if(snr >= 6) //Relacao sinal ruido maior de 6 dB
					celula_atual->score_snr++;
			}



			celula_comparacao = celula_comparacao->next;
		}

		control = 0;

		celula_atual->score = (float)(celula_atual->qualidade/100 + (float)(celula_atual->nivel-(-100))/65 + (float)(max_ovf - celula_atual->factor_overlap)/max_ovf);
		celula_atual->sum_score = celula_atual->sum_score + celula_atual->score;
		celula_atual->cont_score++;
		celula_atual->avg_score = celula_atual->sum_score/celula_atual->cont_score;

		celula_atual = celula_atual->next;

	};

	list->max_olf = max_ovf;

	return 0;
}

void print_canais_utilizados(int* utilizacao){

	int i = 0;

	printf("Vetor de utilizacao de canais:\n");
	printf("Canais\t\t[1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14]\nN-utilizados\t[");

	while(i< 14){
		printf("%d\t",utilizacao[i]);
		++i;
	}

	printf("]\n\n");
}


// Fun����es Auxiliares do WE
void
print_freq_value(double	freq)
{
	if(freq < 1000)
		printf("%g", freq);
	else
	{
		char	scale;
		int	divisor;

		if(freq >= 1000000000)
		{
			scale = 'G';
			divisor = 1000000000;
		}
		else
		{
			if(freq >= 1000000)
			{
				scale = 'M';
				divisor = 1000000;
			}
			else
			{
				scale = 'k';
				divisor = 1000000;
			}
		}
		printf("%g %cHz", freq / divisor, scale);
	}
}


void
print_bitrate(int bitrate)
{
	double	rate = bitrate;
	char		scale;
	int		divisor;

	if(rate >= 1000000000)
	{
		scale = 'G';
		divisor = 1000000000;
	}
	else
	{
		if(rate >= 1000000)
		{
			scale = 'M';
			divisor = 1000000;
		}
		else
		{
			scale = 'k';
			divisor = 1000;
		}
	}
	printf("%g %cb/s", rate / divisor, scale);
}


void print_iw_scan(wireless_scan_head* context, float* fator_sobreposicao){

	int level=0;
	wireless_scan* celula_atual;

	celula_atual = context->result;


	printf("WifiScan - Wifi Network Scanner\n");
	printf("Tipo\tEndere��o F��sico\t\tKey\tCanal\tFrequ��ncia\tQualidade\tN��vel de Sinal\tBitrate\tModo\tF.Sbr\tEESID / Device Name");

	do{
		if(celula_atual == NULL){
			printf("\nNenhuma rede Wifi encontrada\n");
			break;
		}

		/* Imprime tipo de rede */
		printf("\nWifi\t");

		/* Imprime Endere��o f��sico */
		///	printf("%s\t",iw_saether_ntop(&celula_atual->ap_addr, buffer));

		celula_atual->b.key_flags |= IW_ENCODE_NOKEY;
		if(celula_atual->b.key_flags & IW_ENCODE_DISABLED)
			printf("N��o\t");
		else
			printf("Sim\t");

		printf("%d\t",freq2canal(celula_atual->b.freq));

		/* Imprime Frequ��ncia */
		print_freq_value(celula_atual->b.freq);

		printf("\t%0.0f%%\t\t",(float)(celula_atual->stats.qual.qual/70.0) *100.0);

		if (celula_atual->stats.qual.level > 64)
			level  = celula_atual->stats.qual.level - 0x100;

		printf("%d dBm\t\t",level);

		print_bitrate(celula_atual->maxbitrate.value);

		//	printf("\t%s",iw_operation_mode[celula_atual->b.mode]);

		printf("\t%0.2f\t",fator_sobreposicao[freq2canal(celula_atual->b.freq)-1]-1.0);

		/* Imprime ESSID */
		printf("%s",celula_atual->b.essid);

		celula_atual = celula_atual->next;

	}while(celula_atual != NULL);

	printf("\n");
}

void limpar_iw_scan(wireless_scan_head* context){

	wireless_scan *no = context->result;
	wireless_scan *aux;

	while(no != NULL){
		aux = no;
		no = no->next;
		free(aux);
	}

	context->result = NULL;
	context->retry  = 0;
}

void limpar_scan_mi(wireless_scan_mi_list* list){

	wireless_scan_mi *no = list->head_list;
	wireless_scan_mi *aux;

	while(no != NULL){
		aux = no;
		no = no->next;
		free(aux);
	}

	list->head_list = NULL;
	list->size_list  = 0;
	list->end_list = NULL;
	list->factor_diversity = 0;
}

void zerar_vetores(int* utilizacao, float* fator_sobreposicao){

	int i;

	for(i=0; i<CANAL; i++){
		fator_sobreposicao[i] = 0;
		utilizacao[i] = 0;
	}
}


int iw_scan_wifi(wireless_scan_mi_list* list){

	int 				skfd;			/* generic raw socket desc.	*/
	int 				we_version;
	wireless_scan_head 	context;
	int 				cont = 0;

	we_version = iw_get_kernel_we_version();


	if((skfd = iw_sockets_open()) < 0){
		perror("socket");
		return -1;
	}

	// Varre o espectro
	iw_scan(skfd,"wlan0",we_version,&context);

	zerar_vetores(list->channel_util,list->fator_sobreposicao);

	verifica_Canais(list->channel_util,&context);

	// Calcula o Fator de sobreposicao
	calcula_FatorSbr(list->channel_util,list->fator_sobreposicao);

	char 			buffer[100];
	int 				 level=0;
	wireless_scan* celula_atual;

	struct wireless_scan_mi* current_device;
	struct wireless_scan_mi* old_device;

	celula_atual = context.result;

	while(celula_atual != NULL){

		old_device = current_device;

		current_device = (struct wireless_scan_mi *)malloc(sizeof(struct wireless_scan_mi));

		if(list->head_list == NULL)
			list->head_list = current_device;
		else
			old_device->next = current_device;

		/* Imprime Endere��o f��sico */
		current_device->type = 0;

		iw_ether_ntop((const struct ether_addr *)&celula_atual->ap_addr.sa_data, buffer);

		strcpy(current_device->mac_address,buffer);

		celula_atual->b.key_flags |= IW_ENCODE_NOKEY;
		if(celula_atual->b.key_flags & IW_ENCODE_DISABLED)
			current_device->key = 0;
		else
			current_device->key = 1;

		current_device->canal = freq2canal(celula_atual->b.freq);

		current_device->frequencia = (float)celula_atual->b.freq;

		current_device->qualidade = (float)(celula_atual->stats.qual.qual/70.0) *100.0;

		if (celula_atual->stats.qual.level > 64)
			level  = celula_atual->stats.qual.level - 0x100;

		current_device->nivel = level;

		current_device->maxbitrate = celula_atual->maxbitrate.value;

		strcpy(current_device->modo,iw_operation_mode[celula_atual->b.mode]);

		strcpy(current_device->essid,celula_atual->b.essid);

		current_device->factor_overlap = (list->fator_sobreposicao[current_device->canal-1]-1);

		if(!(current_device->key) && (current_device->qualidade >= 75.0) && (current_device->nivel> -50))
			list->factor_diversity++;

		current_device->next = NULL;

		list->end_list = current_device;

		cont++;

		celula_atual = celula_atual->next;
	};

	limpar_iw_scan(&context);

	iw_sockets_close(skfd);

	list->size_list = list->size_list + cont;

	return 0;
}

int add_ap_wifilist(wireless_scan_mi_list* list,wireless_scan_mi *ap_wifi){

	struct wireless_scan_mi* p;
	struct wireless_scan_mi* current_device;
	struct wireless_scan_mi* anterior;

	current_device = (struct wireless_scan_mi *)malloc(sizeof(struct wireless_scan_mi));

	memcpy(current_device,ap_wifi,sizeof(struct wireless_scan_mi));

	if(list->head_list == NULL){
		current_device->next = NULL;
		list->head_list = current_device;
	}
	else{

		p = list->head_list;
		anterior = NULL;

		/*procura posicao para insercao*/
		while(p != NULL && p->score < current_device->score)
		{
			anterior = p;
			p = p->next;
		}

		if(anterior == NULL){
			current_device->next = list->head_list;
			list->head_list = current_device;
		}
		else{
			anterior->next = current_device;
			current_device->next = p;
		}

	}

	return 0;
}


int iw_scan_wifiv2(char *ifname, wireless_scan_mi_list* list){

	int 				skfd;			/* generic raw socket desc.	*/
	int 				we_version;
	wireless_scan_head 	context;
	int 				cont = 0;

	we_version = iw_get_kernel_we_version();


	if((skfd = iw_sockets_open()) < 0){
		perror("socket");
		return -1;
	}

	// Varre o espectro
	iw_scan(skfd,ifname,we_version,&context);

	zerar_vetores(list->channel_util,list->fator_sobreposicao);

	verifica_Canaisv2(list->channel_util,&context);

	// Calcula o Fator de sobreposicao
	calcula_FatorSbr(list->channel_util,list->fator_sobreposicao);

	char 			buffer[100];
	int 				 level =0;
	wireless_scan* celula_atual;

	struct wireless_scan_mi* p;
	struct wireless_scan_mi* current_device;
	struct wireless_scan_mi* anterior;

	//struct wireless_scan_mi* old_device;

	celula_atual = context.result;

	while(celula_atual != NULL){

		//				old_device = current_device;

		current_device = (struct wireless_scan_mi *)malloc(sizeof(struct wireless_scan_mi));

		//				if(list->head_list == NULL)
		//				list->head_list = current_device;
		//		else
		//		old_device->next = current_device;


		/* Imprime Endere��o f��sico */
		current_device->type = 0;

		iw_ether_ntop((const struct ether_addr *) &celula_atual->ap_addr.sa_data, buffer);
		strcpy(current_device->mac_address,buffer);

		celula_atual->b.key_flags |= IW_ENCODE_NOKEY;
		if(celula_atual->b.key_flags & IW_ENCODE_DISABLED)
			current_device->key = 0;
		else
			current_device->key = 1;

		current_device->canal = freq2canal(celula_atual->b.freq);

		current_device->frequencia = (float)celula_atual->b.freq;

		current_device->qualidade = (float)(celula_atual->stats.qual.qual/70.0) *100.0;

		if (celula_atual->stats.qual.level > 64)
			level  = celula_atual->stats.qual.level - 0x100;

		current_device->nivel = level;

		current_device->maxbitrate = celula_atual->maxbitrate.value;

		strcpy(current_device->modo,iw_operation_mode[celula_atual->b.mode]);

		strcpy(current_device->essid,celula_atual->b.essid);

		current_device->factor_overlap = (list->fator_sobreposicao[current_device->canal-1] - (100 + current_device->nivel));

		if(!(current_device->key) && (current_device->qualidade >= 75.0) && (current_device->nivel> -60))
			list->factor_diversity++;


		if(list->head_list == NULL){
			current_device->next = NULL;
			list->head_list = current_device;
		}
		else{

			p = list->head_list;
			anterior = NULL;

			/*procura posicao para insercao*/
			while(p != NULL && p->factor_overlap < current_device->factor_overlap)
			{
				anterior = p;
				p = p->next;
			}

			if(anterior == NULL){
				current_device->next = list->head_list;
				list->head_list = current_device;
			}
			else{
				anterior->next = current_device;
				current_device->next = p;
			}

		}

		list->end_list = current_device;

		cont++;

		celula_atual = celula_atual->next;
	};

	limpar_iw_scan(&context);

	iw_sockets_close(skfd);

	gettimeofday(&list->time,NULL);

	verify_best_channels(list->melhores_canais,list->fator_sobreposicao);

	score_net_snr(list);

	list->size_list = list->size_list + cont;

	return 0;
}

int iw_scan_wifiv3(char *ifname, wireless_scan_mi_list* list){

	int 				skfd;			/* generic raw socket desc.	*/
	int 				we_version;
	wireless_scan_head 	context;
	int 				cont = 0;

	we_version = iw_get_kernel_we_version();


	if((skfd = iw_sockets_open()) < 0){
		perror("socket");
		return -1;
	}

	// Varre o espectro
	iw_scan(skfd,ifname,we_version,&context);

	gettimeofday(&list->time,NULL);

	if(context.result != NULL){

		zerar_vetores(list->channel_util,list->fator_sobreposicao);

		verifica_Canaisv2(list->channel_util,&context);

		// Calcula o Fator de sobreposicao
		calcula_FatorSbr(list->channel_util,list->fator_sobreposicao);

		char 			buffer[100];
		int 				 level =0;
		wireless_scan* celula_atual;

		struct wireless_scan_mi* p;
		struct wireless_scan_mi* current_device;
		struct wireless_scan_mi* anterior;

		celula_atual = context.result;

		current_device = list->head_list;

		while(current_device != NULL){

			current_device->lastsee = 0;

			current_device = current_device->next;

		}

		while(celula_atual != NULL){

			current_device = list->head_list;

			while(current_device != NULL){

				iw_ether_ntop((const struct ether_addr *) &celula_atual->ap_addr.sa_data, buffer);
				if(!strcmp(current_device->mac_address,buffer)){
					break;
				}

				current_device = current_device->next;
			}

			if(current_device == NULL){

				current_device = (struct wireless_scan_mi *)malloc(sizeof(struct wireless_scan_mi));

				memset(current_device,0,sizeof(struct wireless_scan_mi));

				current_device->lastsee = 1;

				memcpy(&current_device->time_lastsee,&list->time,sizeof(struct timeval));

				/* Imprime Endere��o f��sico */
				current_device->type = 0;

				iw_ether_ntop((const struct ether_addr *) &celula_atual->ap_addr.sa_data, buffer);
				strcpy(current_device->mac_address,buffer);

				celula_atual->b.key_flags |= IW_ENCODE_NOKEY;
				if(celula_atual->b.key_flags & IW_ENCODE_DISABLED)
					current_device->key = 0;
				else
					current_device->key = 1;

				current_device->canal = freq2canal(celula_atual->b.freq);

				current_device->frequencia = (float)celula_atual->b.freq;

				current_device->qualidade = (float)(celula_atual->stats.qual.qual/70.0) *100.0;
				current_device->qual_sum =current_device->qual_sum + current_device->qualidade;
				current_device->qual_cont++;
				current_device->qual_avg  = current_device->qual_sum / current_device->qual_cont;

				if (celula_atual->stats.qual.level > 64)
					level  = celula_atual->stats.qual.level - 0x100;

				current_device->nivel = level;
				current_device->sum_level = (float)(current_device->sum_level + current_device->nivel);
				current_device->cont_level++;
				current_device->avg_level = (float)(current_device->sum_level/current_device->cont_level);

				current_device->maxbitrate = celula_atual->maxbitrate.value;

				strcpy(current_device->modo,iw_operation_mode[celula_atual->b.mode]);

				strcpy(current_device->essid,celula_atual->b.essid);

				current_device->factor_overlap = (list->fator_sobreposicao[current_device->canal-1] - (100 + current_device->nivel));
				current_device->sum_olf = current_device->sum_olf + current_device->factor_overlap;
				current_device->cont_olf++;
				current_device->avg_olf = current_device->sum_olf/current_device->cont_olf;

				if(!(current_device->key) && (current_device->qualidade >= 75.0) && (current_device->nivel> -60))
					list->factor_diversity++;


				if(list->head_list == NULL){
					current_device->next = NULL;
					current_device->previus = NULL;
					list->head_list = current_device;
				}
				else{

					p = list->head_list;
					anterior = NULL;

					/*procura posicao para insercao*/
					while(p != NULL && p->factor_overlap < current_device->factor_overlap)
					{
						anterior = p;
						p = p->next;
					}

					if(anterior == NULL){
						current_device->next = list->head_list;
						current_device->previus = anterior;
						list->head_list = current_device;
					}
					else{
						anterior->next = current_device;
						current_device->previus = anterior;
						current_device->next = p;
					}
				}

				list->end_list = current_device;

				cont++;
			}else{

				memcpy(&current_device->time_lastsee,&list->time,sizeof(struct timeval));

				current_device->lastsee = 1;

				/* Imprime Endere��o f��sico */
				current_device->type = 0;

				strcpy(current_device->mac_address,buffer);

				celula_atual->b.key_flags |= IW_ENCODE_NOKEY;
				if(celula_atual->b.key_flags & IW_ENCODE_DISABLED)
					current_device->key = 0;
				else
					current_device->key = 1;

				current_device->canal = freq2canal(celula_atual->b.freq);

				current_device->frequencia = (float)celula_atual->b.freq;

				current_device->qualidade = (float)(celula_atual->stats.qual.qual/70.0) *100.0;
				current_device->qual_sum =current_device->qual_sum + current_device->qualidade;
				current_device->qual_cont++;
				current_device->qual_avg  = current_device->qual_sum / current_device->qual_cont;

				if (celula_atual->stats.qual.level > 64)
					level  = celula_atual->stats.qual.level - 0x100;

				current_device->nivel = level;

				current_device->sum_level = (float)(current_device->sum_level + current_device->nivel);
				current_device->cont_level++;
				current_device->avg_level = (float)(current_device->sum_level/current_device->cont_level);

				current_device->maxbitrate = celula_atual->maxbitrate.value;

				strcpy(current_device->modo,iw_operation_mode[celula_atual->b.mode]);

				strcpy(current_device->essid,celula_atual->b.essid);

				current_device->factor_overlap = (list->fator_sobreposicao[current_device->canal-1] - (100 + current_device->nivel));
				current_device->sum_olf = current_device->sum_olf + current_device->factor_overlap;
				current_device->cont_olf++;
				current_device->avg_olf = current_device->sum_olf/current_device->cont_olf;

			}

			celula_atual = celula_atual->next;
		};

		limpar_iw_scan(&context);

		iw_sockets_close(skfd);

		verify_best_channels(list->melhores_canais,list->fator_sobreposicao);

		score_net_snr(list);

		list->size_list = list->size_list + cont;

		return 0;

	}

	return 1;
}


wireless_scan_mi* find_wifinet(wireless_scan_mi_list* list, wireless_scan_mi* wifinet){

	struct wireless_scan_mi* current_device;

	current_device = list->head_list;

	while(current_device != NULL){

		if(!strcmp(current_device->mac_address,wifinet->mac_address)){
			return current_device;
		}

		current_device = current_device->next;
	}

	return NULL;
}


static int
get_info(int			skfd,
		char *			ifname,
		struct wireless_info *	info)
{
	struct iwreq		wrq;

	memset((char *) info, 0, sizeof(struct wireless_info));

	/* Get basic information */
	if(iw_get_basic_config(skfd, ifname, &(info->b)) < 0)
	{
		/* If no wireless name : no wireless extensions */
		/* But let's check if the interface exists at all */
		struct ifreq ifr;

		strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
		if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
			return(-ENODEV);
		else
			return(-ENOTSUP);
	}

	/* Get ranges */
	if(iw_get_range_info(skfd, ifname, &(info->range)) >= 0)
		info->has_range = 1;

	/* Get AP address */
	if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0)
	{
		info->has_ap_addr = 1;
		memcpy(&(info->ap_addr), &(wrq.u.ap_addr), sizeof (sockaddr));
	}

	/* Get bit rate */
	if(iw_get_ext(skfd, ifname, SIOCGIWRATE, &wrq) >= 0)
	{
		info->has_bitrate = 1;
		memcpy(&(info->bitrate), &(wrq.u.bitrate), sizeof(iwparam));
	}

	/* Get Power Management settings */
	wrq.u.power.flags = 0;
	if(iw_get_ext(skfd, ifname, SIOCGIWPOWER, &wrq) >= 0)
	{
		info->has_power = 1;
		memcpy(&(info->power), &(wrq.u.power), sizeof(iwparam));
	}


	/* Get stats */
	if(iw_get_stats(skfd, ifname, &(info->stats),
			&info->range, info->has_range) >= 0)
	{
		info->has_stats = 1;
	}

#ifndef WE_ESSENTIAL
	/* Get NickName */
	wrq.u.essid.pointer = (caddr_t) info->nickname;
	wrq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
	wrq.u.essid.flags = 0;
	if(iw_get_ext(skfd, ifname, SIOCGIWNICKN, &wrq) >= 0)
		if(wrq.u.data.length > 1)
			info->has_nickname = 1;

	if((info->has_range) && (info->range.we_version_compiled > 9))
	{
		/* Get Transmit Power */
		if(iw_get_ext(skfd, ifname, SIOCGIWTXPOW, &wrq) >= 0)
		{
			info->has_txpower = 1;
			memcpy(&(info->txpower), &(wrq.u.txpower), sizeof(iwparam));
		}
	}

	/* Get sensitivity */
	if(iw_get_ext(skfd, ifname, SIOCGIWSENS, &wrq) >= 0)
	{
		info->has_sens = 1;
		memcpy(&(info->sens), &(wrq.u.sens), sizeof(iwparam));
	}

	if((info->has_range) && (info->range.we_version_compiled > 10))
	{
		/* Get retry limit/lifetime */
		if(iw_get_ext(skfd, ifname, SIOCGIWRETRY, &wrq) >= 0)
		{
			info->has_retry = 1;
			memcpy(&(info->retry), &(wrq.u.retry), sizeof(iwparam));
		}
	}

	/* Get RTS threshold */
	if(iw_get_ext(skfd, ifname, SIOCGIWRTS, &wrq) >= 0)
	{
		info->has_rts = 1;
		memcpy(&(info->rts), &(wrq.u.rts), sizeof(iwparam));
	}

	/* Get fragmentation threshold */
	if(iw_get_ext(skfd, ifname, SIOCGIWFRAG, &wrq) >= 0)
	{
		info->has_frag = 1;
		memcpy(&(info->frag), &(wrq.u.frag), sizeof(iwparam));
	}
#endif	/* WE_ESSENTIAL */

	return(0);
}

int get_rssi_info(char* ifname, wireless_scan_mi* wifinet){

	int 				skfd;			/* generic raw socket desc.	*/
	struct wireless_info 	info;

	if((skfd = iw_sockets_open()) < 0){
		perror("socket");
		return -100;
	}

	get_info(skfd,ifname,&info);

	int 				 level  = -100;

	wifinet->connected  = (int)info.b.freq;

	if (wifinet->connected){

		if (info.stats.qual.level > 64){
			level  = info.stats.qual.level - 0x100;
			iw_sockets_close(skfd);
			return level;
		}

	}

	iw_sockets_close(skfd);
	return -100;
}


int get_mywifi_info(char* ifname, wireless_scan_mi* wifinet){

	int 				skfd;			/* generic raw socket desc.	*/
	struct wireless_info 	info;

	if((skfd = iw_sockets_open()) < 0){
		perror("socket");
		return -1;
	}

	get_info(skfd,ifname,&info);

	char 			buffer[100];
	int 				 level =0;

	wifinet->connected  = (int)info.b.freq;

	if (wifinet->connected){
		/* Imprime Endere��o f��sico */
		strcpy(wifinet->protocol,info.b.name);

		iw_ether_ntop((const struct ether_addr *) &info.ap_addr.sa_data, buffer);
		strcpy(wifinet->mac_address,buffer);

		info.b.key_flags |= IW_ENCODE_NOKEY;
		if(info.b.key_flags & IW_ENCODE_DISABLED)
			wifinet->key = 0;
		else
			wifinet->key = 1;

		wifinet->canal = freq2canal(info.b.freq);

		wifinet->frequencia = (float)info.b.freq;

		wifinet->qualidade = (float)(info.stats.qual.qual/70.0) *100.0;

		if (info.stats.qual.level > 64)
			level  = info.stats.qual.level - 0x100;

		wifinet->nivel = level;

		wifinet->maxbitrate = info.bitrate.value;

		strcpy(wifinet->modo,iw_operation_mode[info.b.mode]);

		strcpy(wifinet->essid,info.b.essid);

	}

	iw_sockets_close(skfd);

	return 0;
}



void init_wifi_info(wireless_scan_mi_list* wifinet){
	wifinet->head_list = NULL;
	wifinet->end_list = NULL;
	wifinet->size_list = 0;
	wifinet->factor_diversity = 0;
}

void fprintf_mywifi_info(FILE * 	 file, wireless_scan_mi* wifinet)
{
	fprintf(file,"%s\t",wifinet->protocol);

	fprintf(file,"%s\t",wifinet->mac_address);

	fprintf(file,"%d\t",wifinet->canal);

	fprintf(file,"%.2f\t",wifinet->frequencia/1e6);

	fprintf(file,"%.2f\t",(float)(wifinet->maxbitrate/1e6));

	fprintf(file,"%.2f\t",wifinet->qualidade);

	fprintf(file,"%d\t",wifinet->nivel);

	fprintf(file,"%s\t",wifinet->essid);

	fprintf(file,"\n");
}


int fprint_current_net(int itr, wireless_scan_mi* wifinet, char* filename){

	FILE *file;
	struct timeval time;
	struct tm *temp;

	gettimeofday(&time,NULL);

	temp = localtime(&time.tv_sec);

	if (itr == 0){
		file = fopen(filename,"w");
		fprintf(file,"#Date\t\tHour\tSample\tProtocolo\tMAC Address\t\tChannel\tFreqGhz\tBitrate\tQual\tSignal\tESSID\n"); //Cabeçalho
	}else
		file = fopen(filename,"a");

	fprintf(file,"%02d/%02d/%02d\t%02d:%02d:%02d %02d\t",
			temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
			temp->tm_hour,temp->tm_min,temp->tm_sec,
			itr);

	fprintf_mywifi_info(file,wifinet);

	fclose(file);

	return 0;
}

int print_scan_mi(wireless_scan_mi_list* list){

	int i;
	wireless_scan_mi* current_device;

	current_device = list->head_list;


	printf("Tipo\tEndereço Físico\t\tLS\tKey\tCanal\tf(Ghz)\tQuality\tSignal(dBm)\tBitrate\tModo\tF.Sbr\tPt-SNR\tScore\tDevice Name");

	do{
		if(current_device == NULL){
			printf("\nNenhuma rede Wifi encontrada\n");
			break;
		}

		/* Imprime tipo de rede */
		printf("\n%s\t",wi_type_device[current_device->type]);
		/* Imprime Endere��o f��sico */
		printf("%s\t",current_device->mac_address);

		printf("%d\t",current_device->lastsee);

		if(!current_device->key)
			printf("Não\t");
		else
			printf("Sim\t");

		printf("%d\t",current_device->canal);

		/* Imprime Frequ��ncia */
		//print_freq_value(current_device->frequencia);
		printf("%g\t",current_device->frequencia/1000000000);

		printf("%0.0f%% %0.0f%%\t",(float)current_device->qualidade,(float)current_device->qual_avg);

		printf("%d %.2f\t",current_device->nivel,current_device->avg_level);

		print_bitrate(current_device->maxbitrate);

		printf("\t%s",current_device->modo);

		printf("\t%0.2f %0.2f\t",current_device->factor_overlap,current_device->avg_olf);

		printf("%d/%d\t",current_device->score_snr,list->size_list-1);

		/* Imprime ESSID */
		printf("%.2f %.2f\t",current_device->score,current_device->avg_score);


		printf("%s",current_device->essid);


		current_device = current_device->next;

	}while(current_device != NULL);

	printf("\n\nF. Sobreposicao\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\n\t\t");

	for(i = 0; i < CANAL ; i++)
		printf("%0.2f\t",list->fator_sobreposicao[i]);

	printf("\nMelhor(es) canal: ");

	for(i = 0; i < CANAL ; i++){
		if(list->melhores_canais[i])
			printf("%d, ",i+1);
	}

	printf("\n");
	//printf("\nFator diversidade da Interface Wi-fi: %d\n",list->factor_diversity);


	return 0;
}



int fprint_scan_mi(int itr, wireless_scan_mi_list* list){

#define SUBD "./nets/"

	char filename[50];
	wireless_scan_mi* current_device;
	FILE *current_file;

	struct tm *temp;
	temp = localtime(&list->time.tv_sec);

	current_device = list->head_list;

	struct stat st = {0};

	if (stat(SUBD, &st) == -1)
		mkdir(SUBD, 0755);

	while(current_device != NULL){

		strcpy(filename,SUBD);
		strcat(filename,current_device->essid);
		strcat(filename,".txt");

		if(itr == 0){
			current_file = fopen(filename,"w");
			fprintf(current_file,"#Date\t\tHour\tSample\tTipo\tMAC\t\t\tKey\tChannel\tFreq.\tQuality\tSignal\tBitrate\tMode\tF.Sbr\tPt-SNR\tSSID\n");
		}
		else{
			current_file = fopen(filename,"a");
		}

		fprintf(current_file,"%02d/%02d/%02d\t%02d:%02d:%02d %02d\t",
				temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
				temp->tm_hour,temp->tm_min,temp->tm_sec,
				itr);

		/* Imprime tipo de rede */
		fprintf(current_file,"%s\t",wi_type_device[current_device->type]);

		/* Imprime Endere��o f��sico */
		fprintf(current_file,"%s\t",current_device->mac_address);

		if(!current_device->key)
			fprintf(current_file,"Não\t");
		else
			fprintf(current_file,"Sim\t");

		fprintf(current_file,"%d\t",current_device->canal);

		/* Imprime Frequ��ncia */
		fprintf(current_file,"%0.3f\t",current_device->frequencia/1000000000);

		fprintf(current_file,"%0.0f\t",(float)current_device->qualidade);

		fprintf(current_file,"%d\t",current_device->nivel);

		fprintf(current_file,"%d\t",current_device->maxbitrate/1000000);

		fprintf(current_file,"%s\t",current_device->modo);

		fprintf(current_file,"%0.2f\t",current_device->factor_overlap);

		fprintf(current_file,"%d\t",current_device->score_snr);

		/* Imprime ESSID */
		fprintf(current_file,"%s\n",current_device->essid);

		fclose(current_file);

		current_device = current_device->next;

	};

	return 0;
}

int fprint_olf(int itr, wireless_scan_mi_list* list, char* filename){

	int i;
	FILE *file;
	struct tm *temp;

	temp = localtime(&list->time.tv_sec);

	if(itr == 0){
		file = fopen(filename,"w");
		fprintf(file,"Overlap factor\n#Date\t\tHour\tSample\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\n");
	}
	else
		file = fopen(filename,"a");

	fprintf(file,"%02d/%02d/%02d\t%02d:%02d:%02d %02d\t",
			temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
			temp->tm_hour,temp->tm_min,temp->tm_sec,
			itr);

	for(i = 0; i < CANAL ; i++)
		fprintf(file,"%0.2f\t",list->fator_sobreposicao[i]);

	fprintf(file,"\n");

	fclose(file);

	return 0;
}

int fprint_best_channels(int itr, wireless_scan_mi_list* list, char* filename){
	int i;

	FILE *file;

	struct tm *temp;

	temp = localtime(&list->time.tv_sec);

	if(itr == 0){
		file = fopen(filename,"w");
		fprintf(file,"#Best Channels\n#Date\t\tHour\tSample\t1 2 3 4 5 6 7 8 9 10 11 12 13 14 Channel\n");
	}
	else
		file = fopen(filename,"a");

	fprintf(file,"%02d/%02d/%02d\t%02d:%02d:%02d %02d\t",
			temp->tm_mday,temp->tm_mon,temp->tm_year+1900,
			temp->tm_hour,temp->tm_min,temp->tm_sec,
			itr);

	for(i = 0; i < CANAL ; i++){
		if(list->melhores_canais[i])
			fprintf(file,"%d ",1);
		else
			fprintf(file,"%d ",0);
		if(i > 8)
			fprintf(file," ");

	}

	fprintf(file,"\n");

	fclose(file);

	return 0;
}

int print_classi_network(wireless_scan_mi_list* list){


	wireless_scan_mi* current_device;

	current_device = list->head_list;


	printf("\n\nRedes Classificadas\n");
	printf("Tipo\tEndereço Físico\t\tKey\tCanal\tFrequência\tQualidade\tNível de Sinal\tBitrate\tModo\tF.Sbr\tPt-SNR\tEESID / Device Name");

	do{
		if(current_device == NULL){
			break;
		}

		if(!current_device->key && current_device->qualidade >= 75.0 && current_device->nivel > -60){

			/* Imprime tipo de rede */
			printf("\n%s\t",wi_type_device[current_device->type]);

			/* Imprime Endere��o f��sico */
			printf("%s\t",current_device->mac_address);

			if(!current_device->key)
				printf("Não\t");
			else
				printf("Sim\t");

			printf("%d\t",current_device->canal);

			/* Imprime Frequ��ncia */
			print_freq_value(current_device->frequencia);

			printf("\t%0.0f%%\t\t",(float)current_device->qualidade);

			printf("%d dBm\t\t",current_device->nivel);

			print_bitrate(current_device->maxbitrate);

			printf("\t%s",current_device->modo);

			printf("\t%0.2f\t",current_device->factor_overlap);

			printf("%d/%d\t",current_device->score_snr,list->size_list-1);

			/* Imprime ESSID */
			printf("%s",current_device->essid);

		}

		current_device = current_device->next;

	}while(current_device != NULL);

	return 0;
}



int save_scan_mi_file(wireless_scan_mi_list* list, time_t currentTime){

	struct tm *timeinfo = localtime(&currentTime);

	int i;
	int menor_fator;
	int melhor_canal;
	wireless_scan_mi* current_device;

	char string[SIZEQUERY];

	FILE *arquivo;


	current_device = list->head_list;

	if(current_device != NULL){

		arquivo = fopen(WIFINETWORKS,"a");

		if (arquivo != NULL)
		{

			while(current_device != NULL){

				snprintf(string,SIZEQUERY,"%d-%d-%d;%d:%d:%d;wifi;%s;%d;%d;%0.2f;%0.2f;%d;%d;%s;%0.2f;%d/%d;%s\n",
						timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday,
						timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
						current_device->mac_address,
						current_device->key,
						current_device->canal,
						current_device->frequencia,
						current_device->qualidade,
						current_device->nivel,
						current_device->maxbitrate,
						current_device->modo,
						current_device->factor_overlap,
						current_device->score_snr,
						list->size_list,
						current_device->essid);

				if(EOF == fputs(string,arquivo))
					printf("Erro ao gravar lista de redes detectadas");

				current_device = current_device->next;

			};

		}
		else
		{
			printf("Falha ao abrir arquivo varredura.txt\n");
		}

		fclose(arquivo);

		menor_fator = list->fator_sobreposicao[0];
		melhor_canal = 1;

		arquivo = fopen(OVERLAPFACTOR,"a");

		if (arquivo != NULL)
		{


			for(i = 0; i < CANAL-1 ; i++){

				if(menor_fator > list->fator_sobreposicao[i]){
					menor_fator = list->fator_sobreposicao[i];
					melhor_canal = i+1;
				}
			}

			snprintf(string,SIZEQUERY,"%d-%d-%d;%d:%d:%d;",
					timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday,
					timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

			if(EOF == fputs(string,arquivo))
				printf("Erro ao gravar lista de fatores de sobreposicao de canais");

			for(i = 0; i < CANAL-1 ; i++){
				snprintf(string,SIZEQUERY,"%0.2f;",list->fator_sobreposicao[i]);

				if(EOF == fputs(string,arquivo))
					printf("Erro ao gravar fator de sobreposicao do canal %d",i+1);
			}

			snprintf(string,SIZEQUERY,"%d;%d\n",melhor_canal,list->factor_diversity);

			if(EOF == fputs(string,arquivo))
				printf("Erro ao gravar lista de fatores de sobreposicao de canais");

		}else{
			printf("Erro ao abrir arquivo fscanais.txt");
		}

		fclose(arquivo);

	}

	return 0;
}

int update_statistics(wireless_scan_mi* device){

	if(device->nivel != -100){
		device->sum_level = device->sum_level + (float)device->nivel;
		device->cont_level++;
		device->avg_level = (float)(device->sum_level/device->cont_level);
	}

	device->sum_olf = device->sum_olf + device->factor_overlap;
	device->cont_olf++;
	device->avg_olf =  device->sum_olf/device->cont_olf;

	device->sum_score = device->sum_score + device->score;
	device->cont_score++;
	device->avg_score =  device->sum_score/device->cont_score;

	if(device->service != -1){
		device->sum_service = device->sum_service + (float)device->service;
		device->cont_service++;
		device->avg_service = (float)(device->sum_service/device->cont_service);
	}

	return 0;
}

int varredura(wireless_scan_mi_list* list){


	list->head_list = NULL;
	list->end_list = NULL;
	list->size_list = 0;
	list->factor_diversity = 0;

	time_t currentTime;
	struct tm *timeinfo;

	iw_scan_wifiv2("wlan0",list);

	score_net_snr(list);

	/* Pega a hora atual do sistema. */
	currentTime= time(NULL);

	/* Converte-o em uma estrutura tm. */
	timeinfo= localtime(&currentTime);

	/* Apresenta a hora atual no formato horas:minutos:segundos */
	printf("\nData: %02d/%02d/%d Hora atual: %02d:%02d:%02d\n", timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year+1900,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	print_classi_network(list);

	print_scan_mi(list);

	return 0;
}




