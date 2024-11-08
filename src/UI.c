#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdio.h>

#include "UI.h"
#include "Maze.h"

extern int goalDots; // Quantidade de dots restantes

// Declaração do ponteiro para a fonte
FTGLfont *font;

// Função de inicialização da fonte
void initFont(const char* fontPath) {
    // Carrega a fonte usando FTGLPixmapFont
    font = ftglCreatePixmapFont(fontPath);
    if (font == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", fontPath);
        exit(1);  // Encerrar se houver erro no carregamento da fonte
    }
    
    // Define o tamanho da fonte
    ftglSetFontFaceSize(font, 48, 48);  // Tamanho da fonte em pixels
}

// Função para renderizar o texto na tela
void renderText(const char* text, float x, float y) {
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(x, y);  // Define a posição do texto
    ftglRenderFont(font, text, FTGL_RENDER_ALL);  // Renderiza o texto
    glPopMatrix();
}

// Função para inicializar a UI
void initUI() {
    // Podemos configurar o background da UI ou qualquer configuração necessária aqui.
}

// Função para renderizar o texto da quantidade de dots restantes
void renderDotCount() {
    char text[50];
    sprintf(text, "%d", goalDots);

    // Renderiza o texto no canto superior esquerdo
    renderText(text, 50, 80);
}

// Função para configurar a projeção 2D da UI
void setup2DProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT), 0, -1, 1); // Definindo a projeção ortogonal para 2D
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Desativa a iluminação para renderizar o texto da UI
    glDisable(GL_LIGHTING);
}

// Função para configurar a projeção 3D do jogo
void setup3DProjection() {
    // Reativa a iluminação para a próxima renderização 3D
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)glutGet(GLUT_WINDOW_WIDTH) / (double)glutGet(GLUT_WINDOW_HEIGHT), 1.0, 200.0); // Definindo a perspectiva para 3D
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
