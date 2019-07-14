/*
 * mensajes.c
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */

#include"mensajes.h"

//tipoRequest es una estructura del tipo "request" osea, puede ser selectEstructura, etc, entonces mi idea es depende de lo que llegue, castear a esa estructura
void enviar_request(cod_operacion cod_op, void* tipoRequest){

	void* buffer;
	int bytes = 0;
	request request = crear_request(cod_op, tipoRequest);


	switch(request->cod_op){

	case SELECT:

		buffer = serializar_select(request);
		bytes = ((select_t)(request->tipo_request))->bytes;
		break;

	case INSERT:

		buffer= serializar_insert(request);
		bytes = ((insert)(request->tipo_request))->bytes;
		break;

	case CREATE:

		buffer = serializar_create(request);
		bytes = ((create)(request->tipo_request))->bytes;
		break;

	case DESCRIBE:

		buffer = serializar_describe(request);
		bytes =  ((describe_t)(request->tipo_request))->bytes;

		break;

	default:
		//no deberia entrar aca
		break;
	}
	// ver este send, si manda todo o ke
	send(socket_lissandra, buffer, bytes, 0);

	free(buffer);

	//liberar_request(request); NO!

	free(request);
}


void enviar_dato(t_dato* dato, int conexion){

	int desplazamiento = 0;
	int bytes = sizeof(dato->timestamp) + sizeof(dato->key) +  sizeof(dato->value->size) + dato->value->size;
	void* buffer = malloc(bytes);

	memcpy(buffer, &(dato->timestamp) , sizeof(dato->timestamp));
	desplazamiento += sizeof(dato->timestamp);

	memcpy(buffer, &(dato->key) , sizeof(dato->key));
	desplazamiento += sizeof(dato->key);

	memcpy(buffer, &(dato->value->size) , sizeof(dato->value->size));
	desplazamiento += sizeof(dato->value->size);

	memcpy(buffer, dato->value->buffer , dato->value->size);
	desplazamiento += dato->value->size;

	send(conexion, buffer, bytes, 0);

	free(buffer);

}

void enviar_metadata(t_list* lista_metadata, int conexion){

	int bytes;

	int bytes_enviados = 0;
	int bytes_restantes;
	int enviados_aux;

	void* buffer = serializar_metadata(lista_metadata &bytes);

	bytes_restantes = bytes;

	while(bytes_enviados < bytes){

		enviados_aux = send(conexion, buffer, bytes_restantes, 0);

		if(enviados_aux == -1){
			perror("Send tiro -1");
			//ver que hacer aca
		}

		bytes_enviados += enviados_aux;
		bytes_restantes -= enviados_aux;

	}

	free(buffer);


}

