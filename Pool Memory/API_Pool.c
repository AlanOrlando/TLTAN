/*
 * API_Pool.c
 *
 *  Created on: 13 may. 2019
 *      Author: utnso
 */

#include "API_Pool.h"

void trabajar_request(request nueva_request , int conexion){

	Dato dato_select;
	t_dato* dato_a_enviar;
	tabla_gossip_dto tabla_recibida;

	switch(nueva_request->cod_op){

		case SELECT:

			printf("\n>>SE REALIZARA EL SELECT\n");

			dato_select = request_select( (select_t) nueva_request->tipo_request);

			/*if(dato_select == NULL){

				bueno, basicamente pueden pasar dos cosas: no existe la key, o en el peor de los casos no existe la tabla.
				podemos simplemente ignorar cual sea el error, y simplemente mandarle un -1 al kernel y decirle que no se pudo realizar y hubo un fallo
				o
				podemos mandar diferentes numeros, uno por cada error. Entonces el kernel va a ser capaz de loggear cual fue realmente el problema
				//TODO VER LO DE ARRIBA

			}
			 else{ LO DE ABAJO!}
			*/
			dato_a_enviar = crear_t_dato(dato_select->key, dato_select->timestamp , dato_select->value);

			enviar_dato(dato_a_enviar, conexion);

			liberar_t_dato(dato_a_enviar);

			liberar_dato(dato_select);

			break;
		case INSERT:

			printf("\n>>SE REALIZARA EL INSERT");

			request_insert((insert) nueva_request->tipo_request);

			printf("se ingreso el insert piola \n");



			break;

		case CREATE:



			break;

		case DESCRIBE:



			break;

		case GOSSIP:

			intercambiar_datos( ((tabla_gossip_dto) nueva_request->tipo_request), conexion);

			break;

		default:

			break;
	}

}



Dato request_select(select_t dato){

	Segmento segmento_tabla;
	Pagina pagina_encontrada;
	Dato dato_encontrado;

	if(existe_segmento(dato->tabla->buffer, &segmento_tabla)){
		printf("Existe un segmento llamado: %s\n", (char*) dato->tabla->buffer);

		if(existe_pagina(segmento_tabla, dato->key, &pagina_encontrada)){

			printf("Existe pagina!\n");

			mostrar_datos(pagina_encontrada);

			dato_encontrado = decodificar_dato_de_memoria(pagina_encontrada->referencia_memoria);

			actualizar_uso(pagina_encontrada);

		}else{

			printf("Le pido las cosas al LFS \n");

			Dato dato_lfs = pedir_dato_al_LFS(dato->tabla->buffer, dato->key);

			//TODO hay que manejar errores al pedir al LFS, entonces si me llega un dato bien, hago lo de abajo, sino tengo que contemplar otra cosa
			//por ejemplo, podria hacer que pedir_dato me devuelva NULL, y adentro contemplar que el LFS me mande un -2 si no existe la KEY
			/*if(dato_lfs == NULL){
				dato_encontrado = NULL;
				//que el trabajar request se encargue de devolver algo al kernel si el dato es null
			}
			else{
				lo de abajo!
			*/
			pagina_encontrada = solicitar_pagina();

			agregar_pagina(segmento_tabla, pagina_encontrada);
			actualizar_pagina(pagina_encontrada, dato_lfs);

			liberar_dato(dato_lfs);

			dato_encontrado = decodificar_dato_de_memoria(pagina_encontrada->referencia_memoria); //agrego esto y no trabajo con dato_lfs para hacer siempre
																								  //lo mismo

			mostrar_dato(pagina_encontrada);

		}

	}else{
		printf("No existe el segmento, lo tenes que crear y pedirle el dato al LFS! \n");

		agregar_segmento(dato->tabla->buffer, memoria->tabla_segmentos);

		if(existe_segmento(dato->tabla->buffer, &segmento_tabla)){ //esto lo uso para encontrarlo nomas (el segmento) xd
			printf("Ahora si existe el segmento\n");

		}else{
			printf("Deberia existir el segmento, pero me dice que no. NANI?  \n");
			exit(1);
		}

		Dato dato_lfs = pedir_dato_al_LFS(dato->tabla->buffer, dato->key);

		//TODO mas de lo mismo que en el elseIf de arriba

		/*if(dato_lfs == NULL){

			dato_encontrado == NULL;

			PERO ACA SE AGREGA ALGO, TENGO QUE SABER SI LA TABLA EXISTE, si el file system me dice que no existe, deberia eliminarla, por que la cree
			sin que nadie me diga que existe

			sacar_segmento(segmento_encontrado);


		}else{
			lo de abajo!
        */

		pagina_encontrada = solicitar_pagina();

		agregar_pagina(segmento_tabla, pagina_encontrada);
		actualizar_pagina(pagina_encontrada, dato_lfs);

		liberar_dato(dato_lfs);

		dato_encontrado = decodificar_dato_de_memoria(pagina_encontrada->referencia_memoria); //agrego esto y no trabajo con dato_lfs para hacer siempre
																							 //lo mismo
		mostrar_dato(pagina_encontrada);


	}

	return dato_encontrado;
}

void request_insert(insert dato){

	Segmento segmento_tabla;
	Pagina pagina_encontrada;
	Dato dato_insert;

	if(string_length(dato->value->buffer) > tamanio_value){
		printf("SEGMENTATION FAULT! Te pasaste en el tamaño del value \n");
		return;
	}
	if(dato->timestamp < 0){
		time_t timestamp = time(NULL);

		if(timestamp < 0){
			//se pudre todooo, el timestamp no esta disponible
			exit(1);
		}

		dato->timestamp = timestamp;
	}

	dato_insert = crear_dato(dato->key, dato->value->buffer, dato->timestamp );

	if(existe_segmento(dato->tabla->buffer ,&segmento_tabla)){
		printf("Existe el segmento!\n");
		if(existe_pagina(segmento_tabla, dato->key, &pagina_encontrada)){

			printf("Existe la pagina!\n");
			actualizar_pagina(pagina_encontrada, dato_insert);
			mostrar_datos(pagina_encontrada);

		}else{
			printf("No existe la pagina!\n");
			pagina_encontrada = solicitar_pagina();
			actualizar_pagina(pagina_encontrada, dato_insert);
			agregar_pagina(segmento_tabla, pagina_encontrada);
			printf("LA TABLA A AHORA TIENE %i PAGINAS\n", segmento_tabla->Tabla_paginas->elements_count);
			mostrar_datos(pagina_encontrada);

		}


	}else{
		printf("No existe el segmento, deberia crearlo!\n");
	}

	liberar_dato(dato_insert);
}


void request_create(create dato_create){

	enviar_request(CREATE, dato_create);

	//seguir!

}
