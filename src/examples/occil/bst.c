#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define bool int
#define true 1
#define false 0

typedef struct _tnode
{
	int conteudo;
	struct _tnode* direita;
	struct _tnode* esquerda;
} node;

node* ab_inserirnode(node* noh, int conteudo, bool* sucesso);
node* ab_buscarnode(node* noh, int conteudo, bool* sucesso);
int ab_deletarnode(node** raiz, int conteudo, bool* sucesso);

void ab_imprimiremordem(node* noh);
void ab_imprimirempreordem(node* noh);
void ab_imprimiremprosordem(node* noh);

void ab_liberaarvore(node* noh);

node* ab_novonode()
{
	node* tmp = (node*)malloc(sizeof(node));
	return tmp;
}

void ab_setanode(node* noh, int conteudo, node* esquerda, node* direita)
{
	noh->conteudo = conteudo;
	noh->esquerda = esquerda;
	noh->direita = direita;
}

node* ab_inserirnode(node* noh, int conteudo, bool* sucesso)
{
	if (noh == NULL)
	{
		noh = ab_novonode();
		ab_setanode(noh, conteudo, NULL, NULL);
	}
	else if (conteudo == noh->conteudo)
	{
		*sucesso = false;
		return NULL;
	}
	else if (conteudo < noh->conteudo)
		noh->esquerda = ab_inserirnode(noh->esquerda, conteudo, sucesso);
	else
		noh->direita = ab_inserirnode(noh->direita, conteudo, sucesso);

	*sucesso = true;
	return noh;
}

node* ab_buscarnode(node* noh, int conteudo, bool* sucesso)
{
	if (noh == NULL)
	{
		*sucesso = false;
		return NULL;
	}
	else if (conteudo == noh->conteudo)
		return noh;
	else if (conteudo < noh->conteudo)
		return ab_buscarnode(noh->esquerda, conteudo, sucesso);
	else
		return ab_buscarnode(noh->direita, conteudo, sucesso);
}

int ab_deletarnode(node** raiz, int conteudo, bool* sucesso)
{
	node* proximo;
	node* tmp;

	node** esquerda;
	node** p = raiz;

	while (true)
	{
		if (*p == NULL)
		{
			*sucesso = false;
			return -1;
		}
		else if (conteudo == (*p)->conteudo)
			break;
		else if (conteudo < (*p)->conteudo)
			p = &((*p)->esquerda);
		else
			p = &((*p)->direita);
	}

	if ((*p)->esquerda == NULL)
		proximo = (*p)->direita;
	else
	{
		esquerda = &((*p)->esquerda);
		while ((*esquerda)->direita != NULL)
			esquerda = &(*esquerda)->direita;
		proximo = *esquerda;
		*esquerda = (*esquerda)->esquerda;
		proximo->esquerda = (*p)->esquerda;
		proximo->direita = (*p)->direita;
	}
	tmp = *p;
	*p = proximo;
	free(tmp);
	*sucesso = true;
	return 0;
}

void ab_imprimeconteudo(node* noh)
{
	printf("%02d\n", noh->conteudo);
}

void ab_imprimiremordem(node* noh)
{
	if (noh)
	{
		ab_imprimiremordem(noh->esquerda);
		ab_imprimeconteudo(noh);
		ab_imprimiremordem(noh->direita);
	}
}

void ab_imprimirempreordem(node* noh)
{
	if (noh)
	{
		ab_imprimeconteudo(noh);
		ab_imprimiremordem(noh->esquerda);
		ab_imprimiremordem(noh->direita);
	}
}

void ab_imprimiremprosordem(node* noh)
{
	if (noh)
	{
		ab_imprimiremordem(noh->esquerda);
		ab_imprimiremordem(noh->direita);
		ab_imprimeconteudo(noh);
	}
}

void ab_liberaarvore(node* noh)
{
	if (noh)
	{
		ab_liberaarvore(noh->esquerda);
		ab_liberaarvore(noh->direita);
		free(noh);
	}
}

#define TAMANHO 50

int listaNumeros[TAMANHO]; // = { 0 };
bool valorExiste(int valor)
{
  int i;
	for (i = 0; i < TAMANHO; i++)
		if (listaNumeros[i] == valor)
			return true;
	return false;
}

int cmpfunc(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}


int main()
{
	node* raiz = NULL;
	//srand(time(NULL));
	int counter = 0;
	bool sucesso = false;
	while (1)
	{
		int numero = rand() % 500;
		if (!valorExiste(numero))
		{
			raiz = ab_inserirnode(raiz, numero, &sucesso);
			listaNumeros[counter] = numero;
			counter++;
		}
		else
			continue;

		if (counter >= TAMANHO)
			break;
	}

	ab_imprimiremordem(raiz);

	qsort(listaNumeros, TAMANHO, sizeof(int), cmpfunc);
	ab_deletarnode(&raiz, listaNumeros[TAMANHO-2], &sucesso);

	ab_imprimiremordem(raiz);

	ab_liberaarvore(raiz);

	return 0;
}
