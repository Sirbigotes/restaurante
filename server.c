#include <unistd.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h> 
#include <string.h> 
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#define BACKLOG 5 //Numero máximo de clientes

void enviar_mensaje(char *msj); //enviar mensajes al cliente

int connfd; // socket de conexión al cliente

int main(){
	int sockfd; // socket del servidor 
	char msg[] = "¡BIENVENIDO!\n\nEn caso de tener una reserva ingresar con el nombre de usuario de dicha reserva\n\n"; //Mensaje a enviar al cliente
	char buffer[1024]; // ayudará a enviar mensajes

	struct sockaddr_in servaddr, cliente; // auxiliares a la contrucción del socket
	socklen_t len; //longitud de la conexión del cliente


	sockfd = socket(AF_INET, SOCK_STREAM, 0); //Se crea el socket del servidor

	if(sockfd == -1){
		printf("[SERVER - error]: creacion del socket fallida\n");
		return -1;
	}else{
		printf("[SERVER]: Socket creado con exito\n");
	}

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //Direccion del servidor
	servaddr.sin_port = htons(41762); // PUERTO

	if((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0){ //Creación del bind: asignar la direccion especifica del socket del servidor
		printf("[SERVER - error]: socket bind fallo\n");
		return -1;
	}else{
		printf("[SERVER]: Socket binded exitoso\n");
	}

	if((listen(sockfd, BACKLOG)) != 0){ // El servidor esta a la espera de clientes
		printf("[SERVER - error]: socket listen fallo\n");
		return -1;
	}else{
		printf("[SEVER]: Escuchando\n");
	}

	len = sizeof(cliente); //Tamaño del cliente

	while(1){
		connfd = accept(sockfd, (struct sockaddr *)&cliente, &len); //Conexión de cliente al servidor, se busca la aceptación de este

		if(connfd < 0){
			printf("[SERVER - error]: conexión no aceptada\n");
			return -1;

		}else{
			if( fork() == 0 ){
		     // hijo
			    close(sockfd);
			    printf("[SERVER]: cliente conectado.\n");

			    int val = 0;
				do{
					if(val == 0){
						enviar_mensaje(msg); //Mensaje de bienvenida
						recv(connfd,buffer,2,0); //Recibe el mensaje del cliente ("1"), cuando mandamos un 1 en el cliente
						val = atoi(buffer); // Cast del mensaje
					}else if(val == 1){
						printf("[SERVER] : Cliente valido.\n"); //El captcha en cliente fue exitoso
						recv(connfd,buffer,2,0); //Recibimos un 2, cliente envio un "2". Es decir, ya salio del sistema
						val = atoi(buffer);
					}							
				}while(val != 2);
				printf("[SERVER]: cliente salio.\n"); //El cliente salio, pago su comida o simplemente salio del menu principal
				close(connfd); //Cerramos al cliente
				exit(0); 
			}else{
				close(connfd);
			}
		}

	}
}

void enviar_mensaje(char *msj){
	send(connfd, msj, strlen(msj), 0); //Enviar mensaje al cliente
	memset(msj, 0, 255); //Limpiamos cadena
}
