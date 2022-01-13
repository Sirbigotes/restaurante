
//librerias usadas
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>

#include "pantallaInterface.h" //libreria de la interface

bool val_captcha(); //función del captcha

int sockfd; //socket  crear

int main(){
	char usuario[300]; //leer el usuario
	char buf[255]; //leer mensajes del servidor

	
	struct sockaddr_in servaddr; //servidor

	sockfd = socket(AF_INET, SOCK_STREAM, 0); //se crea el socket
	if(sockfd == -1){
		printf("[CLIENTE] : creacio de socket fallida...\n"); //ERROR AL CREAR EL SOCKET 
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr)); // llenar con 0

	servaddr.sin_family = AF_INET; // cualquiera puede conectarse
	servaddr.sin_port = htons(41762); //PUERTO

	inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr)); //Direccion de memoria

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) != 0){ 
		printf("Conexion con el servidor fallida...\n"); //FALLA DE CONEXION
		return -1;
	}

	int num, val = 0; //NUM: ayudará a cerrar el arreglo buf
					  //VAL: ayudará a controla salidas y entradas, tanto del cliente al servidor 
	while(1){
		if(val == 0){ //Mensajes al iniciar el programa
			num = recv(sockfd, buf, 511, 0); //Recibe los mensajes del servidor
			buf[num] = '\0'; // CERRAR EL ARREGLO, caso contrario imprimirá basura
			printf("%s\n",buf); //Imprimir los mensajes recibidos
			memset(buf, 0, 512); // Limpiar el arreglo
			
			printf("\nNombre de usuario: "); 
			scanf("%s",usuario);
			printf("\n\n");
			//Pide el usuario

			do{}while(!val_captcha()); //Enviar y recibir el captcha

			send(sockfd, "1", 2, 0); //Cuando el captcha sea correcto, enviará un 1 al servidor
			val = 1; // cambiamos la acción a realizar en el cliente

		}else if(val == 1){
			inicio(sockfd); //INICIA la interfaz, código se encuentra en: pantallaInterface.c
			send(sockfd, "2", 2, 0); //Envia al servidor un 2, significa cierre del cliente
			exit(0); //Salimos del programa
		}
	}	
	close(sockfd); //Cierra el socket del cliente
}

bool val_captcha(){
    srand(time(NULL)); //Cambie el captcha
    int num = 9; //num: cantidad de letras que tendra el captcha.
    char *letras = "abcdefghijklmnopqrstuvwxyzABCDEFGHI"
                  "JKLMNOPQRSTUVWXYZ0123456789";
    char captcha[num],captcha_usuario[num]; 
    while (num--){
        captcha[num] = letras[rand() % 62];   //Agregar un caracter aleatoria a la captcha    
    }
    captcha[num+10] = '\0'; //Agregar final de la oración, caso contrario la cadena tendra basura


    printf("Captcha: %s\n",captcha);
    printf("\nEscribe el captcha: ");
    scanf("%s",captcha_usuario);

    //Validar captcha.
    if(strcmp(captcha_usuario,captcha) == 0){
        return true;
    }else{
    	printf("Captcha incorrecto\n\n");
    }
    return false;  
}