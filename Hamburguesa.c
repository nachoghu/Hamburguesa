
#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <sys/time.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>

sem_t mutex_salero;
sem_t mutex_horno;
sem_t mutex_plancha;
sem_t mutex_ganador;

int hayGanador = 0;
int ganador = 0;

#define LIMITE 50

//creo estructura de semaforos 
struct semaforos {
    sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_armar_medallones;
	sem_t sem_pan_cocinado;
	sem_t sem_cocinar_hamburguesa;
	sem_t sem_armar_hamburguesa;

};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

//Por imprimir yo tomo imprimir en archivo y en consola
void* imprimirAccion(void *data, char *accionIn) {
	

	FILE* fichero;
	fichero = fopen("Resultado.txt", "a+");

	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){

			printf("\tEquipo %d - accion %s \n ", mydata->equipo_param, mydata->pasos_param[i].accion);
			fprintf(fichero, "\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
			//calculo la longitud del array de ingredientes
			int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
			//indice para recorrer array de ingredientes
			int h;
			if (i == 0 || i == 2) {
				printf("\tEquipo %d -----------ingredientes : ----------\n", mydata->equipo_param);
				fprintf(fichero, "\tEquipo %d -----------ingredientes : ----------\n", mydata->equipo_param);
			}
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
					printf("\tEquipo %d ingrediente  %d : %s \n", mydata->equipo_param, h, mydata->pasos_param[i].ingredientes[h]);
					fprintf(fichero, "\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
			if (i == 0 || i == 2) {
				printf("\tEquipo %d ------------------------------------\n", mydata->equipo_param);
				fprintf(fichero, "\tEquipo %d ------------------------------------\n", mydata->equipo_param);
			}
		}
	}
	fclose(fichero);
}

//Por imprimir yo tomo imprimir en archivo y en consola
void* imprimirSeparador() {
	FILE* fichero;
	fichero = fopen("Resultado.txt", "a+");
	fprintf(fichero, "=====================================================\n");
	fclose(fichero);
}

//Por imprimir yo tomo imprimir en archivo y en consola
void* imprimirGanador() {
	FILE* fichero;
	fichero = fopen("Resultado.txt", "a+");
	printf("\n             ¡¡¡Ganador Equipo %d!!!\n\n",ganador);
	fprintf(fichero, "\n             ¡¡¡Ganador Equipo %d!!!\n\n", ganador);
	fclose(fichero);
}

void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);

    pthread_exit(NULL);
}

void* cortarLechugaTomate(void* data) {
	//creo el nombre de la accion de la funcion 
	char* accion = "cortarLechugaTomate";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);

	pthread_exit(NULL);
}

void* mezclar(void* data) {
	//creo el nombre de la accion de la funcion 
	char* accion = "mezclar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	sem_wait(&mydata->semaforos_param.sem_mezclar);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);
	//doy la señal a la siguiente accion
	sem_post(&mydata->semaforos_param.sem_salar);

	pthread_exit(NULL);
}

void* salar(void* data) {

	sem_wait(&mutex_salero);

	//creo el nombre de la accion de la funcion 
	char* accion = "salar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	sem_wait(&mydata->semaforos_param.sem_salar);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);
	//doy la señal a la siguiente accion
	sem_post(&mydata->semaforos_param.sem_armar_medallones);
	
	sem_post(&mutex_salero);

	pthread_exit(NULL);

}

void* armarMedallones(void* data) {
	//creo el nombre de la accion de la funcion 
	char* accion = "armarMedallones";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armar_medallones);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);
	//doy la señal a la siguiente accion
	sem_post(&mydata->semaforos_param.sem_cocinar_hamburguesa);

	pthread_exit(NULL);
}

void* cocinarHamburguesa(void* data) {

	sem_wait(&mutex_plancha);

	//creo el nombre de la accion de la funcion 
	char* accion = "cocinarHamburguesa";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	sem_wait(&mydata->semaforos_param.sem_cocinar_hamburguesa);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(10000000);
	//doy la señal a la siguiente accion
	sem_post(&mydata->semaforos_param.sem_armar_hamburguesa);

	sem_post(&mutex_plancha);

	pthread_exit(NULL);
}

void* cocinarPan(void* data) {

	sem_wait(&mutex_horno);

	//creo el nombre de la accion de la funcion 
	char* accion = "cocinarPan";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(5000000);
	//doy la señal a la siguiente accion
	sem_post(&mydata->semaforos_param.sem_pan_cocinado);//no está 100% confirmado
	
	sem_post(&mutex_horno);

	pthread_exit(NULL);
}

void* armarHamburguesa(void* data) {
	//creo el nombre de la accion de la funcion 
	char* accion = "armarHamburguesa";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro* mydata = data;
	sem_wait(&mydata->semaforos_param.sem_pan_cocinado);
	sem_wait(&mydata->semaforos_param.sem_armar_hamburguesa);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);

	pthread_exit(NULL);
}


