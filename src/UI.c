#include <time.h>
#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>

#include "UI.h"
#include "Sound.h"
#include "stb_image.h"
#include "textureloader.h"
#include "SaveLoad.h"

// Declaração dos ponteiros para as fontes
extern FTGLfont *maxFont;
extern FTGLfont *medFont;
extern FTGLfont *minFont;
extern GLuint icons[10];

extern GLuint backgroundTexture;
extern int wasTheGameSaved;

// Vértices do triângulo
Point triangle[3] = {
    {50, 50},    // Vértice superior
    {40, 80},    // Vértice inferior esquerdo
    {60, 80}     // Vértice inferior direito
};

void loadIcons() {
    icons[0] = loadTexture("assets/Images/Enter.png");      // Ícone da tecla Enter
    icons[1] = loadTexture("assets/Images/Esc.png");        // Ícone da tecla ESC
}

void drawIcons(GLuint icon, float imagePosX, float imagePosY, int imageWidth, int imageHeight) {
    // Ativar texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, icon);  // Usa a textura carregada

    // Ativar o blending para lidar com a transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenhar a imagem como um quadrado com a textura
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        // Coordenadas de textura ajustadas para não inverter a imagem
        glTexCoord2f(0.0f, 0.0f); glVertex2f(imagePosX, imagePosY - imageHeight / 2);  // Inferior esquerdo
        glTexCoord2f(1.0f, 0.0f); glVertex2f(imagePosX + imageWidth, imagePosY - imageHeight / 2);  // Inferior direito
        glTexCoord2f(1.0f, 1.0f); glVertex2f(imagePosX + imageWidth, imagePosY + imageHeight / 2);  // Superior direito
        glTexCoord2f(0.0f, 1.0f); glVertex2f(imagePosX, imagePosY + imageHeight / 2);  // Superior esquerdo
    glEnd();

    // Desativar o blending após desenhar a textura
    glDisable(GL_BLEND);

    // Desativar texturas
    glDisable(GL_TEXTURE_2D);
}

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
    medFont = ftglCreatePixmapFont(fontPath);
    if (medFont == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", fontPath);
        exit(1);
    }
    
    ftglSetFontFaceSize(medFont, med_font_height, med_font_height); 
}

void initMinFont(const char* fontPath) {
    minFont = ftglCreatePixmapFont(fontPath);
    if (minFont == NULL) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", fontPath);
        exit(1);
    }
    
    ftglSetFontFaceSize(minFont, min_font_height, min_font_height); 
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

