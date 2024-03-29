/*
 * Lissandra.h
 *
 *  Created on: 9 abr. 2019
 *      Author: utnso
 */

#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stddef.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<time.h>
#include<commons/config.h>
#include<commons/collections/node.h>
#include<commons/collections/list.h>
#include<string.h>
#include<sys/stat.h>
#include"metadata.h"

#include"comunity_func.h"

pthread_t hilo_dump;


t_dictionary* memtable;
pthread_rwlock_t lock_memtable;
void eliminar_dato_t(dato_t* dato);
void liberar_memtable(void);
void inicializar_dump(void);
void inicializar_memtable(void);
void ingresar_a_memtable(dato_t* dato_a_ingresar, char* nombre_tabla);
t_list* obtener_tabla(char* nombre_tabla);
dato_t* obtener_dato_con_mayor_timestamp_tabla(char* nombre_tabla, u_int16_t key);

#endif /* LISSANDRA_H_ */
