#ifndef UI_H
#define UI_H

// Função para inicializar a UI
void initUI();

void initFont(const char* fontPath);
float getTextWidth(const char* text);
void renderText(const char* text, float x, float y);
void renderDotCount();
void renderGameTime();
void setup2DProjection();
void setup3DProjection();

#endif
