# 💾 Alocação de Memória Contígua

Um simulador interativo de gerenciamento de memória contígua escrito em C. Este projeto demonstra como um Sistema Operacional aloca e libera blocos de memória RAM para diferentes processos utilizando uma estrutura de lista encadeada.

## Sobre o Projeto

O programa simula a memória física como um grande bloco contíguo (um grande "buraco" ou *hole* inicial). À medida que os processos solicitam memória, o gerenciador percorre a lista encadeada de blocos e aloca o espaço com base em três estratégias clássicas de Sistemas Operacionais:

* **First-Fit (F):** Aloca o primeiro bloco de memória vazio que for grande o suficiente.
* **Best-Fit (B):** Procura em toda a memória e aloca o menor bloco vazio que seja suficiente para o processo, deixando a menor sobra possível.
* **Worst-Fit (W):** Procura em toda a memória e aloca o maior bloco vazio disponível, deixando a maior sobra possível.

O sistema também trata automaticamente a **fusão de buracos** (*merge holes*) quando blocos adjacentes são liberados, e possui um comando para **compactação de memória**, movendo todos os processos para o início da memória e unindo o espaço livre no final.

## Compilar e Executar

O projeto inclui um `Makefile` para facilitar a compilação no Linux.

1. Clone o repositório:
   ```bash
   git clone [https://github.com/JoaoVictorNascimento1/Alocacao_Memoria_Contigua.git](https://github.com/JoaoVictorNascimento1/Alocacao_Memoria_Contigua.git)
   cd c-memory-allocator

2. Compile o código:

Bash -> make

4. Execute passando o tamanho total da memória desejada como argumento (ex: 1024 bytes):

Bash -> ./alocador 1024


## Comandos Disponíveis

Dentro do shell interativo `alocador>`, você pode usar os seguintes comandos:

| Comando | Sintaxe | Descrição |
| :--- | :--- | :--- |
| **Request** | `RQ <processo> <tamanho> <F\|B\|W>` | Solicita alocação de memória para um processo usando a estratégia escolhida. |
| **Release** | `RL <processo>` | Libera a memória ocupada por um processo específico. |
| **Compact** | `C` | Compacta a memória, movendo processos e unindo espaços livres. |
| **Status** | `STAT` | Exibe o mapa atual da memória (blocos ocupados e livres). |
| **Exit** | `X` | Encerra o simulador e limpa a memória alocada. |

## Exemplo de Uso
```text
$ ./alocador 1000
alocador> RQ P0 200 F
Alocado P0 em [0:199]
alocador> RQ P1 300 B
Alocado P1 em [200:499]
alocador> STAT
Endereços [0:199] Processo P0
Endereços [200:499] Processo P1
Endereços [500:999] Não utilizados
alocador> RL P0
Memória liberada para P0
alocador> C
Compactação concluída.
alocador> STAT
Endereços [0:299] Processo P1
Endereços [300:999] Não utilizados
alocador> X
```
## Aprofundando: Arquitetura e Implementação

Este projeto foi desenvolvido para consolidar conceitos fundamentais de Sistemas Operacionais e Estruturas de Dados na prática. Abaixo, detalhamos a teoria aplicada, as ferramentas escolhidas e a dissecação do código, arquivo por arquivo.

### Tecnologias Utilizadas

* **C (Linguagem):** A escolha padrão para o desenvolvimento de sistemas operacionais e sistemas embarcados devido ao seu acesso direto e não gerenciado à memória.
* **GCC e Makefile:** Uso do compilador GNU C e de arquivos *Make* para automatizar o processo de *build* modularizado.

A arquitetura do projeto foi modularizada em múltiplos arquivos para separar a definição de dados, a lógica de gerenciamento e a interface do usuário. 

#### `memory_block.h` (Estruturas de Dados Básicas)

Este arquivo de cabeçalho (header) define a estrutura fundamental do nosso simulador: o nó da lista encadeada que representa um segmento de memória.

```c
#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

// Estrutura que representa um bloco contíguo de memória
typedef struct Block {
    char process[10];     // Nome do processo (ex: "P0") ou "HOLE" para espaço livre
    int start;            // Endereço lógico inicial do bloco
    int end;              // Endereço lógico final do bloco
    struct Block *next;   // Ponteiro para o próximo bloco (Lista Encadeada)
} Block;

#endif
```
### Dissecando o código:

  - **#ifndef, #define, #endif (Header Guards)**: Uma boa prática essencial em C e C++. Evita que a estrutura Block seja redefinida caso este arquivo seja incluído (#include) múltiplas vezes em arquivos diferentes (como no main.c e no memory_manager.c), o que causaria erros de compilação.
    
  - **typedef struct Block**: Usamos typedef para não precisarmos escrever struct Block toda vez que formos instanciar a variável, permitindo usar apenas Block.

  - **char process[10]**: Um array de caracteres que armazena quem é o "dono" daquele espaço. Se o bloco estiver livre, ele recebe a string "HOLE". Essa é a forma que o simulador diferencia memória alocada de memória disponível.

  - **int start e int end**: Representam os limites lógicos do bloco na "memória". O tamanho do bloco pode ser facilmente calculado por (end - start) + 1.

  - **struct Block 'next**: A lista encadeada simples (Singly Linked List). Este ponteiro guarda o endereço de memória do próximo bloco na sequência lógica. Se for o último bloco da memória, este ponteiro será NULL.

#### `memory_manager.h` (Interface do Gerenciador)
Este arquivo de cabeçalho contém as assinaturas (protótipos) das funções que compõem o motor do nosso simulador. Ele define como o programa principal (`main.c`) pode interagir com a memória, aplicando o conceito de **encapsulamento**.

```c
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "memory_block.h"

