/*
 * config.c
 *
 *  Created on: 30 abr. 2019
 *      Author: utnso
 */

#include "configuracion.h"


void creacion_del_config(){

	FILE* config_kernel = fopen("kernel.config" , "w");
	fclose(config_kernel);

	g_config = config_create("kernel.config");

	config_set_value(g_config, "IP_MEMORIA", "127.0.0.1");
	config_set_value(g_config, "PUERTO_MEMORIA", "4444");
	config_set_value(g_config, "QUANTUM", "4");
	config_set_value(g_config, "GRADO_MULTIPROCESAMIENTO", "3");
	config_set_value(g_config, "REFRESH_METADATA", "10000");
	config_set_value(g_config, "RETARDO_CICLO_EJECUCION", "5000");
	config_set_value(g_config, "TIEMPO_GOSSIPING_KERNEL", "60000");
	config_save(g_config);
	config_destroy(g_config);

}

void obtener_datos_config(){

	g_config = config_create("kernel.config");

	puerto_memoria = config_get_int_value(g_config, "PUERTO_MEMORIA");
	ip_memoria = config_get_int_value(g_config, "IP_MEMORIA");
	grado_multiprocesamiento = config_get_int_value(g_config , "GRADO_MULTIPROCESAMIENTO");
	quantum = config_get_int_value(g_config , "QUANTUM");
	tiempo_gossiping_kernel = config_get_int_value(g_config , "TIEMPO_GOSSIPING_KERNEL");

	config_destroy(g_config);

}


