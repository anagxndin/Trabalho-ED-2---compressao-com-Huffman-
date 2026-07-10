#include "compressor.h"
#include <string.h>

/* Um struct só cuida de ler OU escrever bits, dependendo de qual função você chama */
typedef struct { FILE* f; unsigned char byte; int bits; } BitIO;

static void bitwOpen(BitIO* b, FILE* f) { b->f = f; b->byte = 0; b->bits = 0; }

static void bitwPut(BitIO* b, int bit) {
    b->byte = (b->byte << 1) | bit;
    if (++b->bits == 8) { fputc(b->byte, b->f); b->byte = 0; b->bits = 0; }
}

static void bitwFlush(BitIO* b) {
    if (b->bits) fputc(b->byte << (8 - b->bits), b->f); // completa com zeros (padding)
}

static void bitrOpen(BitIO* b, FILE* f) { b->f = f; b->byte = 0; b->bits = 8; }

static int bitrGet(BitIO* b) {
    if (b->bits == 8) {
        int c = fgetc(b->f);
        if (c == EOF) return -1;
        b->byte = (unsigned char)c;
        b->bits = 0;
    }
    return (b->byte >> (7 - b->bits++)) & 1;
}

/* Espelho de serializarCabecalho(): '1'+char = folha, '0' = interno */
static NoHuffman* leCabecalho(FILE* f) {
    int m = fgetc(f);
    if (m == EOF) return NULL;
    if (m == '1') return criaNohHuffman((char)fgetc(f), 0, NULL, NULL);
    NoHuffman* esq = leCabecalho(f);
    NoHuffman* dir = leCabecalho(f);
    return criaNohHuffman('\0', 0, esq, dir);
}

bool comprimeArquivo(const char* entrada, const char* saida) {
    int freq[256] = {0};
    if (!contaFrequenciasArquivo(entrada, freq)) return false;

    int total = 0;
    for (int i = 0; i < 256; i++) total += freq[i];
    if (total == 0) return false;

    NoHuffman* raiz = constroiArvoreHuffman(freq);
    gerarCodigos(raiz);

    FILE* out = fopen(saida, "wb");
    FILE* in = fopen(entrada, "r");
    if (!out || !in) {
        if (out) fclose(out);
        if (in) fclose(in);
        liberaNohHuffman(raiz);
        return false;
    }

    serializarCabecalho(raiz, out);
    fwrite(&total, sizeof(int), 1, out); // salva total de chars pra saber onde parar ao descomprimir

    BitIO b;
    bitwOpen(&b, out);
    int c;
    while ((c = fgetc(in)) != EOF) {
        for (char* p = tabelaCodigos[(unsigned char)c].codigo; *p; p++) {
            bitwPut(&b, *p - '0');
        }
    }
    bitwFlush(&b);

    fclose(in);
    fclose(out);
    liberaNohHuffman(raiz);
    return true;
}

bool descomprimeArquivo(const char* entrada, const char* saida) {
    FILE* in = fopen(entrada, "rb");
    if (!in) return false;

    NoHuffman* raiz = leCabecalho(in);
    int total;
    fread(&total, sizeof(int), 1, in);

    FILE* out = fopen(saida, "w");
    if (!out) { fclose(in); liberaNohHuffman(raiz); return false; }

    BitIO b;
    bitrOpen(&b, in);
    bool folha = (!raiz->esquerdo && !raiz->direito); // caso especial: 1 unico caractere

    for (int i = 0; i < total; i++) {
        if (folha) { fputc(raiz->caractere, out); continue; }
        NoHuffman* n = raiz;
        while (n->esquerdo || n->direito) {
            n = bitrGet(&b) ? n->direito : n->esquerdo;
        }
        fputc(n->caractere, out);
    }

    fclose(in);
    fclose(out);
    liberaNohHuffman(raiz);
    return true;
}