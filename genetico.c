#include <stdio.h>
#include <stdlib.h>


//PARÂMETROS
#define POPULACAO_TAM 20
#define NUM_SELECIONADO_POR_GERACAO 2
#define CHANCE_DE_MUTACAO 0.1
#define CRITERIO_DE_QUALIDADE 15
#define MAXIMO_GERACOES 1

// ESTRUTURA DE UM INDIVIDUO
typedef struct individuo
{
    int cromossomo[3];
    float fitness;
} individuo;

typedef struct populacao
{
    individuo *individuos;
    int num_geracao;
    float qualidade;
} populacao;

int compara_individuos(const void *a, const void *b)
{
    // FUNÇÃO AUXILIAR PARA ORDENAÇÃO DA POPULAÇÃO
    individuo *ind_a = (individuo *)a;
    individuo *ind_b = (individuo *)b;
    if (ind_a->fitness > ind_b->fitness)
        return 1;
    else if (ind_a->fitness < ind_b->fitness)
        return -1;
    else
        return 0;
}

// IMPLEMENTAÇÃO DO ALGORITMO GENETICO
void ordena_populacao_qsort(populacao *pop)
{
    qsort(pop->individuos, POPULACAO_TAM, sizeof(individuo), compara_individuos);
}

float fitness(individuo ind)
{
    float fitness_n = (ind.cromossomo[0] + ind.cromossomo[1] + ind.cromossomo[2]) / 3;
    return fitness_n;
}

float calcula_qualidade_populacao(populacao *pop)
{
    int i;
    float qualidade = 0;
    for (i = 0; i < POPULACAO_TAM; i++)
    {
        qualidade += pop->individuos[i].fitness;
    }
    qualidade = qualidade / POPULACAO_TAM; // QUANTO MENOR MELHOR
    return qualidade;
}

individuo *seleciona_melhores_individuos(populacao *pop)
{
    individuo *selecionados = (individuo *)malloc(NUM_SELECIONADO_POR_GERACAO * sizeof(individuo));
    int *verifica_selecionado = (int *)malloc(POPULACAO_TAM * sizeof(int));
    for (int i = 0; i < POPULACAO_TAM; i++)
        verifica_selecionado[i] = 0;
    ordena_populacao_qsort(pop);

    for (int i = 0; i < NUM_SELECIONADO_POR_GERACAO / 2; i++)
    {
        selecionados[i] = pop->individuos[i];
        verifica_selecionado[i] = 1;
    }

    for (int i = 0; i < NUM_SELECIONADO_POR_GERACAO / 2; i++)
    {
        int rand_index = (rand() % (POPULACAO_TAM / 4)) + 5; // SELECIONA ALEATORIAMENTE ENTRE OS 25% MELHORES PARA GARANTIR DIVERSIDADE
        if (verifica_selecionado[rand_index] == 1)
        { // GARANTE QUE NÃO HAVERÁ REPETIÇÕES DE FORMA EFICIENTE
            i--;
            continue;
        }
        selecionados[i + NUM_SELECIONADO_POR_GERACAO / 2] = pop->individuos[rand_index];
        verifica_selecionado[rand_index] = 1;
    }
    return selecionados;
}

individuo *inicializa_populacao()
{
    individuo *populacao = (individuo *)malloc(POPULACAO_TAM * sizeof(individuo));
    int i;
    for (i = 0; i < POPULACAO_TAM; i++)
    {
        populacao[i].cromossomo[0] = rand() % 255;
        populacao[i].cromossomo[1] = rand() % 255;
        populacao[i].cromossomo[2] = rand() % 255;
        populacao[i].fitness = fitness(populacao[i]); // AVALIA CADA INDIVIDUO
    }
    return populacao;
}

void imprime_populacao(populacao *pop)
{
    int i;
    for (i = 0; i < POPULACAO_TAM; i++)
    {
        printf("INDIVIDUO %d: %d %d %d - FITNESS: %.2f\n", i, pop->individuos[i].cromossomo[0], pop->individuos[i].cromossomo[1], pop->individuos[i].cromossomo[2], pop->individuos[i].fitness);
    }
}

void crossover(individuo *pai1, individuo *pai2, individuo *filho1)
{
    int ponto_corte = rand() % 3;
    for (int i = 0; i < 3; i++)
    {
        if (i < ponto_corte)
            filho1->cromossomo[i] = pai1->cromossomo[i];

        else
            filho1->cromossomo[i] = pai2->cromossomo[i];
    }
    filho1->fitness = fitness(*filho1);
}

void mutacao(individuo *ind)
{
    for (int i = 0; i < 3; i++)
    {
        if (((rand() % 100) / 100.0) < CHANCE_DE_MUTACAO)
        {
            ind->cromossomo[i] = rand() % 255;
        }
    }
    ind->fitness = fitness(*ind);
}

void cria_nova_geracao(populacao *pop)
{
    individuo *selecionados = seleciona_melhores_individuos(pop);
    individuo *nova_geracao = (individuo *)malloc(POPULACAO_TAM * sizeof(individuo));
    nova_geracao[0] = selecionados[0];
    nova_geracao[1] = selecionados[1]; // MANTÉM OS 3 MELHORES INDIVIDUOS
    nova_geracao[2] = selecionados[2];
    for (int i = 0; i < POPULACAO_TAM - 3; i++)
    {
        individuo pai1 = selecionados[rand() % NUM_SELECIONADO_POR_GERACAO];
        individuo pai2 = selecionados[rand() % NUM_SELECIONADO_POR_GERACAO];
        individuo filho1;
        crossover(&pai1, &pai2, &filho1);
        mutacao(&filho1);
        nova_geracao[i] = filho1;
    }
    pop->individuos = nova_geracao;
    pop->qualidade = calcula_qualidade_populacao(pop);
    pop->num_geracao++;
}

void algoritmo_genetico(populacao *atual)
{
    int geracao = 0;
    populacao* melhor_pop;
    while(geracao < MAXIMO_GERACOES && atual->qualidade > CRITERIO_DE_QUALIDADE)
    {
        cria_nova_geracao(atual);
        geracao++;
    }
    printf("GERACAO %d - QUALIDADE: %.2f\n", atual->num_geracao, atual->qualidade);
}
int main()
{
    populacao *pop = (populacao *)malloc(sizeof(populacao));
    pop->individuos = inicializa_populacao();
    pop->qualidade = calcula_qualidade_populacao(pop);
    pop->num_geracao = 0;
    imprime_populacao(pop);
    
    algoritmo_genetico(pop);

    imprime_populacao(pop);

    return 0;
}
