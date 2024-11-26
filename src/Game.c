#include <time.h>
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
#define MAX_RANKING_SIZE 10

extern char saveName[256];
extern GLuint batteryTexture;
bool useTexture = true;

// Definindo materiais e iluminação como variáveis globais
GLfloat contourAmbient[] = { 0.0, 0.0, 0.3, 1.0 }; // Azul
GLfloat contourDiffuse[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
GLfloat contourSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat contourShininess = 20.0;

GLfloat contourGroundAmbient[] = { 0.0, 0.0, 0.3, 1.0 }; // Azul
GLfloat contourGroundDiffuse[] = { 0.0, 0.0, 1.0, 1.0 }; // Azul
GLfloat contourGroundSpecular[] = { 0.3, 0.3, 0.3, 1.0 };

GLfloat fillAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Preto
GLfloat fillDiffuse[] = { 0.0, 0.0, 0.1, 1.0 }; // Preto
GLfloat fillSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat fillShininess = 20.0;

GLfloat lightPosition[] = { 0.0, 1.0, 0.0, 1.0 }; // Posição da luz
GLfloat lightAmbient[] = { 0.0, 0.0, 0.0, 1.0 }; // Luz ambiente
GLfloat lightDiffuse[] = { 0.7, 0.7, 0.7, 1.0 }; // Luz difusa
GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 }; // Luz especular

GLfloat groundAmbient[] = { 0.0, 0.0, 0.0, 1.0 };  // Cor ambiente do chão (cinza claro)
GLfloat groundDiffuse[] = { 0.0, 0.0, 0.1, 1.0 };  // Cor difusa do chão
GLfloat groundSpecular[] = { 0.3, 0.3, 0.3, 1.0 }; // Reflexão especular (baixo brilho)
GLfloat groundShininess = 20.0;  // Baixo brilho para simular um chão fosco

