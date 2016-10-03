#include <pthread.h>


struct nodo {
    void * elemento;
    struct nodo * siguiente;
};
typedef struct nodo *pnodo;
typedef pnodo iterator;
struct cabezera {
    pnodo primero;
    pnodo ultimo;
    int tam;
    int pos;
};
typedef struct cabezera * cola;
void crear_cola (cola *,int max);
int cola_vacia (cola);
int insertar (void *, cola *);
iterator primero (cola);
int cola_llena(cola);
void * eliminar(cola *);
void eliminar_dato(cola *,void *);
void * siguiente(iterator *); //Nos devuelve el elemento de la posición actual del iterador y posteriormente lo situa en la siguiente posción
void * obtener_ultimo_dato(cola l);//Obtención del dato de la última posición en O(1);
void liberar(cola * l,void f (void *)); //Función para la liberación de memoria
int isLast(iterator i);//Comprobe si ya acabamos la iteración de la cola
/*ERRORES: eliminar un elemento que no esta en la cola*/
