/*
 * particion_contenido.c
 *
 *  Created on: 7 jul. 2019
 *      Author: utnso
 */


#include"particion_contenido.h"

Particion crear_particion(int size){

	Particion nueva_particion = malloc(sizeof(struct particionContenido));

	nueva_particion->size = size;

	nueva_particion->bloques = list_create();

	return nueva_particion;

}

void liberar_particion(Particion particion){



	void _destruir_bloques(void* bloque){//este nada mas es un int

		free(bloque);

	}

	list_destroy_and_destroy_elements(particion->bloques, _destruir_bloques);

	free(particion);

}

Particion leer_particion(char* path_particion){

	Particion particion_contenido;
	int size;


	FILE* particion = fopen(path_particion, "rb");

	if(!particion){
		printf("RECURSAMOS!\n");
		exit(1);
	}

	fseek(particion, 0, SEEK_SET);

	fread(&size, sizeof(int), 1, particion);

	particion_contenido = crear_particion(size);

	int* bloque = malloc(sizeof(int));

	fread(bloque, sizeof(int), 1, particion);

	while(!feof(particion)){ //ASUMO QUE ESTO LO TIRA BIEN OK? si rompe posiblemente sea esto

		list_add(particion_contenido->bloques, bloque);

		bloque = malloc(sizeof(int));

		fread(bloque, sizeof(int), 1, particion);

	}

	free(bloque);

	fclose(particion);

	return particion_contenido;

}

bool bloque_valido(int bloque){

	return (bloque < blocks) && (bloque >= 0);

}

void actualiar_particion(char* path_particion){

	Particion particion_lectura = leer_particion( path_particion );
	int size_actualizado = 0;

	//mostrar_particion(particion_lectura);

	void _tamanio_por_bloque(void* bloque_lista){

		int* bloque_para_tamanio = (int*) bloque_lista;

		size_actualizado += tamanio_del_bloque( *bloque_para_tamanio );


	}


	list_iterate(particion_lectura->bloques, _tamanio_por_bloque);

	particion_lectura->size = size_actualizado;

	reescribir_bloques_en_particion(path_particion, particion_lectura);
}

void mostrar_particion(Particion particion){

	printf("Bytes = %i\n", particion->size);

	printf("BLOQUES: %i\n", list_size(particion->bloques));
	int* aux = list_get(particion->bloques, 1);

	void _printear(void* _bloque){

		int* bloque = (int *)_bloque;

		printf("---BLOQUE: %i---\n ", *bloque);

		mostrar_bloque(*bloque);

		printf("Termine de leer el bloque!\n");

	}

	list_iterate(particion->bloques, _printear);

	printf("Termine de mostrar la particion\n");
}

int obtener_numero_particion(char* path){
	int numero;
	char** aux;
	char delimitador[2] = ".";

	if(string_equals_ignore_case("bin", extension_del_archivo(path))){
		aux = string_split(path, delimitador);
		numero = atoi(aux[0]);

		return numero;
	}
	else{
		return -1;
	}
}

bool analizar_dato(u_int16_t key, char* dato){

	char** aux = string_split(dato, ";");

	char* key_en_string = string_itoa(key);
	bool nani = string_equals_ignore_case(aux[1], key_en_string);

	free(key_en_string);
	liberar_puntero_doble(aux);

	return nani;



}

dato_t* buscar_dato_en_temporales(char* nombre_tabla, u_int16_t key){

	char* path_tabla = obtenerPathTabla(nombre_tabla);

	DIR* directorio_tabla = opendir(path_tabla);
	struct dirent* entrada_directorio;

	t_list* lista_de_datos = list_create();

	dato_t* dato_mayor;

	if(directorio_tabla != NULL){

		while((entrada_directorio = readdir(directorio_tabla)) !=NULL){

			if(string_ends_with(entrada_directorio->d_name, ".tmp")){

				char* path_temporal = obtenerPathTabla(nombre_tabla);

				string_append(&path_temporal, entrada_directorio->d_name);
				string_append(&path_temporal, "/");

				dato_t* dato_encontrado = buscar_dato_en_particion(path_temporal, key);

				list_add(lista_de_datos, dato_encontrado);

				free(path_temporal);


			}

		}


	}

	void _timestamp_menor(void* _dato){

		dato_t* dato_a_analizar = (dato_t *)_dato;

		dato_t* dato_aux = dato_mayor;

		dato_mayor = timestamp_mas_grande(dato_aux, dato_a_analizar);

		liberar_dato(dato_aux);

	}

	list_iterate(lista_de_datos, _timestamp_menor);

	closedir(directorio_tabla);
	free(path_tabla);

	list_destroy_and_destroy_elements(lista_de_datos, liberar_dato);

	return dato_mayor;

}


