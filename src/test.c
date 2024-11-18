#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

// Definindo o tamanho da matriz e os cubos
#define GRID_SIZE 20  // Tamanho da matriz (20x20)
#define CUBE_SIZE 1.0f  // Tamanho do cubo

// Matriz representando o layout dos cubos brancos (alocada dinamicamente)
int **grid;

// Estrutura para a posição do jogador
struct Player {
    float posX, posY, posZ;  // Posição do jogador
    float targetX, targetZ;  // Posições de destino para o movimento suave
    float speedX, speedZ;    // Velocidade de movimento (delta por segundo)
    int moving;              // Flag para indicar se o jogador está se movendo
};

struct Player player = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };  // Inicializa a posição do jogador

// Distância da câmera e altura
float camHeight = 10.0f;  // A altura da câmera acima do jogador
float camDistance = 10.0f;  // Distância da câmera em relação ao jogador (não será usada diretamente, pois a câmera ficará diretamente em cima)

// Função para configurar a projeção em perspectiva
void setupProjection() {
    glMatrixMode(GL_PROJECTION);  // Define a matriz de projeção
    glLoadIdentity();  // Reseta qualquer transformação existente

    // Define a projeção em perspectiva (campo de visão, razão de aspecto, planos de corte)
    gluPerspective(45.0, 1.0, 0.1, 100.0);  // Campo de visão de 45 graus, razão de aspecto 1:1 (aspecto quadrado), planos de corte próximos e distantes
}

// Função de inicialização
void init() {
    glEnable(GL_DEPTH_TEST);  // Habilita o teste de profundidade (para não desenhar objetos atrás de outros)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Define a cor de fundo (preto)
}

// Função para configurar a câmera
void setupCamera() {
    // A câmera vai ficar diretamente acima do jogador e olhar para ele de cima para baixo.
    float camX = player.posX;  // Posição no eixo X (acima do jogador)
    float camY = player.posY + camHeight;  // Posição no eixo Y (acima do jogador)
    float camZ = player.posZ;  // Posição no eixo Z (mesmo eixo Z do jogador)

    // Define a câmera para olhar para o jogador
    gluLookAt(camX, camY, camZ,    // Posição da câmera (acima e no mesmo eixo Z do jogador)
              player.posX, player.posY, player.posZ, // Para onde a câmera está olhando (o jogador)
              0.0f, 0.0f, -1.0f);    // O vetor "up", indicando que a câmera deve olhar para baixo (olhando no eixo negativo Z)
}

// Função para renderizar a cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Limpa os buffers (cor e profundidade)
    glMatrixMode(GL_MODELVIEW);  // Define a matriz de modelo/vista
    glLoadIdentity();  // Reseta qualquer transformação existente
    setupCamera();  // Configura a câmera

    // Renderiza os cubos brancos baseados na matriz
    for (int z = 0; z < GRID_SIZE; ++z) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (grid[z][x] == 1) {  // Se for um cubo branco (1 na matriz)
                glPushMatrix();
                glTranslatef(x - GRID_SIZE / 2, 0.0f, z - GRID_SIZE / 2);  // Movimenta o cubo para a posição correta
                glColor3f(1.0f, 1.0f, 1.0f);  // Cor branca
                glutSolidCube(CUBE_SIZE);  // Renderiza um cubo com o tamanho de 1 unidade
                glPopMatrix();
            }
        }
    }

    // Renderiza o cubo do jogador (agora verde)
    glPushMatrix();
    glTranslatef(player.posX, player.posY, player.posZ);  // Movimenta o cubo verde de acordo com a posição do jogador
    glColor3f(0.0f, 1.0f, 0.0f);  // Define a cor verde para o jogador
    glutSolidCube(CUBE_SIZE);  // Renderiza o cubo do jogador
    glPopMatrix();

    glutSwapBuffers();  // Troca os buffers para exibir a cena renderizada
}

// Função para verificar colisão com os cubos brancos
int checkCollision(float newPosX,float newPosZ) {
    int gridX = (int)(newPosX + GRID_SIZE / 2);  // Convertendo a posição X para índices da grid
    int gridZ = (int)(newPosZ + GRID_SIZE / 2);  // Convertendo a posição Z para índices da grid

    // Verifica se a nova posição está dentro dos limites da grid e se há um cubo branco
    if (gridX >= 0 && gridX < GRID_SIZE && gridZ >= 0 && gridZ < GRID_SIZE) {
        if (grid[gridZ][gridX] == 1) {  // Se houver um cubo branco na posição
            return 1;  // Há uma colisão
        }
    }
    return 0;  // Não há colisão
}

