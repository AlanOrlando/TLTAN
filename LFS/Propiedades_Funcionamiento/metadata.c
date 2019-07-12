/*
 * metadata.c
 *
 *  Created on: 19 may. 2019
 *      Author: utnso
 */

#include"metadata.h"

metadata_t* obtener_metadata(char* nombre_tabla){

	metadata_t* obtener_metadata = malloc(sizeof(metadata_t));
	t_config* metadata_config;
	char* path_metadata_tabla = obtener_path_metadata_de_tabla(nombre_tabla);
	char* aux_consistencia;
	metadata_config = config_create(path_metadata_tabla);

	aux_consistencia = config_get_string_value(metadata_config,"CONSISTENCY");
	obtener_metadata->particion= config_get_int_value(metadata_config, "PARTITIONS");
	obtener_metadata->compactacion = config_get_int_value(metadata_config, "COMPACTION_TIME");


	if(strcmp(aux_consistencia,"SC") == 0){
	obtener_metadata->consistencia = SC;
	}
	if(strcmp(aux_consistencia,"SHC") == 0){
	obtener_metadata->consistencia = SHC;
	}
	if(strcmp(aux_consistencia, "EC") == 0){
	obtener_metadata->consistencia = EC;
	}

	free(aux_consistencia);

	free(path_metadata_tabla);

	//config_destroy(metadata_config);

	return obtener_metadata;
}


char* obtener_path_metadata_de_tabla(char* nombre_tabla){
	char* path = obtenerPathTabla(nombre_tabla);
	string_append(&path, "/");
	string_append(&path, "Metadata.config");


	return path;
}

void mostrar_metadata_de_tabla(char* nombre_Tabla){
	char* pathMetadata = obtener_path_metadata_de_tabla(nombre_Tabla);
	char* consistencia;
	int particiones, compactacion;

	t_config* configMetadata = config_create(pathMetadata);
	particiones = config_get_int_value(configMetadata, "PARTITIONS");
	compactacion = config_get_int_value(configMetadata, "COMPACTION_TIME");
	consistencia = config_get_string_value(configMetadata, "CONSISTENCY");


	printf("CONSISTENCY = %s\n", consistencia);
	printf("PARTITIONS = %i\n", particiones);
	printf("COMPACTION_TIME = %i\n", compactacion);

	config_destroy(configMetadata);
	free(pathMetadata);


}

void setear_metadata(){

	t_config* metadata_config;

	metadata_config = config_create("Tablas/Tabla_B/Metadata.config");

	config_set_value(metadata_config, "CONSISTENCY", "SC");
	config_set_value(metadata_config, "PARTITIONS", "1");
	config_set_value(metadata_config, "COMPACTION_TIME" , "300");
	config_save(metadata_config);
	config_destroy(metadata_config);

	printf("se configuro el metadata lol\n");

}

char* obtenerPathTabla(char* nombre_tabla){

	char* path = string_new();

	char *prefijo = "Tablas/";

	string_append(&path, punto_montaje);
	string_append(&path, prefijo);
	string_append(&path, nombre_tabla);

	return path;
}

char* obtenerPathParaTemporalEnLaTabla(char* nombreTabla){
//	char* indicadorNombre = string_substring_from(nombreTabla, strlen(nombreTabla)-1);
	char* pathBase = obtenerPathTabla(nombreTabla);
	DIR* dir = opendir(pathBase);
	int numeroParaTemporal = 0;
	int auxiliar;

	struct dirent *ent;

	while((ent = readdir(dir)) != NULL){
		if(no_es_ubicacion_prohibida(ent->d_name)){

			auxiliar = obtenerNumeroTemporal(ent->d_name);


			if(auxiliar > numeroParaTemporal){
				numeroParaTemporal = auxiliar;
			}
			else if (auxiliar == numeroParaTemporal){
				numeroParaTemporal = 1;
			}
		}
	}

	if(numeroParaTemporal > 1){
		numeroParaTemporal++;
	}


	char* numeroDesignado = string_new();
	numeroDesignado = string_itoa(numeroParaTemporal);

	char* pathCompleto = string_new();
	string_append(&pathCompleto, pathBase);
	string_append(&pathCompleto, "/");
//	string_append(&pathCompleto, indicadorNombre);
	string_append(&pathCompleto, numeroDesignado);
	string_append(&pathCompleto, ".tmp");

//	free(indicadorNombre); // sacar ?
//	free(pathBase);			//sacar ?

	return pathCompleto;
}

char* obtenerPathParaTemporalMientrasCompacto(char* nombre_tabla){
//	char* indicadorNombre = string_substring_from(nombre_tabla, strlen(nombre_tabla)-1);
	char* pathBase = obtenerPathTabla(nombre_tabla);
	DIR* dir = opendir(pathBase);
	int numeroParaTemporal = 0;
	int auxiliar;

	struct dirent *ent;

	while((ent = readdir(dir)) != NULL){
		if(no_es_ubicacion_prohibida(pathBase)){
			auxiliar = obtenerNumeroTemporal(ent->d_name);

			if(auxiliar > numeroParaTemporal){
				numeroParaTemporal = auxiliar;
			}
			else if (auxiliar == numeroParaTemporal){
				numeroParaTemporal = 1;
			}
		}
	}

	if(numeroParaTemporal > 1){
		numeroParaTemporal++;
	}


	char* numeroDesignado = string_new();
	numeroDesignado = string_itoa(numeroParaTemporal);

	char* pathCompleto = string_new();
	string_append(&pathCompleto, pathBase);
	string_append(&pathCompleto, "/");
//	string_append(&pathCompleto, indicadorNombre);
	string_append(&pathCompleto, numeroDesignado);
	string_append(&pathCompleto, ".tmpc");

//	free(indicadorNombre); // sacar ?
	free(pathBase);			//sacar ?

	return pathCompleto;
}

