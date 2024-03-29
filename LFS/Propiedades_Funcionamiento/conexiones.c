/*
 * conexiones.c
 *
 *  Created on: 11 jun. 2019
 *      Author: utnso
 */


#include"conexiones.h"

void liberar_conexiones(void){

	list_destroy_and_destroy_elements(memorias_conectadas, destruir_conexion_memoria);

}

void* conectar_varias_memorias(){

	//Creo que si usamos hilos tendriamos mucho overhead, debido a los context switchs

	fd_set memoriasfds;

	struct sockaddr_in client_address;

	fd_set descriptor_lectura;

	int new_fd;

	int address_len;

	int fdmax;

	int nbytes;

	int socket_servidor = iniciar_servidor(ip_lfs , puerto_lfs);
	FD_ZERO(&memoriasfds);

	FD_SET(socket_servidor , &memoriasfds);

	fdmax = socket_servidor;

	while(0){

		descriptor_lectura = memoriasfds;
		//En el 5to parametro, va NULL asi se queda esperando hasta que pase algo
		if(select(fdmax + 1 , &descriptor_lectura , NULL, NULL, NULL) == -1){
			perror("error del select.");
			exit(1);
		}

		for(int i = 0 ; i<=fdmax ; i++){

			if(FD_ISSET(i, &descriptor_lectura)){
				if(i == socket_servidor){

					address_len = sizeof(client_address);

					if((new_fd = aceptar_conexion(&client_address)) == -1){

						perror("fallo el accept del select.");

					}else{

						FD_SET(new_fd , &memoriasfds);
						if(new_fd > fdmax){
							fdmax = new_fd;
						}

						ptrinf("select server: new connection from %s on socket %d\n" , inet_ntoa(client_address.sin_addr) , new_fd);

					}
				}else{

					//En estos casos vamos a hacer que no se puedan desconectar las memorias.

					request nueva_request = recibir_request(i);

					responder_request(nueva_request, i);

					liberar_request(nueva_request);

					}
				}
			}
		}
	return NULL;
}


void inicializar_memorias_conectadas(){

	memorias_conectadas = list_create();

}

void* administrar_conexiones_hilos(int* socket_servidor){

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	while(1){

		int nueva_conexion = aceptar_conexion( *socket_servidor);

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

		printf("[CONEXION] Se conecto un nuevo cliente\n");

		Conexion_memoria nueva_conexion_memoria = crear_conexion_memoria(  nueva_conexion );

		list_add(memorias_conectadas , nueva_conexion_memoria);

		realizar_handshake(nueva_conexion_memoria->socket_memoria);

		printf("[CONEXION] Cliente agregado correctamente\n\n");

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	}

	return NULL;
}


Conexion_memoria crear_conexion_memoria( int conexion ){

	int error_pthread;
	Conexion_memoria memoria_nueva = malloc(sizeof(struct estructuraConexionMemoria));

	memoria_nueva->socket_memoria = conexion;

	pthread_attr_t attr;

	error_pthread = pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);       //NO me fijo el error, ya fue

	if(error_pthread != 0){
		perror("pthread_attr_init");
		close(memoria_nueva->socket_memoria);
		free(memoria_nueva);
		pthread_exit(NULL);
	}

	error_pthread = pthread_create(&(memoria_nueva->hilo_memoria) , &attr , manejar_requests , memoria_nueva);

	if(error_pthread != 0){
		perror("pthread_create");
		close(memoria_nueva->socket_memoria);
		free(memoria_nueva);
		pthread_exit(NULL);
	}

	pthread_attr_destroy(&attr);

	return memoria_nueva;
}


void* manejar_requests(Conexion_memoria memoria_conectada){

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	request request_recibida;

	while(1){

		request_recibida = recibir_request(memoria_conectada->socket_memoria);

		if(request_recibida->cod_op == DESCONEXION){

			printf("VOY A DESCONECTAR AL CLIENTE!\n");

			liberar_request(request_recibida);

			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

			desconectar_memoria(memoria_conectada);

		}else{

			printf("\n//////////////////////////////////////NUEVA REQUEST////////////////////////////\n\n");

			trabajar_request(request_recibida, memoria_conectada->socket_memoria); //nos faltaria esta funcion

		}

		liberar_request(request_recibida);

		printf("\n/////////////////////////////////////FIN REQUEST//////////////////////////////////\n\n");

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

		usleep(retardo * 1000);

	}

	return NULL;
}

void desconectar_memoria(Conexion_memoria memoria_a_desconectar){


	bool _tenes_esta_conexion( void* elemento_memoria){

		Conexion_memoria conexion_a_eliminar = (Conexion_memoria) elemento_memoria;

		return conexion_a_eliminar->socket_memoria == memoria_a_desconectar->socket_memoria;

	}


	list_remove_and_destroy_by_condition(memorias_conectadas, _tenes_esta_conexion, destruir_conexion_memoria);


}



void destruir_conexion_memoria(Conexion_memoria memoria_a_destruir){

	pthread_cancel(memoria_a_destruir->hilo_memoria);

	close(memoria_a_destruir->socket_memoria);

	free(memoria_a_destruir);

}
