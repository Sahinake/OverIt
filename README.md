## Como Executar
### Requisitos
* Compilador C (gcc ou equivalente).
* Make
* OpenGL e GLUT instalados no sistema.

## Instalação dos requisitos (Ubuntu):
```
sudo apt update
sudo apt install gcc
sudo apt install make
sudo apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev
sudo apt install libfreetype6 libfreetype6-dev libftgl-dev libglew-dev libglfw3 libglfw3-dev
```

## Compilação e Execução
* Clone o repositório:

``` 
git clone https://github.com/Sahinake/OverIt.git
```
* Você pode utilizar o Makefile, compilando e executando o programa utilizando os seguintes comandos:
```
make rebuild
make run
```

* Comandos equivalentes ao ```make rebuild```:
```
make clean
make
```

* Ou compile manualmente:
```
gcc src/main.c src/UI.c src/Maze.c src/Time.c -o build/OverIt -Iinclude -Ilib -lGL -lGLU -lglut -lm -lGLEW -lftgl -lfreetype -I/usr/include/freetype2
```

Para executar o programa:
```
./build/OverIt
```
