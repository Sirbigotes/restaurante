//Se define todas las funciones
void inicio(int s);
void menu_regreso(WINDOW *menuw);
void menu();
void ver_reservacion();
void hacer_reservacion();
bool reservarAsiento(int mesa, int asiento);
void guardarReservacion(int mesa, int asiento, int clave);
void ticket(int r, int cantidad);
void mostrar_menu(char *msg, char *menu);
void mostrar_ticket(char *msg);
void dar_cantidad(int r);
void mostrar_propina();
void mostra_pagar();