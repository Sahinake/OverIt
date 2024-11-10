#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <GL/glut.h>

#define WIDTH 20 // Largura do labirinto
#define HEIGHT 20 // Altura do labirinto
#define DOT_COUNT 30 // Quantidade de dots a serem coletados
#define BATTERY_COUNT 5 // Quantidade de baterias a serem spawnadas
#define MAX_BATTERY 70.0f // Capacidade máxima da bateria (100%)
#define BATTERY_DECREASE_RATE 0.02f // Taxa de diminuição da bateria por atualização de frame

extern int maze[WIDTH][HEIGHT];
extern int maze_widht, maze_height;
extern int goalDots; // Quantidade de dots que o jogador precisa coletar
extern int total_batteries;
extern float lightDirX;
extern float lightDirZ; // Inicialmente apontando para "frente"
extern float maxDistance; // Distância máxima para a lanterna
extern float batteryCharge; // Carga atual da bateria (de 0 a MAX_BATTERY)
extern float batteryPercentage;
extern float batteryDecrease;

// Estrutura para o jogador
typedef struct {
    float posX, posY;      // Posição do jogador
    float speed;           // Velocidade de movimento do jogador
    float radius;          // Raio de colisão do jogador
    float moveDirX, moveDirY; // Direção de movimento
    int x, y;              // Posições inteiras (usadas para labirinto)
} Player;

typedef struct {
    int x, y;
    bool collected; // Indica se o dot já foi coletado
} Dot;

// Estrutura de Bateria
typedef struct {
    int x, y;
    bool collected; // Indica se a bateria já foi coletada
} Battery;

extern Dot dots[DOT_COUNT];
extern Battery batteries[BATTERY_COUNT]; // Vetor de baterias
extern Player player;

void initMaze();
void generateMaze(int x, int y);
void spawnPlayer();
void spawnDots();
void spawnBatteries();
void initializePlayer();
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess);
void renderMaze();
bool isObjectVisible(int dotX, int dotY);
void renderPlayerAndObjects();
bool checkObjectCollision(int playerPosX, int playerPosY);
void updateBattery();

#endif // MAZE_H
