#include "huffman_heap.h"
#include <string.h>
#include "compressor.h"

void imprimeArvoreInOrder(NoHuffman* raiz, int nivel) {
    if (raiz == NULL) {
        return;
    }
    
    imprimeArvoreInOrder(raiz->esquerdo, nivel + 1);
    
    for (int i = 0; i < nivel; i++) {
        printf("  ");
    }
    
    if (raiz->caractere == '\0') {
        printf("+-- INTERNO (freq=%d)\n", raiz->frequencia);
    } else {
        if (raiz->caractere == '\n') {
            printf("+-- '\\n' (freq=%d)\n", raiz->frequencia);
        } else if (raiz->caractere == '\t') {
            printf("+-- '\\t' (freq=%d)\n", raiz->frequencia);
        } else if (raiz->caractere == ' ') {
            printf("+-- ' ' (freq=%d)\n", raiz->frequencia);
        } else {
            printf("+-- '%c' (freq=%d)\n", raiz->caractere, raiz->frequencia);
        }
    }
    
    imprimeArvoreInOrder(raiz->direito, nivel + 1);
}

void imprimirArvore(NoHuffman* raiz) {
    if (raiz == NULL) {
        printf("Arvore vazia!\n");
        return;
    }
    
    printf("\n=== ARVORE DE HUFFMAN ===\n\n");
    imprimeArvoreInOrder(raiz, 0);
    printf("==========================\n\n");
}

int main() {
    // Se esta linha não imprimir, o arquivo não está sendo compilado!
    printf("=== TESTE DA FEATURE 2: CONSTRUCAO DA ARVORE ===\n\n");
    
    const char* nomeArquivo = "texto_teste.txt";
    int frequencias[256];
    
    printf("Lendo arquivo: %s...\n", nomeArquivo);
    if (!contaFrequenciasArquivo(nomeArquivo, frequencias)) {
        printf("FALHA: Nao foi possivel ler o arquivo.\n");
        return 1;
    }
    printf("Frequencias contadas com sucesso.\n\n");
    
    printf("Construindo a arvore de Huffman...\n");
    NoHuffman* raiz = constroiArvoreHuffman(frequencias);
    
    if (raiz == NULL) {
        printf("ERRO: Falha ao construir a arvore.\n");
        return 1;
    }
    
    printf("\nEstrutura final da arvore gerada:\n");
    imprimirArvore(raiz);

    printf("\nGerando codigos...\n");
    gerarCodigos(raiz);

    printf("\nTabela de codigos:\n");
    imprimirCodigos();

    /* Teste da serializacao */

    FILE *cab = fopen("cabecalho.huff", "wb");

    if(cab != NULL)
    {
        serializarCabecalho(raiz, cab);
        fclose(cab);

        printf("\nCabecalho gravado com sucesso!\n");
    }
    else
    {
        printf("\nErro ao criar o arquivo do cabecalho!\n");
    }
    
    liberaNohHuffman(raiz);
    printf("\nMemoria liberada. Teste concluido com sucesso!\n");
    

    /* Teste completo: comprime e descomprime o arquivo de verdade */
    printf("\n=== TESTE DE COMPRESSAO COMPLETA ===\n");

    if (comprimeArquivo("texto_teste.txt", "texto_teste.huff")) {
        printf("Arquivo comprimido com sucesso: texto_teste.huff\n");
    } else {
        printf("ERRO ao comprimir o arquivo.\n");
    }

    if (descomprimeArquivo("texto_teste.huff", "texto_teste_saida.txt")) {
        printf("Arquivo descomprimido com sucesso: texto_teste_saida.txt\n");
    } else {
        printf("ERRO ao descomprimir o arquivo.\n");
    }

    return 0;
}