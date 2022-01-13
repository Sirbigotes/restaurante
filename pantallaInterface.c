//Construcción de las funciones

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include "pantallaInterface.h"

#define NOPCIONES 6 //Opciones en el menu principal


WINDOW *menuw, *titlew, *vreservaciow, *hreservacion,*mostrarw,*ticketw,*cantidadw, *pagarw, *finw; //Ventanas a usar

char list[NOPCIONES][20] = {
		"Ver reservación",
		"Hacer reservación",
		"Menu de carnes",
		"Menu vegetariano",
		"Menu de mariscos",
		"Ver ticket"
}; //Opciones principales

char platillos[500][500];
int precio[100];

struct Platillo{
	char nombre[500];
	int precio;
	int cantidad;	
}p[100];

int no_plato = 0,costo_total = 0;
int sck; //socket

int ALTO = 26, ANCHO = 75, M = 1000,N = 9999;
int mesas = 10;
int asientos = 5;
int clave = 0;
int restaurante[10][5];

void inicio(int s){
	sck = s;
	initscr();
	start_color();
	init_pair(9, COLOR_BLACK, COLOR_BLACK);
	init_pair(6, COLOR_BLACK, COLOR_YELLOW);
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(14, COLOR_YELLOW, COLOR_BLACK);
	init_pair(16, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(13, COLOR_RED, COLOR_BLACK);
	refresh();
	menu();
	send(sck, "2", 2, 0); //Enviar un 2 al servidor, en caso de ser borrado, el servidor explota unu
	endwin();
}

void menu_regreso(WINDOW *menuw){
	wattrset(titlew, COLOR_PAIR(6));
	box( titlew, ACS_BULLET, ACS_BULLET);
	mvwprintw(titlew,1,1,"  Restaurante: MENUS   ");
	wattroff(titlew, COLOR_PAIR(6));
	wrefresh(titlew);

	int i;
	char item[20];

	box(menuw, ACS_VLINE, ACS_HLINE);

	for(i = 0; i < NOPCIONES; i++){
		sprintf(item, "%s", list[i]);
		mvwprintw(menuw, i+1, 2, "%s", item);
	}

	mvwprintw(menuw, 11,2, "<Enter> para seleccionar");
	mvwprintw(menuw, 12,2, "Presiona 's' para salir");
	wrefresh(menuw);

}

void menu(){
	char item[20];
	int ch, i = 0, width = 7;

	initscr();
	menuw = newwin(15,30,4,1);
	box(menuw, ACS_VLINE, ACS_HLINE);

	titlew = newwin(3,25,1,3);
	wattrset(titlew, COLOR_PAIR(6));
	box(titlew, ACS_BULLET,ACS_BULLET);
	mvwprintw(titlew, 1,1, "  Restaurante: MENUS   ");
	wattroff(titlew,COLOR_PAIR(6));
	wrefresh(titlew);

	for(i = 0; i < NOPCIONES; i++){
		if(i == 0)
			wattron(menuw, A_STANDOUT);
		else
		wattroff(menuw, A_STANDOUT);
	sprintf(item,"%s",list[i]);
	mvwprintw(menuw, i+1, 2, "%s",item);
	}

	mvwprintw(menuw, 11,2, "<Enter> para seleccionar");
	mvwprintw(menuw, 12,2, "Presiona 's' para salir");
	wrefresh(menuw);

	i = 0;
	noecho();
	keypad(menuw, TRUE);

	curs_set(0);

	while((ch = wgetch(menuw)) != 115){
		sprintf(item, "%s", list[i]);
		mvwprintw(menuw, i+1, 2, "%s", item);
		switch(ch){
			case KEY_UP:
				i--;
				i = (i < 0) ? (NOPCIONES - 1): i;
			break;
			case KEY_DOWN:
				i++;
				i = (i > (NOPCIONES - 1)) ? 0: i;
			break;
			case 10:
				switch(i){
					case 0:
						ver_reservacion();
						clear();
						refresh();
						menu_regreso(menuw);
					break;
					case 1:
						hacer_reservacion();
						clear();
						refresh();
						menu_regreso(menuw);
					break;
					case 2:
						mostrar_menu(NULL,"menu_carnes.txt");
						clear();
						refresh();
						menu_regreso(menuw);
					break;
					case 3:
						mostrar_menu(NULL,"menu_vegetariano.txt");
						clear();
						refresh();
						menu_regreso(menuw);
					break;
					case 4:
						mostrar_menu(NULL,"menu_mariscos.txt");
						clear();
						refresh();
						menu_regreso(menuw);
					break;
					case 5:
						mostrar_ticket(NULL);
						clear();
						refresh();
						menu_regreso(menuw);
					break;
				}
			break;
		}
		wattron(menuw, A_STANDOUT);
		sprintf(item, "%s", list[i]);
		mvwprintw(menuw, i+1, 2, "%s",item);
		wattroff(menuw, A_STANDOUT);
	}
	delwin(menuw);
	endwin();
}

void ver_reservacion(){
	nodelay(stdscr, FALSE);
	char ch;
	char b[500];
	int contrasena;
	FILE *archivo; 
	vreservaciow = newwin(20,60,0,0);
	box(vreservaciow, ACS_VLINE, ACS_HLINE);
	wrefresh(vreservaciow);

	wattrset(vreservaciow, COLOR_PAIR(14));
	mvwprintw(vreservaciow, 2, 20, "## RESERVACION ##");
	mvwprintw(vreservaciow, 4, 10, "Ingresa tu clave : [");
	mvwprintw(vreservaciow, 4, 20 + strlen("Ingresa tu clave : ["),"]");
	mvwprintw(vreservaciow, 18, 40, "<Enter> seleccionar");
	wattroff(vreservaciow, COLOR_PAIR(14));
	
	keypad(vreservaciow, TRUE);
	int n1,n2,n3;
	while((ch = wgetch(vreservaciow)) != 115){
		echo();
		wattrset(vreservaciow, COLOR_PAIR(16));
		mvwscanw(vreservaciow, 4, 33, "%d",&contrasena);
		wattroff(vreservaciow, COLOR_PAIR(16));
		noecho();
		if(ch == 10){
			archivo = fopen("datos.txt","r");
			if(archivo == NULL){
				mvwprintw(vreservaciow, 10, 10, "ERROR");
				wrefresh(vreservaciow);
				ch = getch();
				return;
			}
			
			while(feof(archivo) == 0){
				fscanf(archivo, "%d %d %d", &n1, &n2,&n3);
				if(contrasena == n1){
						mvwprintw(vreservaciow,10,10,"Clave: %d\n\tMesa reservada: %d\n\tAsiento reservado: %d\n",n1,n2,n3);
						wrefresh(vreservaciow);
						break;
				}
			}
			if(contrasena!=n1){
				mvwprintw(vreservaciow,10,20,"\nNo hay coincidencias");
				wrefresh(vreservaciow);
			}

			fclose(archivo);
			wrefresh(vreservaciow);
		}
	}

	nodelay(stdscr, TRUE);
}

void hacer_reservacion(){
	nodelay(stdscr, FALSE);
	char ch;
	int mesa, asiento;
	FILE *archivo; 
	vreservaciow = newwin(20,60,0,0);
	box(vreservaciow, ACS_VLINE, ACS_HLINE);
	wrefresh(vreservaciow);

	wattrset(vreservaciow, COLOR_PAIR(14));
	mvwprintw(vreservaciow, 2, 20, "## RESERVACION ##");
	mvwprintw(vreservaciow, 4, 10, "Mesa : [");
	mvwprintw(vreservaciow, 4, 15 + strlen("Mesa : ["),"]");
	mvwprintw(vreservaciow, 6, 10, "Asiento : [");
	mvwprintw(vreservaciow, 6, 15 + strlen("Asiento : ["),"]");
	mvwprintw(vreservaciow, 18, 40, "<Enter> seleccionar");
	wattroff(vreservaciow, COLOR_PAIR(14));

	do{
		echo();
		wattrset(cantidadw, COLOR_PAIR(16));
		mvwscanw(vreservaciow,4,20,"%d",&mesa);
		mvwscanw(vreservaciow,6,23,"%d",&asiento);
		wattroff(vreservaciow, COLOR_PAIR(16));
		noecho();
	}while(!reservarAsiento(mesa, asiento));

	srand(time(NULL));
	clave = rand() % (N - M +1);
	mvwprintw(vreservaciow,10,10,"\nEl asiento fue reservado, esta es tu clave: %d\n", clave);
	guardarReservacion(mesa ,asiento, clave);
	wrefresh(vreservaciow);
	ch = wgetch(vreservaciow);
	nodelay(stdscr, TRUE);
}

void mostrar_menu(char *msg,char *menu){
	nodelay(stdscr, FALSE);
	int ch;
	char item[500];
	FILE *fp;

	mostrarw = newwin(ALTO, ANCHO, 0, 50);
	box(mostrarw, ACS_VLINE,ACS_HLINE);
	wrefresh(mostrarw);

	wattrset(mostrarw, COLOR_PAIR(14));
	mvwprintw(mostrarw, 2, 23, "## ENTRADAS ##");
	mvwprintw(mostrarw, 2, 50, "$$ PRECIO $$");
	mvwprintw(mostrarw, 22, 3, "s para volver");
	mvwprintw(mostrarw, 22, 50, "<Enter> seleccionar");
	wattroff(mostrarw, COLOR_PAIR(14));

	wattrset(mostrarw, COLOR_PAIR(13));
	mvwprintw(mostrarw, 8, 20, "## PLATOS FUERTES ##");
	wattroff(mostrarw, COLOR_PAIR(13));

	wattrset(mostrarw, COLOR_PAIR(16));
	mvwprintw(mostrarw, 15, 20, "## POSTRES ##");
	wattroff(mostrarw, COLOR_PAIR(16));
	

	if(msg != NULL){
		wattrset(mostrarw, COLOR_PAIR(1));
		mvwprintw(mostrarw, 17, 19, "## %s ##", msg);
		wattroff(mostrarw, COLOR_PAIR(1));
	}
	fp = fopen(menu,"r");
	if(fp == NULL){
		mvwprintw(mostrarw, 10, 10, "ERROR");
		wrefresh(mostrarw);
		ch = getch();
		return;
	}

	int r = 0, i = 10, j = 4;
	while((fscanf(fp,"%[^\t] %d",platillos[r], &precio[r])) != EOF && r != 10){
		fgetc(fp);
		if( r == 3){
			j = 10;
		}
		if( r == 7){
			j = 17;
		}
		if(j == 4){
			wattron(mostrarw, A_STANDOUT);
		}else
			wattroff(mostrarw, A_STANDOUT);
		mvwprintw(mostrarw, j, i, "%s", platillos[r]);
		mvwprintw(mostrarw, j, 55, "%d", precio[r]);
		j++;
		r++;
		wrefresh(mostrarw);
	}
	r = 0;
	j = 4;
	noecho();
	keypad(mostrarw, TRUE);

	curs_set(0);
	while((ch = wgetch(mostrarw)) != 115){
		mvwprintw(mostrarw,j, i, "%s",platillos[r]);
		mvwprintw(mostrarw, j, 55, "%d", precio[r]);
		switch(ch){
			case KEY_UP:
				j--;
				r--;
				switch(j){
					case 3:
						j = 19;
						r = 9;
					break;
					case 9:
						j = 6;
						r = 2;
					break;
					case 16:
						j = 13;
						r = 6;
					break;
				}
			break;
			case KEY_DOWN:
				j++;
				r++;
				switch(j){
					case 7:
						j = 10;
						r = 3;
					break;
					case 14:
						j = 17;
						r = 7;
					break;
					case 20:
						j = 4;
						r = 0;
					break;
				}
			break;
			case 10:
				dar_cantidad(r);
			break;
		}
		wattron(mostrarw, A_STANDOUT);
		mvwprintw(mostrarw,j,i,"%s",platillos[r]);
		mvwprintw(mostrarw, j, 55, "%d", precio[r]);
		wattroff(mostrarw, A_STANDOUT);

	}
	nodelay(stdscr, TRUE);
}

void ticket(int r, int cantidad){
	if(cantidad != 0){
		int i = 0, j = 0;
		for(i = 0; i < no_plato; i++){
			if((strcmp(p[i].nombre,platillos[r])) == 0){
				p[i].cantidad += cantidad;
				costo_total += (cantidad * precio[r]);
				j++;
				break; 
			}
		}
		if(no_plato == 0 || j == 0){
			strcpy(p[no_plato].nombre,platillos[r]);
			p[no_plato].precio = precio[r];
			p[no_plato].cantidad = cantidad;
			costo_total = costo_total + (cantidad * precio[r]);
			no_plato++;
		}
	}
}

void dar_cantidad(int r){
	char ch;

	cantidadw = newwin(20,50,0,0);
	box(cantidadw, ACS_VLINE, ACS_HLINE);
	wrefresh(cantidadw);

	mvwprintw(cantidadw, 5, 3, "Dame la cantidad de platillos : [");
	mvwprintw(cantidadw, 5, 11 + strlen("Dame la cantidad de platillos : ["),"]");
	wattrset(cantidadw, COLOR_PAIR(14));
	mvwprintw(cantidadw, 14, 25, "<Enter> para guardar");
	mvwprintw(cantidadw, 14, 2, "s para salir");
	wattroff(cantidadw, COLOR_PAIR(14));
	wrefresh(cantidadw);

	nodelay(stdscr, FALSE);
	int cantidad, i = 0;
	char can[4];

	while((ch = wgetch(cantidadw)) != 115){
		
		if(ch >= 48 && ch <= 57){
			can[i] = ch;
		}
		if(ch == 10){
			can[i] = '\0';
			cantidad = atoi(can);
			ticket(r,cantidad);
			break;
		}
		i++;
		wattrset(cantidadw, COLOR_PAIR(16));
		mvwprintw(cantidadw, 5, 6 + strlen("Dame la cantidad de platillos : ["), "%s", can);
		wattroff(cantidadw, COLOR_PAIR(16));
		mvwprintw(cantidadw, 5, 11 + strlen("Dame la cantidad de platillos : ["),"]");
		wrefresh(cantidadw);
	}
	nodelay(stdscr, TRUE);
	wclear(cantidadw);
	wrefresh(cantidadw);
	menu_regreso(menuw);
}

void mostrar_ticket(char *msg){
	nodelay(stdscr, FALSE);
	char ch;
	int i;
	wclear(menuw);

	ticketw = newwin(45,78,0,0);
	box(ticketw, ACS_VLINE, ACS_HLINE);
	wrefresh(ticketw);

	wattrset(ticketw, COLOR_PAIR(14));
	mvwprintw(ticketw, 2, 30, "## TICKET ##");
	mvwprintw(ticketw, 42, 2, "< s > para salir");
	mvwprintw(ticketw, 42, 60, "< p > para pagar");
	wattroff(ticketw, COLOR_PAIR(14));
	
	mvwprintw(ticketw, 4, 5, "Cantidad");
	mvwprintw(ticketw, 4, 30, "Platillo");
	mvwprintw(ticketw, 4, 60, "Precio");

	int c = 6;
	for(i = 0; i <= no_plato; i++){
		if(i != no_plato){
			mvwprintw(ticketw,c,10,"%d",p[i].cantidad);
			mvwprintw(ticketw,c,25,"%s",p[i].nombre);
			mvwprintw(ticketw,c,60,"$%d",p[i].precio);
			c++;
		}else{
			mvwprintw(ticketw,c + 4,30,"TOTAL: $%d",costo_total);
		}
	}
	wrefresh(ticketw);

	while((ch = wgetch(ticketw)) != 115){
		if(ch == 112){
			if(costo_total != 0){
				mostrar_propina();
			}
		}	
	}
	nodelay(stdscr, TRUE);
}

void mostrar_propina(){
	char ch;

	pagarw = newwin(20, 50, 0, 90);
	box(pagarw, ACS_VLINE, ACS_HLINE);
	wrefresh(pagarw);

	mvwprintw(pagarw, 2, 3, "Si no desea dar propina escribir 0");
	mvwprintw(pagarw, 5, 3, "Propina : [");
	mvwprintw(pagarw, 5, 11 + strlen("Propina : ["),"]");

	wattrset(pagarw, COLOR_PAIR(14));
	mvwprintw(pagarw, 14, 25, "<Enter> para guardar");
	mvwprintw(pagarw, 14, 2, "s para salir");
	wattroff(pagarw, COLOR_PAIR(14));
	wrefresh(pagarw);

	nodelay(stdscr, FALSE);
	char lee_propina[4];

	int i = 0;

	while((ch = wgetch(pagarw)) != 115){
		if(ch >= 48 && ch <= 57){
			lee_propina[i] = ch;
		}

		if(ch == 10){
			lee_propina[i] = '\0';
			costo_total = costo_total + atoi(lee_propina);
			mostra_pagar();
		}
		i++;
		wattrset(pagarw, COLOR_PAIR(16));
		mvwprintw(pagarw, 5, 6 + strlen("Propina : ["), "%s", lee_propina);
		wattroff(pagarw, COLOR_PAIR(16));
		mvwprintw(pagarw, 5, 11 + strlen("Propina : ["),"]");
		wrefresh(pagarw);
	}
	nodelay(stdscr, TRUE);
	wclear(pagarw);
	wrefresh(pagarw);
}

void mostra_pagar(){
	char ch;
	wclear(pagarw);
	wrefresh(pagarw);
	finw = newwin(45, 78, 0, 0);
	box(finw, ACS_VLINE, ACS_HLINE);
	wrefresh(finw);

	mvwprintw(finw, 5, 3, "Dinero ingresado : [");
	mvwprintw(finw, 5, 11 + strlen("Dinero ingresado : ["),"]");

	wattrset(finw, COLOR_PAIR(14));
	mvwprintw(finw, 2, 3, "TOTAL A PAGAR: $%d",costo_total);
	mvwprintw(finw, 10, 25, "<Enter> para guardar");
	wattroff(finw, COLOR_PAIR(14));
	wrefresh(finw);

	int cambio;
	do{
		echo();
		wattrset(finw, COLOR_PAIR(16));
		mvwscanw(finw, 5, 25, "%d",&cambio);
		wattroff(finw, COLOR_PAIR(16));
	}while(cambio < costo_total);
	noecho();
	cambio = cambio - costo_total;
	
	mvwprintw(finw, 20, 25, "Cambio: $%d", cambio);
	
	mvwprintw(finw, 30, 20, "¡GRACIAS POR SU VISITA!");
	mvwprintw(finw, 32, 15, "Presiona cualquier tecla para salir");
	ch = wgetch(finw);

	wclear(finw);
	wclear(ticketw);
	wclear(menuw);

	wrefresh(ticketw);
	wrefresh(menuw);
	wrefresh(finw);

	delwin(menuw);
	delwin(ticketw);
	delwin(pagarw);
	delwin(finw);

	endwin();
	exit(0);
}
bool reservarAsiento(int mesa,int asiento){
	//Archivo prueba 1
	FILE *archivo = fopen("datos.txt","r");
	if(archivo == NULL){
		perror("Error en la apertura del archivo");
		return 1;
	}
	int n1,n2,n3;
	
	while(feof(archivo) == 0){
		fscanf(archivo, "%d%d%d", &n1, &n2,&n3);
		if(mesa==n2 && asiento==n3){		
			return false;
			
		}
	}
	
	fclose(archivo);

	if(restaurante[mesa-1][asiento-1]==0){
		restaurante[mesa-1][asiento-1] = 1;
		return true; //Si se pudo reservar	
	}
	return false; //No se pudo reservar
}

void guardarReservacion(int mesa, int asiento, int clave){
		
	FILE *archivo = fopen("datos.txt","a");
	if(archivo == NULL){
		perror("Error en la creacion del archivo\n\n");
	}else{
		fprintf(archivo, "%d %d %d\n",clave, mesa, asiento);		
		fflush(archivo);
		fclose(archivo);
	}
}