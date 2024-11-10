#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "Maze.h"
#include "UI.h"
#include "Time.h"

#define WIDTH 20 // Largura do labirinto
#define HEIGHT 20 // Altura do labirinto
#define DOT_COUNT 30 // Quantidade de dots a serem coletados
#define BATTERY_COUNT 5 // Quantidade de baterias a serem spawnadas
#define MAX_BATTERY_PERCENTAGE 100.0f // Capacidade máxima da bateria (100%)
#define MAX_BATTERY 70.0f // Capacidade máxima da bateria (100%)
#define BATTERY_DECREASE_RATE 0.02f // Taxa de diminuição da bateria por atualização de frame
#define M_PI 3.14159265358979323846

int maze[WIDTH][HEIGHT];
int maze_widht = WIDTH, maze_height = HEIGHT;
int window_width = 800, window_height = 600;

int goalDots; // Quantidade de dots que o jogador precisa coletar
int total_batteries = 5;

float lightDirX = 0.0f;
float lightDirZ = -1.0f; // Inicialmente apontando para "frente"
float maxDistance = 5.0f; // Distância máxima para a lanterna

// Inicializa a carga da bateria
float batteryCharge = MAX_BATTERY; // Bateria começa cheia
float batteryPercentage = MAX_BATTERY_PERCENTAGE; // A bateria começa com 100%
float batteryDecrease = BATTERY_DECREASE_RATE;

// Variáveis globais para o tempo
time_t startTime;
time_t currentTime;
int elapsedTime;
int max_font_height = 35;
int min_font_height = 14;

Dot dots[DOT_COUNT];
Battery batteries[BATTERY_COUNT];
Player player; // Instância do jogador

// Função para a câmera seguir o jogador
void cameraFollowPlayer() {
    float camX = player.posX;      // Posicionar a câmera na mesma linha X do jogador
    float camY = 8.0f;         // Eleva a câmera para uma visão de cima (Y mais alto)
    float camZ = player.posY - 5.0f; // Posicionar a câmera atrás do jogador no eixo Z

    gluLookAt(camX, camY, camZ, // Posição da câmera
              player.posX, 1.0f, player.posY, // Olhando para o jogador
              0.0f, 1.0f, 0.0f);     // Up vector
}

// Função para configurar a iluminação de forma mais flexível
void setLighting(float lightPos[4], float lightDir[3], float ambient[4], float diffuse[4], float specular[4], float shininess, float spotExponent) {
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, batteryCharge); // Usando o valor ajustável de spotCutoff
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent); // Define a suavização da borda do cone
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Função para a iluminação dinâmica durante o jogo
void updateLighting() {
    // A posição da luz será sempre a posição do jogador, para simular uma lanterna
    GLfloat lightPos[] = { (float)player.posX, 1.5f, (float)player.posY, 1.0f };

    // A direção da luz será ajustada com base no movimento do jogador
    GLfloat lightDir[] = { lightDirX, -0.5f, lightDirZ }; // Um pequeno ajuste na direção da luz para simular uma lanterna

    // Definindo características da luz ambiente, difusa e especular
    GLfloat ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Ajuste do brilho do material
    GLfloat shininess = 100.0f; // Brilho médio
    GLfloat spotExponent = 5.0f; // Controla a suavização da borda do cone (quanto maior, mais nítido)

    // Fatores de atenuação baseados em maxDistance
    float constantAttenuation = 1.0f;
    float linearAttenuation = 0.2f / maxDistance; // Ajuste linear para atingir 0 em maxDistance
    float quadraticAttenuation = 0.1f / (maxDistance * maxDistance); // Ajuste quadrático para suavizar o decaimento

    // Chamando a função que configura a iluminação
    setLighting(lightPos, lightDir, ambientLight, diffuseLight, specularLight, shininess, spotExponent);

    // Configura a atenuação da luz
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constantAttenuation);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linearAttenuation);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);
}

// Função para verificar a colisão com as paredes considerando o raio da esfera
bool checkCollision(float newX, float newY) {
    // Verifica se a nova posição está dentro dos limites da área de jogo
    if (newX > 0 && newX < WIDTH && newY > 0 && newY < HEIGHT) {
        // Define as coordenadas das bordas da esfera com base no raio
        int minX = (int)(newX - player.radius);
        int maxX = (int)(newX + player.radius);
        int minY = (int)(newY - player.radius);
        int maxY = (int)(newY + player.radius);

        // Verifica se qualquer parte da esfera colide com uma parede
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                // Verifica se (x, y) está dentro dos limites do labirinto
                if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                    // Verifica se há uma parede na posição (x, y)
                    if (maze[x][y] == 1) {
                        float wallCenterX = x + 0.5;
                        float wallCenterY = y + 0.5;

                        // Calcula a distância entre o centro da esfera e o centro da parede
                        float distX = newX - wallCenterX;
                        float distY = newY - wallCenterY;
                        float distance = sqrt(distX * distX + distY * distY);

                        // Se a distância for menor que o raio, houve colisão
                        if (distance < player.radius) {
                            return false; // Colisão detectada
                        }
                    }
                }
            }
        }
        return true; // Sem colisão
    }
    return false; // Fora dos limites
}