char* obtenerPath_ParticionTabla(char* nombre_tabla, int particion){
	char* path = obtenerPathTabla(nombre_tabla);

	char* itoa =  string_itoa(particion);

	string_append(&path, "/");
	string_append(&path, itoa);
	string_append(&path, ".bin");

	free(itoa);

	return path;


}

char* obtenerPath_Bloque(int indice){
	char* path = string_new();

	string_append(&path, punto_montaje);
	string_append(&path, "Bloques/");
	string_append(&path, string_itoa(indice));
	string_append(&path, ".bin");


	return path;


}

char* obtenerPathDirectorio_Tablas(){
	char* path = string_new();

	char *prefijo = "Tablas";

	string_append(&path, punto_montaje);
	string_append(&path, prefijo);

	return path;
}

void crear_metadata(char* nombre_tabla, char* consistencia, int particion, int tiempo_Compactacion){
	t_config* metadata_config;
	char* particionAux = string_new();
	char* tiempo_compactacionAux = string_new();
	char* pathMetadata = obtener_path_metadata_de_tabla(nombre_tabla);

	particionAux = string_itoa(particion);
	tiempo_compactacionAux = string_itoa(tiempo_Compactacion);


	metadata_config = config_create(pathMetadata);
	config_save_in_file(metadata_config, pathMetadata);

	config_set_value(metadata_config,"CONSISTENCY", consistencia);
	config_set_value(metadata_config, "PARTITIONS", particionAux);
	config_set_value(metadata_config, "COMPACTION_TIME", tiempo_compactacionAux);

	config_save(metadata_config);
	config_destroy(metadata_config);

}

char* obtener_ip(){

	t_config* metadata_config = config_create("lissandra.config");
	char* ip = config_get_string_value(metadata_config, "IP");

	//config_destroy(metadata_config); (descomentar si funciona)
	return ip;

}

int obtener_puerto(){

	t_config* metadata_config = config_create("lissandra.config");
	int puerto = config_get_int_value(metadata_config , "PUERTO");

	//config_destroy(metadata_config);
	return puerto;

}

int obtener_cantidad_de_archivos_tmpc(char* nombre_tabla){
	DIR* dir;

	struct dirent* ent;

	int temporales_compactados = 0;

	char* raiz_de_tabla = obtenerPathTabla(nombre_tabla);
//	string_append(&raiz_de_tabla, "/");

	if((dir = opendir(raiz_de_tabla)) != NULL){

		while((ent = readdir(dir)) != NULL){

			if(string_ends_with(ent->d_name, "tmpc")){

				temporales_compactados++;

			}
		}

	}

	closedir(dir);

	return temporales_compactados;
}

void transformar_tmp_a_tmpc(char* nombre_tabla){
	DIR* dir;

	struct dirent* ent;

	char* raiz_de_tabla = obtenerPathTabla(nombre_tabla);

	char** aux;

	printf("VAMOS A ENTRAR A = %s\n", raiz_de_tabla);

	if((dir = opendir(raiz_de_tabla)) != NULL){

		while((ent = readdir(dir)) != NULL){

			printf("ESTAMOS EN = %s\n", ent->d_name);

			if(string_ends_with(ent->d_name, "tmp")){

				char* viejo = string_new();
				string_append(&viejo, raiz_de_tabla);
				string_append(&viejo, "/");
				string_append(&viejo, ent->d_name);

				char* nuevo = string_new();
				aux = string_split(ent->d_name, ".");

				string_append(&nuevo, raiz_de_tabla);
				string_append(&nuevo, "/");
				string_append(&nuevo, aux[0]);
				string_append(&nuevo, ".");
				string_append(&nuevo, "tmpc");

				printf("El viejo path es = %s\n", viejo);
				printf("El nuevo path es = %s\n", nuevo);


				rename(viejo, nuevo);
			}
		}
	}

	closedir(dir);
}

void liberar_bloques_particion(char* path_particion){

	Particion particion = leer_particion(path_particion);

	void _eliminar_bloque(void* _nro_bloque){

		int* nro_bloque = (int *)_nro_bloque;

		printf("   Bloque: %i\n", *nro_bloque);

		eliminar_bloque(*nro_bloque);


	}


	list_iterate(particion->bloques, _eliminar_bloque);

	list_clean_and_destroy_elements(particion->bloques, free);


	//de aca para abajo nose si esta bien, revisar


	if(string_ends_with(path_particion , ".bin")){

		crear_archivo_particion(path_particion);

	}

	liberar_particion(particion);

}

void liberar_tmpc(char* nombre_tabla){
	DIR* dir;
	struct dirent* ent;

	char* path_tabla = obtenerPathTabla(nombre_tabla);

	if((dir = opendir(path_tabla)) != NULL){
		while((ent = readdir(dir)) != NULL){
			if(string_ends_with(ent->d_name, ".tmpc")){
				char* aux = string_new();
				string_append(&aux, path_tabla);
				string_append(&aux, "/");
				string_append(&aux, ent->d_name);

				printf("\nLIBERO LOS BLOQUE: \n");
				liberar_bloques_particion(aux);

				unlink(aux);
				free(aux);
			}

		}

	}

	closedir(dir);
	free(path_tabla);

}

void eliminar_bloque(int bloque){

	char* path_bloque = obtenerPath_Bloque(bloque);

	printf("path bloque: %s\n"  , path_bloque);

	unlink(path_bloque);

	set_estado(bloque, LIBRE);

	free(path_bloque);

}


