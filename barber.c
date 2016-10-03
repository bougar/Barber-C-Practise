#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "cola.h"
#include <time.h>
#include <gtk/gtk.h>

clockid_t reloj;//comento
struct timespec tiempo;
GtkWidget * window= NULL;
int num_chairs = 10;
int num_barbers = 2;
int num_customers = 20;
struct timeval inicio;//Guardamos la hora incia 
int contador=0;//Controla num_customers
int atendidos=0;//Almacena cuantos clientes les ha sido cortado el pelo
cola co;//Cola donde se van introduciendo los clientes en orden de llegada
int max_time = 100000;//tiempo máximo en nanosegundos
pthread_mutex_t mutex_atendidos = PTHREAD_MUTEX_INITIALIZER;//Mutex que controla la variable de clientes atendidos
pthread_mutex_t mutex_num_customers = PTHREAD_MUTEX_INITIALIZER;//Mutex para la variable de num_customers
pthread_mutex_t mutex_cola = PTHREAD_MUTEX_INITIALIZER;
static struct option long_options[] = {
	{ .name = "chairs",
	  .has_arg = required_argument,
	  .flag = NULL,
	  .val = 0},
	{ .name = "barbers",
	  .has_arg = required_argument,
	  .flag = NULL,
	  .val = 0},
	{ .name = "customers",
	  .has_arg = required_argument,
	  .flag = NULL,
	  .val = 0},
	{ .name = "max_time",
	  .has_arg = required_argument,
	  .flag = NULL,
	  .val = 0},
	{ .name = NULL,
	  .has_arg = 0,
	  .flag = NULL,
	  .val = 0}
};

static void usage(int i)
{
	printf(
		"Usage:  producers [OPTION] [DIR]\n"
		"Launch producers and consumers\n"
		"Opciones:\n"
		"  -b n, --barbers=<n>: number of barbers\n"
		"  -c n, --chairs=<n>: number of chairs\n"
		"  -n n, --customers=<n>: number of customers\n"
		"  -t n, --max_time=<n>: maximum time a customer will wait\n"
		"  -h, --help: show this help\n\n"
	);
	exit(i);
}

static int get_int(char *arg, int *value)
{
	char *end;
	*value = strtol(arg, &end, 10);

	return (end != NULL);
}

static void handle_long_options(struct option option, char *arg)
{
	if (!strcmp(option.name, "help"))
		usage(0);

	if (!strcmp(option.name, "barbers")) {
		if (!get_int(arg, &num_barbers)
		    || num_barbers <= 0) {
			printf("'%s': not a valid integer\n", arg);
			usage(-3);
		}
	}
	if (!strcmp(option.name, "chairs")) {
		if (!get_int(arg, &num_chairs)
		    || num_chairs <= 0) {
			printf("'%s': not a valid integer\n", arg);
			usage(-3);
		}
	}
	if (!strcmp(option.name, "customers")) {
		if (!get_int(arg, &num_customers)
		    || num_customers <= 0) {
			printf("'%s': not a valid integer\n", arg);
			usage(-3);
		}
	}
	if (!strcmp(option.name, "max_time")) {
		if (!get_int(arg, &max_time)
		    || max_time <= 0) {
			printf("'%s': not a valid integer\n", arg);
			usage(-3);
		}
	}
}

static int handle_options(int argc, char **argv)
{
	while (1) {
		int c;
		int option_index = 0;

		c = getopt_long (argc, argv, "hb:c:n:t:",
				 long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			handle_long_options(long_options[option_index],
				optarg);
			break;

		case 'b':
			if (!get_int(optarg, &num_barbers)
			    || num_barbers <= 0) {
				printf("'%s': not a valid integer\n",
				       optarg);
				usage(-3);
			}
			break;

		case 'c':
			if (!get_int(optarg, &num_chairs)
			    || num_chairs <= 0) {
				printf("'%s': not a valid integer\n",
				       optarg);
				usage(-3);
			}
			break;

		case 'n':
			if (!get_int(optarg, &num_customers)
			    || num_customers <= 0) {
				printf("'%s': not a valid integer\n",
				       optarg);
				usage(-3);
			}
			break;

		case 't':
			if (!get_int(optarg, &max_time)
			    || max_time <= 0) {
				printf("'%s': not a valid integer\n",
				       optarg);
				usage(-3);
			}
			break;

		case '?':
		case 'h':
			usage(0);
			break;

		default:
			printf ("?? getopt returned character code 0%o ??\n", c);
			usage(-1);
		}
	}
	return 0;
}

pthread_cond_t no_customers = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;




int free_barbers;

struct barber_info {    /* Used as argument to thread_start() */
	pthread_t thread_id;        /* ID returned by pthread_create() */
	int       barber_num;
};

struct customer_info {    /* Used as argument to thread_start() */
	pthread_t thread_id;        /* ID returned by pthread_create() */
	int 	barber_num;//Añadimos aquí el barbero que le cortara el pelo
	int       customer_num;
	int 	ido;
	pthread_cond_t condicion;
};

unsigned int seed = 12345678;