void printMaze(Game* game) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            printf("%d ", game->maze[i][j]);
        }
        printf("\n");
    }
    printf("\n ========================== \n");
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
void drawMainMenu(Game* game) {
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
        if (i == game->selectedOption) {
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

void drawNewGameMenu(Game* game) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Fundo translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0.0f);
        glVertex2f(0.0f, 0.0f);
    glEnd();
    glDisable(GL_BLEND);

    int totalSlots = 4;  // Número total de slots de save
    int selectedSlot = game->selectedOption;  // Índice do slot selecionado
    float slotHeight = 0.1f * glutGet(GLUT_WINDOW_HEIGHT);
    float slotWidth = 0.5f * glutGet(GLUT_WINDOW_WIDTH);
    int yOffset = 0;
    char slotText[50];

    // Itera pelos slots e exibe as informações
    for (int i = 0; i < totalSlots; i++) {
        float xStart = 0.25f * glutGet(GLUT_WINDOW_WIDTH);
        float yStart = 0.15f * glutGet(GLUT_WINDOW_HEIGHT) + i * (slotHeight + 0.02f * glutGet(GLUT_WINDOW_HEIGHT));

        glColor3f((i == selectedSlot) ? 1.0f : 1.0f,
                  (i == selectedSlot) ? 0.5f : 1.0f,
                  (i == selectedSlot) ? 0.0f : 1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(xStart, yStart + yOffset);
            glVertex2f(xStart + slotWidth, yStart + yOffset);
            glVertex2f(xStart + slotWidth, yStart + slotHeight + yOffset);
            glVertex2f(xStart, yStart + slotHeight + yOffset);
        glEnd();

        // Renderiza texto do slot
        if (game->slotFiles[i] != NULL) {
            snprintf(slotText, sizeof(slotText), "Slot %d: %s", i + 1, game->slotFiles[i]);
            renderText(medFont, slotText, xStart + 20, yStart + slotHeight / 2 + 8 + yOffset);
        } else {
            snprintf(slotText, sizeof(slotText), "Slot %d: Vazio", i + 1);
            renderText(medFont, slotText, xStart + 20, yStart + slotHeight / 2 + 8 + yOffset);
        }

        yOffset += 20;
    }

    // Mensagens de instrução
    char enter_line[] = "SELECT";
    char back_line[] = "BACK";

    int iconWidth = 50;
    int iconSpacing = 10;
    int totalWidth = 2 * iconWidth + iconSpacing + getTextWidth(minFont, enter_line) + getTextWidth(minFont, back_line);
    int startX = (glutGet(GLUT_WINDOW_WIDTH) - totalWidth) / 2;

    drawIcons(icons[0], startX, glutGet(GLUT_WINDOW_HEIGHT) - 105, iconWidth, iconWidth);
    renderText(minFont, enter_line, startX + iconWidth + iconSpacing, glutGet(GLUT_WINDOW_HEIGHT) - 100);

    drawIcons(icons[1], startX + iconWidth + iconSpacing + getTextWidth(minFont, enter_line) + iconSpacing, glutGet(GLUT_WINDOW_HEIGHT) - 105, iconWidth, iconWidth);
    renderText(minFont, back_line, startX + iconWidth + iconSpacing + getTextWidth(minFont, enter_line) + iconSpacing + iconWidth, glutGet(GLUT_WINDOW_HEIGHT) - 100);
}

void drawLoadGameMenu(Game* game) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBackground(backgroundTexture);

    // Configuração para desenhar com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenha o fundo translúcido
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, glutGet(GLUT_WINDOW_HEIGHT));
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0.0f);
        glVertex2f(0.0f, 0.0f);
    glEnd();
    glDisable(GL_BLEND);

    // Variáveis para controle de slots
    int totalSlots = 4;
    int selectedSlot = game->selectedOption; // Índice do slot selecionado
    float slotHeight = 0.1f * glutGet(GLUT_WINDOW_HEIGHT);
    float slotWidth = 0.5f * glutGet(GLUT_WINDOW_WIDTH);
    int yOffset = 0;
    char slotText[50];

    // Carregar os slots usando a função loadSaveSlots
    loadSaveSlots(game);

    // Loop para desenhar os slots de save
    for (int saveCount = 0; saveCount < totalSlots; saveCount++) {
        float xStart = 0.25f * glutGet(GLUT_WINDOW_WIDTH);
        float yStart = 0.15f * glutGet(GLUT_WINDOW_HEIGHT) + saveCount * (slotHeight + 0.02f * glutGet(GLUT_WINDOW_HEIGHT));

        // Caso o slot esteja ocupado, desenhe com a cor indicada
        if (game->slotFiles[saveCount] != NULL) {
            // Montar caminho completo e abrir arquivo
            char filePath[512];
            snprintf(filePath, sizeof(filePath), "./saves/%s", game->slotFiles[saveCount]);
            FILE* file = fopen(filePath, "rb");

            if (file != NULL) {
                SavedGame savedGame;
                fread(&savedGame, sizeof(SavedGame), 1, file);
                fclose(file);

                // Desenha o retângulo para o slot
                glColor3f((saveCount == selectedSlot) ? 1.0f : 1.0f, 
                        (saveCount == selectedSlot) ? 0.5f : 1.0f, 
                        (saveCount == selectedSlot) ? 0.0f : 1.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex2f(xStart, yStart + yOffset);
                    glVertex2f(xStart + slotWidth, yStart + yOffset);
                    glVertex2f(xStart + slotWidth, yStart + slotHeight + yOffset);
                    glVertex2f(xStart, yStart + slotHeight + yOffset);
                glEnd();

                // Formatar texto e renderizar
                char formattedTime[50];
                strftime(formattedTime, sizeof(formattedTime), "%d/%m/%Y %H:%M:%S", localtime(&savedGame.lastPlayed));
                renderText(medFont, savedGame.saveName,  xStart + 20, yStart + slotHeight / 2 + yOffset);
                renderText(minFont, formattedTime, xStart + 20, yStart + slotHeight / 2 + 20 + yOffset);

                yOffset += 20;
            }
        } else {
            // Caso o slot esteja vazio, desenha "Vazio"
            glColor3f((saveCount == selectedSlot) ? 1.0f : 1.0f, 
                      (saveCount == selectedSlot) ? 0.5f : 1.0f, 
                      (saveCount == selectedSlot) ? 0.0f : 1.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(xStart, yStart + yOffset);
                glVertex2f(xStart + slotWidth, yStart + yOffset);
                glVertex2f(xStart + slotWidth, yStart + slotHeight + yOffset);
                glVertex2f(xStart, yStart + slotHeight + yOffset);
            glEnd();

            snprintf(slotText, sizeof(slotText), "Slot %d: Vazio", saveCount + 1);
            renderText(medFont, slotText, xStart + 20, yStart + slotHeight / 2 + 8 + yOffset);

            yOffset += 20;
        }
    }

    // Desenhar ícones e textos adicionais
    char enter_line[] = "SELECT";
    char back_line[] = "BACK";

    int iconWidth = 50;
    int iconSpacing = 10;  // Distância entre os ícones
    int totalWidth = 2 * iconWidth + iconSpacing + getTextWidth(minFont, enter_line) + getTextWidth(minFont, back_line);
    int startX = (glutGet(GLUT_WINDOW_WIDTH) - totalWidth) / 2;

    drawIcons(icons[0], startX, glutGet(GLUT_WINDOW_HEIGHT) - 105, iconWidth, iconWidth);
    renderText(minFont, enter_line, startX + iconWidth + iconSpacing, glutGet(GLUT_WINDOW_HEIGHT) - 100);

    drawIcons(icons[1], startX + iconWidth + iconSpacing + getTextWidth(minFont, enter_line) + iconSpacing, glutGet(GLUT_WINDOW_HEIGHT) - 105, iconWidth, iconWidth);
    renderText(minFont, back_line, startX + iconWidth + iconSpacing + getTextWidth(minFont, enter_line) + iconSpacing + iconWidth, glutGet(GLUT_WINDOW_HEIGHT) - 100);
}


// Função para desenhar o menu de Ranking
void drawRankingMenu(Game* game) {
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
        if (i == game->selectedOption) {
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
void drawOptionsMenu(Game* game) {
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
        if (i == game->selectedOption) {
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
void renderLevel(Player* player) {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);

    char level[50];
    char level_line[] = "LEVEL";
    sprintf(level, "%d", player->level);

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
    int minutes = (elapsedTime + elapsedSaveTime) / 60;  // Calcula os minutos
    int seconds = (elapsedTime + elapsedSaveTime) % 60;  // Calcula os segundos

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

void renderBatteryUI(Player* player) {
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
    float batteryWidth = width * (player->flashlightPercentage / 100.0f);  // A largura será proporcional à porcentagem da bateria

    // Configurando a cor da barra de bateria (verde quando alta, vermelho quando baixa)
    if (player->flashlightPercentage > 50.0f) {
        glColor3f(1.0f, 1.0f, 1.0f); // Branco
    } else if (player->flashlightPercentage > 20.0f){
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

void renderHealthUI(Player* player) {
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    
    int health = (int) player->health;  

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

void renderSanityUI(Player* player) {
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
    float sanityWidth = width * (player->sanity / 100.0f);  // A largura será proporcional à porcentagem da bateria

    // Configurando a cor da barra de bateria (verde quando alta, vermelho quando baixa)
    if (player->sanity > 50.0f) {
        glColor3f(1.0f, 1.0f, 1.0f); // Branco
    } else if (player->sanity > 20.0f){
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
