#include <stdio.h>
#include <stdlib.h>

#define bool int
#define true 1
#define false 0

// == linkedlist proto ==
typedef struct _node
{
	int index;
	int conteudo;
	struct _node* proximo;
	struct _node* anterior;
} noh;

typedef struct
{
	int indexcounter;
	int tamanho;
	noh* listanoh;
	noh* head;
	noh* tail;
} lista;

lista* ll_inicializalista();
void ll_inserirfinal(lista* lst, int valor);
void ll_inseririnicio(lista* lst, int valor);
void ll_liberalista(lista* lst);
void ll_removernoh(lista *lst, int index);
int ll_removertail(lista *lst);
int ll_removerhead(lista *lst);
void ll_removertudo(lista *lst);
noh* ll_head(lista *lst);
noh* ll_tail(lista *lst);
noh* ll_buscanoh(lista *lst, int index);
void ll_reindexar(lista *lst);
int ll_tamanho(lista *lst);

// == stack proto ==
typedef struct _pilha_l_
{
	int tamanho;
	lista* lst;
} pilha;

pilha* inicializapilha(int tamanho);
void pl_push(pilha* pl, int elemento, bool* sucesso);
int pl_pop(pilha* pl, bool* sucesso);
int pl_topo(pilha *pl, bool* sucesso);
int pl_base(pilha* pl, bool *sucesso);
int pl_tamanho(pilha* pl);
bool pl_pilhacheia(pilha* pl);
bool pl_pilhavazia(pilha* pl);
void pl_liberapilha(pilha *pl);
void pl_esvaziarpilha(pilha *pl);

// === implementation of linked-list ===
lista* ll_inicializalista()
{
	lista* tmp = (lista*)malloc(sizeof(lista));
	tmp->indexcounter = 0;
	tmp->tamanho = 0;
	tmp->listanoh = NULL;
	tmp->head = NULL;
	tmp->tail = NULL;
	return tmp;
}

noh* crianoh()
{
	noh* novo = (noh*)malloc(sizeof(noh));
	novo->conteudo = 0;
	novo->proximo = NULL;
	novo->anterior = NULL;
	return novo;
}

void ll_inserirfinal(lista* lst, int valor)
{
	noh* novo = crianoh();
	novo->conteudo = valor;

	if (lst->listanoh == NULL)
	{
		novo->index = lst->indexcounter++;
		lst->listanoh = lst->head = lst->tail = novo;
		lst->tamanho++;
	}
	else
	{
		noh* tmp = lst->tail;
		novo->index = lst->indexcounter++;
		lst->tail->proximo = novo;
		lst->tail = novo;		
		lst->tail->anterior = tmp;
		lst->tamanho++;
	}
}

void ll_inseririnicio(lista* lst, int valor)
{
	noh* novo = crianoh();
	novo->conteudo = valor;

	if (lst->listanoh == NULL)
	{
		novo->index = lst->indexcounter++;
		lst->listanoh = lst->head = lst->tail = novo;
		lst->tamanho++;
	}
	else
	{
		novo->index = lst->indexcounter++;
		novo->proximo = lst->head;
		lst->head->anterior = novo;
		lst->head = novo;
		lst->listanoh = lst->head;
		lst->tamanho++;
	}
}

void ll_removernoh(lista *lst, int index)
{
	noh* aux = NULL;
	noh* ant = NULL;

	if (lst->tamanho == 0)
		return;
    
  aux = lst->head;

	while (aux != NULL)
	{
		if (aux->index == index)
		{
			if (aux == lst->head)
			{
				lst->head = lst->head->proximo;
				lst->head->anterior = NULL;
				lst->listanoh = lst->head;
			}
			else if (aux == lst->tail)
			{
				ant->proximo = NULL;
				lst->tail = ant;
			}
			else
			{
				aux->proximo->anterior = ant;
				ant->proximo = aux->proximo;
			}

			free(aux);
			lst->tamanho--;
			break;
		}
		ant = aux;
		aux = aux->proximo;
	}
}

int ll_removertail(lista *lst)
{
	int valor = 0;
	valor = lst->tail->conteudo;

	if (lst->tail->anterior != NULL) 
	{
		noh* ant = lst->tail->anterior;
		ant->proximo = NULL;
		lst->tail = ant;
	}
	else // chegou no head
		lst->listanoh = lst->head = lst->tail = NULL;

	lst->tamanho--;
	return valor;
}

