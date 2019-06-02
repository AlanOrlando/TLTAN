/*
 * random.h
 *
 *  Created on: 1 jun. 2019
 *      Author: utnso
 */

#ifndef MANEJODEMEMORIA_H_
#define MANEJODEMEMORIA_H_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<commons/collections/list.h>
#include<stdbool.h>
#include<time.h>

/////////////////////////////////////////////// ESTRUCTURAS //////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{

	u_int16_t key;
	char* value;      // controlar con el max_value
	time_t timestamp;

}dato_en_memoria;

typedef struct{

	dato_en_memoria* referencia_memoria;  //puntero a la "memoria_principal"
	bool flag_modificado;

}pagina;

typedef struct{

	char* nombre_tabla;
	t_list* Tabla_paginas; //en el campo "data" va a contener un puntero a una estructura "pagina"

}segmento;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// VARIABLES GLOBALES /////////////////////////////////////////////////////////////////////////////////
t_list* tabla_segmentos;  //en el campo "data" va a contener un puntero a una estructura "segmento"

void* memoria_principal;

int memoria_disponible;

int max_value;

int tamanio_memoria;

int max_paginas_memoria;

int tamanio_dato_memoria;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// DEFINICION DE FUNCIONES /////////////////////////////////////////////////////////////////////////////

void inicializar_memoria(void);
void ingresar_dato_a_memoria(dato_en_memoria* nuevo_dato, void** referencia_a_memoria);
void inicializar_tabla_segmentos(t_list* tabla_a_inicializar);
segmento* crear_segmento(char* nombre_tabla);
void liberar_segmento(segmento* segmento_a_liberar);
bool existe_segmento(char* tabla, segmento** segmento_encontrado);
void liberar_tabla_paginas(t_list* tabla_a_liberar);
bool existe_pagina(segmento* segmento_tabla, u_int16_t key, pagina** pagina_encontrado);
pagina* crear_pagina(dato_en_memoria* nuevo_dato);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif /* MANEJODEMEMORIA_H_ */