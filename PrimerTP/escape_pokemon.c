#include "src/sala.h"
#include <stdio.h>
#include <stdlib.h>

void imprimir_validez(bool esValido)
{
	if (esValido) {
		printf("Valido\n");
	} else {
		printf("Invalido\n");
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Numero de argumentos es invalido");
		return -1;
	}

	//Los archivos deben venir como parámetros del main
	sala_t *sala = sala_crear_desde_archivos(argv[1], argv[2]);
	if (sala == NULL) {
		printf("Error al crear la sala de escape\n");
		return -1;
	}

	printf("Objetos...\n");
	int tope_nombres;
	char **nombres = sala_obtener_nombre_objetos(sala, &tope_nombres);
	if (nombres != NULL) {
		for (int i = 0; i < tope_nombres; i++) {
			printf("%i: %s\n", i, nombres[i]);
		}
	}
	free(nombres);

	printf("Interacciones...\n");

	//Mostrar si son válidas las siguientes interacciones
	//1. examinar habitacion
	printf("Examinar la habitación: ");
	imprimir_validez(sala_es_interaccion_valida(sala, "examinar", "habitacion", ""));
	//2. abrir pokebola
	printf("Abrir la pokebola: ");
	imprimir_validez(sala_es_interaccion_valida(sala, "abrir", "pokebola", ""));
	//3. usar llave cajon
	printf("Usar llave en el cajón: ");
	imprimir_validez(sala_es_interaccion_valida(sala, "usar", "llave", "cajon"));
	//4. quemar mesa
	printf("Quemar mesa: ");
	imprimir_validez(sala_es_interaccion_valida(sala, "quemar", "mesa", ""));

	sala_destruir(sala);
	return 0;
}