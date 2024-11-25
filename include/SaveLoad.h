#ifndef SAVELOAD
#define SAVELOAD

#include <time.h>
#include "Game.h"
#include "UI.h"

extern time_t lastSaveTime;

// Estrutura que armazena o estado do jogo para salvar e carregar
typedef struct {
    char saveName[100];       // Nome do arquivo de save
    time_t lastPlayed;        // Data da última vez que o jogo foi jogado
    Dot dots[DOT_COUNT];             // Objetos do jogo
    Battery batteries[BATTERY_COUNT];     // Bateria do jogo
    Player player;           // Posição e status do jogador
    int elapsedSaveTime;          // Tempo decorrido
    Exit exitDoor;            // Saída do 
    int maze[WIDTH][HEIGHT];
    GameState currentState;
} SavedGame;

void createSavesDirectory();
void removeFinishedSaves(const char* directoryPath) ;
void saveGame(const char* filename, Player* player, Game* game, int elapsedTime);
void loadGame(const char* filename, Player* player, Game* game);
int countFilesInDirectory(const char* directoryPath);
void loadSaveSlots(Game* game);
void loadSelectedGame(Game* game, Player* player);
void cleanupSaveSlots(Game* game);
void printSave(const char* filename);

#endif