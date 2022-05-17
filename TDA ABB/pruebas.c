#include "src/abb.h"
#include <stdio.h>
#include "pa2mm.h"
#include "string.h"

typedef struct cosa {
	int clave;
	char contenido[10];
} cosa;

cosa *crear_cosa(int clave)
{
	cosa *c = (cosa *)malloc(sizeof(cosa));
	if (c)
		c->clave = clave;
	return c;
}

void destruir_cosa(cosa *c)
{
	free(c);
}

int comparar_cosas(void *c1, void *c2)
{
	cosa *cosa1 = c1;
	cosa *cosa2 = c2;

  if(!cosa1 || !cosa2){
    return !NULL;
  }
	return cosa1->clave - cosa2->clave;
}

void destructor_de_cosas(void *c)
{
	cosa *cosa = c;
	if (!cosa)
		printf("El destructor del árbol intenta destruir una cosa NULL D:\n");
	else {
		printf("El destructor del árbol destruye una cosa con clave %d\n",
		       cosa->clave);
		destruir_cosa(c);
	}
}

void pruebas_rapidas()
{
  abb_t *abb = abb_crear(comparar_cosas);
  cosa *cosa1 = crear_cosa(5);
  cosa *cosa2 = crear_cosa(10);
  cosa *cosa3 = crear_cosa(6);
  cosa *cosa4 = crear_cosa(8);
  cosa *cosa5 = crear_cosa(4);
  cosa *aux = crear_cosa(0);

  abb_insertar(abb, cosa1);
  abb_insertar(abb, cosa2);
  abb_insertar(abb, cosa3);
  abb_insertar(abb, cosa4);
  abb_insertar(abb, cosa5);

  pa2m_afirmar(abb_tamanio(abb) == 5, "El arbol tiene 5 elementos");

  aux->clave = 5;
  pa2m_afirmar(abb_quitar(abb, aux) == cosa1, "Cosa 1 eliminada con exito");
  aux->clave = 10;
  pa2m_afirmar(abb_quitar(abb, aux) == cosa2, "Cosa 2 eliminada con exito");
  aux->clave = 6;
  pa2m_afirmar(abb_quitar(abb, aux) == cosa3, "Cosa 3 eliminada con exito");
  aux->clave = 8;
  pa2m_afirmar(abb_quitar(abb, aux) == cosa4, "Cosa 4 eliminada con exito");
  aux->clave = 4;
  pa2m_afirmar(abb_quitar(abb, aux) == cosa5, "Elimino el ultimo elemento");

  pa2m_afirmar(abb_vacio(abb), "El arbol está vacío");
  pa2m_afirmar(abb_tamanio(abb) == 0, "El arbol tiene tamaño cero");

  abb_insertar(abb, cosa1);
  pa2m_afirmar(abb_buscar(abb, cosa1) == cosa1, "Se insertó el elemento cosa1");
  pa2m_afirmar(!abb_vacio(abb), "El arbol no está vacío");
  pa2m_afirmar(abb_tamanio(abb) == 1, "El arbol tiene un elemento");

  abb_destruir_todo(abb, destructor_de_cosas);
  destruir_cosa(cosa2);
  destruir_cosa(cosa3);
  destruir_cosa(cosa4);
  destruir_cosa(cosa5);
  destruir_cosa(aux);
}

int main()
{
  pa2m_nuevo_grupo("Pruebas rapidas");
  pruebas_rapidas();

  return pa2m_mostrar_reporte();
}