GLfloat exitAmbient[] = { 0.0, 0.5, 0.0, 1.0 }; // Verde
GLfloat exitDiffuse[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat exitSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat exitShininess = 20.0;

// Define o material do jogador
GLfloat playerAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat playerDiffuse[] = { 1.0, 1.0, 0.0, 1.0 };
GLfloat playerSpecular[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat playerShininess = 50.0; // Brilho médio

// Define o material dos dots
GLfloat dotAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat dotDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat dotSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat dotShininess = 10.0; // Brilho baixo para dots

// Define o material das baterias
GLfloat batteryAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat batteryDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat batterySpecular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat batteryShininess = 10.0; // Brilho baixo para dots

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
void renderPlayerAndObjects(Game* game, Player* player, Object* batteryModel) {
    // Renderiza a esfera caso o modelo não tenha sido carregado
    setMaterial(playerAmbient, playerDiffuse, playerSpecular, playerShininess);
    glPushMatrix();
    glTranslatef(player->posX, player->posY, player->posZ);
    glutSolidSphere(player->radius, 20, 20);
    glPopMatrix();

    // Renderiza os dots usando o objeto importado
    setMaterial(dotAmbient, dotDiffuse, dotSpecular, dotShininess);
    for (int i = 0; i < DOT_COUNT; i++) {
        if (!game->dots[i].collected && isObjectVisible(player, game->dots[i].x, game->dots[i].y)) {
            glPushMatrix();
            glTranslatef(game->dots[i].x, 0, game->dots[i].y);
            glutSolidSphere(0.15, 10, 10); // Usa uma esfera pequena para os dots
            glPopMatrix();
        }
    }

    setMaterial(batteryAmbient, batteryDiffuse, batterySpecular, batteryShininess);
    for (int i = 0; i < BATTERY_COUNT; i++) {
        if (!game->batteries[i].collected && isObjectVisible(player, game->batteries[i].x, game->batteries[i].y)) {
            glPushMatrix();
            glTranslatef(game->batteries[i].x, 0, game->batteries[i].y);
            glScalef(0.08, 0.08, 0.08); 
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, batteryTexture); // Aplica a textura da bateria
            
            // Renderiza o modelo da bateria
            if (batteryModel != NULL) {
                for (int i = 0; i < batteryModel->size; i++) {
                    glBegin(GL_TRIANGLES);
                    Face face = batteryModel->faces[i];

                    // Define normais e vértices para cada face
                    glNormal3f(face.normaA.x, face.normaA.y, face.normaA.z);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(face.vertexA.x, face.vertexA.y, face.vertexA.z);

                    glNormal3f(face.normaB.x, face.normaB.y, face.normaB.z);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(face.vertexB.x, face.vertexB.y, face.vertexB.z);

                    glNormal3f(face.normaC.x, face.normaC.y, face.normaC.z);
                    glTexCoord2f(0.5f, 1.0f);
                    glVertex3f(face.vertexC.x, face.vertexC.y, face.vertexC.z);
                    glEnd();
                }
                glPopMatrix();
            }
            else {
                // Renderiza as baterias usando a textura
                glPushMatrix();
                glTranslatef(game->batteries[i].x, 0, game->batteries[i].y);
                glBindTexture(GL_TEXTURE_2D, batteryTexture); // Aplica a textura
                glutSolidSphere(0.15, 10, 10); // Usa uma esfera com a textura
                glPopMatrix();
            }
            glDisable(GL_TEXTURE_2D);
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
                playFlashlightChangeSound();
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
void updatePlayerStatus(Game* game, Player* player) {
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

    if (player->health == 0.0f) {
        game->currentState = FINISHED;
        addToRanking(game, "Oliver", elapsedTime, calculateScore(player->level, elapsedTime));
        saveRankingToFile(game, "./ranking.dat");
        if(wasTheGameSaved == 0) {
            saveGame(saveName, player, game, elapsedTime);
            wasTheGameSaved = 1;
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
void renderScene(Game* game, Player* player, Object* batteryModel) {
    renderMaze(game);
    renderPlayerAndObjects(game, player, batteryModel);
}

void adjustBrightness(Game* game, float factor) {
    game->brightness *= factor;

    // Garantir que o brilho esteja dentro de limites razoáveis
    if (game->brightness <= 0.0f) game->brightness = 0.1f;  // Valor mínimo de brilho
    if (game->brightness > 4.0f) game->brightness = 4.0f;  // Valor máximo de brilho

    // Ajustar a intensidade das luzes e materiais
    lightDiffuse[0] = lightDiffuse[1] = lightDiffuse[2] = 0.7f * game->brightness;
    lightSpecular[0] = lightSpecular[1] = lightSpecular[2] = 1.0f * game->brightness;

    // Atualiza as luzes no OpenGL
    glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);

    // Refatorar o modelo de iluminação
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fillDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, playerSpecular);
}

// Função para calcular o score com base no nível e no tempo
int calculateScore(int level, int elapsedTime) {
    // A penalização do tempo diminui com o aumento do nível
    float timePenalty = (float)elapsedTime / (1 + log((float)(level + 1)));
    
    // A pontuação é o nível multiplicado por um fator de 1000, subtraído da penalização do tempo
    int score = (level * 1000) - (int)timePenalty;
    
    // Garantir que a pontuação não seja negativa
    if (score < 0) {
        score = 0;
    }
    return score;
}

// Função para adicionar um jogador ao ranking
void addToRanking(Game* game, const char* name, int elapsedTime, int score) {
    if (game->rankingCount < MAX_RANKING_SIZE) {
        // Adiciona o jogador ao ranking
        strncpy(game->rankingList[game->rankingCount].name, name, sizeof(game->rankingList[game->rankingCount].name) - 1);
        game->rankingList[game->rankingCount].elapsedTime = elapsedTime;
        game->rankingList[game->rankingCount].score = score;

        // Captura a data e hora de encerramento
        time_t now = time(NULL);  // Obtém o tempo atual
        game->rankingList[game->rankingCount].endTime = *localtime(&now);  // Converte para o formato local de data e hora

        game->rankingCount++;
    } else {
        // O ranking está cheio, adicionar o jogador apenas se ele for melhor que o pior jogador
        int minIndex = 0;
        for (int i = 1; i < MAX_RANKING_SIZE; i++) {
            if (game->rankingList[i].score < game->rankingList[minIndex].score ||
                (game->rankingList[i].score == game->rankingList[minIndex].score && game->rankingList[i].elapsedTime < game->rankingList[minIndex].elapsedTime)) {
                minIndex = i;
            }
        }
        // Se a pontuação ou o tempo for melhor, substitui a entrada no ranking
        if (score > game->rankingList[minIndex].score || 
            (score == game->rankingList[minIndex].score && elapsedTime < game->rankingList[minIndex].elapsedTime)) {
            strncpy(game->rankingList[minIndex].name, name, sizeof(game->rankingList[minIndex].name) - 1);
            game->rankingList[minIndex].elapsedTime = elapsedTime;
            game->rankingList[minIndex].score = score;

            // Captura a data e hora de encerramento
            time_t now = time(NULL);  // Obtém o tempo atual
            game->rankingList[minIndex].endTime = *localtime(&now);  // Converte para o formato local de data e hora
        }
    }
    // Ordenar o ranking por pontuação e tempo (do maior para o menor)
    for (int i = 0; i < game->rankingCount - 1; i++) {
        for (int j = i + 1; j < game->rankingCount; j++) {
            if (game->rankingList[i].score < game->rankingList[j].score || 
                (game->rankingList[i].score == game->rankingList[j].score && game->rankingList[i].elapsedTime > game->rankingList[j].elapsedTime)) {
                // Trocar as posições
                Ranking temp = game->rankingList[i];
                game->rankingList[i] = game->rankingList[j];
                game->rankingList[j] = temp;
            }
        }
    }
}

// Função para exibir o ranking
void displayRanking(Game* game) {
    printf("Ranking:\n");
    for (int i = 0; i < game->rankingCount; i++) {
        printf("%d. %s - %d pontos - Tempo: %d segundos\n", i + 1, game->rankingList[i].name, game->rankingList[i].score, game->rankingList[i].elapsedTime);
    }
}

// Função para salvar o ranking em um arquivo binário
void saveRankingToFile(Game* game, const char* fileName) {
    FILE* file = fopen(fileName, "wb");
    if (file == NULL) {
        printf("Erro ao salvar o ranking.\n");
        return;
    }
    
    // Salva o número de entradas no ranking
    fwrite(&game->rankingCount, sizeof(int), 1, file);
    
    // Salva cada entrada do ranking, incluindo a data de encerramento
    for (int i = 0; i < game->rankingCount; i++) {
        fwrite(&game->rankingList[i], sizeof(Ranking), 1, file);
    }
    
    fclose(file);
}

// Função para carregar o ranking de um arquivo binário
void loadRankingFromFile(Game* game, const char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Nenhum ranking encontrado, criando novo ranking.\n");
        game->rankingCount = 0;
        return;
    }
    
    // Carrega o número de entradas no ranking
    fread(&game->rankingCount, sizeof(int), 1, file);
    
    // Carrega cada entrada do ranking, incluindo a data de encerramento
    for (int i = 0; i < game->rankingCount; i++) {
        fread(&game->rankingList[i], sizeof(Ranking), 1, file);
    }
    
    fclose(file);
}