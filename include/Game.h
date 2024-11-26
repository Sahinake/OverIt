#ifndef GAME_H
#define GAME_H

#include <time.h> 
#include <stdbool.h>
#include <GL/glut.h>
#include "ObjLoader.h"

#define WIDTH 20 
#define HEIGHT 20 
#define DOT_COUNT 30 
#define MAX_SAVES 4
#define BATTERY_COUNT 5 
#define MAX_BATTERY 70.0f 
#define MAX_HEALTH 100.0f             
#define MAX_SANITY 100.0f            
#define BATTERY_DECREASE_RATE 0.01f 
#define HEALTH_DECREASE_RATE 0.02f
#define SANITY_DECREASE_RATE 0.02f
#define MAX_RANKING_SIZE 10

extern int maze_widht, maze_height;
extern int goalDots;            
extern int total_batteries;
extern float lightDirX;
extern float lightDirZ;         
extern float maxDistance;
extern int wasTheGameSaved;

// Estrutura para o jogador
typedef struct {
    float posX, posY, posZ;     // Posição do jogador
    float targetX, targetZ;     // Posições de destino para o movimento suave
    float speedX, speedZ;       // Velocidade de movimento
    float health;               // Vida máxima do jogador
    float sanity;               // Sanidade máxima do jogador
    float radius;               // Raio de colisão do jogador
    int moving;                 // Flag para indicar se o jogador está se movendo
    int flashlight;             // Lanterna ligada = 1, desligada = 0
    float flashlightPercentage;
    float flashlightCharge;
    int dotsCount;
    int x, y;                   // Posições inteiras (usadas para labirinto)
    int level;
    float rotation;
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

// Definindo os estados possíveis do jogo
typedef enum {
    MAIN_MENU,       // Menu Principal
    NEW_GAME_MENU,   // Menu de Novo Jogo
    LOAD_GAME_MENU,  // Menu de Carregar Jogo
    RANKING_MENU,    // Menu de Ranking
    OPTIONS_MENU,    // Menu de Opções
    PLAYING,         // Jogo em execução
    FINISHED         // Jogador já morreu   
} GameState;

typedef struct {
    char name[32];
    int elapsedTime;
    int score;     // Indica se o dot já foi coletado
    struct tm endTime;       // Data e hora do encerramento do jogo
} Ranking;

// Estrutura para armazenar o estado do jogo
typedef struct {
    char* slotFiles[MAX_SAVES];  // Array para armazenar os nomes dos arquivos de save
    GameState currentState;     // Estado atual do jogo
    int selectedOption;         // Opção selecionada no menu
    int maze[WIDTH][HEIGHT];
    Dot dots[DOT_COUNT];
    Battery batteries[BATTERY_COUNT];
    Exit exitDoor;
    float volumeEffects;
    float volumeMusic;
    float volumeAmbient;
    float brightness;
    Ranking rankingList[MAX_RANKING_SIZE];  // Lista de ranking
    int rankingCount;                       // Contador de jogadores no ranking
} Game;

// Estrutura para armazenar coordenadas
typedef struct {
    float x, y;
} Point;

void initializeRendering();
void initMaze(Game* game);
void initGame(Game* game, Player* player);
void generateMaze(Game* game, int x, int y);
void spawnPlayer(Game* game, Player* player);
void spawnDots(Game* game);
void spawnBatteries(Game* game);
void initializePlayer(Player* player);
void initializeExit(Game* game);
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess);
void renderMaze(Game* game);
bool isObjectVisible(Player* player, int objX, int objY);
void renderPlayerAndObjects(Game* game, Player* player, Object* batteryModel);
bool checkObjectCollision(Game* game, Player* player) ;
void updateBattery(Player* player);
void updatePlayerStatus(Game* game, Player* player);
void generateExit(Game* game);
int updateGame(Game* game, Player* player);
void renderScene(Game* game, Player* player, Object* batteryModel);
void renderScene(Game* game, Player* player, Object* batteryModel);
void adjustBrightness(Game* game, float factor);
void addToRanking(Game* game, const char* name, int elapsedTime, int score);
void displayRanking(Game* game);
void saveRankingToFile(Game* game, const char* fileName);
void loadRankingFromFile(Game* game, const char* fileName);
int calculateScore(int level, int elapsedTime);

#endif
