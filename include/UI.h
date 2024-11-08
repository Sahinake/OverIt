#ifndef UI_H
#define UI_H

// Função para inicializar a UI
void initUI();

void initFont(const char* fontPath);

void renderText(const char* text, float x, float y);

// Função para renderizar o texto da quantidade de dots restantes
void renderDotCount();

// Função para configurar a projeção 2D da UI
void setup2DProjection();

// Função para configurar a projeção 3D do jogo
void setup3DProjection();

#endif // UI_H
