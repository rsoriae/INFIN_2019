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
#include <unistd.h> 
#include <string.h>


#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		500
#define SERVER_PORT_NUM		5001


/************************
*
*
* ImprimirMenu
*
*
*/
char missatge[100];
int marxa=0;
int result=0;
int temps=0;
int mostres=0;
char		buffer[10];
float		numero;
void ImprimirMenu(void)
{
	printf("\n\nMenu:\n\n");

	printf("1: Temperatura mitja\n");
	printf("2: Temperatura màxima\n");
	printf("3: Temperatura mínima\n");
	printf("4: Reset màxim i mínim\n");
	printf("5: Comptador\n");
	printf("m: Marxa o parada de l'adquisició\n");
	printf("s: Sortir\n");

}
void conectar(void)
{

	
struct sockaddr_in	serverAddr;


char	    serverName[] = "172.28.16.5"; //Adreça IP on esta el servidor
int			sockAddrSize;
int			sFd;
int 		result;

 
	/*Crear el socket*/
	sFd=socket(AF_INET,SOCK_STREAM,0);

	/*Construir l'adreça*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);


	/*Conexió*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	if (result < 0)
	{
		printf("Error en establir la connexió\n");
		exit(-1);
	}
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n",	inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
	
	/*Enviar*/
	strcpy(buffer,missatge); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, missatge);
	
	 

	/*Rebre*/
	memset(buffer,'\0',10);  
	result = read(sFd, buffer, 10);
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);	


	/*Tancar el socket*/
	close(sFd);
	
}
int main(int argc, char **argv)
{      
	                                                       
     /*Declaració de variables*/                                                               
	char input;
	
	/*Trucada al void per mostrar el menú en pantalla*/
	ImprimirMenu();                             
	input = getchar();
	/*Bucle per mantenir el programa en continu funcionament i segons la selecció del menú*/
	while (input != 's')
	{
		switch (input)
		{
			/*Case marxa*/
			case 'm':
				printf("Introdueix 1 marxa o 2 parada\n");
				valor2: scanf("%i",&marxa);
				
					if (marxa==1)
						{
						printf("Si us plau, introdueix el temps de mostreig, màxim 20 segons\n");
						valor1: scanf("%i",&temps);
						
						if (temps<0 || temps>20)
							{
							printf("Error, si us plau, introdueix un valor entre 0 i 20\n");
							goto valor1;
							}
						
						printf("Si us plau, introdueix el número de mostres, màxim 9 mostres\n");
						valor: scanf("%i",&mostres);
						if (mostres<0 || mostres>9)
							{
							printf("Error, si us plau, introdueix un valor entre 0 i 9\n");
							goto valor;
							}
						sprintf(missatge,"{M1%02i%i}",temps,mostres);
						printf("%s",missatge);	
						conectar(); 	
						}
					else if (marxa==2)
						{
										
						sprintf(missatge,"{M0}");	
						conectar(); 
						}
					else
						{
						printf("Valor equivocat, si us plau, introdueix 1 marxa o 2 parada\n");
						goto valor2;
						}
				ImprimirMenu();		                            
				break;
			case 0x0a: //Això és per enviar els 0x0a (line feed) que s'envia quan li donem al Enter
				break;
			default:
				printf("Opció incorrecta\n");	
				break;
				
			
			
			/*Case temepratura mitja*/
			case '1':
				printf("Heu seleccionat temperatura mitja\n");	
				sprintf(missatge,"{U}");
				conectar();
				
				numero = ((buffer[3]-'0')*10+(buffer[4]-'0')+(buffer[6]-'0')*0.1+(buffer[7]-'0')*0.01);
				printf("La temperatura mitja és de %.02fºC\n", numero); 
				ImprimirMenu();                      
				break;
				
				
			/*Case temperatura màxima*/
			case '2':
				printf("Heu seleccionat temperatura màxima\n");
				sprintf(missatge,"{X}");
				conectar();
				numero= ((buffer[3]-'0')*10+(buffer[4]-'0')+(buffer[6]-'0')*0.1+(buffer[7]-'0')*0.01);
				printf("La temperatura màxima és de %.02fºC\n", numero);  
				ImprimirMenu();                             
				break;
				
				
			/*Case temperatura mínima*/
			case '3':
				printf("Heu seleccionat temperatura mínima\n");	
				sprintf(missatge,"{Y}");
				conectar();
				numero = ((buffer[3]-'0')*10+(buffer[4]-'0')+(buffer[6]-'0')*0.1+(buffer[7]-'0')*0.01);
				printf("La temperatura mínima és de %.02fºC\n", numero);
				ImprimirMenu();                             
				break;
			
			
			input = getchar();
			

	}
	



	return 0;
}			}
			
