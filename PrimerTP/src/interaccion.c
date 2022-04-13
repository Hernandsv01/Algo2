#include "estructuras.h"
#include "interaccion.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIN_OBJETO "_"
#define MAX_STRING 1024

struct interaccion *interaccion_crear_desde_string(const char *string)
{
	if (string == NULL) {
		return NULL;
	}
	struct interaccion *inter = malloc(sizeof(struct interaccion));
	if (inter == NULL) {
		return NULL;
	}

	char tipo_accion = ' ';
	char objeto2[MAX_STRING];
	char objeto_accion[MAX_STRING];
	int leidos = sscanf(string, "%[^;];%[^;];%[^;];%c:%[^:]:%[^\n]\n", inter->objeto, inter->verbo, objeto2, &tipo_accion, objeto_accion, inter->accion.mensaje);
	if (leidos != 6) {
		printf("Formato de archivo de interacciones incorrecto\n");
		free(inter);
		return NULL;
	}

	if (strcmp(objeto2, SIN_OBJETO) == 0) {
		strcpy(inter->objeto_parametro, "");
	} else {
		strcpy(inter->objeto_parametro, objeto2);
	}
	if (strcmp(objeto_accion, SIN_OBJETO) == 0) {
		strcpy(inter->accion.objeto, "");
	} else {
		strcpy(inter->accion.objeto, objeto_accion);
	}
	switch (tipo_accion) {
	case 'd':
		inter->accion.tipo = DESCUBRIR_OBJETO;
		break;
	case 'e':
		inter->accion.tipo = ELIMINAR_OBJETO;
		break;
	case 'r':
		inter->accion.tipo = REEMPLAZAR_OBJETO;
		break;
	case 'm':
		inter->accion.tipo = MOSTRAR_MENSAJE;
		break;
	default:
		inter->accion.tipo = ACCION_INVALIDA;
	}

	return inter;
}
