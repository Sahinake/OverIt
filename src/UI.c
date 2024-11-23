#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "UI.h"
#include "Maze.h"
#include "Sound.h"
#include "stb_image.h"
#include "textureloader.h"

// Declaração dos ponteiros para as fontes
extern FTGLfont *maxFont;
extern FTGLfont *medFont;
extern FTGLfont *minFont;

extern Game game;

extern GLuint backgroundTexture;

// Vértices do triângulo
Point triangle[3] = {
    {50, 50},    // Vértice superior
    {40, 80},    // Vértice inferior esquerdo
    {60, 80}     // Vértice inferior direito
};

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

void initMedFont(const char* fontPath) {
    // Carrega a fonte usando FTGLPixmapFont
    medFont = ftglCreatePixmapFont(fontPath);
    if (medFont == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", fontPath);
        exit(1);  // Encerrar se houver erro no carregamento da fonte
    }
    
    // Define o tamanho da fonte
    ftglSetFontFaceSize(medFont, med_font_height, med_font_height);  // Tamanho da fonte em pixels
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

// Função para inicializar o jogo
void initGame() {
    // Inicialização do OpenGL
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Cor de fundo (preto)

    setup2DProjection();

    // Carregar a imagem de fundo
    backgroundTexture = loadTexture("./assets/Images/Background.png");

    initMaxFont("./fonts/Rexlia.ttf");  
    initMedFont("./fonts/Rexlia.ttf");  
    initMinFont("./fonts/Rexlia.ttf");
    initAudio();

    game.currentState = MAIN_MENU;
    game.selectedOption = 0;
    glutPostRedisplay();
}

// Função para desenhar a textura no fundo
void drawBackground(GLuint texture) {
    // Habilitar o uso de texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Desenhar um quadrado com a textura
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f); // canto inferior esquerdo
        glTexCoord2f(1.0f, 0.0f); glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0.0f); // canto inferior direito
        glTexCoord2f(1.0f, 1.0f); glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); // canto superior direito
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT)); // canto superior esquerdo
    glEnd();

    // Desabilitar o uso de texturas
    glDisable(GL_TEXTURE_2D);
}

// Função para desenhar o menu principal
void drawMainMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para desenhar com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenha o quadrado preto translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // Preto com 50% de transparência
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior esquerdo
        glVertex2f(300.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior direito
        glVertex2f(300.0f, 0.0f);  // Canto superior direito
        glVertex2f(0.0f, 0.0f);  // Canto superior esquerdo
    glEnd();

    // Desabilita o blending após desenhar
    glDisable(GL_BLEND);
    
    char *menuOptions[] = {"New Game", "Load Game", "Ranking", "Options", "Exit"};
    
    for (int i = 0; i < 5; i++) {
        if (i == game.selectedOption) {
            glColor3f(1.0f, 0.5f, 0.0f);  // Cor de destaque para a opção selecionada
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // Cor padrão
        }

        float xPos = 70.0f;
        float yPos = (glutGet(GLUT_WINDOW_HEIGHT) / 2) + (i * 50);
        renderText(medFont, menuOptions[i], xPos, yPos);  // Substituir o glutBitmapCharacter
    }

    glutSwapBuffers();
}

// Função para desenhar o menu de Novo Jogo
void drawNewGameMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para desenhar com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenha o quadrado preto translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // Preto com 50% de transparência
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior esquerdo
        glVertex2f(300.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior direito
        glVertex2f(300.0f, 0.0f);  // Canto superior direito
        glVertex2f(0.0f, 0.0f);  // Canto superior esquerdo
    glEnd();

    // Desabilita o blending após desenhar
    glDisable(GL_BLEND);
    
    // Exemplo de opções para o Menu de Novo Jogo
    char *menuOptions[] = {"Start New Game", "Back"};
    
    for (int i = 0; i < 2; i++) {
        if (i == game.selectedOption) {
            glColor3f(1.0f, 0.5f, 0.0f);  // Cor de destaque para a opção selecionada
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // Cor padrão
        }

        float xPos = 70.0f;
        float yPos = (glutGet(GLUT_WINDOW_HEIGHT) / 2) + (i * 50);
        renderText(medFont, menuOptions[i], xPos, yPos);  // Substituir o glutBitmapCharacter
    }

    glutSwapBuffers();
}


// Função para desenhar o menu de Carregar Jogo
void drawLoadGameMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para desenhar com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenha o quadrado preto translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // Preto com 50% de transparência
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior esquerdo
        glVertex2f(300.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior direito
        glVertex2f(300.0f, 0.0f);  // Canto superior direito
        glVertex2f(0.0f, 0.0f);  // Canto superior esquerdo
    glEnd();

    // Desabilita o blending após desenhar
    glDisable(GL_BLEND);
    
    // Exemplo de opções para o Menu de Carregar Jogo
    char *menuOptions[] = {"Load Saved Game", "Back"};
    
    for (int i = 0; i < 2; i++) {
        if (i == game.selectedOption) {
            glColor3f(1.0f, 0.5f, 0.0f);  // Cor de destaque para a opção selecionada
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // Cor padrão
        }

        float xPos = 70.0f;
        float yPos = (glutGet(GLUT_WINDOW_HEIGHT) / 2) + (i * 50);
        renderText(medFont, menuOptions[i], xPos, yPos);  // Substituir o glutBitmapCharacter
    }

    glutSwapBuffers();
}

