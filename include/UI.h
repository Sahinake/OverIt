#ifndef UI_H
#define UI_H

#include <FTGL/ftgl.h>
#include "Maze.h"

#define M_PI 3.14159265358979323846

// Estrutura para armazenar coordenadas
typedef struct {
    float x, y;
} Point;

extern time_t startTime;
extern time_t currentTime;
extern int elapsedTime;
extern int max_font_height;
extern int min_font_height;
extern int window_width, window_height;
extern int goalDots;
extern float batteryPercentage;
extern float batteryDecrease;
extern Exit exitDoor;

void initMaxFont(const char* fontPath);
void initMinFont(const char* fontPath);
float getTextWidth(FTGLfont *font, const char* text);
void renderText(FTGLfont *font, const char* text, float x, float y);
void renderDotCount();
void renderGameTime();
void setup2DProjection();
void setup3DProjection();
void renderBatteryUI();
void renderSanityUI();
void renderHealthUI();

#endif
