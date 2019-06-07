/*
 * file_system.h
 *
 *  Created on: 4 jun. 2019
 *      Author: utnso
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#include"comunity_func.h"

typedef struct {
	u_int16_t key;
	char* value;
	time_t timestamp;
}bloque_t;

typedef enum{
	LIBRE,
	OCUPADO
}estado;

int existe_la_tabla(char* tabla);
void crear_directorio(char* path_tabla);
void ingresar_A_Un_binario(char* nombre_tabla, dato_t* dato_ingresar, int particion);
void crear_Binario(char* nombre_tabla , int key , char* valor, time_t timestamp);
void crear_archivos_particiones(char* path_tabla, int numero_particiones);
dato_t* buscar_dato_en_binario(char* path_tabla, int key);
void estadoDelBloque(int numeroBloque);
void mostrarEstadosTotales();
void setearEstado(int numeroBloque, estado estado);

#endif /* FILE_SYSTEM_H_ */