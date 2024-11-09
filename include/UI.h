#ifndef UI_H
#define UI_H

#include <FTGL/ftgl.h>

extern int goalDots; // Quantidade de dots restantes
extern int font_height;

// Variáveis globais para o tempo
extern time_t startTime;
extern time_t currentTime;
extern int elapsedTime;
extern int font_height;
extern int window_width, window_height;
extern float batteryPercentage;

void initFont(const char* fontPath);
float getTextWidth(const char* text);
void renderText(const char* text, float x, float y);
void renderDotCount();
void renderGameTime();
void setup2DProjection();
void setup3DProjection();
void renderBatteryUI();

#endif
