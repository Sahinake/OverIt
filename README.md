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
gcc src/main.c src/UI.c src/Maze.c src/Time.c -o build/OverIt -Iinclude -Ilib -lGL -lGLU -lglut -lm -lGLEW -lftgl -lglfw -lfreetype -I/usr/include/freetype2
```

Para executar o programa:
```
./build/OverIt
```

## Controles do jogo
Esse jogo utiliza as teclas do teclado para realizar várias ações, como movimento, alternância de luz, interação com menus e configurações de volume. Aqui estão os controles principais para interação.
### Controles de Movimentação do Jogador

    W: Move o jogador para frente (no eixo Z).
    S: Move o jogador para trás (no eixo Z).
    A: Move o jogador para a esquerda (no eixo X).
    D: Move o jogador para a direita (no eixo X).

A movimentação é limitada por colisões, e o personagem só pode se mover se não houver nenhum obstáculo no caminho.
### Controle da Lanterna

    F: Ativa ou desativa a lanterna do jogador.
    +: Aumenta a carga da lanterna em 5%, ampliando o raio da luz.
    -: Diminui a carga da lanterna em 5%, reduzindo o raio da luz.

### Reiniciar o Jogo
    R: Reinicia o jogo, reinicializando o labirinto, o jogador e os itens.

## Navegação nos Menus
* ESC: Dependendo do estado atual do jogo:
    * Menu Principal: Fecha o jogo.
    * Menu Novo Jogo: Volta para o Menu Principal.
    * Menu Carregar Jogo: Volta para o Menu Principal.
    * Menu Ranking: Volta para o Menu Principal.
    * Menu Opções: Volta para o Menu Principal.
    * Jogando: Nenhuma ação.

* Tecla Enter: Seleciona uma opção no menu atual:
    * Menu Principal:
        * Nova Jogo: Inicia um novo jogo.
        * Carregar Jogo: Carrega o jogo salvo.
        * Ranking: Exibe o ranking.
        * Opções: Abre o menu de opções.
        * Sair: Encerra o jogo.
    * Menu Novo Jogo: Começa um novo jogo e salva a partida.
    * Menu Carregar Jogo: Carrega o jogo salvo selecionado.
    * Menu Ranking: Exibe as opções do ranking.
    * Menu Opções: Exibe as configurações do jogo.

## Controle de Volume
    V: Aumenta o volume dos efeitos sonoros.
    C: Diminui o volume dos efeitos sonoros.

## Excluir Jogo Salvo
* Delete: Exclui o jogo salvo selecionado, se disponível.

## Observações
* O jogo pode ser controlado com o teclado, e a movimentação é limitada por colisões no ambiente 3D.
* Os menus permitem navegar pelas opções de novo jogo, carregar jogo, ranking, e opções de configuração.
* A lanterna do jogador pode ser ativada/desativada e ajustada conforme a necessidade durante a jogabilidade.

## Controles de Navegação no Menu
* Utilize as setas do teclado para selecionar opções nos menus.
* Pressione Enter para confirmar a seleção.
* ESC para voltar ao menu anterior.