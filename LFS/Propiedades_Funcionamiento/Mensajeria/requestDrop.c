/*
 * requestDrop.c
 *
 *  Created on: 18 jul. 2019
 *      Author: utnso
 */

#include"requestDrop.h"



void* serializar_drop(request drop){

	Drop request_drop = (Drop) drop->tipo_request;

	int desplazamiento = 0;
	void* buffer = malloc(request_drop->bytes);

	memcpy(buffer + desplazamiento, drop->cod_op, sizeof(cod_operacion));
	desplazamiento += sizeof(cod_operacion);

	memcpy(buffer + desplazamiento, &(request_drop->tabla->size), sizeof(request_drop->tabla->size));
	desplazamiento += sizeof(request_drop->tabla->size);

	memcpy(buffer + desplazamiento, request_drop->tabla->buffer, request_drop->tabla->size);
	desplazamiento += request_drop->tabla->size;

	return buffer;

}

Drop crear_drop(char* tabla){

	Drop nuevo_drop = malloc(sizeof(struct dropEstructura));

	nuevo_drop->tabla = malloc(sizeof(t_stream));

	nuevo_drop->tabla->size = string_length(tabla) + 1;

	nuevo_drop->tabla->buffer = malloc(nuevo_drop->tabla->size);

	memcpy(nuevo_drop, tabla, nuevo_drop->tabla->size);

	nuevo_drop->bytes = sizeof(cod_operacion) + sizeof(int) + nuevo_drop->tabla->size;

	return nuevo_drop;

}

void liberar_drop(Drop drop_a_liberar){

	free(drop_a_liberar->tabla->buffer);

	free(drop_a_liberar->tabla);

	free(drop_a_liberar);

}
