/*
 * request.h
 *
 *  Created on: 6 jun. 2019
 *      Author: utnso
 */

#ifndef MENSAJERIA_REQUEST_H_
#define MENSAJERIA_REQUEST_H_

#include<stdio.h>
#include<stdlib.h>

typedef enum{
	SELECT,
	INSERT,
	CREATE
}cod_operacion;

struct requestEstructura;
typedef struct requestEstructura* request;

#endif /* MENSAJERIA_REQUEST_H_ */