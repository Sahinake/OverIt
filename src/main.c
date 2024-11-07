#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 20 // Largura do labirinto
#define HEIGHT 20 // Altura do labirinto

int maze[WIDTH][HEIGHT];
int playerX = 1, playerY = 1; // Posição inicial do jogador

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

// Função para renderizar o labirinto
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

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

    glutSwapBuffers();
}

// Função para movimentar o jogador
void movePlayer(int key, int x, int y) {
    int nextX = playerX, nextY = playerY;
    switch (key) {
        case GLUT_KEY_UP:    nextY--; break;
        case GLUT_KEY_DOWN:  nextY++; break;
        case GLUT_KEY_LEFT:  nextX--; break;
        case GLUT_KEY_RIGHT: nextX++; break;
    }

    // Verifica se o movimento é válido
    if (nextX >= 0 && nextX < WIDTH && nextY >= 0 && nextY < HEIGHT && maze[nextX][nextY] == 0) {
        playerX = nextX;
        playerY = nextY;
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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Labirinto Procedural");

    initOpenGL();
    glutDisplayFunc(display);
    glutSpecialFunc(movePlayer);

    glutMainLoop();
    return 0;
}
