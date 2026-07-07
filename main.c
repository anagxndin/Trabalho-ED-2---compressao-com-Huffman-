#include "huffman_heap.h"
#include <string.h>

void contarFrequencias(const char* texto, int* frequencias) {
    // Inicializa todas as frequências com 0
    for (int i = 0; i < 256; i++) {
        frequencias[i] = 0;
    }
    
    // Conta cada caractere
    for (int i = 0; texto[i] != '\0'; i++) {
        unsigned char c = (unsigned char)texto[i];
        frequencias[c]++;
    }
}

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
        printf("Árvore vazia!\n");
        return;
    }
    
    printf("\n=== ARVORE DE HUFFMAN ===\n\n");
    imprimeArvoreInOrder(raiz, 0);
    printf("==========================\n\n");
}

int main() {
    printf("=== TESTE DO TAD ARVORE DE HUFFMAN ===\n\n");
    
    // 1. Exemplo com texto simples
    const char* texto = "PARALELEPIPEDO";
    printf("Texto original: %s\n", texto);
    printf("Tamanho: %lu caracteres\n\n", strlen(texto));
    
    // 2. Contar frequências
    int frequencias[256] = {0};
    contarFrequencias(texto, frequencias);
    
    // 3. Criar fila de prioridade
    FilaPrioridade* fila = criaFilaPrioridade(10);
    if (fila == NULL) {
        printf("Erro ao criar fila\n");
        return 1;
    }
    
    // 4. Inserir nós para cada caractere com frequência > 0
    printf("Inserindo caracteres na fila:\n");
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            char c = (char)i;
            NoHuffman* no = criaNohHuffman(c, frequencias[i], NULL, NULL);
            if (no != NULL) {
                insere(fila, no);
                printf("  '%c' (freq=%d)\n", c, frequencias[i]);
            }
        }
    }
    printf("\n");
    
    // 5. Mostrar estado inicial da fila
    printf("Fila inicial:\n");
    imprimeFila(fila);
    printf("\n");
    
    // 6. Construir a árvore de Huffman
    printf("Construindo arvore de Huffman...\n");
    int passo = 1;
    while (tamanhoFila(fila) > 1) {
        // Remove os dois menores
        NoHuffman* n1 = removeMenor(fila);
        NoHuffman* n2 = removeMenor(fila);
        
        // Cria nó interno
        int somaFreq = n1->frequencia + n2->frequencia;
        NoHuffman* novo = criaNohHuffman('\0', somaFreq, n1, n2);
        
        // Insere de volta na fila
        insere(fila, novo);
        
        printf("  Passo %d: %d + %d = %d\n", passo, n1->frequencia, n2->frequencia, somaFreq);
        passo++;
    }
    printf("\n");
    
    // 7. A raiz da árvore é o último elemento
    NoHuffman* raiz = removeMenor(fila);
    
    printf("Arvore construida com sucesso!\n");
    printf("Raiz: frequencia total = %d\n\n", raiz->frequencia);
    
    // 8. Função recursiva para imprimir a árvore (in-order)
    printf("Estrutura da arvore (in-order):\n");
    imprimirArvore(raiz); 
    
    // 9. Liberar memória
    printf("\nLiberando memoria...\n");
    liberaNohHuffman(raiz);        // Libera toda a árvore
    liberaFilaPrioridade(fila);    // Libera a fila (já vazia)
    
    printf("Memoria liberada com sucesso!\n");
    
    // 10. Teste adicional: fila vazia
    printf("\n--- Teste com fila vazia ---\n");
    FilaPrioridade* filaVazia = criaFilaPrioridade(5);
    printf("Fila vazia? %s\n", estaVazia(filaVazia) ? "Sim" : "Não");
    printf("Tamanho: %d\n", tamanhoFila(filaVazia));
    
    NoHuffman* removido = removeMenor(filaVazia);
    if (removido == NULL) {
        printf("Removeu corretamente de fila vazia (retornou NULL)\n");
    }
    
    liberaFilaPrioridade(filaVazia);
    
    printf("\n=== FIM DO TESTE ===\n");
    return 0;
}