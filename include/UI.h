#ifndef UI_H
#define UI_H

#include <FTGL/ftgl.h>
#include "Maze.h"
#include <time.h>

#define NUM_MENU_OPTIONS 5
#define M_PI 3.14159265358979323846

// Definindo os estados possíveis do jogo
typedef enum {
    MAIN_MENU,       // Menu Principal
    NEW_GAME_MENU,   // Menu de Novo Jogo
    LOAD_GAME_MENU,  // Menu de Carregar Jogo
    RANKING_MENU,    // Menu de Ranking
    OPTIONS_MENU,    // Menu de Opções
    PLAYING          // Jogo em execução
} GameState;

// Estrutura para armazenar o estado do jogo
typedef struct {
    GameState currentState;  // Estado atual do jogo
    int selectedOption;      // Opção selecionada no menu
} Game;

// Estrutura para armazenar coordenadas
typedef struct {
    float x, y;
} Point;

extern time_t startTime;
extern time_t currentTime;
extern int elapsedTime;
extern int max_font_height;
extern int med_font_height;
extern int min_font_height;
extern int window_width, window_height;
extern int goalDots;
extern float batteryPercentage;
extern float batteryDecrease;
extern Exit exitDoor;

void initMaxFont(const char* fontPath);
void initMedFont(const char* fontPath);
void initMinFont(const char* fontPath);
float getTextWidth(FTGLfont *font, const char* text);
void renderText(FTGLfont *font, const char* text, float x, float y);
void initGame();
void drawBackground(GLuint texture);
void drawMainMenu();
void drawNewGameMenu();
void drawLoadGameMenu();
void drawRankingMenu();
void drawOptionsMenu();
void renderDotCount();
void renderLevel();
void renderGameTime();
void setup2DProjection();
void setup3DProjection();
void renderBatteryUI();
void renderSanityUI();
void renderHealthUI();

#endif