// Função para mover o jogador suavemente com as teclas
void movePlayer(int value) {
    // Movimento suave
    if (player.moving) {
        if (player.posX != player.targetX) {
            player.posX += player.speedX;
            if ((player.speedX > 0 && player.posX >= player.targetX) || (player.speedX < 0 && player.posX <= player.targetX)) {
                player.posX = player.targetX;  // Chegou ao destino
                player.speedX = 0.0f;  // Parar movimento no eixo X
            }
        }

        if (player.posZ != player.targetZ) {
            player.posZ += player.speedZ;
            if ((player.speedZ > 0 && player.posZ >= player.targetZ) || (player.speedZ < 0 && player.posZ <= player.targetZ)) {
                player.posZ = player.targetZ;  // Chegou ao destino
                player.speedZ = 0.0f;  // Parar movimento no eixo Z
            }
        }

        if (player.speedX == 0.0f && player.speedZ == 0.0f) {
            player.moving = 0;  // Interrompe o movimento quando ambos os eixos atingem o destino
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, movePlayer, 0);  // Chama a função novamente após 16ms (aproximadamente 60 FPS)
}

// Função para movimentação do jogador com base nas teclas
void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;  // Velocidade de movimento por delta de tempo (por segundo)
    float posX = player.posX;
    float posZ = player.posZ;
    if (key == 'w' && !player.moving) {  // Move para frente (no eixo Z)
        posZ = player.posZ - 1.0f;
        if (checkCollision(posX,posZ) == 0){
            player.targetZ = posZ;
            player.speedZ = -moveSpeed;
            player.moving = 1;
        }
    }
    if (key == 's' && !player.moving) {  // Move para trás (no eixo Z)
        posZ = player.posZ + 1.0f;
        if (checkCollision(posX,posZ) == 0){
            player.targetZ = posZ;
            player.speedZ = moveSpeed;
            player.moving = 1;
        }
    }
    if (key == 'a' && !player.moving) {  // Move para a esquerda (no eixo X)
        posX = player.posX - 1.0f;
        if (checkCollision(posX,posZ) == 0){
            player.targetX = posX;
            player.speedX = -moveSpeed;
            player.moving = 1;
        }
    }
    if (key == 'd' && !player.moving) {  // Move para a direita (no eixo X)
        posX = player.posX + 1.0f;
        if (checkCollision(posX,posZ) == 0){
            player.targetX = posX;
            player.speedX = moveSpeed;
            player.moving = 1;
        }
    }

    glutPostRedisplay();  // Solicita uma nova renderização da cena
}

// Função para ajustar a janela ao redimensionar
void reshape(int width, int height) {
    glViewport(0, 0, width, height);  // Define o tamanho da janela de visualização
    glMatrixMode(GL_PROJECTION);  // Configura a projeção
    glLoadIdentity();  // Reseta a projeção
    gluPerspective(45.0, (double)width / (double)height, 0.1, 100.0);  // Ajusta a razão de aspecto
    glMatrixMode(GL_MODELVIEW);  // Retorna à matriz de modelo/vista
}

// Função para inicializar a matriz dinamicamente
void initializeGrid() {
    // Aloca memória para a matriz
    grid = (int**)malloc(GRID_SIZE * sizeof(int*));
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[i] = (int*)malloc(GRID_SIZE * sizeof(int));
    }

    // Preenche a matriz com o layout específico de cubos (do seu exemplo anterior)
    int layout[GRID_SIZE][GRID_SIZE] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1},
        {1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    // Preenche a grid com o layout
    for (int z = 0; z < GRID_SIZE; ++z) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            grid[z][x] = layout[z][x];  // Copia o layout diretamente para a grid
        }
    }
}

// Função para liberar a memória alocada para a matriz
void freeGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        free(grid[i]);
    }
    free(grid);
}

// Função para imprimir o estado do jogo no console para debug
void debugPrint(int value) {
    // Imprime a posição do jogador e o estado da movimentação
    printf("Posição do jogador: (%.2f, %.2f, %.2f)\n", player.posX, player.posY, player.posZ);
    printf("Velocidade do jogador: (%.2f, %.2f)\n", player.speedX, player.speedZ);
    printf("Movendo? %s\n", player.moving ? "Sim" : "Não");
    printf("Destino do jogador: (%.2f, %.2f)\n", player.targetX, player.targetZ);
    
    // Imprime o estado da grid na posição do jogador
    int gridX = (int)(player.posX + GRID_SIZE / 2);
    int gridZ = (int)(player.posZ + GRID_SIZE / 2);
    if (gridX >= 0 && gridX < GRID_SIZE && gridZ >= 0 && gridZ < GRID_SIZE) {
        printf("Estado da grid na posição (%d, %d): %d\n", gridX, gridZ, grid[gridZ][gridX]);
    }
    // Chamamos novamente a função de debug após 500ms (aproximadamente 2 vezes por segundo)
    glutTimerFunc(1000, debugPrint, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Modo de exibição com duplo buffer e profundidade
    glutInitWindowSize(800, 600); // Tamanho inicial da janela
    glutCreateWindow("Jogador Verde com Movimentação Suave");

    init();  // Função de inicialização

    // Inicializa a matriz dinamicamente
    initializeGrid();

    // Configuração de projeção em perspectiva
    setupProjection();

    glutDisplayFunc(display);  // Define a função de exibição (renderização)
    glutKeyboardFunc(keyboard);  // Define a função de entrada do teclado
    glutReshapeFunc(reshape);  // Função de redimensionamento da janela

    // Chama a função de movimentação com temporização
    glutTimerFunc(16, movePlayer, 0);

  // Chama a função de debug a cada 500ms
    debugPrint(0);
    // glutTimerFunc(100, debugPrint, 0);

    glutMainLoop();  // Inicia o loop principal do OpenGL

    // Libera a memória alocada ao final do programa
    freeGrid();

    return 0;
}
