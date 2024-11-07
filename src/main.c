#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH 20 // Largura do labirinto
#define HEIGHT 20 // Altura do labirinto
#define DOT_COUNT 30 // Quantidade de dots a serem coletados

int maze[WIDTH][HEIGHT];
int playerX, playerY; // Posição do jogador
int goalDots; // Quantidade de dots que o jogador precisa coletar

typedef struct {
    int x, y;
    bool collected; // Indica se o dot já foi coletado
} Dot;

Dot dots[DOT_COUNT];

// Função para inicializar o labirinto com paredes
void initMaze() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            maze[i][j] = 1; // Coloca uma parede em cada célula
        }
    }
}

// Algoritmo para geração procedural do labirinto
void generateMaze(int x, int y) {
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    
    maze[x][y] = 0; // Marca o caminho atual como vazio

    // Embaralha as direções
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp[2] = {directions[i][0], directions[i][1]};
        directions[i][0] = directions[r][0];
        directions[i][1] = directions[r][1];
        directions[r][0] = temp[0];
        directions[r][1] = temp[1];
    }

    // Avança em cada direção
    for (int i = 0; i < 4; i++) {
        int newX = x + directions[i][0] * 2;
        int newY = y + directions[i][1] * 2;
        
        if (newX > 0 && newX < WIDTH && newY > 0 && newY < HEIGHT && maze[newX][newY] == 1) {
            maze[x + directions[i][0]][y + directions[i][1]] = 0; // Remove a parede
            generateMaze(newX, newY);
        }
    }
}

// Função para posicionar o jogador no ponto inicial
void spawnPlayer() {
    playerX = 1;
    playerY = 1;
    maze[playerX][playerY] = 0; // Certifica que o ponto inicial está vazio
}

// Função para gerar dots aleatoriamente em posições válidas no labirinto
void spawnDots() {
    goalDots = DOT_COUNT; // Total de dots que o jogador precisa coletar
    for (int i = 0; i < DOT_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (WIDTH - 2) + 1; // Evita as bordas
            y = rand() % (HEIGHT - 2) + 1;
        } while (maze[x][y] != 0); // Garante que o dot não está em uma parede
        dots[i].x = x;
        dots[i].y = y;
        dots[i].collected = false;
    }
}

// Função para renderizar o labirinto em 3D
void renderMaze() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (maze[x][y] == 1) { // Desenhar paredes
                glColor3f(0.0, 0.0, 1.0); // Azul para paredes
                glPushMatrix();
                glTranslatef(x, 0, y);
                glutSolidCube(1); // Usa um cubo sólido para a parede
                glPopMatrix();
            }
        }
    }
}

// Função para renderizar o jogador e os dots
void renderPlayerAndDots() {
    // Desenha o jogador
    glColor3f(1.0, 0.0, 0.0); // Vermelho para o jogador
    glPushMatrix();
    glTranslatef(playerX, 0, playerY);
    glutSolidSphere(0.3, 20, 20); // Usa uma esfera para o jogador
    glPopMatrix();

    // Desenha os dots
    glColor3f(1.0, 1.0, 0.0); // Amarelo para os dots
    for (int i = 0; i < DOT_COUNT; i++) {
        if (!dots[i].collected) {
            glPushMatrix();
            glTranslatef(dots[i].x, 0, dots[i].y);
            glutSolidSphere(0.15, 10, 10); // Usa uma esfera pequena para os dots
            glPopMatrix();
        }
    }
}

// Função para a câmera seguir o jogador
void cameraFollowPlayer() {
    float camX = playerX;      // Posicionar a câmera na mesma linha X do jogador
    float camY = 8.0f;         // Eleva a câmera para uma visão de cima (Y mais alto)
    float camZ = playerY - 5.0f; // Posicionar a câmera atrás do jogador no eixo Z

    gluLookAt(camX, camY, camZ, // Posição da câmera
              playerX, 1.0f, playerY, // Olhando para o jogador
              0.0f, 1.0f, 0.0f);     // Up vector
}


// Função para renderizar a cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cameraFollowPlayer(); // Atualiza a câmera para seguir o jogador

    renderMaze();
    renderPlayerAndDots();

    glutSwapBuffers();
}

// Função para capturar o teclado e mover o jogador
void keyboard(unsigned char key, int x, int y) {
    int nextX = playerX, nextY = playerY;

    switch (key) {
        case 's': nextY--; break; // Move para frente (em direção ao eixo negativo do Z)
        case 'w': nextY++; break; // Move para trás (em direção ao eixo positivo do Z)
        case 'd': nextX--; break; // Move para a esquerda (X negativo)
        case 'a': nextX++; break; // Move para a direita (X positivo)
        case 'r': // Gera um novo labirinto e posiciona os dots
            initMaze();
            generateMaze(1, 1);
            spawnPlayer();
            spawnDots();
            break;
    }

    // Verifica se o movimento é válido
    if (nextX >= 0 && nextX < WIDTH && nextY >= 0 && nextY < HEIGHT && maze[nextX][nextY] == 0) {
        playerX = nextX;
        playerY = nextY;
    }

    // Checa se o jogador coletou um dot
    for (int i = 0; i < DOT_COUNT; i++) {
        if (!dots[i].collected && playerX == dots[i].x && playerY == dots[i].y) {
            dots[i].collected = true;
            goalDots--; // Diminui o número de dots restantes
            if (goalDots == 0) {
                printf("Parabéns! Você coletou todos os dots!\n");
                initMaze();
                generateMaze(1, 1);
                spawnPlayer();
                spawnDots();
            }
        }
    }

    glutPostRedisplay();
}


// Configurações de inicialização do OpenGL para 3D
void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 50.0); // Define uma perspectiva 3D
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    initMaze();
    generateMaze(1, 1);
    spawnPlayer();
    spawnDots();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Labirinto Procedural 3D com Coleta de Dots");

    initOpenGL();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
