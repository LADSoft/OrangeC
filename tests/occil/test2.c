#define NULL ((void*)0)

void printf(char *, ...);
void scanf(char *, ...);
void exit(int);
void* malloc(unsigned);
void free(void*);

// -------------------------------------------------------------

typedef struct _pilha_
{
	int tamanho;
	int totaldeelementos;
	int* pilha;
} pilha;

pilha* pl_inicializapilha(int tamanho);
void pl_push(pilha* pl, int elemento, int* sucesso);
int pl_pop(pilha* pl, int* sucesso);
int pl_topo(pilha *pl, int* sucesso);
int pl_base(pilha* pl, int *sucesso);
int pl_pilhacheia(pilha* pl);
int pl_pilhavazia(pilha* pl);
void pl_liberapilha(pilha *pl);
void pl_esvaziapilha(pilha *pl);

pilha* pl_inicializapilha(int tamanho)
{
	pilha* pl = (pilha*)malloc(sizeof(pilha));
	pl->pilha = (int*)malloc(sizeof(int) * tamanho);
	pl->tamanho = tamanho;
	pl->totaldeelementos = 0;
	return pl;
}

void pl_push(pilha* pl, int elemento, int* sucesso)
{           
  if (!pl_pilhacheia(pl))
	{
		pl->pilha[pl->totaldeelementos++] = elemento;   
    /*
    void* voidTemp
    int totalDeElementos
    
    // voidTemp = (void*)(pl + 4)
    ldarg 'pl'
    ldc.i4 4
    add
    stloc
    
    // totalDeElementos = *(int*)voidTemp
    ldLoc voidTemp
    ldind.i4
    stloc totalDeElementos
    
    // *(int*)voidTemp = totalDeElementos + 1
    ldloc voidTemp
    ldloc totalDeElementos
    ldc.i4 1
    add
    stind.i4
    
    // *(int*) ((totalDeElementos << 2) + (int) *(uint*)(pl + 8)) = elemento;
    ldloc totalDeElementos
    ldc.i4 2
    shl
    ldarg pl
    ldc.i4 8
    add
    ldind.u4
    add
    ldarg elemento
    stdind.i4
     */
		*sucesso = 1;
	}
	else
		*sucesso = 0;
}

int pl_pop(pilha* pl, int* sucesso)
{                 
	if (!pl_pilhavazia(pl))
	{                
		*sucesso = 1;
		return pl->pilha[--pl->totaldeelementos];
	}
	else
	{
		*sucesso = 0;
		return -1;
	}
}

int pl_topo(pilha *pl, int* sucesso)
{
	if (pl_pilhavazia(pl))
	{
		*sucesso = 0;
		return -1;
	}
	else
	{
		*sucesso = 1;
		return pl->pilha[pl->totaldeelementos-1];
	}
}

int pl_base(pilha* pl, int *sucesso)
{
	if (pl_pilhavazia(pl))
	{
		*sucesso = 0;
		return -1;
	}
	else
	{
		*sucesso = 1;
		return pl->pilha[0];
	}
}

int pl_pilhacheia(pilha* pl)
{
  /*
   int tmpValue
  
   ldloc   'pl'
   ldc.i4  4
   add
   ldind.i4
   ldloc   'pl'
   ldind.i4
   blt L_ZERO
   ldc.i4  1
   stloc   tmpValue
   br  L_ONE
   
L_ZERO:
   ldc.i4  0
   stloc tmpValue

L_ONE:
   ldloc tmpValue
   ret
   */
  return pl->totaldeelementos >= pl->tamanho;
}

int pl_pilhavazia(pilha* pl)
{
	return pl->totaldeelementos == 0;
}

void pl_liberapilha(pilha* pl)
{
	free(pl->pilha);
	free(pl);
}

void pl_esvaziapilha(pilha *pl)
{
  pl->pilha = malloc(sizeof(int) * pl->tamanho);
	pl->totaldeelementos = 0;
}


int main()
{
	int i;
	int sucesso = 1;
	pilha* pl = pl_inicializapilha(5);  
	printf("---------- Empilhando ----------\n");
	for (i = 0; (sucesso == 1); i++)
	{
		int valor = i * 10;
		pl_push(pl, valor, &sucesso);
		if (sucesso == 1)
			printf("%d\n", valor);
	}
	printf("-----------------------------------\n\n");
  
  printf("-----------------------------------\n");
	printf("TOPO = %d\n", pl_topo(pl, &sucesso));
	printf("BASE = %d\n", pl_base(pl, &sucesso));
	printf("-----------------------------------\n\n");

	printf("---------- Desempilhando ----------\n");
	while (1)
	{
		int valor = pl_pop(pl, &sucesso);
		if (sucesso == 0)
			break;
		printf("%d\n", valor);
	}
	printf("-----------------------------------\n");
  
	pl_liberapilha(pl);
	return 0;
}