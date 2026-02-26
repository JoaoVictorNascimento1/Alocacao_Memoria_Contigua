#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

// estrutura Block
typedef struct Block {
    char process[10];     // Nome do processo (ex: "P0") ou "HOLE" (vazio)
    int start;            // Endereço inicial
    int end;              // Endereço final
    struct Block *next;   // Próximo bloco da lista
} Block;

#endif