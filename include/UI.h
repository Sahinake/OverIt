#ifndef UI_H
#define UI_H

#include <FTGL/ftgl.h>
#include "Game.h"
#include <time.h>

#define NUM_MENU_OPTIONS 5
#define M_PI 3.14159265358979323846
#define WIDTH 20                        
#define HEIGHT 20                       
#define DOT_COUNT 30                    
#define BATTERY_COUNT 5  

extern time_t startTime;
extern time_t currentTime;
extern int elapsedTime;
extern int elapsedSaveTime;
extern int max_font_height;
extern int med_font_height;
extern int min_font_height;
extern int window_width, window_height;
extern int goalDots;
extern float batteryDecrease;

void initMaxFont(const char* fontPath);
void initMedFont(const char* fontPath);
void initMinFont(const char* fontPath);
void loadIcons();
void drawIcons(GLuint icon, float imagePosX, float imagePosY, int imageWidth, int imageHeight);
float getTextWidth(FTGLfont *font, const char* text);
void renderText(FTGLfont *font, const char* text, float x, float y);
void drawBackground(GLuint texture);
void drawMainMenu(Game* game);
void drawNewGameMenu(Game* game);
void drawLoadGameMenu(Game* game);
void drawRankingMenu(Game* game);
void drawOptionsMenu(Game* game);
void renderDotCount();
void renderLevel(Player* player);
void renderGameTime();
void setup2DProjection();
void setup3DProjection();
void renderBatteryUI(Player* player);
void renderSanityUI(Player* player);
void renderHealthUI(Player* player);

#endif
