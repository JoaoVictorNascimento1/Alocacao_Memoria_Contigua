#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <limits.h>     
#include "memory_manager.h"

// variaveis globais (acessíveis por todas as funções deste arquivo)
Block *head = NULL;       // aponta para o primeiro bloco da lista / começa vazio
int total_memory_size = 0; // guarda o tamanho total da memoria simulada

// função auxiliar
// criar um novo bloco na memoria do computador
Block* create_block(char *process, int start, int end) {
    // pede ao Sistema Operacional um espaco na RAM para guardar os dados deste bloco
    // ponteiro que aponta para o primeiro endereco onde malloc criou o espaco
    Block *new_block = (Block*) malloc(sizeof(Block));
    
    // copia o nome do bloco (terminal) recebido para dentro da estrutura
    strcpy(new_block->process, process);
    
    // preenche os enderecos de inicio e fim
    new_block->start = start;
    new_block->end = end;
    
    // inicialmente nao aponta para ninguem (eh o fim da lista)
    new_block->next = NULL;
    
    // retorna o endereço de onde esse bloco foi criado na RAM
    return new_block;
}

// inicializa o sistema
void init_memory(int size) {
    total_memory_size = size;
    // Cria o primeiro bloco / um "HOLE" (local disponivel) que vai do zero até o tamanho total
    // (0 a 999 = 1000 numeros)
    head = create_block("HOLE", 0, size - 1);
}

// funcao auxiliar -> junta buracos vizinhos
void merge_holes() {
    Block *current = head;
    
    // percorre a lista enquanto houver um bloco atual E um proximo bloco
    while (current != NULL && current->next != NULL) {
        
        // se o bloco ATUAL estah vazio && o PROXIMO tambem estah vazio
        // strcmp compara strings se forem iguais retorna 0, se forem diferentes podem retornar >0 ou <0
        // <0 se a string1 for alfabeticamente maior que string2 e vice versa
        if (strcmp(current->process, "HOLE") == 0 && strcmp(current->next->process, "HOLE") == 0) {
            
            // juntar
            Block *temp = current->next; // guarda o endereco do proximo (que sera apagado)
            // [COISA start1 end1] ** current->[HOLE start2 end2] temp,next->[HOLE start3 end3] ** [COISA start4 end4]
            // current -> process: "HOLE" ; current->next->process, "HOLE"
            // temp aponta para o proximo bloco
            // estende o fim do bloco atual para cobrir o proximo
            current->end = temp->end; 
            // [COISA start1 end1] **current->[HOLE start2 end3] temp,next->[HOLE start3 end3]** [COISA start4 end4]

            // o atual pula o proximo e aponta para o seguinte
            current->next = temp->next;
            // [COISA start1 end1] **current->[HOLE start2 end3] temp->[HOLE start3 end3]** next->[COISA start4 end4]
            
            // devolve a memoria do bloco apagado para o computador (limpeza)
            free(temp);
            // [COISA start1 end1] **current->[HOLE start2 end3]** next->[COISA start4 end4]

            // pode ser vizinho de outro vazio a frente. O loop checara de novo
        } else {
            // se nao precisou, avanca para o proximo
            current = current->next;
        }
    }
}

//(RQ)
void request_memory(char *process, int size, char strategy) {
    Block *current = head;
    Block *best = NULL; // vai guardar o ponteiro para o "buraco escolhido"
    
    // variaveis para ajudar a comparar tamanhos
    int worst_size = -1;       // para achar o maior (começa pequeno)
    int best_size = INT_MAX;   // Para achar o menor (começa muito grande)
    
    // escolher o buraco (loop de busca)
    while (current != NULL) {
        // soh nos interessa se for um "HOLE" (buraco vazio)
        if (strcmp(current->process, "HOLE") == 0) {
            
            // calcula o tamanho real deste buraco
            int hole_size = current->end - current->start + 1;
            //[HOLE start(0) end(99)] -> hole_size = 100

            // se o buraco for grande o suficiente para o pedido
            if (hole_size >= size) {
                
                // estrategia F: first fit (primeiro encontrado)
                if (strategy == 'F') { 
                    best = current; // escolhe o primeiro -> current
                    break;          // para de procurar 0 -> achou
                } 
                // estrategia B: best fit (menor sobra)
                else if (strategy == 'B') { 
                    if (hole_size < best_size) {
                        best_size = hole_size; // atualiza o menor buraco encontrado
                        best = current;        // marca este como o melhor candidato agora
                    }
                } 
                // estrategia W: worst fit (maior sobra)
                else if (strategy == 'W') { 
                    if (hole_size > worst_size) {
                        worst_size = hole_size; // atualiza o maior buraco encontrado
                        best = current;         // marca este como o melhor candidato
                    }
                }
            }
        }
        current = current->next; // vai para o próximo bloco analisar
    }

    // Se varreu tudo e 'best' continua NULL, não achou lugar.
    if (best == NULL) {
        printf("Erro: Memória insuficiente para %s (tamanho %d, estratégia %c)\n", process, size, strategy);
        return;
    }

    // ocupar o espaco
    // agora que temos o bloco 'best' vamos usar
    // encontrar o tamanho do bloco 'best'
    int hole_size = best->end - best->start + 1;
    
    // cenario A -> o buraco tem EXATAMENTE o tamanho que precisa
    if (hole_size == size) {
        strcpy(best->process, process); // apenas muda o nome de "HOLE" para "P1"
    } 
    // cenario B: o buraco eh maior que o pedido (sobra)
    else {
        // criar um novo nó para ser o resto (o novo buraco)
        // começa onde o pedido termina + 1
        // [COISA 0 99][HOLE 100 199]
        // RQ P1 50
        Block *new_hole = create_block("HOLE", best->start + size, best->end);
        // HOLE | START -> 100+50 = 150 | END -> 199
        // se P1 tem 50bytes ele vai de 100 ateh 149
        // [HOLE 150 199]

        // insere esse novo buraco na lista (linkagem)
        new_hole->next = best->next; // o novo buraco aponta para quem o antigo apontava
        // [COISA 0 99][HOLE 150 199]

        // atualiza o bloco atual
        strcpy(best->process, process); // muda nome para o processo
        best->end = best->start + size - 1; // encolhe o fim dele para o tamanho pedido
        best->next = new_hole; // aponta para o novo buraco que criamos
        // [COISA 0 99][P1 100 149][HOLE 150 199]
    }
    printf("Alocado %s em [%d:%d]\n", process, best->start, best->end);
}

