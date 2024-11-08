#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#include "UI.h"
#include "Maze.h"

extern int goalDots; // Quantidade de dots restantes
extern int font_height;

// Variáveis globais para o tempo
extern int startTime;
extern time_t currentTime;
extern int elapsedTime;
extern int font_height;
extern int window_width, window_height;

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
    ftglSetFontFaceSize(font, font_height, font_height);  // Tamanho da fonte em pixels
}

// Função para renderizar o texto na tela
void renderText(const char* text, float x, float y) {
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(x, y);  // Define a posição do texto
    ftglRenderFont(font, text, FTGL_RENDER_ALL);  // Renderiza o texto
    glPopMatrix();
}

// Função para calcular a largura do texto
float getTextWidth(const char* text) {
    float width = 0.0f;

    // Itera por cada caractere do texto
    for (int i = 0; text[i] != '\0'; i++) {
        // Passa a string de um único caractere para ftglGetFontAdvance
        char charStr[2] = { text[i], '\0' };  // Cria uma string de um único caractere
        width += ftglGetFontAdvance(font, charStr);  // Obtém a largura (avanço) do caractere
    }

    return width;
}

// Função para renderizar o texto da quantidade de dots restantes
void renderDotCount() {
    char text[50];
    sprintf(text, "%d", goalDots);

    // Renderiza o texto no canto superior esquerdo
    renderText(text, 50, 50);
}

// Função para renderizar o tempo na UI
void renderGameTime() {
    int minutes = elapsedTime / 60;  // Calcula os minutos
    int seconds = elapsedTime % 60;  // Calcula os segundos

    char timeText[50];
    sprintf(timeText, "%02d:%02d", minutes, seconds);  // Converte o tempo para formato min:seg

    float text_width = getTextWidth(timeText);

    // Renderiza o texto do tempo na tela no canto superior direito
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);  // Cor do texto (branco)
    renderText(timeText, window_width - text_width - 50, 50);  // Exibe o tempo ajustado
    glEnable(GL_LIGHTING);
}

// Função para configurar a projeção 2D da UI
void setup2DProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, -1, 1); // Definindo a projeção ortogonal para 2D
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
    gluPerspective(45.0, (double)glutGet(GLUT_WINDOW_WIDTH) / (double)glutGet(GLUT_WINDOW_HEIGHT), 1.0, 100.0); // Definindo a perspectiva para 3D
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
