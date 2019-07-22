/*
 * config.h
 *
 *  Created on: 30 abr. 2019
 *      Author: utnso
 */

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>


t_config* g_config;

char* ip_memoria;
int puerto_memoria;
int quantum;
int grado_multiprocesamiento;
int tiempo_refresh_metadata;
int tiempo_gossiping_kernel;
int tiempo_ejecucion;


void creacion_del_config(void);
void obtener_datos_config(void);

#endif /* CONFIGURACION_H_ */