//(RL)
void release_memory(char *process) {
    Block *current = head;
    int found = 0; // flag para saber se foi encontrado

    // procura o processo pelo nome
    while (current != NULL) {
        if (strcmp(current->process, process) == 0) {
            strcpy(current->process, "HOLE"); // apenas muda o nome para "HOLE"
            // nao precisa liberar o espaco da memoria, pq se tiver mais blocos na frente ele so vira um espaco agora disponivel
            // [COISA 0 99][P1 100 149][P2 150 169][HOLE 170 199]
            // RL P1
            // [COISA 0 99][HOLE 100 149][P2 150 169][HOLE 170 199]
            found = 1;
            printf("Memória liberada para %s\n", process);
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Erro: Processo %s não encontrado.\n", process);
    } else {
        merge_holes(); // chama a funcao que cola buracos vizinhos
        //caso o proximo bloco tambem seja buraco
        // [COISA 0 99][P1 100 149][P2 150 169][HOLE 170 199]
        // RL P2
        // [COISA 0 99][P1 100 149][HOLE 150 169][HOLE 170 199]
        //merge_holes
        // [COISA 0 99][P1 100 149][HOLE 150 199]
    }
}

//(C)
void compact_memory() {
    Block *current = head;
    Block *new_head = NULL; // inicio da nova lista organizada
    Block *tail = NULL;     // fim da nova lista (para inserir rapido no final)
    int current_addr = 0;   // contador de onde alocar (começa do zero)

    // varre a lista antiga procurando SO PROCESSOS (ignora buracos)
    while (current != NULL) {
        if (strcmp(current->process, "HOLE") != 0) {
            // calcula tamanho do processo
            int p_size = current->end - current->start + 1;
            
            // cria uma COPIA desse processo, mas com endereço novo (current_addr)
            Block *moved_blk = create_block(current->process, current_addr, current_addr + p_size - 1);

            // adiciona na nova lista
            if (new_head == NULL) {
                new_head = moved_blk; // se for o primeiro
                tail = moved_blk;
            } else {
                tail->next = moved_blk; // entra no final
                tail = tail->next;      // atualiza o fim
            }
            // avanca o contador de endereço
            current_addr += p_size;
        }
        current = current->next;
    }
    // loop por iteracao
    // lista original: current->[COISA 0 99]|[HOLE 100 149][P2 150 169][HOLE 170 199]
    // [COISA 0 99]-> != HOLE -> moved_blk
    // lista compactada-> [COISA 0 99]
    // current_addr sobe de 0 para 100
    
    // lista original: current->[COISA 0 99]|[HOLE 100 149]|[P2 150 169][HOLE 170 199]
    // [HOLE 100 149] -> == HOLE -> ignorado
    // lista compactada-> [COISA 0 99]
    // current_addr continua de 0 para 100

    // lista original: current->[COISA 0 99][HOLE 100 149]|[P2 150 169]|[HOLE 170 199]
    // [P2 150 169] -> != HOLE -> moved_blk
    // lista compactada-> [COISA 0 99] [P2 100 119]
    // current_addr sobe de 100 para 120

    // lista original: current->[COISA 0 99]|[HOLE 100 149]|[P2 150 169][HOLE 170 199]
    // [HOLE 170 149] -> == HOLE -> ignorado
    // lista compactada-> [COISA 0 99] [P2 100 119]
    // current_addr continua de 100 para 120


    // se sobrou espaco na memoria total, cria um BURACO no final
    if (current_addr < total_memory_size) {
        Block *big_hole = create_block("HOLE", current_addr, total_memory_size - 1);
        if (new_head == NULL) {
            new_head = big_hole; // caso a memoria estivesse vazia antes
        } else {
            tail->next = big_hole; // entra no final
        }
    }

    // jogar fora a lista antiga e assumir a nova (organizada)
    cleanup_memory(); // limpa a antiga da RAM
    head = new_head;  // aponta o head global para a nova lista
    printf("Compactação concluída.\n");
}

void print_status() {
    Block *current = head;
    // percorre a lista imprimindo os dados
    while (current != NULL) {
        if (strcmp(current->process, "HOLE") == 0) {
            printf("Endereços [%d:%d] Não utilizados\n", current->start, current->end);
        } else {
            printf("Endereços [%d:%d] Processo %s\n", current->start, current->end, current->process);
        }
        current = current->next;
    }
}

// limpeza final pra nao deixar lixo na memoria
void cleanup_memory() {
    Block *current = head;
    while (current != NULL) {
        Block *temp = current; // segura o atual
        current = current->next; // avança para o próximo
        free(temp); // Destrói o que seguramos
    }
    head = NULL;
}