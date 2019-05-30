/*
 * API_Pool.h
 *
 *  Created on: 13 may. 2019
 *      Author: utnso
 */

#ifndef API_POOL_H_
#define API_POOL_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<time.h>
#include<commons/config.h>
#include<string.h>
#include<sys/stat.h>

#include"Config/configuracion.h"
#include"Mensajeria/recibir.h"
#include"Mensajeria/mensajes.h"

int socket_sv;
int conexion_lissandra;

typedef struct{


	int key;
	char* value; //hay que controlar el tamanio maximo!
	time_t timestamp;

}dato_t;


typedef enum{
	FALSE, TRUE
} boolean;



typedef struct{

	int numero_pagina;
	boolean flag_modificado;
	dato_t* dato_en_Memoria; // tiene que estar dentro de memoria
	struct pagina* siguiente_pagina;

}pagina;

typedef struct{

	char* tabla; //ESTO DESPUES TIENE QUE SER EL PATH
	pagina* primera_pagina;
	struct segmento* siguiente_segmento;

}segmento;

int tamanio_memoria;
dato_t* memoria_principal;

segmento* tabla_segmentos;

void realizar_select(char* nombre_tabla, int key);
void insert(char* nombre_tabla, int key, char* value);
dato_t* buscar_key(int key, segmento* segmento_tabla);
dato_t* pedir_key_a_LFS(int key, char* nombre_tabla);
dato_t* convertir_a_dato_t(t_dato_recibido* dato_recibido);
void crear_pagina(segmento* tabla, dato_t* nuevos_datos);
pagina* ultima_pagina(segmento* tabla);
segmento* ultimo_segmento_tabla();
void crear_segmento(char* tabla);
void agregar_segmento_tabla(segmento* segmento);


#endif /* API_POOL_H_ */
