#ifndef HUFFMAN_HEAP_H
#define HUFFMAN_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct NoHuffman {
    char caractere;              // Caractere ('\0' para nós internos)
    int frequencia;              // Frequência ou soma das frequências
    struct NoHuffman* esquerdo;  // Filho esquerdo
    struct NoHuffman* direito;   // Filho direito
} NoHuffman;

typedef struct {
    NoHuffman** heap;        // Array de ponteiros para os nós
    int tamanho;             // Quantos elementos estão na fila
    int capacidade;          // Capacidade alocada para o array
} FilaPrioridade;

NoHuffman* criaNohHuffman(char caractere, int frequencia, 
                           NoHuffman* esquerdo, NoHuffman* direito);

void liberaNohHuffman(NoHuffman* raiz);

FilaPrioridade* criaFilaPrioridade(int capacidadeInicial);

bool estaVazia(FilaPrioridade* fila);

int tamanhoFila(FilaPrioridade* fila);

bool insere(FilaPrioridade* fila, NoHuffman* no);

NoHuffman* removeMenor(FilaPrioridade* fila);

void liberaFilaPrioridade(FilaPrioridade* fila);

void imprimeFila(FilaPrioridade* fila);

// Funções de: Contagem e Construção da Árvore
bool contaFrequenciasArquivo(const char* nomeArquivo, int* frequencias);
NoHuffman* constroiArvoreHuffman(int* frequencias);

#define MAX_CODIGO 256

typedef struct{
    char codigo[MAX_CODIGO];
} CodigoHuffman;

extern CodigoHuffman tabelaCodigos[256];

void gerarCodigos(NoHuffman* raiz);
void imprimirCodigos(void);
void serializarCabecalho(NoHuffman* raiz, FILE* arquivo);

#endif // HUFFMAN_HEAP_H