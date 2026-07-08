#include "huffman_heap.h"

/*
 ============================================================================
 IMPLEMENTAÇÃO DAS FUNÇÕES PARA MANIPULAÇÃO DOS NÓS
 ============================================================================
 */

NoHuffman* criaNohHuffman(char caractere, int frequencia, 
                           NoHuffman* esquerdo, NoHuffman* direito) {
    NoHuffman* novoNo = (NoHuffman*)malloc(sizeof(NoHuffman));
    if (novoNo == NULL) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para no Huffman\n");
        return NULL;
    }
    
    novoNo->caractere = caractere;
    novoNo->frequencia = frequencia;
    novoNo->esquerdo = esquerdo;
    novoNo->direito = direito;
    
    return novoNo;
}

void liberaNohHuffman(NoHuffman* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    // Libera recursivamente os filhos (pós-ordem)
    liberaNohHuffman(raiz->esquerdo);
    liberaNohHuffman(raiz->direito);
    
    // Libera o nó atual
    free(raiz);
}

/*
 ============================================================================
 IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES INTERNAS (PRIVADAS)
 ============================================================================
 */

static bool redimensionarHeap(FilaPrioridade* fila) {
    int novaCapacidade = fila->capacidade * 2;
    NoHuffman** novoHeap = (NoHuffman**)realloc(fila->heap, 
                                                  novaCapacidade * sizeof(NoHuffman*));
    
    if (novoHeap == NULL) {
        fprintf(stderr, "ERRO: Falha ao redimensionar heap\n");
        return false;
    }
    
    fila->heap = novoHeap;
    fila->capacidade = novaCapacidade;
    return true;
}

static void trocarNos(NoHuffman** heap, int i, int j) {
    NoHuffman* temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

static void subir(FilaPrioridade* fila, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        
        if (fila->heap[indice]->frequencia < fila->heap[pai]->frequencia) {
            trocarNos(fila->heap, indice, pai);
            indice = pai;
        } else {
            break;
        }
    }
}

static void descer(FilaPrioridade* fila, int indice) {
    int tamanho = fila->tamanho;
    NoHuffman** heap = fila->heap;
    
    while (indice < tamanho) {
        int esquerdo = 2 * indice + 1;
        int direito = 2 * indice + 2;
        int menor = indice;
        
        if (esquerdo < tamanho && heap[esquerdo]->frequencia < heap[menor]->frequencia) {
            menor = esquerdo;
        }
        
        if (direito < tamanho && heap[direito]->frequencia < heap[menor]->frequencia) {
            menor = direito;
        }
        
        if (menor == indice) {
            break;
        }
        
        trocarNos(heap, indice, menor);
        indice = menor;
    }
}

/*
 ============================================================================
 IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS DA FILA DE PRIORIDADE
 ============================================================================
 */

FilaPrioridade* criaFilaPrioridade(int capacidadeInicial) {
    if (capacidadeInicial <= 0) {
        capacidadeInicial = 10;
    }
    
    FilaPrioridade* fila = (FilaPrioridade*)malloc(sizeof(FilaPrioridade));
    if (fila == NULL) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para fila\n");
        return NULL;
    }
    
    fila->heap = (NoHuffman**)malloc(capacidadeInicial * sizeof(NoHuffman*));
    if (fila->heap == NULL) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para heap\n");
        free(fila);
        return NULL;
    }
    
    fila->tamanho = 0;
    fila->capacidade = capacidadeInicial;
    
    return fila;
}

bool estaVazia(FilaPrioridade* fila) {
    if (fila == NULL) {
        return true;
    }
    return (fila->tamanho == 0);
}

int tamanhoFila(FilaPrioridade* fila) {
    if (fila == NULL) {
        return 0;
    }
    return fila->tamanho;
}