void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_armar_medallones;
	sem_t sem_pan_cocinado;
	sem_t sem_cocinar_hamburguesa;
	sem_t sem_armar_hamburguesa;

	
	//variables hilos
	pthread_t p1; 
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;
	
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_armar_medallones = sem_armar_medallones;
	pthread_data->semaforos_param.sem_pan_cocinado = sem_pan_cocinado;
	pthread_data->semaforos_param.sem_cocinar_hamburguesa = sem_cocinar_hamburguesa;
	pthread_data->semaforos_param.sem_armar_hamburguesa = sem_armar_hamburguesa;

	

	FILE* fp;
	char arrayLinea[8][50];
	int cont = 0;

	//abro el archivo en modo lectura
	fp = fopen("Receta.txt", "r");

	const size_t line_size = 50;
	char* line = malloc(line_size);
	while (fgets(line, line_size, fp) != NULL) {
		strcpy(arrayLinea[cont++], line);
	}
	free(line);

	for (int j = 0; j < 8; j++) {
		char* str = arrayLinea[j];
		char* array[5];
		char* ptop, * pend;
		char wk[1024];
		int i, size = sizeof(array) / sizeof(char*);

		strcpy(wk, str);
		strcat(wk, "|");
		for (i = 0, ptop = wk; i < size; ++i) {
			if (NULL != (pend = strstr(ptop, "|"))) {
				*pend = '\0';
				array[i] = strdup(ptop);
				ptop = pend + 1;
			}
			else {
				array[i] = NULL;
				break;
			}
		}

		//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
		if (strcmp(array[0], "cortar") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
			for (int k = 1; k < size-1;k++) {
				strcpy(pthread_data->pasos_param[j].ingredientes[k-1], array[k]);
			}
		}
		else if (strcmp(array[0], "cortarLechugaTomate") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
		}
		else if (strcmp(array[0], "mezclar") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
			for (int k = 1; k < size; k++) {
				strcpy(pthread_data->pasos_param[j].ingredientes[k-1], array[k]);
			}
		}
		else if (strcmp(array[0], "salar") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
		}
		else if (strcmp(array[0], "armarMedallones") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
		}
		else if (strcmp(array[0], "cocinarPan") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
		}
		else if (strcmp(array[0], "cocinarHamburguesa") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
		}
		else if (strcmp(array[0], "armarHamburguesa") == 0) {
			strcpy(pthread_data->pasos_param[j].accion, array[0]);
		}

	}
	fclose(fp);
	
	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_armar_medallones), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar_hamburguesa), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_pan_cocinado), 0, 0);


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p2,                           //identificador unico
							NULL,                          //atributos del thread
								cortarLechugaTomate,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p3,                           //identificador unico
							NULL,                          //atributos del thread
								mezclar,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p4,                           //identificador unico
							NULL,                          //atributos del thread
								salar,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p5,                           //identificador unico
							NULL,                          //atributos del thread
								armarMedallones,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p6,                           //identificador unico
							NULL,                          //atributos del thread
								cocinarPan,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p7,                           //identificador unico
							NULL,                          //atributos del thread
								cocinarHamburguesa,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p8,                           //identificador unico
							NULL,                          //atributos del thread
								armarHamburguesa,             //funcion a ejecutar
								pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui
	
	
	//join de todos los hilos
	pthread_join (p1, NULL);
	pthread_join (p2, NULL);
	pthread_join (p3, NULL);
	pthread_join (p4, NULL);
	pthread_join (p5, NULL);
	pthread_join (p6, NULL);
	pthread_join (p7, NULL);
	pthread_join (p8, NULL);


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

  
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_armar_medallones);
	sem_destroy(&sem_pan_cocinado);
	sem_destroy(&sem_cocinar_hamburguesa);
	sem_destroy(&sem_armar_hamburguesa);
	
	if (p == 1) {
		sem_wait(&mutex_ganador);
		if (hayGanador==0) {
			ganador = 1;
			hayGanador = 1;
		}
		sem_post(&mutex_ganador);

	}else if (p == 2) {
		sem_wait(&mutex_ganador);
		if (hayGanador==0) {
			ganador = 2;
			hayGanador = 1;
		}
		sem_post(&mutex_ganador);

	}else if (p == 3) {
		sem_wait(&mutex_ganador);
		if (hayGanador==0) {
			ganador = 3;
			hayGanador = 1;
		}
		sem_post(&mutex_ganador);

	}

	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{

	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3;

	sem_init(&mutex_salero, 0, 1); 
	sem_init(&mutex_horno, 0, 1);
	sem_init(&mutex_plancha, 0, 1);
	sem_init(&mutex_ganador, 0, 1);

	imprimirSeparador();

	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);

   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join(equipo1, NULL);
    pthread_join(equipo2, NULL);
    pthread_join(equipo3, NULL);

	imprimirGanador();

	//Destruyo los mutex
	sem_destroy(&mutex_salero);
	sem_destroy(&mutex_horno);
	sem_destroy(&mutex_plancha);
	sem_destroy(&mutex_ganador);

	pthread_exit(NULL);
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable