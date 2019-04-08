

/***************************************************************************
                          main.c  -  server
                             -------------------
    begin                : lun feb  4 15:30:41 CET 2002
    copyright            : (C) 2002 by A. Moreno
    email                : amoreno@euss.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <unistd.h>
#include <pthread.h>


#define SERVER_PORT_NUM		5001
#define SERVER_MAX_CONNECTIONS	4
#define REQUEST_MSG_SIZE	1024
#define TEMP_SIZE               3600
double temperatures[TEMP_SIZE];
int    imostres;
int    max_mostres;
int    num_mostres;
int    iactual;
double tmax;
double tmin;
double tavg;
int    sensor_run;
int    sensor_temps_mostreig;
int    sensor_mostres_mitjana;
int    i=0;
void mitjana(int newFd);	
void maxim(int newFd);
void minim(int newFd);
void resetmax(int newFd);
void icomptador(int newFd);
void marxastop(int newFd);

void * tcp_server_main(void * arg);
void * sensor_server_main(void * arg);

void llegir_sensor ();


/************************
*
*
* tcpServidor
*
*
*/

int main()
{
	
	
	struct sockaddr_in	serverAddr;
	struct sockaddr_in	clientAddr;
	int			sockAddrSize;
	int			sFd;
	int			newFd;
	int 		result;
	int r=0;
	

	/*Preparar l'adreça local*/
	sockAddrSize=sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*Crear un socket*/
	sFd=socket(AF_INET, SOCK_STREAM, 0);
	
	/*Nominalitzar el socket*/
	result = bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	
	/*Crear una cua per les peticions de connexió*/
	result = listen(sFd, SERVER_MAX_CONNECTIONS);
	
	/*Bucle d'acceptació de connexions*/
	
	while(1){
		
		sensor_main(r);
		r=1;
		printf("\nServidor esperant connexions\n");

		/*Esperar conexió. sFd: socket pare, newFd: socket fill*/
		newFd=accept(sFd, (struct sockaddr *) &clientAddr, &sockAddrSize);
		printf("Connexión acceptada del client: adreça %s, port %d\n",	inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		/*Rebre*/
        char itrama;
        result = read(newFd, &itrama, 1); 
        if ((result == 1) && (itrama == '{')) { //si result es 1(no està buit) i  el primer caracter de la trama és "{"
            char comanda;
            result = read(newFd, &comanda, 1);
            if (result == 1) {
                switch (comanda) {
				case 'M':
					marxastop(newFd);
                case 'U':
                    mitjana(newFd);
                    break;
                case 'X':
                    maxim(newFd);
                    break;
                case 'Y':
                    minim(newFd);
                    break;
                case 'R':
                    resetmax(newFd);
                    break;
                case 'B':
                    icomptador(newFd);
                    break;     
                default:
                    fprintf(stderr, "comanda incorrecte: comanda='%c'\n", comanda);
                    break;
						
			}
          }
		
		 else
            {fprintf(stderr, "manca comanda: result=%d\n", result);
            }
        }
        else {
            fprintf(stderr, "inici trama incorrecte: '%c' (result=%d)\n", itrama, result);
        }
		

		/*Tancar el socket fill*/
		result = close(newFd);
		}
			
	return (void *)0;
	}



void sensor_main(r)
{
    if (r==0)
    {
    iactual       = 0;
    tmax          = 10.0;
    tmin          = 99.99;
    tavg	      = 0.0;
    imostres      = 0;
    num_mostres   = 0;
    max_mostres   = TEMP_SIZE;
    sensor_run 	  = 0;
	
	}
   
    {
	
        if (sensor_run != 0) {
			  llegir_sensor ();
			
            /* calcular mitjana */
            double sum = 0.0;
            if (imostres < max_mostres) {
                for (i = 0 ; i < imostres ; ++i ) {
                    sum += temperatures[i];
                }
                tavg = sum / (double)imostres;
            }
            else {
                for (i = 0 ; i < max_mostres ; ++i ) {
                    sum += temperatures[i];
                }
                tavg = sum / (double)max_mostres;
            }

            fprintf (stdout, "sensor_server_main() => avg=%f max=%f min=%f sleep=%d\n", tavg, tmax, tmin, sensor_temps_mostreig);
            //sleep (sensor_temps_mostreig);
        }
        else {
            //fprintf (stdout, "sensor_main() => sleep(1)\n");
            //sleep (1);
        }
    }
}


void llegir_sensor () {
	
    double temp = (((double)random() * 99.00) / (double)RAND_MAX); 
    fprintf (stdout, "llegir_sensor() = %f (%d)\n", temp, imostres);
	
    temperatures[imostres] = temp;
    if (temp > tmax) tmax = temp;
    if (temp < tmin) tmin = temp;
    imostres = imostres + 1;
    imostres = imostres % max_mostres;
    num_mostres = num_mostres + 1;
 
}

void marxastop(int newFd) {
	int result;
    char trama[5];
    char resposta[10];
    int  start;
    int  temps;
    int  mostres;
    int  ok = 1;


    result = read(newFd, &trama, 5);
    if ((result == 5) && (trama[4] == '}')) {

        /* comprovar el byte de parada */
        if ((trama[0] == '0') || (trama[0] == '1'))
        {
            start = (trama[0] == '1');

            /* comprovar numero mostres */
            if ((trama[3] >= '1') && (trama[3] <= '9')) {

                mostres = trama[1] - '0';

                /* comprovar el temps */
                switch (trama[1]) {    
                case '0': /* si el temps es entre 1 i 9*/
                    if ((trama[2] >= '1') && (trama[2] <= '9'))
                    {
                        temps = trama[2] - '0';
                    }
                    else {
                        ok = 0;
                    }
                    break;

                case '1':/* si el temps es entre 11 i 9*/
                    if ((trama[2] >= '0') && (trama[2] <= '9'))
                    {
                        temps = 10 + (trama[2] - '0');
                    }
                    else {
                        ok = 0;
                    }
                    break;

                case '2':/* si el temps es 20 */
                    if (trama[2] == '0')
                    {
                        temps = 20;
                    }
                    else {
                        ok = 0;
                    }
                    break;

                default:
                    ok = 0;
                    break;
                }
            }
            else {
                ok = 0;
            }
        }
        else {
            ok = 0;
        }

        if (ok)
        {
            sensor_temps_mostreig  = temps;
            sensor_mostres_mitjana = mostres;
            sensor_run		       = start;
            
			memset(resposta,'\0',7);
            /* crear resposta correcte */
            strcpy (resposta, "{M0}");
        }
        else {
            /* crear resposta parametre incorrecte */
            strcpy (resposta, "{M2}");
        }
    }
    else {
        fprintf(stderr, "marxastop(): fi trama incorrecte: '%c' (result=%d)\n", trama[0], result);
        /* crear resposta */
        strcpy (resposta, "{M1}");
    }

    /* enviar resposta */
    result = write (newFd, resposta, strlen(resposta));
    if (result != strlen(resposta)) {
        perror("marxastop(write)\n");
        
    }
	
}
void mitjana(int newFd) {
	
	int result;
    char fitrama;
    char resposta[10];

    result = read(newFd, &fitrama, 1);
    if ((result == 1) && (fitrama == '}')) {

        /* crear resposta */
        snprintf (resposta, 10, "{U0%05.2f}", tavg);

    }
    else {
        fprintf(stderr, "mitjana(): fi trama incorrecte: '%c' (result=%d)\n", fitrama, result);
        /* crear resposta fi trama incorrecte */
        strcpy (resposta, "{U1}");
    }

    /* enviar resposta */
    result = write (newFd, resposta, strlen(resposta));
    if (result != strlen(resposta)) {
        perror("mitjana(write)\n");
    }

	
}
void maxim(int newFd) {
	
	int result;
    char fi_trama;
    char resposta[10];

    result = read(newFd, &fi_trama, 1);
    if ((result == 1) && (fi_trama == '}')) {

        /* crear resposta */
        snprintf (resposta, 10, "{X0%05.2f}", tmax);
    }
    else {
        fprintf(stderr, "maxim(): fi trama incorrecte: '%c' (result=%d)\n", fi_trama, result);
        /* crear resposta fi trama incorrecte */
        strcpy (resposta, "{X1}");
    }

    /* enviar resposta */
    printf ("resposta es %s", resposta);
    result = write (newFd, resposta, strlen(resposta));
    if (result != strlen(resposta)) {
        perror("maxim(write)\n");
    }

	
}
void minim(int newFd) {
	int result;
    char fi_trama;
    char resposta[10];

    result = read(newFd, &fi_trama, 1);
    if ((result == 1) && (fi_trama == '}')) {

        /* crear resposta */
        snprintf (resposta, 10, "{Y0%05.2f}", tmin);
       
    }
    else {
        fprintf(stderr, "minim(): fi trama incorrecte: '%c' (result=%d)\n", fi_trama, result);
        /* crear resposta fi trama incorrecte */
        strcpy (resposta, "{Y1}");
    }

    /* enviar resposta */
    result = write (newFd, resposta, strlen(resposta));
    if (result != strlen(resposta)) {
        perror("demanar_minim(write)\n");
    }

	
}
void resetmax(int newFd) {
	int result;
    char fi_trama;
    char resposta[5];

    result = read(newFd, &fi_trama, 1);
    if ((result == 1) && (fi_trama == '}')) {

        tmax     = 0.0; 	/* enviem valors al contrari */
        tmin     = 99.99;	
		num_mostres = 0.0; 
        /* crear resposta */
        strcpy (resposta, "{R0}");
    }
    else {
        fprintf(stderr, "resetmax(): fi trama incorrecte: '%c' (result=%d)\n", fi_trama, result);
        /* crear resposta fi trama incorrecte */
        strcpy (resposta, "{R1}");
    }

    /* enviar resposta */
    result = write (newFd, resposta, strlen(resposta));
    if (result != strlen(resposta)) {
        perror("resetmax(write)\n");
    }

	
}
void icomptador(int newFd) {
	int result;
    char fi_trama;
    char resposta[9];

    result = read(newFd, &fi_trama, 1);
    if ((result == 1) && (fi_trama == '}')) {

        /* crear resposta */
        snprintf (resposta, 9, "{B0%04d}", num_mostres);
    }
    else {
        fprintf(stderr, "icomptador(): fi trama incorrecte: '%c' (result=%d)\n", fi_trama, result);
        /* crear resposta fi trama incorrecte */
        strcpy (resposta, "{B1}");
    }

    /* enviar resposta */
    result = write (newFd, resposta, strlen(resposta));
    if (result != strlen(resposta)) {
        perror("icomptador(write)\n");
    }

	
}