int ll_removerhead(lista *lst)
{
	int valor = lst->head->conteudo;
	if (lst->head->proximo != NULL)
	{
		lst->head = lst->head->proximo;
		lst->head->anterior = NULL;
	}
	else
		lst->head = lst->tail  = NULL;
	
	lst->listanoh = lst->head;
	lst->tamanho--;
	return valor;
}

void ll_removertudo(lista *lst)
{
	free(lst->tail);
	free(lst->head);

	lst->indexcounter = 0;
	lst->tamanho = 0;
	lst->listanoh = NULL;
	lst->head = NULL;
	lst->tail = NULL;
}

noh* ll_buscanoh(lista *lst, int index)
{
	noh* aux = lst->head;
	while (aux != NULL)
	{
		if (aux->index == index)
			return aux;
		aux = aux->proximo;
	}
	return NULL;
}

noh* ll_head(lista *lst)
{
	return lst->head;
}

noh* ll_tail(lista *lst)
{
	return lst->tail;
}

void ll_reindexar(lista *lst)
{
	noh* aux = lst->head;
	int i = 0;
	for (; aux != NULL;i++)
	{
		aux->index = i;
		aux = aux->proximo;
	}
	lst->indexcounter = i;
}

int ll_tamanho(lista *lst)
{
	return lst->tamanho;
}

void ll_liberalista(lista* lst)
{
	free(lst);
}

// === implementation of stack ===
pilha* inicializapilha(int tamanho)
{
	pilha* tmp = (pilha*)malloc(sizeof(pilha));
	tmp->tamanho = tamanho;
	tmp->lst = ll_inicializalista();
	return tmp;
}

void pl_push(pilha* pl, int elemento, bool* sucesso)
{
	if (pl_pilhacheia(pl))
	{
		*sucesso = false;
		return;
	}

	ll_inserirfinal(pl->lst, elemento);
	*sucesso = true;
}

int pl_pop(pilha* pl, bool* sucesso)
{
  int v;
	if (pl_pilhavazia(pl))
	{
		*sucesso = false;
		return -1;
	}

	v = ll_removertail(pl->lst);
	*sucesso = true;
	return v;
}

int pl_topo(pilha *pl, bool* sucesso)
{
	if (pl_pilhavazia(pl))
	{
		*sucesso = false;
		return -1;
	}
	return ll_tail(pl->lst)->conteudo;
}

int pl_base(pilha* pl, bool *sucesso)
{
	if (pl_pilhavazia(pl))
	{
		*sucesso = false;
		return -1;
	}
	return ll_head(pl->lst)->conteudo;
}

bool pl_pilhacheia(pilha* pl)
{
	return ll_tamanho(pl->lst) >= pl->tamanho;
}

bool pl_pilhavazia(pilha* pl)
{
	return ll_tamanho(pl->lst) == 0;
}

int pl_tamanho(pilha* pl)
{
	return ll_tamanho(pl->lst);
}

void pl_liberapilha(pilha* pl)
{
	ll_liberalista(pl->lst);
	free(pl);
}

void pl_esvaziarpilha(pilha *pl)
{
	ll_removertudo(pl->lst);
	pl->tamanho = 0;
}

// ========================================================

int main()
{
  int i;
  int valor;
	bool sucesso = true;
	pilha* pl = inicializapilha(20);
	printf("---------- Empilhando ----------\n");
	for (i = 0; (sucesso == true); i++)
	{
		valor = i * 10;
		pl_push(pl, valor, &sucesso);
		if (sucesso)
			printf("%d\n", valor);
	}
	printf("-----------------------------------\n\n");

	printf("-----------------------------------\n");
	printf("TOPO = %d\n", pl_topo(pl, &sucesso));
	printf("BASE = %d\n", pl_base(pl, &sucesso));
	printf("-----------------------------------\n\n");

	printf("---------- Desempilhando ----------\n");
	while (true)
	{
		int valor = pl_pop(pl, &sucesso);
		if (sucesso == false)
			break;
		printf("%d\n", valor);
	}
	printf("-----------------------------------\n");

	pl_liberapilha(pl);
	return 0;
}
