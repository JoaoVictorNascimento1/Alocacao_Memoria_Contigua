# Define qual compilador usar (gcc é o padrão do Linux/C)
CC = gcc

# Flags (opções) de compilação:
# -Wall: Mostre todos os avisos (Warnings All) se eu fizer bobagem
# -Wextra: Mostre avisos extras
# -g: Adicione info de debug (ajuda a achar erros depois)
CFLAGS = -Wall -Wextra -g

# Regra principal: O objetivo é criar o arquivo executável "alocador"
all: alocador

# Para criar "alocador", preciso de "main.o" e "memory_manager.o"
alocador: main.o memory_manager.o
	# Comando que liga (linka) as partes e cria o executável final
	$(CC) $(CFLAGS) -o alocador main.o memory_manager.o

# Para criar "main.o", preciso do código fonte "main.c" e dos headers
main.o: main.c memory_manager.h memory_block.h
	# Compila apenas (-c) o main.c
	$(CC) $(CFLAGS) -c main.c

# Para criar "memory_manager.o", preciso da sua implementação e headers
memory_manager.o: memory_manager.c memory_manager.h memory_block.h
	# Compila apenas (-c) o memory_manager.c
	$(CC) $(CFLAGS) -c memory_manager.c

# Regra de limpeza: Digite "make clean" para apagar os arquivos gerados
clean:
	rm -f *.o alocador