//bloque_siguiente, si no hay siguiente, tiene que ser menor que 0 ( < 0)
char* buscar_dato_bloque(u_int16_t key, int bloque_a_analizar, int indice_bloque_siguiente){

	Bloque bloque = leer_bloque(bloque_a_analizar);

	int index_dato = 0;

	char* dato_cortado = NULL;

	bool _tenes_la_key(void* _dato_a_analizar){


		//TODO PASO 2: analizo el dato de la lista
		char* dato_a_analizar = (char *) _dato_a_analizar;

		//TODO SI NO ES EL ULTIMO DATO
		if(index_dato < (list_size(bloque->datos)) - 1){ //lion

			//TODO LO ANALIZO NORMAL
			index_dato++; //lion
			return analizar_dato(key, dato_a_analizar);

		}else{

				//TODO SI ES EL ULTIMO DATO, PRIMERO ME FIJO SI EL SIGUIENTE ES VALIDO
			if(indice_bloque_siguiente >= 0){

				//SI ES VALIDO, OSEA ME PASARON UN BLOQUE SIGUIENTE

				Bloque bloque_siguiente = leer_bloque(indice_bloque_siguiente);

				if(es_dato_cortado((char *) list_get(bloque_siguiente->datos, 0)) || es_dato_cortado(dato_a_analizar)){

					//OBTENGO EL PRIMER DATO DEL BLOQUE SIGUIENTE (QUE SERIA EL CORTADO)
					dato_cortado = string_duplicate((char *) list_get(bloque_siguiente->datos, 0));

					char* dato_cortado_aux = string_new();

					string_append(&dato_cortado_aux, dato_a_analizar);
					string_append(&dato_cortado_aux, dato_cortado);

					printf("dato cortado : %s\n" , dato_cortado);

					liberar_bloque(bloque_siguiente);

					bool nani = analizar_dato(key, dato_cortado_aux);

					free(dato_cortado_aux);

					return nani;  //lo que pasa aca es que retornamos un bool true o false, y no retornamos el bloque_cor
				}

				liberar_bloque(bloque_siguiente);

			}else{

				return false;

			}

		}

		index_dato++;
	}




	//TODO PASO 1: itero la lista de datos del bloque a analizar buscando el dato
	char* dato_encontrado = list_find(bloque->datos, _tenes_la_key);

	char* dato_completo = NULL;

	if(dato_encontrado != NULL){

		dato_completo = string_new();

		string_append(&dato_completo, dato_encontrado);

		if(dato_cortado != NULL){
			string_append(&dato_completo, dato_cortado);

		}

	}


	printf("LLEGUE HASTA ACA!\n");
	//free(dato_cortado);

	liberar_bloque(bloque);

	return dato_completo;

}

bool es_dato_cortado(char* dato_a_analizar){

	char** dato_dividido = string_split(dato_a_analizar, ";");

	for( int i = 0 ; i < 3 ; i++){

		if( dato_dividido[i] == NULL){

			liberar_puntero_doble(dato_dividido);
			return true;

		}

	}

	liberar_puntero_doble(dato_dividido);
	return false;

}

dato_t* buscar_dato_en_particion(char* path_particion_a_buscar , int key){
//asumimos que hay una sola key en la particion

	Particion particion = leer_particion(path_particion_a_buscar);
	int index_bloque;

	dato_t* dato_encontrado = NULL;
	char* dato;

	int* bloque;
	int *bloque_siguiente;

	//por cada bloque de la particion
	for( index_bloque = 0 ; index_bloque < list_size(particion->bloques) ; index_bloque++){

		bloque = list_get(particion->bloques, index_bloque);

		//si el bloque tiene un bloque siguiente
		if( index_bloque < (list_size(particion->bloques) -1 )){

			int *bloque_siguiente = list_get( particion->bloques , index_bloque + 1 ) ;

			dato  = buscar_dato_bloque(key, *bloque, *bloque_siguiente);

			printf("dato: %s\n" , dato);

		//si no tiene un bloque siguiente
		}else{

			dato = buscar_dato_bloque(key , *bloque  , -1);
		}

		//si se encontro el dato!
		if(dato != NULL){

			dato_encontrado = convertir_a_dato(dato);
			free(dato);
			printf("entre aca\n");

			break;

		}


	}

	if(dato_encontrado == NULL){

		//logear que no lo encontre ! no existe

	}

	liberar_particion(particion);

	return dato_encontrado;
}


void aniadir_bloque(Particion particion, int bloque){

	int* bloque_aux = malloc(sizeof(int));

	*bloque_aux = bloque;

	list_add(particion->bloques, bloque_aux);
}


