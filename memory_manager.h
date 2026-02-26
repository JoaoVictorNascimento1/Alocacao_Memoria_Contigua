#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "memory_block.h"

// inicializa a memória com um único buraco do tamanho total
void init_memory(int size);

// solicitar memoria -> processo, tamanho, estrategia (F, B, W)
void request_memory(char *process, int size, char strategy);

// Libera memória de um processo específico
void release_memory(char *process);

// Compacta os buracos de memória
void compact_memory();

// Exibe o status atual da memória
void print_status();

// Libera toda a memória alocada pelo programa (limpeza final)
void cleanup_memory();

#endif