// Inicializa a memória com um único buraco do tamanho total
void init_memory(int size);

// Solicita memória -> processo, tamanho, estratégia (F, B, W)
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
```
### Dissecando o código:

- **#include "memory_block.h"**: Como as funções do gerenciador manipulam os blocos de memória, precisamos importar a estrutura Block definida no arquivo anterior.

- **void init_memory(int size)**: Prepara o terreno. É chamada uma única vez no início do programa para definir o tamanho total da memória física simulada, criando o primeiro grande "buraco" (HOLE).

- **void request_memory(...)**: Recebe o nome do processo que quer memória, o tamanho em bytes e a estratégia escolhida ('F', 'B', ou 'W'). Toda a lógica de varrer a lista e dividir blocos fica escondida atrás desta função.

- **void release_memory(char *process)**: Simula o encerramento de um processo. A função buscará o bloco ocupado por esse processo e mudará seu status para "HOLE". Internamente, ela também deve lidar com a fusão de blocos livres vizinhos.

- **void compact_memory()**: Resolve o problema da fragmentação externa. Esta assinatura expõe o comando que reorganizará toda a lista encadeada, empurrando os processos para o início e juntando o espaço livre no final.

- **void print_status() e void cleanup_memory()**: Funções utilitárias. A primeira itera sobre a lista para mostrar o mapa de memória ao usuário. A segunda garante que não haja vazamento de memória (memory leak) no próprio simulador, chamando a função free() para cada nó da lista encadeada antes de o programa fechar.

#### `memory_manager.c` (Motor de Alocação e Lógica)
Este é o arquivo principal onde a mágica das Estruturas de Dados e de Sistemas Operacionais acontece. Ele gerencia a memória simulada manipulando ponteiros da nossa lista encadeada.

Para facilitar o entendimento, dividimos a explicação deste arquivo em suas responsabilidades principais:

##### Setup e Variáveis Globais
O estado da nossa memória é controlado por duas variáveis globais e inicializado por funções auxiliares:

```c
#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <limits.h>     
#include "memory_manager.h"

// Variáveis globais
Block *head = NULL;       
int total_memory_size = 0; 

// Função auxiliar para criar um novo nó da lista na Heap
Block* create_block(char *process, int start, int end) {
    Block *new_block = (Block*) malloc(sizeof(Block));
    strcpy(new_block->process, process);
    new_block->start = start;
    new_block->end = end;
    new_block->next = NULL;
    return new_block;
}

// Inicializa a memória com um grande buraco
void init_memory(int size) {
    total_memory_size = size;
    head = create_block("HOLE", 0, size - 1);
}
```

* **`Block *head = NULL;`**: O ponteiro inicial da nossa lista. Ele é a "porta de entrada" para varrer a memória.
* **`create_block()`**: Uma função auxiliar que pede memória real ao Sistema Operacional hospedeiro (usando `malloc`) apenas para criar a *estrutura de controle* do bloco, retornando o ponteiro para este novo nó.
* **`init_memory(int size)`**: Inicializa a simulação criando um único e gigantesco bloco livre chamado `"HOLE"`, que vai do endereço `0` até `size - 1`.

##### Alocação de Memória (`request_memory`)
Esta função implementa os três algoritmos de busca (First-Fit, Best-Fit e Worst-Fit).
```c
void request_memory(char *process, int size, char strategy) {
    Block *current = head;
    Block *best = NULL; 
    int worst_size = -1;       
    int best_size = INT_MAX;   
    
    // Varre a lista buscando o buraco ideal
    while (current != NULL) {
        if (strcmp(current->process, "HOLE") == 0) {
            int hole_size = current->end - current->start + 1;
            if (hole_size >= size) {
                if (strategy == 'F') { 
                    best = current; break; 
                } else if (strategy == 'B' && hole_size < best_size) { 
                    best_size = hole_size; best = current; 
                } else if (strategy == 'W' && hole_size > worst_size) { 
                    worst_size = hole_size; best = current; 
                }
            }
        }
        current = current->next;
    }

    if (best == NULL) {
        printf("Erro: Memória insuficiente\n");
        return;
    }

    // Ocupa o espaço e faz o "Split" se necessário
    int hole_size = best->end - best->start + 1;
    if (hole_size == size) {
        strcpy(best->process, process); 
    } else {
        Block *new_hole = create_block("HOLE", best->start + size, best->end);
        new_hole->next = best->next; 
        strcpy(best->process, process); 
        best->end = best->start + size - 1; 
        best->next = new_hole; 
    }
    printf("Alocado %s em [%d:%d]\n", process, best->start, best->end);
}
```
1. O código percorre a lista encadeada procurando nós do tipo `"HOLE"`.
2. Quando encontra buracos grandes o suficiente, ele aplica a estratégia escolhida para decidir qual é o "melhor" buraco (`best`).
3. **Divisão de Bloco (Split):** Se o buraco escolhido for maior que o tamanho solicitado, a função não desperdiça o resto. Ela altera o bloco atual para receber o processo, encolhe o seu tamanho, e cria um novo nó `"HOLE"` com o espaço que sobrou, inserindo-o na frente do bloco atual (atualizando os ponteiros `next`).

##### Liberação e Fusão (`release_memory` e `merge_holes`)
A liberação de memória em sistemas operacionais pode gerar um problema chamado **Fragmentação Externa**, que é quando temos muito espaço livre, mas espalhado em pequenos pedaços. 
```c
void merge_holes() {
    Block *current = head;
    while (current != NULL && current->next != NULL) {
        if (strcmp(current->process, "HOLE") == 0 && strcmp(current->next->process, "HOLE") == 0) {
            Block *temp = current->next; 
            current->end = temp->end; 
            current->next = temp->next;
            free(temp); // Limpa o nó redundante da RAM
        } else {
            current = current->next;
        }
    }
}

