/*
 * bloque.c
 *
 *  Created on: 7 jul. 2019
 *      Author: utnso
 */

#include"bloque.h"

Bloque leer_bloque(int indice_bloque){

	char* path_bloque = obtenerPath_Bloque(indice_bloque); //lion
	Bloque bloque;

	//ABRO EL BLOQUE
	int fd_bloque = open(path_bloque, O_RDONLY , S_IRUSR);

	if(fd_bloque <= -1){
		printf("[LECTURA] Fallo el abrir el bloque\n");
	}

	struct stat* atributos = malloc(sizeof(struct stat));

	fstat(fd_bloque, atributos);

	char* datos = mmap(NULL, atributos->st_size, PROT_READ, MAP_SHARED, fd_bloque, 0); //lion

	if(datos == MAP_FAILED){

		printf("[LECTURA] Fallo el map!\n");

		if(errno == EINVAL){
			printf("[LECTURA] Tamanio del archivo es 0\n");
		}

		bloque = crear_bloque(indice_bloque, NULL);

	}else{

		bloque= crear_bloque(indice_bloque, datos);

		munmap(datos, atributos->st_size);

	}

	free(atributos);
	free(path_bloque);

	close(fd_bloque);

	return bloque;

}

/*
void mostrar_bloque(char* path_bloque){


	int bloque = open(path_bloque, O_RDONLY, S_IRUSR);

	if(bloque < 0){
		printf("no se puede abrir el archivo :c\n");
	}

	struct stat* atributos_bloque = malloc(sizeof(stat));

	char* bloque_map = mmap(NULL, atributos_bloque->st_size, PROT_READ, MAP_SHARED, bloque, 0);

	int i;

	for(i = 0; i < atributos_bloque->st_size; i++){

		printf("%c", *(bloque_map + i));

	}

	printf("\n");

	munmap(bloque_map, atributos_bloque->st_size);

	close(bloque);

	free(atributos_bloque);

}
*/

void mostrar_bloques(t_list* bloques){



	void _mostrar_bloque(void* nro_bloque){


		int* bloque = (int *)nro_bloque;
		//char* path_bloque = obtenerPath_Bloque(*bloque);
		printf("BLOQUE %i\n", *bloque);
		mostrar_bloque(*bloque);

		//free(path_bloque);

	}

	list_iterate(bloques, _mostrar_bloque);

}

int ultimo_bloque_particion(Particion particion){

	int* bloque = list_get(particion->bloques, list_size(particion->bloques) - 1);

	return *bloque;

}

void cargar_a_particion(char* path_particion, dato_t* dato_a_escribir){

	Particion particion = leer_particion(path_particion);

	int bloque = ultimo_bloque_particion(particion);

	liberar_particion(particion);

	char* dato_convertido = convertir_dato_en_string(dato_a_escribir);

	if(string_ends_with(path_particion, ".tmp")){
		log_info(logger_dump, "Lleno el bloque: %i", bloque);
	}

	dato_convertido = llenar_bloque(bloque, dato_convertido);


	while( !string_is_empty(dato_convertido) ){

		if(string_ends_with(path_particion, ".tmp")){
			log_info(logger_dump, "No se pudo cargar por completo el dato en el bloque, quedo por cargar: %s", dato_convertido);
		}

		pthread_rwlock_wrlock(&semaforo_bitmap);

		int nuevo_bloque = buscar_primer_indice_vacio();

		if(nuevo_bloque == -1){
			exit(1); // VER ESTO
		}

		asignar_bloque_vacio_a_particion(path_particion, nuevo_bloque);

		pthread_rwlock_unlock(&semaforo_bitmap);

		if(string_ends_with(path_particion, ".tmp")){
			log_info(logger_dump, "Asigno y lleno un nuevo bloque vacio: %i", nuevo_bloque);
		}

		dato_convertido = llenar_bloque(nuevo_bloque, dato_convertido);
	}

	if(string_ends_with(path_particion, ".tmp")){
		log_info(logger_dump, "Termine de cargar el dato en el/los bloque/s");
	}

	actualiar_particion(path_particion);

	free(dato_convertido); // MIEDO, MUCHO!

}



int tamanio_del_bloque( int bloque ){

	char* path = obtenerPath_Bloque(bloque);
	int descriptor_bloque = open(path, O_RDONLY, S_IRUSR);

	int puntero = lseek(descriptor_bloque, 0, SEEK_END);

	close(descriptor_bloque);
	free(path);

	return puntero;

}



char* llenar_bloque(int bloque , char* dato) {

	char* pathBloque = obtenerPath_Bloque(bloque);
	int fichero_bloque = open(pathBloque, O_RDWR, S_IRUSR | S_IWUSR);
	int puntero_bloque;
	int indice_dato = 0;

	int cantidad_caracteres = strlen(dato);

	puntero_bloque = lseek(fichero_bloque, 0 ,SEEK_END);

	while( puntero_bloque < block_size && cantidad_caracteres > 0){

		write( fichero_bloque , dato + indice_dato , sizeof(char) );
		*(dato + indice_dato) = ' ';
		puntero_bloque++;
		cantidad_caracteres--;
		indice_dato++;

	}

	string_trim_left(&dato);

	close(fichero_bloque);

	free(pathBloque);

	return dato;

}

void set_all_estados( estadoBloque estado){

	for( int i = 0; i < blocks ; i++ ){

		set_estado( i , estado);

	}

}