t_list* obtener_datos_particiones(char* nombre_tabla){

	metadata_t* metadata_tabla = obtener_metadata(nombre_tabla);

	printf("NANI 1\n");

	t_list* lista_datos_particion = list_create();

	printf("NANI 2\n");

	char* datos_totales = string_new();

	printf("Cantidad de particiones: %i\n", metadata_tabla->particion);
	for (int i = 0; i < metadata_tabla->particion ; i++){

		printf("Obtengo el path de una particion\n");
		char* path_particion = obtenerPath_ParticionTabla(nombre_tabla, i);

		printf("Path = %s\n", path_particion);

		printf("Leo su size y bloques\n");
		Particion particion_a_obtener = leer_particion(path_particion);

		int i = 0;

		int fd_bloque;

		void _generar_lista_datos(void* dato_bloque){

			printf("Agarro un bloque\n");
			int * bloque = (int* ) dato_bloque;
			printf("Obtengo el path de un bloque\n");
			char* path_bloque = obtenerPath_Bloque(*bloque);

			printf("Abro el bloque\n");
			int fd_bloque = open(path_bloque, O_RDONLY , S_IRUSR);

			printf("Leo los atributos del bloque\n");
			struct stat* atributos = malloc(sizeof(struct stat));

			fstat(fd_bloque, atributos);

			printf("Lo mapeo a memoria\n");
			char* datos = mmap(NULL, atributos->st_size, PROT_READ, MAP_SHARED, fd_bloque, 0);

			if(datos == MAP_FAILED){
				if(atributos->st_size == 0){
					printf("Bloque vacio c:\n");

					close(fd_bloque);

					free(path_bloque);

					free(atributos);

					return;

				}else{
					perror("Se fue todo al carajo, mmap");
					exit(1);
				}

			}

			printf("Lo concateno en el char*\n Esta mierda tiene: %s\n", datos);

			string_append(&datos_totales, datos);

			printf("Libero el map, los archivos, y los char* \n");
			munmap(fd_bloque, atributos->st_size);

			close(fd_bloque);

			free(path_bloque);

			free(atributos);

		}

		printf("Por cada bloque de la particion leida, agrego los datos a un char* \n");
		list_iterate(particion_a_obtener->bloques , _generar_lista_datos);

	}

	int j = 0;

	printf("Separo todos los datos para poder meterlos en lista que voy a devolver\n");
	char** cadena_datos = string_split(datos_totales , "\n");

	printf("Meto los datos en la lista\n");
	while( cadena_datos[j] != NULL ){

		list_add(lista_datos_particion , cadena_datos[j]);
		j++;
	}

	printf("Libero estructuras administrativas\n");
	//liberar_puntero_doble(cadena_datos);

	free(metadata_tabla);

	return lista_datos_particion;
}


t_list* obtener_datos_temporales(char* nombre_tabla){

	metadata_t* metadata_tabla = obtener_metadata(nombre_tabla);

		t_list* lista_datos_particion = list_create();

		int cantidad_temporales = obtener_cantidad_de_archivos_tmpc(nombre_tabla);

		char* datos_totales = string_new();


		for (int i = 0; i < cantidad_temporales ; i++){

			char* path_particion = obtenerPathParaTemporalMientrasCompacto(nombre_tabla, i);

			Particion particion_a_obtener = leer_particion(path_particion);

			int i = 0;

			int fd_bloque;

			void _generar_lista_datos(void* dato_bloque){

				int * bloque = (int* ) dato_bloque;

				char* path_bloque = obtenerPath_Bloque(*bloque);

				int fd_bloque = open(path_bloque, O_RDONLY , S_IRUSR);

				struct stat* atributos = malloc(sizeof(struct stat));

				fstat(fd_bloque, atributos);

				char* datos = mmap(NULL, atributos->st_size, PROT_READ, MAP_SHARED, fd_bloque, 0);

				if(datos == MAP_FAILED){
					if(atributos->st_size == 0){
						printf("Bloque vacio c:\n");

						close(fd_bloque);

						free(path_bloque);
						free(atributos);

						free(metadata_tabla);

						return;

					}else{
						perror("mmap");
						exit(1);
					}

				}

				string_append(&datos_totales,datos);

				munmap(fd_bloque, atributos->st_size);

				close(fd_bloque);

				free(path_bloque);

				free(atributos);

			}


			list_iterate(particion_a_obtener->bloques , _generar_lista_datos);

		}

		int j = 0;

		char** cadena_datos = string_split(datos_totales , "\n");

		while( cadena_datos[j] != NULL ){

			list_add(lista_datos_particion , cadena_datos[j]);
			j++;
		}

	//	liberar_puntero_doble(cadena_datos);

		free(metadata_tabla);

		return lista_datos_particion;

}



