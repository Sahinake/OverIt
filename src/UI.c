#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#include "UI.h"
#include "Maze.h"

// Declaração dos ponteiros para as fontes
FTGLfont *maxFont;
FTGLfont *minFont;

// Função de inicialização da fonte
void initMaxFont(const char* fontPath) {
    // Carrega a fonte usando FTGLPixmapFont
    maxFont = ftglCreatePixmapFont(fontPath);
    if (maxFont == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", fontPath);
        exit(1);  // Encerrar se houver erro no carregamento da fonte
    }
    
    // Define o tamanho da fonte
    ftglSetFontFaceSize(maxFont, max_font_height, max_font_height);  // Tamanho da fonte em pixels
}

void initMinFont(const char* fontPath) {
    // Carrega a fonte usando FTGLPixmapFont
    minFont = ftglCreatePixmapFont(fontPath);
    if (minFont == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", fontPath);
        exit(1);  // Encerrar se houver erro no carregamento da fonte
    }
    
    // Define o tamanho da fonte
    ftglSetFontFaceSize(minFont, min_font_height, min_font_height);  // Tamanho da fonte em pixels
}

// Função para renderizar o texto na tela
void renderText(FTGLfont *font, const char* text, float x, float y) {
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(x, y);  // Define a posição do texto
    ftglRenderFont(font, text, FTGL_RENDER_ALL);  // Renderiza o texto
    glPopMatrix();
}

// Função para calcular a largura do texto
float getTextWidth(FTGLfont *font, const char* text) {
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
    char dots_count[50];
    char dots_line1[] = "REMAINING";
    char dots_line2[] = "DOTS";
    sprintf(dots_count, "%d", goalDots);

    // Renderiza o texto no canto superior esquerdo
    renderText(minFont, dots_line1, 50, 50);         // Primeira linha ("REMAINING")
    renderText(minFont, dots_line2, 50, 70);         // Segunda linha ("DOTS")
    renderText(maxFont, dots_count, 105, 85);        // Quantidade de dots
}

// Função para renderizar o tempo na UI
void renderGameTime() {
    int minutes = elapsedTime / 60;  // Calcula os minutos
    int seconds = elapsedTime % 60;  // Calcula os segundos

    char timeText[50];
    sprintf(timeText, "%02d:%02d", minutes, seconds);  // Converte o tempo para formato min:seg

    float text_width = getTextWidth(maxFont, timeText);

    // Renderiza o texto do tempo na tela no canto superior direito
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);  // Cor do texto (branco)
    renderText(maxFont, timeText, window_width - text_width - 50, 50);  // Exibe o tempo ajustado
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

void renderBatteryUI() {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    
    char battery_line1[] = "POWER";
    char battery_line2[] = "FLASHLIGHT";

    // Renderiza o texto no canto superior esquerdo
    renderText(minFont, battery_line1, 50, glutGet(GLUT_WINDOW_HEIGHT) - 100.0f);         
    renderText(minFont, battery_line2, 50, glutGet(GLUT_WINDOW_HEIGHT) - 50.0f);         
    
    // Definindo as coordenadas e o tamanho da barra
    float x = 50.0f;  // Posição X do canto esquerdo da barra (próximo à borda esquerda da tela)
    float y = glutGet(GLUT_WINDOW_HEIGHT) - 70.0f;   // Posição Y do canto superior da barra (próximo à borda superior da tela)
    float width = 150.0f; // Largura total da barra de bateria
    float height = 20.0f; // Altura da barra de bateria

    glRasterPos2f(x, y);  // Define a posição do texto

    // Ajustando a largura da barra de bateria com base na porcentagem
    float batteryWidth = width * (batteryPercentage / 100.0f);  // A largura será proporcional à porcentagem da bateria

    // Configurando a cor da barra de bateria (verde quando alta, vermelho quando baixa)
    if (batteryPercentage > 50.0f) {
        glColor3f(1.0f, 1.0f, 1.0f); // Branco
    } else if (batteryPercentage > 20.0f){
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    }
    

    // Desenhando o preenchimento da barra de bateria
    glBegin(GL_QUADS);
    glVertex2f(x, y); // Canto superior esquerdo
    glVertex2f(x + batteryWidth, y); // Canto superior direito (ajustado pela porcentagem)
    glVertex2f(x + batteryWidth, y - height); // Canto inferior direito
    glVertex2f(x, y - height); // Canto inferior esquerdo
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f); // Cor cinza para o fundo
}