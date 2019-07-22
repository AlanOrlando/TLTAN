
#include"compactador.h"

void* compactar(thread_args* argumentos){


	log_trace(logger_compactador, "INICIO DE LA -- COMPACTACION -- DE LA TABLA %s\n", argumentos->nombre_tabla);
	//los comentarios entre parentesis son para ver donde se libera cada variable;

	char* nombre_tabla = argumentos->nombre_tabla;


	pthread_rwlock_wrlock(&(argumentos->lock_tabla));

	transformar_tmp_a_tmpc(nombre_tabla);

	pthread_rwlock_unlock(&(argumentos->lock_tabla));

/*	t_list* datos_particiones;*/                                                             // (2) Lista con los datos de los .bin antes de la compactacion

	t_list* datos_tmpc;																				// (3) Lista con los datos de los .tmpc

	t_list* datos_finales;																		// (4)  Lista con los datos de los .bin DESPUES de la particion

	pthread_rwlock_rdlock((&(argumentos->lock_tabla)));

	printf("vamos con la metadata\n");

	metadata_t* metadata_tabla = obtener_metadata(nombre_tabla);

	printf("voy a obtener los datos particiones\n");

	datos_finales = obtener_datos_particiones(nombre_tabla);

	printf("voy a obtener los datos temporales\n");

	datos_tmpc = obtener_datos_temporales(nombre_tabla);

	printf("Compactacion %s paso 1\n", nombre_tabla);

	void _generar_lista_datos_finales(void* _dato_tmpc){

		char* dato_tmpc = (char *)_dato_tmpc;

		//t_list* dato_con_key_particular = filtrar_dato_por_key(dato_particiones, datos_tmpc);   // (1) Lista filtrada

		int index = buscar_index_misma_key( dato_tmpc, datos_finales  );

		if(index >= 0){

			char* dato_timestamp_mayor = comparar_y_obtener_dato_actualizado( (char*) list_get(datos_finales, index) , dato_tmpc);

			list_replace_and_destroy_element(datos_finales , index  , dato_timestamp_mayor , free);

		}

		else{

			char* aux_dato_tmpc = string_duplicate(dato_tmpc);

			list_add(datos_finales , aux_dato_tmpc);

		}
                                              // (1) Libero lista filtrada SIN liberar los elementos

	}

	list_iterate(datos_tmpc, _generar_lista_datos_finales);

	//ACA EMPIEZA EL PROCESO DE SACAR Y PONER
	//MUTEX?
	//HAY QUE BLOQUEAR PARA QUE NADIE OPERE SOBRE ESTA TABLA! SI NO SE PUDRE TODOO

	//LIBERO LAS PARTICIONES, DEBERIA QUEDARME NUEVOS .bin CON SIZE=0 y 1 BLOQUE

	pthread_rwlock_unlock(&(argumentos->lock_tabla));

	pthread_rwlock_wrlock(&(argumentos->lock_tabla));

	printf("Compactacion %s paso 2\n", nombre_tabla);

	for(int i = 0; i < metadata_tabla->particion; i++){

		char* path_particion = obtenerPath_ParticionTabla(nombre_tabla, i);

		printf("PATH PARTICION: %s\n", path_particion);

		liberar_bloques_particion(path_particion);

		free(path_particion);
	}

	printf("Compactacion %s paso 3\n", nombre_tabla);

	void _funcion_loca2(void* _dato_final){

		char* dato_final = (char *)_dato_final;

		u_int16_t key = obtener_key_dato(dato_final);

		int particion_perteneciente = calcular_particion(metadata_tabla->particion, key );

		char* path_particion = obtenerPath_ParticionTabla(nombre_tabla, particion_perteneciente);  // (5)

		dato_t* dato_a_cargar = convertir_a_dato(dato_final);                                     // (6)

		cargar_a_particion(path_particion, dato_a_cargar);

		free(path_particion);                                                                    // (5)

		liberar_dato(dato_a_cargar);															 // (6)



	}

	//time_t inicio_de_bloqueo = time(NULL);
	list_iterate(datos_finales, _funcion_loca2);
	//time_t fin_de_bloqueo = time(NULL)
	//Sacar la diferencia entre estos dos para saber cuanto tiempo estuvo bloqueadoa la tabla

	printf("Compactacion %s paso 4\n", nombre_tabla);
	liberar_tmpc(nombre_tabla);

	pthread_rwlock_unlock(&(argumentos->lock_tabla));


	//Por aca habria que liberar el "mutex"
	//time_t fin_de_bloqueo = time(NULL)
	//Sacar la diferencia entre estos dos para saber cuanto tiempo estuvo bloqueadoa la tabla

//	list_destroy_and_destroy_elements(datos_particiones, free);										//  (2) Libero los datos del .bin ANTES de la compactacion

	printf("Compactacion %s paso 5\n", nombre_tabla);

	list_destroy_and_destroy_elements(datos_tmpc, free);											//  (3) Libero los datos del .tmpc

	list_destroy_and_destroy_elements(datos_finales, free);										// (4) Libero los datos del .bin DESPUES de la compactacion

	//TODO liberar_metadata(metadata_tabla); resuelto abajo por ahora, seguramente cambie la estructura metadata!
	free(metadata_tabla);

	log_trace(logger_compactador, "FINALIZACION DE LA -- COMPACTACION -- DE LA TABLA %s\n", argumentos->nombre_tabla);

	return NULL;

}

/////////////////////////////////FUNCIONES COMPACTADOR HILO/////////////////////////////////////77


