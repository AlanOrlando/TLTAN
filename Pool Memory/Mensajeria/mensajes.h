/*
 * mensajes.h
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>
#include<readline/readline.h>
#include<readline/history.h>

#include"configuracion.h"

#include"requestDrop.h"
#include"requestDescribe.h"
#include"requestCreate.h"
#include"requestInsert.h"
#include"requestSelect.h"
#include"requestGossiping.h"
#include"Cliente/cliente.h"
#include"t_dato.h"
#include"commons/collections/list.h"

void enviar_metadata(t_list* lista_metadata, int conexion, estado_request estado);
void enviar_estado(int conexion, estado_request estado);
bool enviar_request(request request, int conexion);
void liberar_request(request dato);
void mandar_mensaje(int conexion);
void enviar_dato(t_dato* dato, int conexion, estado_request estado);


#endif /* MENSAJES_H_ */