void cut_hair(int barber, int customer)
{
	struct timeval aux;
	gettimeofday(&aux,NULL);
	unsigned int timef = rand_r(&seed) % 500000;
	pthread_mutex_lock(&mutex_num_customers);
	contador++;//Cada vez que procesamos un consumidor vamos restando esta variable para poder detener los threads cuando lleguemos a cero
	pthread_mutex_unlock(&mutex_num_customers);
	// time that takes to do the haircut
	usleep(timef);
	
	printf("barber %d cut hair of customer %d took %d time at %ld.%ld seconds\n", barber, customer, timef,(long) aux.tv_sec-inicio.tv_sec,(long) aux.tv_usec);
}
void controlar_barbero(){//Procedimiento que comprueba si un barbero(thread) debe terminar su ejecución
	pthread_mutex_lock(&mutex_num_customers);
	if (contador==num_customers){//Si se cumple la condición el barbero termina
		pthread_cond_broadcast(&no_customers);
		pthread_mutex_unlock(&mutex_num_customers);
		pthread_mutex_unlock(&mutex);
		pthread_exit(0);
	}
	pthread_mutex_unlock(&mutex_num_customers);
}
void get_hair_cut(int barber, int customer);
void *barber_function(void *ptr)
{
	struct barber_info *t =  ptr;
	struct customer_info * aux;
	printf("barber thread %d\n", t->barber_num);

	while(1) {
		pthread_mutex_lock(&mutex);
		if (cola_vacia(co)) {
			free_barbers++;
			printf("barber %d goes to sleep\n", t->barber_num);
			pthread_cond_wait(&no_customers, &mutex);
			controlar_barbero();
		}
		controlar_barbero();
		pthread_mutex_lock(&mutex_cola);
		aux=eliminar(&co);
		aux->ido=1;
		pthread_mutex_unlock(&mutex_cola);
		aux->barber_num=t->barber_num;
		pthread_cond_signal(&aux->condicion);
		pthread_mutex_unlock(&mutex);
		cut_hair(t->barber_num, (aux)->customer_num);
		controlar_barbero();
	}
}

void get_hair_cut(int barber, int customer)
{
	pthread_mutex_lock(&mutex_atendidos);
	atendidos++;
	pthread_mutex_unlock(&mutex_atendidos);
	printf("customer %d got hair cut from barber %d\n", customer, barber);
}

void *customer_function(void *ptr)
{
	struct timespec tim;
	struct customer_info * t =  ptr;
	unsigned int timef = rand_r(&seed) % 100000 + t->customer_num * 100000;
	// time that takes until it arrives to the barber shop
	usleep(timef);

	printf("consumer thread %d arrives in %d\n", t->customer_num, timef);
	if (num_barbers ==0){
		printf("¿Me estás mandando a mí al gran 'consumidor %d' a una barbería sin barbero? A fregar, me marcho.\n",t->customer_num);
		pthread_exit(0);
	}
	pthread_mutex_lock(&mutex);
	if(cola_llena(co))  {
		printf("waiting room full for customer %d\n", t->customer_num);
		pthread_mutex_lock(&mutex_num_customers);
		contador++;//Cada vez que procesamos un consumidor vamos restando esta variable para poder detener los threads cuando lleguemos a cero
		pthread_mutex_unlock(&mutex_num_customers);
		pthread_mutex_unlock(&mutex);
		return NULL;//El cliente no entra en la cola si esta esta llena(se marcha).
	} else {
		insertar((void *) t,&co);
		if (free_barbers>0){
			free_barbers--;
			pthread_cond_signal(&no_customers);//Despertamos a los barberos si alguno estuviera durmiendo.
		}
		srand(time(NULL));
		clock_gettime(reloj,&tim);
		tim.tv_nsec += (rand() % max_time)*1000;
		while (tim.tv_nsec >= 1000000000){
			tim.tv_nsec-=1000000000;
			tim.tv_sec++;
		}
		if (ETIMEDOUT==(pthread_cond_timedwait(&t->condicion,&mutex,&tim))){
			pthread_mutex_trylock(&mutex);
			if(t->ido){
					pthread_mutex_unlock(&mutex);
					get_hair_cut(t->barber_num,t->customer_num);	
					return NULL;
			}
			printf("El consumidor %d , sale de la cola.\n",t->customer_num);
			eliminar_dato(&co,(void *) t);//Elimina la posición con el dato t, es decir, el cliente que le toque irse.
			pthread_mutex_lock(&mutex_num_customers);
			contador++;//Necesitamos incrementarla si un cliente se marcha de la cola.
			pthread_mutex_unlock(&mutex_num_customers);
			pthread_mutex_unlock(&mutex);
			pthread_exit(0);//El cliente se marcha pasado el tiempo.
		}
	}
	pthread_mutex_unlock(&mutex);
	get_hair_cut(t->barber_num,t->customer_num);	
	return NULL;
}

