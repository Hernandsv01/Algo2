#include "objeto.h"
#include "estructuras.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_COPIA 1024
#define MAX_BOOL 5

struct objeto *objeto_crear_desde_string(const char *string)
{
	if (string == NULL) {
		return NULL;
	}
	struct objeto *obj = malloc(sizeof(struct objeto));
	if (obj == NULL) {
		return NULL;
	}

	char flag[MAX_BOOL];
	int leidos = sscanf(string, "%[^;];%[^;];%[^\n]\n", obj->nombre, obj->descripcion, flag);
	if (leidos != 3) {
		printf("Formato de archivo de objetos incorrecto\n");
		free(obj);
		return NULL;
	}

	if (strcmp(flag, "true") == 0) {
		obj->es_asible = true;
	} else if (strcmp(flag, "false") == 0) {
		obj->es_asible = false;
	} else {
		free(obj);
		printf("El objeto no se pudo construir correctamente\n");
		return NULL;
	}

	return obj;
}
