# Variáveis
CC = gcc
CFLAGS = -Iinclude -Ilib -I/usr/include/freetype2   		# Incluir os diretórios include e lib
LDFLAGS = -lGL -lGLU -lglut -lm -lGLEW -lftgl -lfreetype   	# Bibliotecas necessárias
SRCDIR = src
BUILDDIR = build
TARGET = $(BUILDDIR)/OverIt

# Coleta todos os arquivos .c em src/
SRC = $(wildcard $(SRCDIR)/*.c)

# Converte os arquivos .c para .o
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Regra padrão - compilar o executável
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Regra para compilar os arquivos .c em .o
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)    # Cria o diretório build, se não existir
	$(CC) -c $< -o $@ $(CFLAGS)

# Limpa os arquivos gerados (útil para recomeçar a compilação)
clean:
	rm -rf $(BUILDDIR)/*

# Executa o programa
run: $(TARGET)
	./$(TARGET)

# Recompilar tudo
rebuild: clean all