void release_memory(char *process) {
    Block *current = head;
    int found = 0;
    while (current != NULL) {
        if (strcmp(current->process, process) == 0) {
            strcpy(current->process, "HOLE"); // Liberação lógica
            found = 1;
            break;
        }
        current = current->next;
    }
    if (found) merge_holes(); 
}
```
* **`release_memory()`**: Procura o processo pelo nome e simplesmente muda seu rótulo de volta para `"HOLE"`. Não há deleção de nó aqui.
* **`merge_holes()`**: Para combater a fragmentação, esta função é chamada logo após a liberação. Ela varre a lista e verifica: se o bloco atual e o próximo são ambos `"HOLE"`, ela os **funde**. O bloco atual tem seu `end` estendido para cobrir o próximo, o ponteiro `next` pula o bloco redundante, e o nó extra é destruído com `free()`.

##### Compactação de Memória (`compact_memory`)
Quando a fragmentação externa atinge um nível onde não há um bloco contíguo grande o suficiente (mesmo com espaço total disponível), a compactação é acionada.
```c
void compact_memory() {
    Block *current = head;
    Block *new_head = NULL; 
    Block *tail = NULL;     
    int current_addr = 0;   

    // Copia apenas os processos para uma nova lista
    while (current != NULL) {
        if (strcmp(current->process, "HOLE") != 0) {
            int p_size = current->end - current->start + 1;
            Block *moved_blk = create_block(current->process, current_addr, current_addr + p_size - 1);

            if (new_head == NULL) {
                new_head = moved_blk; tail = moved_blk;
            } else {
                tail->next = moved_blk; tail = tail->next;      
            }
            current_addr += p_size;
        }
        current = current->next;
    }

    // Cria um grande buraco no final com o espaço restante
    if (current_addr < total_memory_size) {
        Block *big_hole = create_block("HOLE", current_addr, total_memory_size - 1);
        if (new_head == NULL) new_head = big_hole; 
        else tail->next = big_hole; 
    }

    cleanup_memory(); // Descarta a lista antiga
    head = new_head;  // Assume a nova lista organizada
}
```
* A função atua criando uma **nova lista encadeada** a partir do zero.
* Ela varre a lista antiga e copia apenas os blocos de Processos, recalculando seus endereços lógicos (`start` e `end`) para que fiquem todos colados no início da memória (endereço 0 em diante).
* No final, se sobrar espaço, ela cria um único grande `"HOLE"` englobando todo o resto.
* A lista antiga é descartada da RAM (`cleanup_memory`) e o ponteiro global `head` passa a apontar para esta nova lista organizada.

##### Status e Prevenção de Memory Leaks (`cleanup_memory`)
* Itera sobre toda a lista encadeada quando o usuário digita o comando `X` (Exit), usando a função `free()` do C para devolver a memória de cada estrutura de bloco (`struct Block`) ao Sistema Operacional real. Isso garante que o simulador seja um "bom cidadão" e não cause vazamentos de memória na máquina onde está rodando.

```c
void print_status() {
    Block *current = head;
    while (current != NULL) {
        if (strcmp(current->process, "HOLE") == 0) {
            printf("Endereços [%d:%d] Não utilizados\n", current->start, current->end);
        } else {
            printf("Endereços [%d:%d] Processo %s\n", current->start, current->end, current->process);
        }
        current = current->next;
    }
}

void cleanup_memory() {
    Block *current = head;
    while (current != NULL) {
        Block *temp = current; 
        current = current->next; 
        free(temp); // Devolve a memória ao SO hospedeiro
    }
    head = NULL;
}
```

#### `main.c` (Interface CLI e Loop Principal)
*(Explicação detalhada será inserida aqui)*

#### `Makefile` (Automação de Build)
*(Explicação detalhada será inserida aqui)*