void set_estado(int indiceBloque, estadoBloque estado){

	//semaforo arafue

	char* path_bitmap = obtener_path_bitmap();

	struct stat* myStat = malloc(sizeof(struct stat));

	int fichero = open(path_bitmap, O_RDWR, S_IRUSR | S_IWUSR);

	fstat(fichero, myStat);

	char* bitmap = mmap(NULL, myStat->st_size, PROT_WRITE, MAP_SHARED, fichero, 0);

	if(bitmap == MAP_FAILED){
		perror("mmap");
		close(fichero);
		exit(1);
	}
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap, myStat->st_size, MSB_FIRST);
	if(estado == LIBRE){
		bitarray_clean_bit(bitarray, indiceBloque);
	}
	else{
		bitarray_set_bit(bitarray, indiceBloque);
	}


	munmap(bitmap, myStat->st_size);
	bitarray_destroy(bitarray);

	free(myStat);
	free(path_bitmap);

	close(fichero);

}


bool get_estado(int indice){

	//agarrar el semaforo antes

	char* path_bitmap = obtener_path_bitmap();

	int fichero = open(path_bitmap, O_RDONLY, S_IRUSR);

	struct stat myStat;
	fstat(fichero, &myStat);

	char* bitmap = mmap(NULL, myStat.st_size, PROT_READ, MAP_SHARED, fichero, 0);
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap, myStat.st_size, MSB_FIRST);

	bool i = bitarray_test_bit(bitarray, indice);

	munmap(bitmap, myStat.st_size);

	bitarray_destroy(bitarray);

	close(fichero);
	free(path_bitmap);

	return i;

}


int buscar_primer_indice_vacio(){

	int i;

	//semaforo bitmap arafue

	for (i = 0; i < blocks ; i++){

		if( get_estado(i) == LIBRE ){

			return i;

		}

	}

	return -1;
}


char* convertir_dato_en_string( dato_t* dato ){

	char* string_key, *string_timestamp;

	char* string_dato = string_new();

	string_key = string_from_format("%i", dato->key);
	string_timestamp = string_from_format("%i", dato->timestamp);

	string_append(&string_dato, string_timestamp);
	string_append(&string_dato, ";");
	string_append(&string_dato, string_key);
	string_append(&string_dato, ";");
	string_append(&string_dato, dato->value);
	string_append(&string_dato, "\n");

	free(string_key);
	free(string_timestamp);

	return string_dato;

}

Bloque crear_bloque(int numero, char* datos){

	Bloque bloque = malloc(sizeof(struct bloque*));

	int i = 0;

	bloque->numero = numero;

	bloque->datos = list_create();


	if(datos != NULL){

		if(string_starts_with(datos, "\n")){

			char* barra_ene = malloc(2);
			memcpy(barra_ene, "\n", 2);

			list_add(bloque->datos,  barra_ene );

			i++;
		}

		char** aux_datos = string_split(datos, "\n");

		i = 0;

		while(*(aux_datos + i) != NULL){

			list_add(bloque->datos, *(aux_datos + i));

			i++;

		}

		free(aux_datos);
	}

	return bloque;

}

void liberar_bloque(Bloque bloque_a_liberar){

	list_destroy_and_destroy_elements(bloque_a_liberar->datos, free);

	free(bloque_a_liberar);

}


void mostrar_bloque(int bloque){

	char* path_bloque = obtenerPath_Bloque(bloque);

	int fichero_bloque = open(path_bloque, O_RDONLY , S_IRUSR);

	struct stat* atributos_bloque = malloc(sizeof(struct stat));

	fstat(fichero_bloque, atributos_bloque);

	if(fichero_bloque < 0){

		printf("\n>No existe el bloque %i\n" , bloque);

		return;

	}

	printf("\n---LEYENDO BLOQUE %d---\n" , bloque);

	int i = 0;

	char* archivo = mmap(NULL, atributos_bloque->st_size, PROT_READ, MAP_SHARED, fichero_bloque, 0);

//	printf("Tamanio del bloque: %i\n", atributos_bloque->st_size);

	if(archivo == MAP_FAILED){
		if(atributos_bloque->st_size == 0){
			printf("\n>Bloque vacio c:\n");

			close(fichero_bloque);

			free(path_bloque);
			free(atributos_bloque);

			return;

		}else{
			perror("mmap");
			exit(1);
		}

	}

	if(string_starts_with(archivo, "\n")){

		char* barra_ene = malloc(2);
		memcpy(barra_ene, "\n", 2);

		printf("%s\n", barra_ene);

		free(barra_ene);
	}

	char** datos = string_split(archivo, "\n");

//	printf("Llegue hasta aca\n");

	while(*(datos + i) != NULL){

		printf("%s\n",*(datos + i) );
		i++;

	}

	munmap(archivo, atributos_bloque->st_size);
	close(fichero_bloque);

	for(; i > 0 ; i--){

		free(*(datos + i));
	}

	free(datos);
	free(path_bloque);
	free(atributos_bloque);

	printf("\n");

}

void get_all_estados(){

	int fichero = open("Metadata/bitmap.bin", O_RDONLY, S_IRUSR);

	struct stat myStat;
	fstat(fichero, &myStat);

	char* bitmap = mmap(NULL, myStat.st_size, PROT_READ, MAP_SHARED, fichero, 0);
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap, myStat.st_size, MSB_FIRST);

	if(bitmap == MAP_FAILED){
		perror("mmap");
		exit(1);
	}

	int i;

	printf("######Lector de estados del bitmap######\n");
	for(i = 0; i < blocks; i++){
		printf("Indice %i = %i\n", i, bitarray_test_bit(bitarray, i));
	}

	munmap(bitmap, myStat.st_size);
	bitarray_destroy(bitarray);

	close(fichero);

}

