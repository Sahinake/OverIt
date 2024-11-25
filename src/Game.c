#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ObjLoader.h"
#include "UI.h"
#include "Game.h"
#include "Sound.h"
#include "miniaudio.h"
#include "SaveLoad.h"

#define M_PI 3.14159265358979323846
#define BATTERY_DECREASE_RATE 0.01f 
#define HEALTH_DECREASE_RATE 0.02f
#define SANITY_DECREASE_RATE 0.02f
#define MAX_BATTERY_PERCENTAGE 100.0f
#define MAX_BATTERY 70.0f 
#define DOT_COUNT 30

extern char saveName[256];

// Definindo materiais e iluminação como variáveis globais
GLfloat contourAmbient[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
GLfloat contourDiffuse[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
GLfloat contourSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat contourShininess = 20.0;

GLfloat contourGroundAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Azul
GLfloat contourGroundDiffuse[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
GLfloat contourGroundSpecular[] = { 0.3, 0.3, 0.3, 1.0 };

GLfloat fillAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Preto
GLfloat fillDiffuse[] = { 0.1, 0.1, 0.1, 1.0 }; // Preto
GLfloat fillSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat fillShininess = 20.0;

GLfloat lightPosition[] = { 0.0, 10.0, 0.0, 1.0 }; // Posição da luz
GLfloat lightAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Luz ambiente
GLfloat lightDiffuse[] = { 0.2, 0.2, 0.2, 1.0 }; // Luz difusa
GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 }; // Luz especular

GLfloat groundAmbient[] = { 0.0, 0.0, 0.0, 1.0 };  // Cor ambiente do chão (cinza claro)
GLfloat groundDiffuse[] = { 0.1, 0.1, 0.1, 1.0 };  // Cor difusa do chão
GLfloat groundSpecular[] = { 0.3, 0.3, 0.3, 1.0 }; // Reflexão especular (baixo brilho)
GLfloat groundShininess = 10.0;  // Baixo brilho para simular um chão fosco

GLfloat exitAmbient[] = { 0.0, 1.0, 0.0, 1.0 }; // Verde
GLfloat exitDiffuse[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat exitSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat exitShininess = 20.0;

void initializeRendering() {
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
}

// Função para inicializar o labirinto com paredes
void initMaze(Game* game) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            game->maze[i][j] = 1; // Coloca uma parede em cada célula
        }
    }
}

// Função para inicializar o jogo
void initGame(Game* game, Player* player) {
    initMaze(game);
    generateMaze(game, 1, 1);
    initializePlayer(player);
    spawnPlayer(game, player);
    spawnDots(game);
    spawnBatteries(game);
    generateExit(game);
    if(wasTheGameSaved == 0) {
        saveGame(saveName, player, game, elapsedTime);
        wasTheGameSaved = 1;
    }

    glutPostRedisplay();
}

// Algoritmo para geração procedural do labirinto
void generateMaze(Game* game, int x, int y) {
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    
    game->maze[x][y] = 0; // Marca o caminho atual como vazio

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
        
        if (newX > 0 && newX < WIDTH && newY > 0 && newY < HEIGHT && game->maze[newX][newY] == 1) {
            game->maze[x + directions[i][0]][y + directions[i][1]] = 0; // Remove a parede
            generateMaze(game, newX, newY);
        }
    }
}

// Função para posicionar o jogador no ponto inicial
void spawnPlayer(Game* game, Player* player) {
    player->x = 1;
    player->y = 1;
    player->posX = (float)1;
    player->posZ = (float)1;
    game->maze[player->x][player->y] = 0; // Certifica que o ponto inicial está vazio
}

// Função para gerar dots aleatoriamente em posições válidas no labirinto
void spawnDots(Game* game) {
    goalDots = DOT_COUNT; // Total de dots que o jogador precisa coletar
    for (int i = 0; i < DOT_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (WIDTH - 2) + 1; // Evita as bordas
            y = rand() % (HEIGHT - 2) + 1;
        } while (game->maze[x][y] != 0); // Garante que o dot não está em uma parede
        game->dots[i].x = x;
        game->dots[i].y = y;
        game->dots[i].collected = false;
        game->maze[x][y] = 2;
    }
}

