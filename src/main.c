#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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

// Função para renderizar o labirinto e os elementos
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Desenha o labirinto
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (maze[x][y] == 1) {
                glColor3f(0.0, 0.0, 1.0); // Cor azul para as paredes
            } else {
                glColor3f(1.0, 1.0, 1.0); // Cor branca para os caminhos
            }
            glRecti(x, y, x + 1, y + 1);
        }
    }

    // Desenha o jogador
    glColor3f(1.0, 0.0, 0.0); // Cor vermelha para o jogador
    glRecti(playerX, playerY, playerX + 1, playerY + 1);

    // Desenha os dots restantes
    glColor3f(1.0, 1.0, 0.0); // Cor amarela para os dots
    for (int i = 0; i < DOT_COUNT; i++) {
        if (!dots[i].collected) {
            glBegin(GL_QUADS); // Usa GL_QUADS para desenhar um quadrado
            glVertex2f(dots[i].x + 0.3f, dots[i].y + 0.3f);
            glVertex2f(dots[i].x + 0.7f, dots[i].y + 0.3f);
            glVertex2f(dots[i].x + 0.7f, dots[i].y + 0.7f);
            glVertex2f(dots[i].x + 0.3f, dots[i].y + 0.7f);
            glEnd();
        }
    }

    glutSwapBuffers();
}

// Função para capturar o teclado e mover o jogador
void keyboard(unsigned char key, int x, int y) {
    int nextX = playerX, nextY = playerY;

    switch (key) {
        case 'w': nextY++; break; // Move para cima
        case 's': nextY--; break; // Move para baixo
        case 'a': nextX--; break; // Move para a esquerda
        case 'd': nextX++; break; // Move para a direita
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

void initOpenGL() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    initMaze();
    generateMaze(1, 1);
    spawnPlayer();
    spawnDots();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Labirinto Procedural com Coleta de Dots");

    initOpenGL();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
