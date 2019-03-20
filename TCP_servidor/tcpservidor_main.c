

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
#include <arpa/inet.h>
#include <string.h>

#define SERVER_PORT_NUM		5001
#define SERVER_MAX_CONNECTIONS	4
#define REQUEST_MSG_SIZE	1024


/************************
*
*
* tcpServidor
*
*
*/

int main(int argc, char *argv[])
{
	struct sockaddr_in	serverAddr;
	struct sockaddr_in	clientAddr;
	int			sockAddrSize;
	int			sFd;
	int			newFd;
	int			nRead;
	int 		result;
	char		buffer[256];
	char		missatge[] = "#(2)(0,23.3)(10,23.3)";

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
		
		/*Enviar*/
		strcpy(buffer,missatge); //Copiar missatge a buffer
		result = write(newFd, buffer, strlen(buffer)+1); //+1 per enviar el 0 final de cadena
		printf("Missatge enviat a client(bytes %d): %s\n",	result, missatge);

		/*Tancar el socket fill*/
		result = close(newFd);
		}
	}
}



void marxastop(int newFd) {
	result = read(newFd, &comanda, 2);
	if (comanda==0){
	marxastop=0
	}
	
}
void mitjana(int newFd) {
	

	
}
void maxim(int newFd) {
	

	
}
void minim(int newFd) {
	

	
}
void resetmax(int newFd) {
	

	
}
void icomptador(int newFd) {
	

	
}