// Inicializa as baterias com valores padrão
void spawnBatteries(Game* game) {
    for (int i = 0; i < BATTERY_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (WIDTH - 2) + 1; 
            y = rand() % (HEIGHT - 2) + 1;
        } while (game->maze[x][y] != 0);
        game->batteries[i].x = x;
        game->batteries[i].y = y;
        game->batteries[i].collected = false;
        game->maze[x][y] = 3;
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
void initializePlayer(Player* player) {
    player->posX = 1.0f;            
    player->posZ = 0.0f;            
    // player.speed = 1.0f;          
    player->health = MAX_HEALTH;
    player->sanity = MAX_SANITY;
    player->radius = 0.3f;          
    // player.moveDirX = 0.0f;        
    // player.moveDirY = 0.0f;        
    player->x = (int)floor(player->posX); 
    player->y = (int)floor(player->posZ); 
    player->level = 1;
    player->flashlight = 1;
    player->flashlightPercentage = MAX_BATTERY_PERCENTAGE;
    player->flashlightCharge = MAX_BATTERY;
    player->dotsCount = DOT_COUNT;
    player->rotation = 90.0f;
}

void initializeExit(Game* game) {
    game->exitDoor.x = -1;
    game->exitDoor.y = -1;
    game->exitDoor.active = false; // A saída começa desativada
    game->exitDoor.reached = false;
}

// Função para renderizar o labirinto em 3D usando materiais e iluminação
void renderMaze(Game* game) {
    for (int x = 0; x < WIDTH; x++) {
        for (int z = 0; z < HEIGHT; z++) {
            if (game->maze[x][z] == 1) {  // Desenhar paredes
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

                if (game->exitDoor.active) {
                    setMaterial(exitAmbient, exitDiffuse, exitSpecular, exitShininess);

                    glPushMatrix();
                    glTranslatef(game->exitDoor.x, -1.0f, game->exitDoor.y); // Coloca a saída no chão
                    glutSolidCube(1); // Desenha um cubo para representar a saída
                    glPopMatrix();
                }
            }
        }
    }
}

bool isObjectVisible(Player* player, int objX, int objY) {
    // Calcula a distância entre o jogador e o dot
    float dx = objX - player->x;
    float dy = objY - player->y;
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
    return angleDiff <= player->flashlightCharge;
}

// Função para renderizar o jogador e os dots usando materiais
void renderPlayerAndObjects(Game* game, Player* player, Object* playerModel) {
    // Define o material do jogador
    GLfloat playerAmbient[] = { 0.2, 0.0, 0.0, 1.0 };
    GLfloat playerDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat playerSpecular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat playerShininess = 50.0; // Brilho médio

    // Renderiza o jogador (substituindo a esfera pelo modelo)
    if (playerModel != NULL) {
        setMaterial(playerAmbient, playerDiffuse, playerSpecular, playerShininess);
        glPushMatrix();
        glTranslatef(player->posX, player->posY, player->posZ); // Move para a posição do jogador
        glScalef(player->radius, player->radius, player->radius); // Escala para o tamanho do jogador
        glRotatef(player->rotation, 0.0f, 1.0f, 0.0f);  // Rotaciona o player no eixo Y
        
        // Renderiza o modelo do jogador
        for (int i = 0; i < playerModel->size; i++) {
            glBegin(GL_TRIANGLES);
            Face face = playerModel->faces[i];

            // Define normais e vértices para cada face
            glNormal3f(face.normaA.x, face.normaA.y, face.normaA.z);
            glVertex3f(face.vertexA.x, face.vertexA.y, face.vertexA.z);

            glNormal3f(face.normaB.x, face.normaB.y, face.normaB.z);
            glVertex3f(face.vertexB.x, face.vertexB.y, face.vertexB.z);

            glNormal3f(face.normaC.x, face.normaC.y, face.normaC.z);
            glVertex3f(face.vertexC.x, face.vertexC.y, face.vertexC.z);
            glEnd();
        }
        glPopMatrix();
    } else {
        // Renderiza a esfera caso o modelo não tenha sido carregado
        setMaterial(playerAmbient, playerDiffuse, playerSpecular, playerShininess);
        glPushMatrix();
        glTranslatef(player->posX, player->posY, player->posZ);
        glutSolidSphere(player->radius, 20, 20);
        glPopMatrix();
    }

    // Define o material dos dots
    GLfloat dotAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat dotDiffuse[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat dotSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat dotShininess = 10.0; // Brilho baixo para dots

    // Renderiza os dots
    setMaterial(dotAmbient, dotDiffuse, dotSpecular, dotShininess);
    for (int i = 0; i < DOT_COUNT; i++) {
        if (!game->dots[i].collected && isObjectVisible(player, game->dots[i].x, game->dots[i].y)) {
            glPushMatrix();
            glTranslatef(game->dots[i].x, 0, game->dots[i].y);
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
        if (!game->batteries[i].collected && isObjectVisible(player, game->batteries[i].x, game->batteries[i].y)) {
            glPushMatrix();
            glTranslatef(game->batteries[i].x, 0, game->batteries[i].y);
            glutSolidSphere(0.15, 10, 10); // Usa uma esfera pequena para os dots
            glPopMatrix();
        }
    }
}

// Função para verificar colisão com dots ou baterias
bool checkObjectCollision(Game* game, Player* player) {
    if (game->maze[player->x][player->y] == 2) {
        for (int i = 0; i < DOT_COUNT; i++) {
            if (!game->dots[i].collected && player->x == game->dots[i].x && player->y == game->dots[i].y) {
                game->dots[i].collected = true;
                playDotCollectSound();
                goalDots--;
                game->maze[player->x][player->y] = 0;
                if (goalDots == 0) {
                    printf("Você coletou todos os dots!\n");
                }
            }
        }
    }

    if (game->maze[player->x][player->y] == 3) {
            for (int i = 0; i < BATTERY_COUNT; i++) {
            if (!game->batteries[i].collected && player->x == game->batteries[i].x && player->y == game->batteries[i].y) {
                game->batteries[i].collected = true;
                total_batteries--;
                game->maze[player->x][player->y] = 0;
                player->flashlightPercentage += 15.0f;
            }
        }
    }
    
}

// Função para diminuir a bateria ao longo do tempo
void updateBattery(Player* player) {
    if (player->flashlight == 0) {
        player->flashlightPercentage += BATTERY_DECREASE_RATE;
        player->flashlightCharge = 0.0f;
        maxDistance = 0.0f;

        if (player->flashlightPercentage >= 100.0f) {
            player->flashlightPercentage = 100.0f;
        }
    }
    else {
        if (player->flashlightPercentage > 100.0f) {
            player->flashlightCharge = MAX_BATTERY; // Diminui a bateria por frame
            player->flashlightPercentage = 100.0;
            maxDistance = 5.0f;
        }
        else if (player->flashlightPercentage > 50.0f) {
            player->flashlightCharge = MAX_BATTERY; // Diminui a bateria por frame
            player->flashlightPercentage -= BATTERY_DECREASE_RATE;
            maxDistance = 5.0f;
        }
        else if (player->flashlightPercentage > 20.0f) {
            player->flashlightCharge = 50.0f;
            player->flashlightPercentage -= BATTERY_DECREASE_RATE;
            maxDistance = 4.0f;
        }
        else if (player->flashlightPercentage > 0.0f) {
            player->flashlightCharge = 30.0f;
            player->flashlightPercentage -= BATTERY_DECREASE_RATE;
            maxDistance = 3.0f;
        }
        else if (player->flashlightPercentage <= 0.0f) {
            player->flashlightPercentage = 0.0f;
            player->flashlightPercentage = 0.0f;
            maxDistance = 0.0f;
        }
    }
}

// Função para atualizar a sanidade e a vida do jogador
void updatePlayerStatus(Player* player) {
    // Verifica se a bateria está vazia
    if (player->flashlightPercentage <= 0.0f) {
        // A sanidade diminui quando a bateria está zerada
        player->sanity -= SANITY_DECREASE_RATE;
        
        // Garante que a sanidade não seja menor que 0
        if (player->sanity < 0.0f) {
            player->sanity = 0.0f;
        }
    }

    if (player->flashlight == 0) {
        player->sanity -= 0.01f;

        // Garante que a sanidade não seja menor que 0
        if (player->sanity < 0.0f) {
            player->sanity = 0.0f;
        }
    }

    // Se a sanidade for 0, a vida também começa a diminuir
    if (player->sanity == 0.0f) {
        player->health -= HEALTH_DECREASE_RATE;
        
        // Garante que a vida não seja menor que 0
        if (player->health < 0.0f) {
            player->health = 0.0f;
        }
    }
}

// Função para gerar a saída em uma posição aleatória
void generateExit(Game* game) {
    int x, y;
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (game->maze[x][y] == 1); // Garante que a saída não está em uma parede
    
    game->exitDoor.x = x;
    game->exitDoor.y = y;
    printf("Coordenadas da porta: %d, %d\n", game->exitDoor.x, game->exitDoor.y);
}

// Função principal de atualização
int updateGame(Game* game, Player* player) {
    if (goalDots == 0 && !game->exitDoor.active) {
        game->exitDoor.active = true; // Cria a saída após coletar todos os dots
    }

    // Checa se o jogador chegou à saída
    if (game->exitDoor.active && player->x == game->exitDoor.x && player->y == game->exitDoor.y) {
        printf("Nível concluído! Indo para o próximo nível...\n");
        player->level ++;
        initMaze(game);
        generateMaze(game, 1, 1);
        spawnPlayer(game, player);
        spawnDots(game);
        spawnBatteries(game);
        initializeExit(game);
        generateExit(game);
        saveGame(saveName, player, game, elapsedTime);
        return 1;
    }
    return 0;
}

// Função para renderizar o labirinto e outros elementos
void renderScene(Game* game, Player* player, Object* playerModel) {
    renderMaze(game);
    renderPlayerAndObjects(game, player, playerModel);
}