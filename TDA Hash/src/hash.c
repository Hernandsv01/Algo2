#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hash.h"
#include "lista.h"

#define ERROR -1
#define EXITOSO 0
#define CAPACIDAD_MIN 3
#define CAPACIDAD_MAX_CARGA 0.75

struct hash {
    size_t cantidad;
    size_t capacidad;
    lista_t **tabla;
};

typedef struct par{
    char *clave;
    void *elemento;
} par_t;

/* --------------------------- Funciones privadas --------------------------- */

/* 
* Se encarga de calcular la posicion dada una clave y la capacidad de un hash. 
*/
uint32_t funcion_hash(const char *clave)
{
	uint32_t hash = 5381;
	uint8_t c;
	while ((c = (uint8_t) *clave++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

/*
* Se encarga de trasladar los elementos de la tabla de hash anterior 
* a una nueva con la capacidad renovada.
*/
int funcion_rehash(hash_t* hash)
{
    size_t nueva_capacidad = hash->capacidad*2;
    lista_t** nueva_tabla = malloc(sizeof(lista_t*)*nueva_capacidad);
    size_t cantidad = hash->cantidad;
    if(!nueva_tabla) return ERROR;
    for(size_t i = 0; i < nueva_capacidad; i++){
        nueva_tabla[i] = lista_crear();
    }

    par_t* par_auxiliar;
    for(size_t i = 0; i < hash->capacidad; i++){
        while (!lista_vacia(hash->tabla[i])){
            par_auxiliar = (par_t*)lista_quitar_de_posicion(hash->tabla[i], 0);
            size_t posicion = funcion_hash(par_auxiliar->clave) % nueva_capacidad;
            lista_insertar(nueva_tabla[posicion], par_auxiliar);
        }
        lista_destruir(hash->tabla[i]);
    }

    free(hash->tabla);
    hash->tabla = nueva_tabla;
    hash->capacidad = nueva_capacidad;
    hash->cantidad = cantidad;
    return EXITOSO;
}

/*
* Copia la clave en un string auxiliar
*/
char* copiar_clave(const char* clave){
    if(!clave) return NULL;
    char* copia = malloc(strlen(clave)+1);
    strcpy(copia, clave);
    return copia;
}

/*
* Reserva memoria para el nodo que contiene la clave y el elemento.
*/
par_t* crear_par(const char* clave, void* elemento)
{
    if (!clave) return NULL;
    par_t* par_creado = malloc(sizeof(par_t));
    if (!par_creado) return NULL;
    par_creado->clave = copiar_clave(clave);
    par_creado->elemento = elemento;
    return par_creado;
}

/*
* Libera la memoria del par almacenado, la clave y el elemento aplicando el destructor. 
*/
void destruir_par(par_t* par, void (*destructor)(void *)){
    if(!par) return;
    if(destructor)
        destructor(par->elemento);
    free(par->clave);
    free(par);
}

int misma_clave(void *a, void *clave)
{
    return strcmp(((par_t *)a)->clave, (char *)clave);
}

/* ------------------------ Fin de funciones privadas ----------------------- */


hash_t *hash_crear(size_t capacidad)
{
    hash_t* hash = malloc(sizeof(hash_t));
    if(!hash) return NULL;

    if (capacidad < CAPACIDAD_MIN) {
        capacidad = CAPACIDAD_MIN;
    }
    
    lista_t** tabla = malloc(sizeof(lista_t)*capacidad);
    if(!tabla){
        free(hash);
        return NULL;
    }
    hash->tabla = tabla;

    for(size_t i = 0; i < capacidad; i++){
        lista_t *lista = lista_crear();
        if(!lista){
            free(hash);
            return NULL;
        }
        hash->tabla[i] = lista;
    }

    hash->capacidad = capacidad;
    hash->cantidad = 0;

    return hash;
}


hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento, void **anterior)
{
    if (!hash || !clave) return NULL;

    if((float)(hash->cantidad/hash->capacidad) >= CAPACIDAD_MAX_CARGA){
        if(funcion_rehash(hash) == ERROR){
            return NULL;
        }
    }

    par_t *par = crear_par(clave, elemento);
    size_t posicion = (size_t)funcion_hash(clave) % hash->capacidad;
    
    bool se_agrego_par = false;
    size_t cantidad_elementos = lista_tamanio(hash->tabla[posicion]);
    for(size_t i = 0; i < cantidad_elementos; i++){
        par_t* par_en_tabla = (par_t*)lista_elemento_en_posicion(hash->tabla[posicion], i);
        if(par_en_tabla){
            if(strcmp(par->clave, par_en_tabla->clave) == 0){
                par_t * par_sacado = lista_reemplazar_elemento(hash->tabla[posicion], par, misma_clave, par->clave);
                if(anterior){
                    *anterior = par_sacado->elemento;
                }
                destruir_par(par_sacado, NULL);
                se_agrego_par = true;
            }
        }
    }
    if(!se_agrego_par){
        lista_insertar(hash->tabla[posicion], par);
        if(anterior){
            *anterior = NULL;
        }
        hash->cantidad++;
    }

    return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
    if (!hash || !clave) return NULL;

    par_t *auxiliar = crear_par(clave, NULL);

    size_t posicion = funcion_hash(clave) % hash->capacidad;
    par_t *resultado = lista_buscar_y_quitar(hash->tabla[posicion], auxiliar, misma_clave, auxiliar->clave);

    destruir_par(auxiliar, NULL);
    if(resultado){
        void *elemento = resultado->elemento;
        destruir_par(resultado, NULL);
        hash->cantidad--;
        return elemento;
    }else{
        return NULL;
    }
}

void *hash_obtener(hash_t *hash, const char *clave)
{
    if(!hash || !clave) return NULL;

    size_t posicion = funcion_hash(clave) % hash->capacidad;
    par_t *par = lista_buscar_elemento(hash->tabla[posicion], misma_clave, (void *)clave);

    if(!par){
        return NULL;
    }else{
        return par->elemento;
    }
}

bool hash_contiene(hash_t *hash, const char *clave)
{
    if (!hash || !clave) return NULL;

    void *elemento = hash_obtener(hash, clave);

    return elemento?true:false;
}

size_t hash_cantidad(hash_t *hash)
{
    if (!hash) return 0;
    return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
    hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
    if (!hash) return;
    par_t* par_a_eliminar = NULL;
    size_t capacidad = hash->capacidad;
    for (size_t i = 0; i < capacidad; i++) {
        while (!lista_vacia(hash->tabla[i])) {
            par_a_eliminar = (par_t *)lista_quitar_de_posicion(hash->tabla[i], 0);
            destruir_par(par_a_eliminar, destructor);
        }
        lista_destruir(hash->tabla[i]);
    }
    free(hash->tabla);
    free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
    if(!hash || !f){
        return 0;
    }
    bool seguir_iterando = true;
    size_t iterados = 0;
    lista_iterador_t *it = NULL;
    for(int i = 0; i < hash->capacidad; i++){
        for (it = lista_iterador_crear(hash->tabla[i]); lista_iterador_elemento_actual(it) != NULL && seguir_iterando; lista_iterador_avanzar(it)) {
            par_t *actual = lista_iterador_elemento_actual(it);
            seguir_iterando = f(actual->clave, actual->elemento, aux);
            iterados++;
        }
        lista_iterador_destruir(it);
    }
    return iterados;
}