// Função para desenhar o menu de Ranking
void drawRankingMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para desenhar com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenha o quadrado preto translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // Preto com 50% de transparência
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior esquerdo
        glVertex2f(300.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior direito
        glVertex2f(300.0f, 0.0f);  // Canto superior direito
        glVertex2f(0.0f, 0.0f);  // Canto superior esquerdo
    glEnd();

    // Desabilita o blending após desenhar
    glDisable(GL_BLEND);
    
    // Exemplo de opções para o Menu de Ranking
    char *menuOptions[] = {"View Ranking", "Back"};
    
    for (int i = 0; i < 2; i++) {
        if (i == game.selectedOption) {
            glColor3f(1.0f, 0.5f, 0.0f);  // Cor de destaque para a opção selecionada
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // Cor padrão
        }

        float xPos = 70.0f;
        float yPos = (glutGet(GLUT_WINDOW_HEIGHT) / 2) + (i * 50);
        renderText(medFont, menuOptions[i], xPos, yPos);  // Substituir o glutBitmapCharacter
    }

    glutSwapBuffers();
}

// Função para desenhar o menu de Opções
void drawOptionsMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para desenhar com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenha o quadrado preto translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // Preto com 50% de transparência
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior esquerdo
        glVertex2f(300.0f, glutGet(GLUT_WINDOW_HEIGHT));  // Canto inferior direito
        glVertex2f(300.0f, 0.0f);  // Canto superior direito
        glVertex2f(0.0f, 0.0f);  // Canto superior esquerdo
    glEnd();

    // Desabilita o blending após desenhar
    glDisable(GL_BLEND);
    
    // Exemplo de opções para o Menu de Opções
    char *menuOptions[] = {"Sound On/Off", "Back"};
    
    for (int i = 0; i < 2; i++) {
        if (i == game.selectedOption) {
            glColor3f(1.0f, 0.5f, 0.0f);  // Cor de destaque para a opção selecionada
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // Cor padrão
        }

        float xPos = 70.0f;
        float yPos = (glutGet(GLUT_WINDOW_HEIGHT) / 2) + (i * 50);
        renderText(medFont, menuOptions[i], xPos, yPos);  // Substituir o glutBitmapCharacter
    }

    glutSwapBuffers();
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
void renderLevel() {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);

    char level[50];
    char level_line[] = "LEVEL";
    sprintf(level, "%d", player.level);

    float text_width = getTextWidth(maxFont, level);
    float line_width = getTextWidth(maxFont, level);

    renderText(minFont, level_line, window_width - line_width - 115, 100);  

    glColor3f(1.0, 1.0, 1.0);  
    renderText(maxFont, level, window_width - text_width - 50, 100);
    glEnable(GL_LIGHTING);
    glPopMatrix();
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
    renderText(maxFont, timeText, window_width - text_width - 50, 60);  // Exibe o tempo ajustado
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void renderBatteryUI() {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    
    char battery_line1[] = "POWER";
    char battery_line2[] = "FLASHLIGHT";

    // Renderiza o texto no canto superior esquerdo
    renderText(minFont, battery_line1, 50, glutGet(GLUT_WINDOW_HEIGHT) - 175.0f);         
    renderText(minFont, battery_line2, 50, glutGet(GLUT_WINDOW_HEIGHT) - 130.0f);         
    
    // Definindo as coordenadas e o tamanho da barra
    float x = 50.0f;  // Posição X do canto esquerdo da barra (próximo à borda esquerda da tela)
    float y = glutGet(GLUT_WINDOW_HEIGHT) - 150.0f;   // Posição Y do canto superior da barra (próximo à borda superior da tela)
    float width = 150.0f; // Largura total da barra de bateria
    float height = 15.0f; // Altura da barra de bateria

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

void renderHealthUI() {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    
    int health = (int) player.health;  

    char healthText[50];
    char health_line[] = "HEALTH";

    glColor3f(1.0, 1.0, 1.0);  // Cor do texto (branco)
    renderText(minFont, health_line, 50, glutGet(GLUT_WINDOW_HEIGHT) - 40.0f); 
    sprintf(healthText, "%02d", health);  // Converte o tempo para formato min:seg

    float text_width = getTextWidth(maxFont, healthText);

    // Renderiza o texto do tempo na tela no canto superior direito
    glDisable(GL_LIGHTING);
    renderText(maxFont, healthText, 130, window_height - 40);  // Exibe o tempo ajustado
    glEnable(GL_LIGHTING);
}

void renderSanityUI() {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    
    char sanyty_line[] = "SANITY";

    // Renderiza o texto no canto superior esquerdo
    renderText(minFont, sanyty_line, 50, glutGet(GLUT_WINDOW_HEIGHT) - 235.0f);          
    
    // Definindo as coordenadas e o tamanho da barra
    float x = 50.0f;  // Posição X do canto esquerdo da barra (próximo à borda esquerda da tela)
    float y = glutGet(GLUT_WINDOW_HEIGHT) - 210.0f;   // Posição Y do canto superior da barra (próximo à borda superior da tela)
    float width = 150.0f; // Largura total da barra de bateria
    float height = 15.0f; // Altura da barra de bateria

    glRasterPos2f(x, y);  // Define a posição do texto

    // Ajustando a largura da barra de bateria com base na porcentagem
    float sanityWidth = width * (player.sanity / 100.0f);  // A largura será proporcional à porcentagem da bateria

    // Configurando a cor da barra de bateria (verde quando alta, vermelho quando baixa)
    if (player.sanity > 50.0f) {
        glColor3f(1.0f, 1.0f, 1.0f); // Branco
    } else if (player.sanity > 20.0f){
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    }
    
    // Desenhando o preenchimento da barra de bateria
    glBegin(GL_QUADS);
    glVertex2f(x, y); // Canto superior esquerdo
    glVertex2f(x + sanityWidth, y); // Canto superior direito (ajustado pela porcentagem)
    glVertex2f(x + sanityWidth, y - height); // Canto inferior direito
    glVertex2f(x, y - height); // Canto inferior esquerdo
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
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