void inicializar_compactadores(void){

	DIR *dir;
	struct dirent* ent;
	metadata_t* metadata;

	char* path_raiz = obtenerPathDirectorio_Tablas();

	if((dir = opendir(path_raiz)) != NULL){

		while((ent = readdir(dir)) != NULL){

			if(!string_equals_ignore_case(ent->d_name, ".") && !string_equals_ignore_case(ent->d_name, "..")){

				//printf("Cree un hilo de compactacion para la tabla: %s\n", ent->d_name);
				metadata = obtener_metadata(ent->d_name);

				correr_compactacion(metadata->compactacion, ent->d_name);

				thread_args* argumento_tabla = dictionary_get(diccionario_compactador, ent->d_name);

				pthread_rwlock_unlock(&argumento_tabla->lock_tabla);
				pthread_rwlock_unlock(&lock_diccionario_compactacion);

				free(metadata);
			}
		}
	}

	closedir(dir);



}

thread_args* crear_argumentos_tabla( int tiempo_compactacion , char* nombre_tabla ){

	thread_args* argumentos_hilo = malloc(sizeof(thread_args));

	argumentos_hilo->tiempo_compactacion = tiempo_compactacion * 1000; //recibo en milisegundos y lo paso a microsegundos

	argumentos_hilo->nombre_tabla = malloc(string_length(nombre_tabla ) + 1);

	pthread_rwlock_init(&(argumentos_hilo->lock_tabla), NULL);

	memcpy(argumentos_hilo->nombre_tabla , nombre_tabla , (strlen(nombre_tabla) + 1));

	return argumentos_hilo;
}

//ESTA FUNCION DEVUELVE THREAD ARGS, HACE LOCK DEL SEMAFORO DE LA TABLA CREADA Y DEL DICCIONARIO DE TABLAS EN ESCRITURA, EL QUE LA LLAMA TIENE QUE DESBLOQUEARLO
void correr_compactacion(int tiempo_compactacion , char* nombre_tabla){

	int error_pthr;

	thread_args* argumentos_hilos = crear_argumentos_tabla(tiempo_compactacion , nombre_tabla);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


	pthread_rwlock_wrlock(&(argumentos_hilos->lock_tabla));

	error_pthr = pthread_create(&(argumentos_hilos->hilo_compactacion) , &attr , ciclo_compactacion , argumentos_hilos);

	if(error_pthr != 0){
		perror("pthread create");
	}

	pthread_attr_destroy(&attr);

	pthread_rwlock_wrlock(&(lock_diccionario_compactacion));

	dictionary_put(diccionario_compactador , nombre_tabla , argumentos_hilos);

}

void* ciclo_compactacion(thread_args* argumentos){

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(1){

		usleep(argumentos->tiempo_compactacion);

		pthread_rwlock_rdlock(&(lock_diccionario_compactacion));

		compactar(argumentos);

		pthread_rwlock_unlock(&(lock_diccionario_compactacion));
	}

	return NULL;
}

//ESTA FUNCION ASUME QUE YA TOMASTE SEMAFORO DE LA TABLA, LA COMPACTACION DEBERIA ESTAR ESPERANDO ESE SEMAFORO
void abortar_hilo_compactador(char* nombre_tabla){

	void _abortar_hilo(void* thread_arg){

		thread_args* argumentos = (thread_args *) thread_arg;

		pthread_cancel(argumentos->hilo_compactacion);

		pthread_rwlock_destroy(&(argumentos->lock_tabla));

		free(argumentos->nombre_tabla);
		free(argumentos);

	}


	dictionary_remove_and_destroy(diccionario_compactador, nombre_tabla, _abortar_hilo );

}

////////////////////////////////////////////////////////////////////////////////////////777

u_int16_t obtener_key_dato(char* dato){

	char** dato_separado = string_split(dato, ";");

	u_int16_t key = (u_int16_t )atoi(dato_separado[1]);

	liberar_puntero_doble(dato_separado);

	return key;

}

time_t obtener_timestamp_dato(char* dato){

	char** dato_separado = string_split(dato, ";");

	time_t timestamp =  (time_t)atoi(dato_separado[0]);

	liberar_puntero_doble(dato_separado);

	return timestamp;
}


t_list* filtrar_dato_por_key(char* dato_particiones, t_list* datos_tmpc){

	u_int16_t key = obtener_key_dato(dato_particiones);

	t_list* datos_filtrados = list_create();

	bool _filtrar_por_key(void* _dato){

		char* dato = (char *)_dato;

		u_int16_t key_analizada = obtener_key_dato(dato);

		return key == key_analizada;

	}

	datos_filtrados = list_filter(datos_tmpc, _filtrar_por_key);

	return datos_filtrados;

}

//No pasarle null :)
char* comparar_y_obtener_dato_actualizado(char* dato_a_ser_comparado, char* dato_a_analizar){

	char* dato_actualizado = string_duplicate(dato_a_ser_comparado);

	time_t timestamp_mayor = obtener_timestamp_dato(dato_a_ser_comparado);

	time_t timestamp_menor= obtener_timestamp_dato(dato_a_analizar);

	if(timestamp_menor >= timestamp_mayor){

		free(dato_actualizado);

		dato_actualizado = string_duplicate(dato_a_analizar);
		timestamp_mayor = timestamp_menor;

	}

	return dato_actualizado;

}


int buscar_index_misma_key(char*  dato_tmpc, t_list*  datos_finales  ){

	for( int i = 0 ; i < list_size(datos_finales) 	; i++){

		if( obtener_key_dato(dato_tmpc) == obtener_key_dato( (char*)list_get(datos_finales, i) ) ){

			return i;

		}

	}

	return -1;

}