bool insere(FilaPrioridade* fila, NoHuffman* no) {
    if (fila == NULL || no == NULL) {
        fprintf(stderr, "ERRO: Parametros invalidos para insere()\n");
        return false;
    }
    
    // Redimensiona se necessário
    if (fila->tamanho >= fila->capacidade) {
        if (!redimensionarHeap(fila)) {
            return false;
        }
    }
    
    // Insere no final e sobe
    fila->heap[fila->tamanho] = no;
    fila->tamanho++;
    subir(fila, fila->tamanho - 1);
    
    return true;
}

NoHuffman* removeMenor(FilaPrioridade* fila) {
    if (fila == NULL || estaVazia(fila)) {
        fprintf(stderr, "AVISO: Tentativa de remover de fila vazia\n");
        return NULL;
    }
    
    // Guarda o menor elemento (raiz)
    NoHuffman* menor = fila->heap[0];
    
    // Coloca o último elemento na raiz
    fila->tamanho--;
    if (fila->tamanho > 0) {
        fila->heap[0] = fila->heap[fila->tamanho];
        descer(fila, 0);
    }
    
    return menor;
}

void liberaFilaPrioridade(FilaPrioridade* fila) {
    if (fila == NULL) {
        return;
    }
    
    // Libera o array da heap (mas NÃO os nós)
    free(fila->heap);
    
    // Libera a estrutura da fila
    free(fila);
}

/*
 ============================================================================
 FUNÇÕES DE DEBUG
 ============================================================================
 */

void imprimeFila(FilaPrioridade* fila) {
    if (fila == NULL || estaVazia(fila)) {
        printf("Fila vazia\n");
        return;
    }
    
    printf("FILA (tamanho=%d, capacidade=%d):\n", fila->tamanho, fila->capacidade);
    for (int i = 0; i < fila->tamanho; i++) {
        NoHuffman* no = fila->heap[i];
        if (no->caractere == '\0') {
            printf("  [%d] INTERNO (freq=%d)\n", i, no->frequencia);
        } else {
            printf("  [%d] '%c' (freq=%d)\n", i, no->caractere, no->frequencia);
        }
    }
}
/*
 ============================================================================
 CONTAGEM DE FREQUÊNCIAS E ÁRVORE
 ============================================================================
 */

bool contaFrequenciasArquivo(const char* nomeArquivo, int* frequencias) {
    // 1. Inicializa o array de frequências com zeros
    for (int i = 0; i < 256; i++) {
        frequencias[i] = 0;
    }
    
    // 2. Abre o arquivo em modo leitura de texto ("r")
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        fprintf(stderr, "ERRO: Nao foi possivel abrir o arquivo %s\n", nomeArquivo);
        return false;
    }
    
    int c;
    while ((c = fgetc(arquivo)) != EOF) {
        frequencias[c]++;
    }
    
    fclose(arquivo);
    return true;
}

NoHuffman* constroiArvoreHuffman(int* frequencias) {
    // Cria uma fila de prioridade com capacidade máxima de 256 (total de caracteres ASCII)
    FilaPrioridade* fila = criaFilaPrioridade(256);
    if (fila == NULL) return NULL;
    
    // 1. Transforma os caracteres encontrados em nós folha e insere na Min-Heap
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            NoHuffman* no = criaNohHuffman((char)i, frequencias[i], NULL, NULL);
            insere(fila, no);
        }
    }
    
    while (tamanhoFila(fila) > 1) {
        NoHuffman* menor1 = removeMenor(fila);
        NoHuffman* menor2 = removeMenor(fila);
        
        int somaFreq = menor1->frequencia + menor2->frequencia;
        
        // Cria um nó interno (indicado pelo caractere '\0')
        NoHuffman* novoPai = criaNohHuffman('\0', somaFreq, menor1, menor2);
        
        insere(fila, novoPai);
    }
    
    // 3. O último nó que sobra na fila é a raiz da Árvore de Huffman completa
    NoHuffman* raiz = removeMenor(fila);
    
    // A árvore está construída, podemos limpar a estrutura da fila da memória
    liberaFilaPrioridade(fila);
    
    return raiz;
}