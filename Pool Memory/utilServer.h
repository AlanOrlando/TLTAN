/*
 * utilServer.h
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#ifndef UTILSERVER_H_
#define UTILSERVER_H_

	#include<stdio.h>
	#include<stdlib.h>
	#include<sys/socket.h>
	#include<unistd.h>
	#include<netdb.h>
	#include<commons/log.h>
	#include<commons/collections/list.h>
	#include<string.h>

	typedef enum
	{
		MENSAJE,
		PAQUETE
	}op_code;


	typedef struct
	{
		int size;
		void* stream;
	} t_buffer;

	typedef struct
	{
		op_code codigo_operacion;
		t_buffer* buffer;
	} t_paquete;

	#define IP "127.0.0.1"
	#define PUERTO "4444"

	void* recibir_buffer(int*, int);

	int iniciar_servidor(void);
	int esperar_cliente(int);
	t_list* recibir_paquete(int);
	void recibir_mensaje(int);
	int recibir_operacion(int);



#endif /* UTILSERVER_H_ */