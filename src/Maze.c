#include "Maze.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "UI.h"
#include "Sound.h"
#include "miniaudio.h"

#define M_PI 3.14159265358979323846

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
    player.x = 1;
    player.y = 1;
    player.posX = (float)1;
    player.posZ = (float)1;
    maze[player.x][player.y] = 0; // Certifica que o ponto inicial está vazio
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
        maze[x][y] = 2;
    }
}

// Inicializa as baterias com valores padrão
void spawnBatteries() {
    for (int i = 0; i < BATTERY_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (WIDTH - 2) + 1; 
            y = rand() % (HEIGHT - 2) + 1;
        } while (maze[x][y] != 0);
        batteries[i].x = x;
        batteries[i].y = y;
        batteries[i].collected = false;
        maze[x][y] = 3;
    }
}

// Função para configurar o material de uma superfície
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Função para inicializar o jogador
void initializePlayer() {
    player.posX = 1.0f;            
    player.posZ = 0.0f;            
    // player.speed = 1.0f;          
    player.health = MAX_HEALTH;
    player.sanity = MAX_SANITY;
    player.radius = 0.3f;          
    // player.moveDirX = 0.0f;        
    // player.moveDirY = 0.0f;        
    player.x = (int)floor(player.posX); 
    player.y = (int)floor(player.posZ); 
    player.level = 1;
    player.flashlight = 1;
}

void initializeExit() {
    exitDoor.x = -1;
    exitDoor.y = -1;
    exitDoor.active = false; // A saída começa desativada
    exitDoor.reached = false;
}

// Função para renderizar o labirinto em 3D usando materiais e iluminação
void renderMaze() {
    // Define o material para o contorno azul
    GLfloat contourAmbient[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
    GLfloat contourDiffuse[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
    GLfloat contourSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat contourShininess = 20.0; // Brilho baixo

    // Define o material para o contorno azul
    GLfloat contourGroundAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Azul
    GLfloat contourGroundDiffuse[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
    GLfloat contourGroundSpecular[] = { 0.3, 0.3, 0.3, 1.0 };

    // Define o material para o preenchimento preto
    GLfloat fillAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Preto
    GLfloat fillDiffuse[] = { 0.2, 0.2, 0.2, 1.0 }; // Preto
    GLfloat fillSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat fillShininess = 20.0; // Brilho baixo

    // Configuração de iluminação
    GLfloat lightPosition[] = { 0.0, 10.0, 0.0, 1.0 }; // Posição da luz
    GLfloat lightAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Luz ambiente
    GLfloat lightDiffuse[] = { 0.2, 0.2, 0.2, 1.0 }; // Luz difusa
    GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 }; // Luz especular

    // Material para o chão (pode ser uma cor cinza ou marrom)
    GLfloat groundAmbient[] = { 0.0, 0.0, 0.0, 1.0 };  // Cor ambiente do chão (cinza claro)
    GLfloat groundDiffuse[] = { 0.2, 0.2, 0.2, 1.0 };  // Cor difusa do chão
    GLfloat groundSpecular[] = { 0.3, 0.3, 0.3, 1.0 }; // Reflexão especular (baixo brilho)
    GLfloat groundShininess = 10.0;  // Baixo brilho para simular um chão fosco

    GLfloat exitAmbient[] = { 0.0, 1.0, 0.0, 1.0 }; // Verde
    GLfloat exitDiffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat exitSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat exitShininess = 20.0;

    // Ativa a luz
    glLightfv(GL_LIGHT2, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);
    glEnable(GL_LIGHT2); // Habilita a luz
    glEnable(GL_LIGHT2); // Ativa a iluminação

    // Fatores de atenuação baseados em maxDistance
    float constantAttenuation = 0.2f;
    float linearAttenuation = 0.9f / maxDistance; // Ajuste linear para atingir 0 em maxDistance
    float quadraticAttenuation = 1.0f / (maxDistance * maxDistance); // Ajuste quadrático para suavizar o decaimento

    // Configura a atenuação da luz
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, constantAttenuation);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, linearAttenuation);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);

    // Configura a espessura do contorno
    glLineWidth(5.0f); // Aumenta a espessura do contorno (ajuste conforme necessário)

    for (int x = 0; x < WIDTH; x++) {
        for (int z = 0; z < HEIGHT; z++) {
            if (maze[x][z] == 1) {  // Desenhar paredes
                // Primeiro, desenha o preenchimento preto
                setMaterial(fillAmbient, fillDiffuse, fillSpecular, fillShininess);
                glPushMatrix();
                glTranslatef(x, 0.0f, z);  // Movimenta o cubo para a posição correta
                glutSolidCube(1);  // Renderiza um cubo com o tamanho de 1 unidade
                glPopMatrix();

                // Agora, desenha o contorno azul com iluminação
                setMaterial(contourAmbient, contourDiffuse, contourSpecular, contourShininess);
                glPushMatrix();
                glTranslatef(x, 0, z);
                glutWireCube(1); // Desenha o contorno com um cubo de wireframe
                glPopMatrix();
            }
            else { // Desenhar o chão
                // Aplicar o material do chão
                setMaterial(groundAmbient, groundDiffuse, groundSpecular, groundShininess);
                glPushMatrix();
                glTranslatef(x, -1.0f, z); // Coloca o chão abaixo das paredes
                glutSolidCube(1); // Desenha o cubo para o chão
                glPopMatrix();

                // Agora, desenha o contorno azul com iluminação
                setMaterial(contourGroundAmbient, contourGroundDiffuse, contourGroundSpecular, contourShininess);
                glPushMatrix();
                glTranslatef(x, -1.0f, z);
                glutWireCube(1); // Desenha o contorno com um cubo de wireframe
                glPopMatrix();

                if (exitDoor.active) {
                    setMaterial(exitAmbient, exitDiffuse, exitSpecular, exitShininess);

                    glPushMatrix();
                    glTranslatef(exitDoor.x, -1.0f, exitDoor.y); // Coloca a saída no chão
                    glutSolidCube(1); // Desenha um cubo para representar a saída
                    glPopMatrix();
                }
            }
        }
    }
}

