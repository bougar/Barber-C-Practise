#include <stdlib.h>
#include <stdio.h>
#include "cola.h"
void crear_cola (cola *c,int max) {
    struct cabezera * tmp = malloc(sizeof(struct cabezera));
    if (tmp == NULL) {
        printf("memoria agotada\n");
        exit(EXIT_FAILURE);
    }
    tmp->primero = NULL;
    tmp->ultimo = NULL;
    tmp->tam=max;
    tmp->pos=0;
    *c = tmp;
}

int cola_vacia (cola c) {
    return (c->pos == 0);
}
int cola_llena(cola c){
	return (c->pos>=c->tam);
}
int insertar (void * x, cola * c){
	cola p = *c;
    pnodo tmp = (pnodo) malloc(sizeof(struct nodo));
    if (tmp == NULL) {
        printf("memoria agotada\n");
        exit (EXIT_FAILURE);
    }
    if (p->pos >= p->tam)
    	return 0;
    tmp->elemento = x;
    tmp->siguiente = NULL;
    if ((p->primero)==NULL)
    	p->primero=tmp;
    if (p->ultimo!=NULL)
    	p->ultimo->siguiente=tmp;
    (p->pos)++;
    p->ultimo=tmp;
    return 1;
}

iterator primero(cola c){
	return c->primero;
}

void * siguiente(iterator * p){
	void * tmp;
	tmp = (*p)->elemento;
	(*p)=((*p)->siguiente);
	return tmp;
}

void * obtener_ultimo_dato(cola c){
	return c->ultimo->elemento;
}

int isLast(iterator i){
	return !(i == NULL);
}

void * eliminar(cola * c){
	void * tmp;
	pnodo nodo_aux;
	((*c)->pos)--;
	tmp=(*c)->primero->elemento;
	nodo_aux=(*c)->primero->siguiente;
	free((*c)->primero);
	(*c)->primero=nodo_aux;
	return tmp;
}
void eliminar_dato(cola * c,void * d){
	pnodo aux;
	aux=(*c)->primero;
	if (aux->elemento==d){
		(*c)->primero=aux->siguiente;
		free(aux);
		((*c)->pos)--;
		return;
	}	
	while(aux->siguiente!=NULL && aux->siguiente->elemento!=d){
		aux=aux->siguiente;
	}
	if(aux->siguiente==NULL)
		return;
	pnodo tmp;
	tmp=aux->siguiente;
	aux->siguiente=tmp->siguiente;
	free(tmp);
}






































