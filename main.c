#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

// argc -> quantidade de argumentos
// arg -> lista dos argumentos (textos)
int main(int argc, char *argv[]) {
    // verifica se o usuario digitou o tamanho da memoria
    if (argc != 2) {
        printf("Uso: %s <tamanho_total_memoria>\n", argv[0]);
        return 1;
    }

    // converte o texto "1024" para o número inteiro 1024
    int mem_size = atoi(argv[1]);
    
    // verifica se nao digitou numero negativo ou zero
    if (mem_size <= 0) {
        printf("Erro: O tamanho da memória deve ser positivo.\n");
        return 1;
    }

    // inicializa o gerenciador
    init_memory(mem_size);

    // Variaveis para ler o comando do usuario
    char command[100]; // guardar a linha digitada
    char type[10];     // guardar "RQ", "RL", etc.
    char process[10];  // guardar "P0", "P1"
    int size;          // guardar o tamanho pedido
    char strategy;     // guardar 'F', 'B', ou 'W'

    // loop infinito -> o programa roda para sempre ateh receber comando de saida (X)
    while (1) {
        printf("alocador> ");
        
        // fgets -> le o que o usuario digitou ateh ele apertar Enter
        if (fgets(command, sizeof(command), stdin) == NULL) break;

        // fgets leh o "Enter" (\n)
        // ./alocardor "string\n"
        // essa linha substitui o \n por \0 (fim de string) para limpar o texto.
        command[strcspn(command, "\n")] = 0;

        // strtok: separar string por espacos
        // pegar a primeira parte (ex: "RQ")
        char *token = strtok(command, " ");
        if (token == NULL) continue; // de digitou nada, volta pro começo
        strcpy(type, token);

        // verifica qual foi o comando
        if (strcmp(type, "X") == 0) {
            break; // sai do loop (encerra o programa)
        } 
        else if (strcmp(type, "RQ") == 0) {
            // RQ precisa de mais 3 partes: Nome, Tamanho, Estrategia
            char *p = strtok(NULL, " "); // corta proxima parte (Nome)
            char *s = strtok(NULL, " "); // corta proxima parte (Tamanho)
            char *st = strtok(NULL, " "); // corta proxima parte (Estrategia)

            // Se as 3 partes existirem...
            if (p && s && st) {
                strcpy(process, p);
                size = atoi(s); // converte texto do tamanho para numero
                strategy = st[0]; // pega soh a primeira letra ('F', 'B', 'W')
                request_memory(process, size, strategy);
            } else {
                printf("Comando RQ inválido. Uso: RQ <processo> <tamanho> <estrategia>\n");
            }
        } 
        else if (strcmp(type, "RL") == 0) {
            // RL precisa de mais 1 parte: Nome
            char *p = strtok(NULL, " ");
            if (p) {
                strcpy(process, p);
                release_memory(process);
            } else {
                printf("Comando RL inválido. Uso: RL <processo>\n");
            }
        } 
        else if (strcmp(type, "C") == 0) {
            compact_memory();
        } 
        else if (strcmp(type, "STAT") == 0) {
            print_status();
        } 
        else {
            printf("Comando desconhecido.\n");
        }
    }

    // antes de fechar o programa limpa tudo
    cleanup_memory();
    return 0;
}