bool isObjectVisible(int objX, int objY) {
    // Calcula a distância entre o jogador e o dot
    float dx = objX - player.x;
    float dy = objY - player.y;
    float distance = sqrt(dx * dx + dy * dy);

    // Verifica se o dot está dentro do alcance da lanterna
    if (distance > maxDistance) {
        return false;
    }

    // Calcula o ângulo entre a direção da lanterna e o dot
    float angle = atan2(dy, dx) * 180 / M_PI;  // Converte para graus
    float lightAngle = atan2(lightDirZ, lightDirX) * 180 / M_PI;

    // Calcula a diferença angular
    float angleDiff = fabs(angle - lightAngle);

    // Ajusta a diferença angular para que esteja entre 0 e 180 graus
    if (angleDiff > 180.0f) {
        angleDiff = 360.0f - angleDiff;
    }

    // Verifica se o dot está dentro do ângulo de abertura da lanterna
    return angleDiff <= batteryCharge;
}

// Função para renderizar o jogador e os dots usando materiais
void renderPlayerAndObjects() {
    // Define o material do jogador
    GLfloat playerAmbient[] = { 0.2, 0.0, 0.0, 1.0 };
    GLfloat playerDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat playerSpecular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat playerShininess = 50.0; // Brilho médio

    // Renderiza o jogador
    setMaterial(playerAmbient, playerDiffuse, playerSpecular, playerShininess);
    glPushMatrix();
    glTranslatef(player.posX, player.posY, player.posZ);
    glutSolidSphere(player.radius, 20, 20); // Usa uma esfera para o jogador
    glPopMatrix();

    // Define o material dos dots
    GLfloat dotAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat dotDiffuse[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat dotSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat dotShininess = 10.0; // Brilho baixo para dots

    // Renderiza os dots
    setMaterial(dotAmbient, dotDiffuse, dotSpecular, dotShininess);
    for (int i = 0; i < DOT_COUNT; i++) {
        if (!dots[i].collected && isObjectVisible(dots[i].x, dots[i].y)) {
            glPushMatrix();
            glTranslatef(dots[i].x, 0, dots[i].y);
            glutSolidSphere(0.15, 10, 10); // Usa uma esfera pequena para os dots
            glPopMatrix();
        }
    }

    // Define o material das baterias
    GLfloat batteryAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat batteryDiffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat batterySpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat batteryShininess = 10.0; // Brilho baixo para dots

    // Renderiza as baterias
    setMaterial(batteryAmbient, batteryDiffuse, batterySpecular, batteryShininess);
    for (int i = 0; i < BATTERY_COUNT; i++) {
        if (!batteries[i].collected && isObjectVisible(batteries[i].x, batteries[i].y)) {
            glPushMatrix();
            glTranslatef(batteries[i].x, 0, batteries[i].y);
            glutSolidSphere(0.15, 10, 10); // Usa uma esfera pequena para os dots
            glPopMatrix();
        }
    }
}

// Função para verificar colisão com dots ou baterias
bool checkObjectCollision() {
    if (maze[player.x][player.y] == 2) {
        for (int i = 0; i < DOT_COUNT; i++) {
            if (!dots[i].collected && player.x == dots[i].x && player.y == dots[i].y) {
                dots[i].collected = true;
                playDotCollectSound();
                goalDots--;
                maze[player.x][player.y] = 0;
                if (goalDots == 0) {
                    printf("Você coletou todos os dots!\n");
                }
            }
        }
    }

    if (maze[player.x][player.y] == 3) {
            for (int i = 0; i < BATTERY_COUNT; i++) {
            if (!batteries[i].collected && player.x == batteries[i].x && player.y == batteries[i].y) {
                batteries[i].collected = true;
                total_batteries--;
                maze[player.x][player.y] = 0;
                if(batteryPercentage <= 85.0f) {
                    batteryPercentage += 15.0f;
                }
                else {
                    batteryPercentage = 100.0f;
                }
                
            }
        }
    }
    
}

// Função para diminuir a bateria ao longo do tempo
void updateBattery() {
    if (player.flashlight == 0) {
        batteryPercentage += 0.01f;
        batteryCharge = 0.0f;
        maxDistance = 0.0f;

        if (batteryPercentage >= 100.0f) {
            batteryPercentage = 100.0f;
        }
    }
    else if (batteryPercentage > 100.0f) {
        batteryCharge = MAX_BATTERY; // Diminui a bateria por frame
        batteryPercentage = 100.0;
        maxDistance = 5.0f;
    }
    else if (batteryPercentage > 50.0f) {
        batteryCharge = MAX_BATTERY; // Diminui a bateria por frame
        batteryPercentage -= BATTERY_DECREASE_RATE;
        maxDistance = 5.0f;
    }
    else if (batteryPercentage > 20.0f) {
        batteryCharge = 50.0f;
        batteryPercentage -= BATTERY_DECREASE_RATE;
        maxDistance = 4.0f;
    }
    else if (batteryPercentage > 0.0f) {
        batteryCharge = 30.0f;
        batteryPercentage -= BATTERY_DECREASE_RATE;
        maxDistance = 3.0f;
    }
    else if (batteryPercentage <= 0.0f) {
        batteryPercentage = 0.0f;
        batteryPercentage = 0.0f;
        maxDistance = 0.0f;
    }
}

// Função para atualizar a sanidade e a vida do jogador
void updatePlayerStatus() {
    // Verifica se a bateria está vazia
    if (batteryPercentage <= 0.0f) {
        // A sanidade diminui quando a bateria está zerada
        player.sanity -= SANITY_DECREASE_RATE;
        
        // Garante que a sanidade não seja menor que 0
        if (player.sanity < 0.0f) {
            player.sanity = 0.0f;
        }
    }

    if (player.flashlight == 0) {
        player.sanity -= 0.01f;

        // Garante que a sanidade não seja menor que 0
        if (player.sanity < 0.0f) {
            player.sanity = 0.0f;
        }
    }

    // Se a sanidade for 0, a vida também começa a diminuir
    if (player.sanity == 0.0f) {
        player.health -= HEALTH_DECREASE_RATE;
        
        // Garante que a vida não seja menor que 0
        if (player.health < 0.0f) {
            player.health = 0.0f;
        }
    }
}

// Função para gerar a saída em uma posição aleatória
void generateExit() {
    int x, y;
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (maze[x][y] == 1); // Garante que a saída não está em uma parede
    
    exitDoor.x = x;
    exitDoor.y = y;
    printf("Coordenadas da porta: %d, %d\n", exitDoor.x, exitDoor.y);
}

// Função principal de atualização
void updateGame() {
    if (goalDots == 0 && !exitDoor.active) {
        exitDoor.active = true; // Cria a saída após coletar todos os dots
    }

    // Checa se o jogador chegou à saída
    if (exitDoor.active && player.x == exitDoor.x && player.y == exitDoor.y) {
        printf("Nível concluído! Indo para o próximo nível...\n");
        player.level ++;
        initMaze();
        generateMaze(1, 1);
        spawnPlayer();
        spawnDots();
        spawnBatteries();
        initializeExit();
        generateExit();
        batteryPercentage = 100.0f;
    }
}

// Função para renderizar o labirinto e outros elementos
void renderScene() {
    renderMaze();
    renderPlayerAndObjects();
}