// Função para mover o jogador gradualmente em direção à direção definida
void movePlayer() {
    // Calcula a nova posição baseada na velocidade e direção
    float newX = player.posX + player.moveDirX * player.speed;
    float newY = player.posY + player.moveDirY * player.speed;

    if(newX > 0 && newX < WIDTH && newY > 0 && newY < HEIGHT) {
        if(maze[(int)floor(newX)][(int)floor(newY)] == 0) {
            // Verifica colisão com as paredes
            if (checkCollision(newX, newY)) {
                player.posX = newX;
                player.posY = newY;
                player.x = (int)(player.posX);
                player.y = (int)(player.posY);
                
                // Verifica colisão com objetos (se houver)
                checkObjectCollision(player.x, player.y);
            }
        }
    }
}

// Função para renderizar a cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLoadIdentity();

    // Configura a projeção 3D para renderizar o jogo
    setup3DProjection();  // Configura a projeção 3D para o jogo
    glPushMatrix();       // Salva o estado da transformação atual
    cameraFollowPlayer(); // Move a câmera para seguir o jogador
    updateLighting();     // Atualiza a iluminação de acordo com o jogador
    renderMaze();         // Renderiza o labirinto
    renderPlayerAndObjects(); // Renderiza o jogador e os dots
    glPopMatrix();        // Restaura o estado da transformação

    // Configura a projeção 2D para renderizar a UI
    setup2DProjection();
    glPushMatrix(); // Salva o estado atual da transformação
    renderDotCount(); // Renderiza o contador de dots
    renderGameTime();  // Renderiza o tempo de jogo
    renderBatteryUI();
    glPopMatrix(); // Restaura o estado de transformação

    glutSwapBuffers();
}

// Função para capturar o pressionamento do teclado e definir a direção de movimento
void keyboardDown(unsigned char key, int x, int y) {
    if(key == 's' || key == 'S') {
        player.moveDirX = 0.0f; 
        player.moveDirY = -0.1f; 
        lightDirX = 0.0f; 
        lightDirZ = -1.0f;
    }
    else if(key == 'w' || key == 'W') {
        player.moveDirX = 0.0f; 
        player.moveDirY = 0.1f;
        lightDirX = 0.0f; 
        lightDirZ = 1.0f;
    }
    else if(key == 'a' || key == 'A') {
        player.moveDirX = 0.1f; 
        player.moveDirY = 0.0f;
        lightDirX = 1.0f; 
        lightDirZ = 0.0f; 
    }
    else if(key == 'd' || key == 'D') {
        player.moveDirX = -0.1f; 
        player.moveDirY = 0.0f;
        lightDirX = -1.0f; 
        lightDirZ = 0.0f; 
    }
    else if(key == '+') {
        if (batteryCharge < 90.0f) {
            batteryCharge += 5.0f; 
            batteryPercentage += 7.143f; // Aumenta o raio da luz
        }
    }
    else if(key == '-') {
        if (batteryCharge > 10.0f) {
            batteryCharge -= 5.0f; 
            batteryPercentage -= 7.143f; // Diminui o raio da luz
        }
    }
    else if(key == 'r' || key == 'R') {
        initMaze();
        generateMaze(1, 1);
        spawnPlayer();
        spawnDots();
        spawnBatteries();
    }  
    else if(key == 27) {
        exit(0);
    } // ESC para sair
    
    glutPostRedisplay();
}

// Função para capturar o evento de soltar a tecla e zerar a direção de movimento
void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 's': case 'w': player.moveDirY = 0.0f; break;
        case 'd': case 'a': player.moveDirX = 0.0f; break;
    }
    glutPostRedisplay();
}

// Configurações de inicialização do OpenGL para 3D
void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); // Ativa a iluminação
    glEnable(GL_LIGHT0);    // Ativa a luz 0

    glClearColor(0.0, 0.0, 0.0, 1.0);
    initMaxFont("./fonts/Rexlia.ttf");  
    initMinFont("./fonts/Rexlia.ttf");
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Função para lidar com o redimensionamento da janela
void reshape(int w, int h) {
    // Define a proporção da janela
    window_width = w;
    window_height = h;

    glViewport(0, 0, w, h);

    // Define a matriz de projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Mantém a proporção da cena
    GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
    
    // Ajusta a perspectiva de acordo com a proporção
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

// Função de callback do GLUT para o loop do jogo
void update(int value) {
    movePlayer();            // Move o jogador de acordo com a direção e velocidade
    updateBattery();
    glutPostRedisplay();

    // Define o próximo loop de atualização (geralmente 16ms para 60fps)
    glutTimerFunc(16, update, 0); // Chama `update()` a cada 16ms (aproximadamente 60fps)
}

int main(int argc, char** argv) {
    srand(time(NULL));

    // Inicialização do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600); // Tamanho inicial da janela
    glutCreateWindow("Maze Game");

    // Configura o loop de atualização
    glutTimerFunc(25, update, 0); // Define o intervalo para 25ms (aproximadamente 40fps)

    // Funções de inicialização do OpenGL
    initOpenGL();
    initMaze();
    initializePlayer();
    generateMaze(1, 1);
    spawnPlayer();
    spawnDots();
    spawnBatteries();

    startGameTimer();  // Inicia o tempo no começo do jogo
    glutTimerFunc(1000, updateGameTime, 0);  // Inicia o timer para atualizar o tempo a cada segundo

    // Registra as funções de callback
    glutDisplayFunc(display);
    // Define callbacks de teclado
    glutKeyboardFunc(keyboardDown);  // Para quando a tecla é pressionada
    glutKeyboardUpFunc(keyboardUp);   // Para quando a tecla é liberada
    glutReshapeFunc(reshape); // Registra a função de reshape

    // Loop principal do GLUT
    glutMainLoop();

    return 0;
}
