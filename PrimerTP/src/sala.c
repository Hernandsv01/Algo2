#include "estructuras.h"
#include "sala.h"
#include "objeto.h"
#include "interaccion.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_UNA_LINEA 1024

sala_t *sala_crear_desde_archivos(const char *objetos, const char *interacciones)
{
	int tamanio_objetos = 100;
	int tamanio_interacciones = 100;
	if (objetos == NULL || interacciones == NULL) {
		printf("Nombres de archivos Nulos, verifiquelos y vuelva a intentar");
		return NULL;
	}

	sala_t *sala = malloc(sizeof(sala_t));
	if (sala == NULL) {
		printf("No se pudo conseguir espacio en memoria para sala\n");
		return NULL;
	}
	sala->objetos = malloc((long unsigned int)tamanio_objetos * sizeof(struct objeto));
	if (sala->objetos == NULL) {
		printf("No se pudo conseguir espacio en memoria para sala->objetos\n");
		free(sala);
		return NULL;
	}
	sala->interacciones = malloc((long unsigned int)tamanio_interacciones * sizeof(struct interaccion));
	if (sala->interacciones == NULL) {
		printf("No se pudo conseguir espacio en memoria para sala->interacciones\n");
		free(sala);
		free(sala->objetos);
		return NULL;
	}

	sala->cantidad_objetos = 0;
	sala->cantidad_interacciones = 0;

	FILE *arch_objetos = fopen(objetos, "r");
	if (arch_objetos == NULL) {
		printf("No se pudo abrir el archivo de objetos\n");
		sala_destruir(sala);
		return NULL;
	}
	char linea[MAX_UNA_LINEA];
	bool hay_objetos = false;
	while (fgets(linea, MAX_UNA_LINEA, arch_objetos) != NULL) {
		hay_objetos = true;

		struct objeto *obj = objeto_crear_desde_string(linea);
		if (obj != NULL) {
			if (sala->cantidad_objetos+1 == tamanio_objetos) {
				struct objeto *aux = realloc(sala->objetos, ((long unsigned int)tamanio_objetos * sizeof(struct objeto)) * 2);
				if (aux != NULL) {
					*sala->objetos = aux;
					tamanio_objetos *= 2;
				} else {
					printf("No alcanza la memoria para seguir guardando objetos\n");
					sala_destruir(sala);
					fclose(arch_objetos);
					return NULL;
				}
			}
			sala->objetos[sala->cantidad_objetos] = obj;
			sala->cantidad_objetos++;
		} else {
			printf("No se pudo crear el objeto\n");
			sala_destruir(sala);
			fclose(arch_objetos);
			return NULL;
		}
	}
	if (!hay_objetos) {
		printf("El archivo de objetos está vacío\n");
		sala_destruir(sala);
		fclose(arch_objetos);
		return NULL;
	}
	fclose(arch_objetos);
	FILE *arch_interacciones = fopen(interacciones, "r");
	if (arch_interacciones == NULL) {
		printf("No se pudo abrir el archivo de interacciones\n");
		sala_destruir(sala);
		return NULL;
	}
	bool hay_interacciones = false;
	while ((fgets(linea, MAX_UNA_LINEA, arch_interacciones) != NULL)) {
		hay_interacciones = true;

		struct interaccion *inter = interaccion_crear_desde_string(linea);
		if (inter != NULL) {
			if (sala->cantidad_interacciones+1 == tamanio_interacciones) {
				struct interaccion *aux = realloc(sala->interacciones, ((long unsigned int)tamanio_interacciones * sizeof(struct objeto)) * 2);
				if (aux != NULL) {
					*sala->interacciones = aux;
					tamanio_interacciones *= 2;
				} else {
					printf("No alcanza la memoria para seguir guardando interacciones\n");
					sala_destruir(sala);
					fclose(arch_interacciones);
					return NULL;
				}
			}
			sala->interacciones[sala->cantidad_interacciones] = inter;
			sala->cantidad_interacciones++;
		} else {
			printf("No se pudo crear la interaccion\n");
			sala_destruir(sala);
			fclose(arch_interacciones);
			return NULL;
		}
	}
	if (!hay_interacciones) {
		printf("El archivo de objetos está vacío\n");
		sala_destruir(sala);
		fclose(arch_interacciones);
		return NULL;
	}
	fclose(arch_interacciones);

	return sala;
}

char **sala_obtener_nombre_objetos(sala_t *sala, int *cantidad)
{
	bool cantidadEsNull = (cantidad == NULL);
	if (sala == NULL) {
		printf("La sala es nula\n");
		if (!cantidadEsNull) {
			(*cantidad) = -1;
		}
		return NULL;
	}

	char **vector_resultado = malloc(500);
	if (vector_resultado == NULL) {
		printf("No se pudo obtener memoria para la lista de nombres\n");
		if (!cantidadEsNull) {
			(*cantidad) = -1;
		}
		return NULL;
	}

	int i;
	for (i = 0; i < sala->cantidad_objetos; i++) {
		vector_resultado[i] = sala->objetos[i]->nombre;
	}
	if (!cantidadEsNull) {
		(*cantidad) = i;
	}
	return vector_resultado;
}

bool sala_es_interaccion_valida(sala_t *sala, const char *verbo, const char *objeto1, const char *objeto2)
{
	if (sala == NULL || verbo == NULL || objeto1 == NULL || objeto2 == NULL) {
		printf("Uno de los parametros es nulo\n");
		return false;
	}
	bool resultado = false;
	int i = 0;
	while (!resultado && i < sala->cantidad_interacciones) {
		if (strcmp(sala->interacciones[i]->objeto, objeto1) == 0 && strcmp(sala->interacciones[i]->verbo, verbo) == 0 && strcmp(sala->interacciones[i]->objeto_parametro, objeto2) == 0) {
			resultado = true;
		}
		i++;
	}
	return resultado;
}

void sala_destruir(sala_t *sala)
{
	if (sala == NULL) {
		printf("No hay nada que destruir\n");
		return;
	}
	for (int i = 0; i < sala->cantidad_objetos; i++) {
		free(sala->objetos[i]);
	}
	free(sala->objetos);
	for (int i = 0; i < sala->cantidad_interacciones; i++) {
		free(sala->interacciones[i]);
	}
	free(sala->interacciones);
	free(sala);
}