void create_threads(void)
{
	int i;
	struct barber_info *barber_info;
	struct customer_info *customer_info;

	printf("creating the barber\n");
	barber_info = malloc(sizeof(struct barber_info) * num_barbers);

	if (barber_info == NULL) {
		printf("Not enough memory\n");
		exit(1);
	}

	free_barbers = 0;
	crear_cola(&co,num_chairs);
	/* Create independent threads each of which will execute function */
	for (i = 0; i < num_barbers; i++) {
		barber_info[i].barber_num = i;//Inicializar un mutex por consumidor.
		if ( 0 != pthread_create(&barber_info[i].thread_id, NULL,
					 barber_function, &barber_info[i])) {
			printf("Failing creating barber thread %d", i);
			exit(1);
		}
	}

	printf("creating %d consumers\n", num_customers);
	customer_info = malloc(sizeof(struct customer_info) * num_customers);

	if (customer_info == NULL) {
		printf("Not enough memory\n");
		exit(1);
	}
	for (i = 0; i < num_customers; i++) {
		customer_info[i].customer_num = i;
		customer_info[i].ido= 0;
		pthread_cond_init(&(customer_info[i].condicion),NULL);
		if ( 0 != pthread_create(&customer_info[i].thread_id, NULL,
					 customer_function, &customer_info[i])) {
			printf("Failing creating customer thread %d", i);
			exit(1);
		}
	}
	for (i = 0; i < num_barbers; i++) 
		pthread_join(barber_info[i].thread_id,NULL);
	
	for (i = 0; i < num_customers; i++)
		pthread_join(customer_info[i].thread_id,NULL);
	
	free(customer_info);
	free(barber_info);
	printf("Clientes atendidos por los barberos: %d\n",atendidos);
	printf("Clientes que no han podido ser atendidos: %d\n",num_customers-atendidos);
}
void bye(){
	gtk_main_quit();
	printf("Bye Bye guy\n");
	exit(0);
}
void my_fun(GtkWidget * button, gpointer data){
	GtkEntry ** text=data;
	num_barbers=atoi(gtk_entry_get_text(text[0]));
	num_customers=atoi(gtk_entry_get_text(text[1]));
	num_chairs=atoi(gtk_entry_get_text(text[2]));
	contador = 0;
	atendidos = 0;
	gtk_widget_hide(window);
	gtk_main_quit();
}
void grafica(){
	/*Se inicializan todas la interfaces graficas
	sin interesarse por las funciones que van a 
	realizar*/
	static GtkWidget * text[3];
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * button;
	GtkWidget * box;
	if (window == NULL){
		/*Creación de la ventana*/
		window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
		gtk_widget_set_size_request(window,200,100);
		g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(bye),NULL);
	
		/*Se ira creando un label con su correspondiente texto,
		Una caja de texto para introducir un parametro y finalmente
		se introducirá en en una fila de una cuadrícula.*/
	
	
		grid = gtk_grid_new();
		label = gtk_label_new("Barberos: ");
		text[0] = gtk_entry_new();
		gtk_entry_set_width_chars (GTK_ENTRY(text[0]),4);
		gtk_grid_attach(GTK_GRID(grid),label,0,0,1,1);
		gtk_grid_attach(GTK_GRID(grid),text[0],2,0,1,1);
	
		label = gtk_label_new("Personas: ");
		text[1] = gtk_entry_new();
		gtk_entry_set_width_chars (GTK_ENTRY(text[1]),4);
		gtk_grid_attach(GTK_GRID(grid),label,0,1,1,1);
		gtk_grid_attach(GTK_GRID(grid),text[1],2,1,1,1);
	
		label = gtk_label_new("Sillas: ");
		text[2] = gtk_entry_new();
		gtk_entry_set_width_chars (GTK_ENTRY(text[2]),4);
		gtk_grid_attach(GTK_GRID(grid),label,0,2,1,1);
		gtk_grid_attach(GTK_GRID(grid),text[2],2,2,1,1);
	
		//Creación de botón y lo metemos en la cuadrícula al final
		box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
		button = gtk_button_new_with_label("¡Ejecutar!");
		gtk_grid_attach(GTK_GRID(grid),button,0,3,2,2);
		gtk_grid_remove_column(GTK_GRID(grid),3);
		gtk_box_pack_start(GTK_BOX(box),grid,TRUE,FALSE,0);
	
		//gtk_grid_attach_next_to(GTK_GRID(grid),text,label,GTK_POS_RIGHT,1,1);
		gtk_container_add(GTK_CONTAINER(window),box);
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(my_fun),text);
		gtk_widget_show_all(window);
		gtk_main();
	}else{
		gtk_widget_show_all(window);
		gtk_main();
	}
}
int main (int argc, char **argv)
{
	int result = handle_options(argc, argv);
	gtk_init(&argc,&argv);
	if (result != 0)
		exit(result);

	if (argc - optind != 0) {
		printf ("Extra arguments\n\n");
		while (optind < argc)
			printf ("'%s' ", argv[optind++]);
		printf ("\n");
		usage(-2);
	}
	while(1){
		grafica();
		clock_gettime(reloj,&tiempo);
		gettimeofday(&inicio,NULL);
		create_threads();
		printf ("\nSiguiente ejecucion: \n");
	}

	exit (0);
}
