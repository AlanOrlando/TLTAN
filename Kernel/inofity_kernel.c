/*
 * inofity_kernel.c
 *
 *  Created on: 28 jul. 2019
 *      Author: utnso
 */

#include"inotify_kernel.h"

void liberar_inotify(inotify_config a_liberar){

	close(a_liberar->fd_inotify);

	free(a_liberar->path_config);

	free(a_liberar);

}

inotify_config crear_inotify(void){

	inotify_config nuevo = malloc(sizeof(struct inotifyEstructura));

	nuevo->fd_inotify = inotify_init();  // OJO SI DEVUELVE -1

	nuevo->path_config = obtener_path_config();

	nuevo->fd_watch = inotify_add_watch(nuevo->fd_inotify, nuevo->path_config, IN_OPEN | IN_MODIFY | IN_CLOSE); // OJO SI DEVUELVE -1

	return nuevo;
}

void* realizar_inotify(inotify_config argumento){

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

		printf("[INOTIFY] Se creo el hilo\n");

		int  tamanio_leido;

		int tamanio_buffer = sizeof(struct inotify_event) + string_length("kernel.config")  + 1;

		char buffer[tamanio_buffer*4];
		char* buffer_aux;
		struct inotify_event* cambio_config;

		printf("[INOTIFY] VOY A EMPEZAR A MIRAR EL ARCHIVO");


		while(1){



			printf("\n[INOTIFY] voy a leer un nuevo evento\n");
			tamanio_leido = read(argumento->fd_inotify, buffer, tamanio_buffer);

			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

			//si read devuelve -1 y errno es EAGAIN, significa que no hay eventos, entonces si no es EAGAIN es error de lectura
			if(tamanio_leido == -1 && errno != EAGAIN){
				perror("read");
				pthread_exit(NULL);
			}

			if(tamanio_leido <= 0){
				printf("No hay eventos!\n");
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
				continue;
			}

			for(buffer_aux = buffer; buffer_aux < buffer + tamanio_leido;
				buffer_aux += sizeof(struct inotify_event) + cambio_config->len)
			{

				cambio_config = (struct inotify_event*)buffer_aux;

				if(cambio_config->mask & IN_OPEN){
					printf("[INOTIFY] Abrieron el archivo config\n");

				}else if(cambio_config->mask & IN_CLOSE_WRITE){ // mirar IN_MODIFY, diferencias
					printf("[INOTIFY] Se modifico el metadata.config\n");


					t_config* archivo_config = config_create(argumento->path_config);

					//SEMAFORO
					tiempo_refresh_metadata = config_get_int_value(archivo_config, "REFRESH_METADATA");

					//SEMAFORO
					tiempo_ejecucion = config_get_int_value(archivo_config, "RETARDO_CICLO_EJECUCION");

					//SEMAFORO
					quantum = config_get_int_value(archivo_config, "QUANTUM");

					config_destroy(archivo_config);


					printf("[INOTIFY] Se Cerro el metadata.config");

				}else if(cambio_config->mask & IN_CLOSE_NOWRITE){

					printf("[INOTIFY] Cerraron el archivo\n");

				}else{

					continue;
				}

			}

			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

		}

		return NULL;
}