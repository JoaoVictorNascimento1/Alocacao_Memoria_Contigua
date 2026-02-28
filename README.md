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
