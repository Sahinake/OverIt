#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <GL/glut.h>

#define WIDTH 20 
#define HEIGHT 20 
#define DOT_COUNT 30 
#define BATTERY_COUNT 5 
#define MAX_BATTERY 70.0f 
#define MAX_HEALTH 100.0f             
#define MAX_SANITY 100.0f            
#define BATTERY_DECREASE_RATE 0.01f 
#define HEALTH_DECREASE_RATE 0.02f
#define SANITY_DECREASE_RATE 0.02f

extern int maze[WIDTH][HEIGHT];
extern int maze_widht, maze_height;
extern int goalDots;            
extern int total_batteries;
extern float lightDirX;
extern float lightDirZ;         
extern float maxDistance;      
extern float batteryCharge;     
extern float batteryPercentage;

// Estrutura para o jogador
typedef struct {
    float posX, posY;           // Posição do jogador
    float health;               // Vida máxima do jogador
    float sanity;               // Sanidade máxima do jogador
    float speed;                // Velocidade de movimento do jogador
    float radius;               // Raio de colisão do jogador
    float moveDirX, moveDirY;   // Direção de movimento
    int flashlight;             // Lanterna ligada = 1, desligada = 0
    int x, y;                   // Posições inteiras (usadas para labirinto)
    int level;
} Player;

typedef struct {
    int x, y;
    bool collected;     // Indica se o dot já foi coletado
} Dot;

// Estrutura de Bateria
typedef struct {
    int x, y;
    bool collected;     // Indica se a bateria já foi coletada
} Battery;

typedef struct {
    int x, y;             // Posições X e Y no labirinto
    bool active;          // Indica se a saída está visível/ativada
    bool reached;         // Indica se o jogador alcançou a saída
} Exit;

extern Dot dots[DOT_COUNT];
extern Battery batteries[BATTERY_COUNT]; // Vetor de baterias
extern Player player;
extern Exit exitDoor;

void initMaze();
void generateMaze(int x, int y);
void spawnPlayer();
void spawnDots();
void spawnBatteries();
void initializePlayer();
void initializeExit();
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess);
void renderMaze();
bool isObjectVisible(int dotX, int dotY);
void renderPlayerAndObjects();
bool checkObjectCollision();
void updateBattery();
void updatePlayerStatus();
void generateExit();
void updateGame();
void renderScene();

#endif
