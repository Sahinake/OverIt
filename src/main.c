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

float lightDirX = 0.0f;
float lightDirZ = -1.0f; // Inicialmente apontando para "frente"
float spotCutoff = 30.0f; // Ângulo da lanterna ajustável

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

// Função para configurar o material de uma superfície
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Função para renderizar o labirinto em 3D usando materiais
void renderMaze() {
    // Define o material das paredes
    GLfloat wallAmbient[] = { 0.0, 0.0, 0.2, 1.0 };
    GLfloat wallDiffuse[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat wallSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat wallShininess = 20.0; // Brilho baixo

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (maze[x][y] == 1) { // Desenhar paredes
                setMaterial(wallAmbient, wallDiffuse, wallSpecular, wallShininess);
                glPushMatrix();
                glTranslatef(x, 0, y);
                glutSolidCube(1); // Usa um cubo sólido para a parede
                glPopMatrix();
            }
        }
    }
}

// Função para renderizar o jogador e os dots usando materiais
void renderPlayerAndDots() {
    // Define o material do jogador
    GLfloat playerAmbient[] = { 0.2, 0.0, 0.0, 1.0 };
    GLfloat playerDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat playerSpecular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat playerShininess = 50.0; // Brilho médio

    // Renderiza o jogador
    setMaterial(playerAmbient, playerDiffuse, playerSpecular, playerShininess);
    glPushMatrix();
    glTranslatef(playerX, 0, playerY);
    glutSolidSphere(0.3, 20, 20); // Usa uma esfera para o jogador
    glPopMatrix();

    // Define o material dos dots
    GLfloat dotAmbient[] = { 0.2, 0.2, 0.0, 1.0 };
    GLfloat dotDiffuse[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat dotSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat dotShininess = 10.0; // Brilho baixo para dots

    // Renderiza os dots
    setMaterial(dotAmbient, dotDiffuse, dotSpecular, dotShininess);
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

// Função para configurar a iluminação de forma mais flexível
void setLighting(float lightPos[4], float lightDir[3], float ambient[4], float diffuse[4], float specular[4], float shininess) {
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutoff); // Usando o valor ajustável de spotCutoff
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Função para a iluminação dinâmica durante o jogo
void updateLighting() {
    // A posição da luz será sempre a posição do jogador, para simular uma lanterna
    GLfloat lightPos[] = { (float)playerX, 1.0f, (float)playerY, 1.0f };

    // A direção da luz será ajustada com base no movimento do jogador
    GLfloat lightDir[] = { lightDirX, -0.5f, lightDirZ }; // Um pequeno ajuste na direção da luz para simular uma lanterna

    // Definindo características da luz ambiente, difusa e especular
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Ajuste do brilho do material
    GLfloat shininess = 50.0f; // Brilho médio

    // Ângulo de corte da luz (spot)
    float spotCutoff = 30.0f; // Ângulo da lanterna

    // Chamando a função que configura a iluminação
    setLighting(lightPos, lightDir, ambientLight, diffuseLight, specularLight, shininess);
}

// Função para renderizar a cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cameraFollowPlayer();

    // Atualiza a iluminação de acordo com o jogador
    updateLighting();

    renderMaze();
    renderPlayerAndDots();

    glutSwapBuffers();
}

// Função para capturar o teclado e mover o jogador
void keyboard(unsigned char key, int x, int y) {
    int nextX = playerX, nextY = playerY;

    switch (key) {
        case 's': nextY--; lightDirX = 0.0f; lightDirZ = -1.0f; break;
        case 'w': nextY++; lightDirX = 0.0f; lightDirZ = 1.0f; break;
        case 'd': nextX--; lightDirX = -1.0f; lightDirZ = 0.0f; break;
        case 'a': nextX++; lightDirX = 1.0f; lightDirZ = 0.0f; break;
        case '+':
            if (spotCutoff < 90.0f) spotCutoff += 5.0f; // Aumenta o raio da luz
            break;
        case '-':
            if (spotCutoff > 10.0f) spotCutoff -= 5.0f; // Diminui o raio da luz
            break;
        case 'r':
            initMaze();
            generateMaze(1, 1);
            spawnPlayer();
            spawnDots();
            break;
    }

    if (nextX >= 0 && nextX < WIDTH && nextY >= 0 && nextY < HEIGHT && maze[nextX][nextY] == 0) {
        playerX = nextX;
        playerY = nextY;
    }

    for (int i = 0; i < DOT_COUNT; i++) {
        if (!dots[i].collected && playerX == dots[i].x && playerY == dots[i].y) {
            dots[i].collected = true;
            goalDots--;
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
    glEnable(GL_LIGHTING); // Ativa a iluminação
    glEnable(GL_LIGHT0);    // Ativa a luz 0
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 50.0);
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
