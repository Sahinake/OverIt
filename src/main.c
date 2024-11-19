#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "Maze.h"
#include "UI.h"
#include "Time.h"
#include "miniaudio.h"
#include "Sound.h"
#include "stb_image.h"
#include "textureloader.h"

#define WIDTH 20                        // Largura do labirinto
#define HEIGHT 20                       // Altura do labirinto
#define DOT_COUNT 30                    // Quantidade de dots a serem coletados
#define BATTERY_COUNT 5                 // Quantidade de baterias a serem spawnadas
#define MAX_BATTERY_PERCENTAGE 100.0f   // Capacidade máxima da bateria (100%)
#define MAX_BATTERY 70.0f               // Capacidade máxima da lanterna
#define MAX_HEALTH 100.0f               // Capacidade máxima da vida
#define MAX_SANITY 100.0f               // Capacidade máxima da sanidade
#define BATTERY_DECREASE_RATE 0.005f    // Taxa de diminuição da bateria por atualização de frame
#define HEALTH_DECREASE_RATE 0.02f      // Taxa de diminuição da bateria por atualização de frame
#define SANITY_DECREASE_RATE 0.02f      // Taxa de diminuição da bateria por atualização de frame
#define NUM_MENU_OPTIONS 5              // Opções do Menu
#define M_PI 3.14159265358979323846
#define STB_IMAGE_IMPLEMENTATION

int maze[WIDTH][HEIGHT];
int maze_widht = WIDTH, maze_height = HEIGHT;
int window_width = 800, window_height = 600;

int goalDots; // Quantidade de dots que o jogador precisa coletar
int total_batteries = 5;

float lightDirX = 0.0f;
float lightDirZ = 1.0f;     // Inicialmente apontando para "frente"
float maxDistance = 5.0f;   // Distância máxima para a lanterna

float batteryCharge = MAX_BATTERY; // Bateria começa cheia
float batteryPercentage = MAX_BATTERY_PERCENTAGE; // A bateria começa com 100%
float batteryDecrease = BATTERY_DECREASE_RATE;

// Variáveis globais para o tempo
time_t startTime;
time_t currentTime;
int elapsedTime;
int max_font_height = 35;
int med_font_height = 20;
int min_font_height = 14;

Dot dots[DOT_COUNT];
Battery batteries[BATTERY_COUNT];
Player player; 
Exit exitDoor;

ma_engine engine;
ma_sound soundAmbient;
ma_sound soundDotCollect;

// Variável global para o volume (pode ser ajustada entre 0.0 e 1.0)
float volumeEffects = 1.0f;
GLuint backgroundTexture;

bool isGamePaused = false;

// Instância global do jogo
Game game;

// Declaração dos ponteiros para as fontes
FTGLfont *maxFont;
FTGLfont *medFont;
FTGLfont *minFont;

// Função para a câmera seguir o jogador
void cameraFollowPlayer() {
    float camX = player.posX;      // Posicionar a câmera na mesma linha X do jogador
    float camY = 8.0f;         // Eleva a câmera para uma visão de cima (Y mais alto)
    float camZ = player.posZ - 5.0f; // Posicionar a câmera atrás do jogador no eixo Z

    gluLookAt(camX, camY, camZ, // Posição da câmera
              player.posX, player.posY, player.posZ, // Olhando para o jogador
              0.0f, 0.0f, 1.0f);     // Up vector
}

// Função para configurar a iluminação de forma mais flexível
void setLighting(float lightPos[4], float lightDir[3], float ambient[4], float diffuse[4], float specular[4], float shininess, float spotExponent) {
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent); // Define a suavização da borda do cone
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Função para a iluminação dinâmica durante o jogo
void updateLighting() {
    // A posição da luz será sempre a posição do jogador, para simular uma lanterna
    GLfloat lightPos[] = { (float)player.posX, 1.0f, (float)player.posZ, 1.0f };

    // A direção da luz será ajustada com base no movimento do jogador
    GLfloat lightDir[] = { lightDirX, -0.5f, lightDirZ }; // Um pequeno ajuste na direção da luz para simular uma lanterna

    // Definindo características da luz ambiente, difusa e especular
    GLfloat ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightIntensity;

    if(batteryCharge == 0 || player.flashlight == 0) {
        lightIntensity = 0.0f;
    }
    else {
         // A intensidade da luz vai diminuir com a carga da bateria
        lightIntensity = batteryCharge / 100.0f; // A intensidade é proporcional à carga da bateria
    }
    
    GLfloat diffuseLight[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };  // A luz fica mais fraca à medida que a bateria acaba
    // Especular (reflexão) também pode ser ajustado, mas normalmente é mais sutil
    GLfloat specularLight[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };

    // Ajuste do brilho do material
    GLfloat shininess = 100.0f; // Brilho médio
    GLfloat spotExponent = 10.0f; // Controla a suavização da borda do cone (quanto maior, mais nítido)
    GLfloat spotCutoff = 30.0f;  // Ângulo do cone de iluminação (quanto menor, mais estreito o cone)

    // Fatores de atenuação baseados em maxDistance
    float constantAttenuation = 1.0f;
    float linearAttenuation = 0.1f / maxDistance; // Ajuste linear para atingir 0 em maxDistance
    float quadraticAttenuation = 0.05f / (maxDistance * maxDistance); // Ajuste quadrático para suavizar o decaimento

    // Chamando a função que configura a iluminação
    setLighting(lightPos, lightDir, ambientLight, diffuseLight, specularLight, shininess, spotExponent);

    // Configura a atenuação da luz
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constantAttenuation);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linearAttenuation);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);

    // Define o ângulo do cone da lanterna (spotCutoff) e o foco (spotExponent)
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutoff);
}

// Função para verificar a colisão com as paredes considerando o raio da esfera
bool checkCollision(float newX, float newZ) {
    // // Verifica se a nova posição está dentro dos limites da área de jogo
    // if (newX > 0 && newX < WIDTH && newZ > 0 && newZ < HEIGHT) {
    //     // Define as coordenadas das bordas da esfera com base no raio
    //     int minX = (int)(newX - player.radius);
    //     int maxX = (int)(newX + player.radius);
    //     int minZ = (int)(newZ - player.radius);
    //     int maxZ = (int)(newZ + player.radius);

    //     // Verifica se qualquer parte da esfera colide com uma parede
    //     for (int x = minX; x <= maxX; x++) {
    //         for (int z = minZ; z <= maxZ; z++) {
    //             // Verifica se (x, z) está dentro dos limites do labirinto
    //             if (x >= 0 && x < WIDTH && z >= 0 && z < HEIGHT) {
    //                 // Verifica se há uma parede na posição (x, z)
    //                 if (maze[x][z] == 1) {
    //                     float wallCenterX = x + 0.5;
    //                     float wallCenterZ = z + 0.5;

    //                     // Calcula a distância entre o centro da esfera e o centro da parede
    //                     float distX = newX - wallCenterX;
    //                     float distZ = newZ - wallCenterZ;
    //                     float distance = sqrt(distX * distX + distZ * distZ);

    //                     // Se a distância for menor que o raio, houve colisão
    //                     if (distance < player.radius) {
    //                         return false; // Colisão detectada
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //     return true; // Sem colisão
    // }
    // return false; // Fora dos limites
    int gridX = (int)(newX);  // Convertendo a posição X para índices da grid
    int gridZ = (int)(newZ);  // Convertendo a posição Z para índices da grid

    // Verifica se a nova posição está dentro dos limites da grid e se há um cubo branco
    if (gridX >= 0 && gridX < WIDTH && gridZ >= 0 && gridZ < HEIGHT) {
        if (maze[gridX][gridZ] == 1) {  // Se houver um cubo na posição
            return true;  // Há uma colisão
        }
    }
    return false; // Sem colisão
}


// Função para mover o jogador gradualmente em direção à direção definida
void movePlayer() {
    // // Calcula a nova posição baseada na velocidade e direção
    // float newX = player.posX + player.moveDirX * player.speed;
    // float newY = player.posY + player.moveDirY * player.speed;

    // if(newX > 0 && newX < WIDTH && newY > 0 && newY < HEIGHT) {
    //     if(maze[(int)floor(newX)][(int)floor(newY)] != 1) {
    //         // Verifica colisão com as paredes
    //         if (checkCollision(newX, newY)) {
    //             player.posX = newX;
    //             player.posY = newY;
    //             player.x = (int)(player.posX);
    //             player.y = (int)(player.posY);
                
    //             if(maze[player.x][player.y] == 2 || maze[player.x][player.y] == 3) {
    //                 // Verifica colisão com objetos (se houver)
    //                 checkObjectCollision();
    //             }
    //         }
    //     }
    // }
    if (player.moving) {
        if (player.posX != player.targetX) {
            player.posX += player.speedX;
            if ((player.speedX > 0 && player.posX >= player.targetX) || (player.speedX < 0 && player.posX <= player.targetX)) {
                player.posX = player.targetX;  // Chegou ao destino
                player.speedX = 0.0f;  // Parar movimento no eixo X
                player.x = (int)(player.posX);
            }
        }

        if (player.posZ != player.targetZ) {
            player.posZ += player.speedZ;
            if ((player.speedZ > 0 && player.posZ >= player.targetZ) || (player.speedZ < 0 && player.posZ <= player.targetZ)) {
                player.posZ = player.targetZ;  // Chegou ao destino
                player.speedZ = 0.0f;  // Parar movimento no eixo Z
                player.y = (int)(player.posZ);
            }
        }

        if (player.speedX == 0.0f && player.speedZ == 0.0f) {
            player.moving = 0;  // Interrompe o movimento quando ambos os eixos atingem o destino
        }

        if(maze[player.x][player.y] == 2 || maze[player.x][player.y] == 3) {
            checkObjectCollision();
        }

    }

    glutPostRedisplay();
    // glutTimerFunc(16, movePlayer, 0);  // Chama a função novamente após 16ms (aproximadamente 60 FPS)
}

// Função para renderizar a cena
void display() {
    if(game.currentState == MAIN_MENU) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        setup2DProjection();
        glPushMatrix(); // Salva o estado atual da transformação
        drawMainMenu();
        glFlush();  // Força a execução do desenho
        glPopMatrix(); // Restaura o estado de transformação
    }
    else if(game.currentState == PLAYING) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Configura a projeção 3D para renderizar o jogo
        setup3DProjection();  // Configura a projeção 3D para o jogo
        glPushMatrix();       // Salva o estado da transformação atual
        cameraFollowPlayer(); // Move a câmera para seguir o jogador
        updateLighting();     // Atualiza a iluminação de acordo com o jogador
        renderScene();        // Renderiza o jogador e os dots
        glPopMatrix();        // Restaura o estado da transformação

        // Configura a projeção 2D para renderizar a UI
        setup2DProjection();
        glPushMatrix(); // Salva o estado atual da transformação
        renderDotCount(); // Renderiza o contador de dots
        renderGameTime();  // Renderiza o tempo de jogo
        renderLevel();
        renderBatteryUI();
        renderSanityUI();
        renderHealthUI();
        glFlush();  // Força a execução do desenho
        glPopMatrix(); // Restaura o estado de transformação

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
    }

    glutSwapBuffers();
    glFlush();
}

// Configurações de inicialização do OpenGL para 3D
void initPlaying() {
    if (game.currentState == PLAYING) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_LIGHTING); // Ativa a iluminação
        glEnable(GL_LIGHT0);    // Ativa a luz 0
        initMaze();
        initializePlayer();
        generateMaze(1, 1);
        spawnPlayer();
        spawnDots();
        spawnBatteries();
        generateExit();
        startGameTimer();  // Inicia o tempo no começo do jogo
        playAmbientMusic();
    }
}

// Função para desenhar a tela dependendo do estado atual
void drawScene() {
    switch (game.currentState) {
        case MAIN_MENU:
            drawMainMenu();
            break;
        case NEW_GAME_MENU:
            drawNewGameMenu();
            break;
        case LOAD_GAME_MENU:
            drawLoadGameMenu();
            break;
        case RANKING_MENU:
            drawRankingMenu();
            break;
        case OPTIONS_MENU:
            drawOptionsMenu();
            break;
        case PLAYING:
            initPlaying();
            break;
        default:
            break;
    }
}

// Função para capturar o pressionamento do teclado e definir a direção de movimento
void keyboardDown(unsigned char key, int x, int y) {
    // if(key == 's' || key == 'S') {
    //     player.moveDirX = 0.0f; 
    //     player.moveDirY = -0.1f; 
    //     lightDirX = 0.0f; 
    //     lightDirZ = -1.0f;
    // }
    // else if(key == 'w' || key == 'W') {
    //     player.moveDirX = 0.0f; 
    //     player.moveDirY = 0.1f;
    //     lightDirX = 0.0f; 
    //     lightDirZ = 1.0f;
    // }
    // else if(key == 'a' || key == 'A') {
    //     player.moveDirX = 0.1f; 
    //     player.moveDirY = 0.0f;
    //     lightDirX = 1.0f; 
    //     lightDirZ = 0.0f; 
    // }
    // else if(key == 'd' || key == 'D') {
    //     player.moveDirX = -0.1f; 
    //     player.moveDirY = 0.0f;
    //     lightDirX = -1.0f; 
    //     lightDirZ = 0.0f; 
    // }
    float moveSpeed = 0.1f;  // Velocidade de movimento por delta de tempo (por segundo)
    float posX = player.posX;
    float posZ = player.posZ;
    if ((key == 'w' || key == 'W') && !player.moving) {  // Move para frente (no eixo Z)
        posZ = player.posZ + 1.0f;
        if (!checkCollision(posX,posZ)){
            player.targetZ = posZ;
            player.speedZ = +moveSpeed;
            player.moving = 1;
        }
        lightDirX = 0.0f; 
        lightDirZ = 1.0f;
    }
    else if ((key == 's' || key == 'S') && !player.moving) {  // Move para trás (no eixo Z)
        posZ = player.posZ - 1.0f;
        if (!checkCollision(posX,posZ)){
            player.targetZ = posZ;
            player.speedZ = -moveSpeed;
            player.moving = 1;
        }
        lightDirX = 0.0f; 
        lightDirZ = -1.0f;
    }
    else if ((key == 'a' || key == 'A') && !player.moving) {  // Move para a esquerda (no eixo X)
        posX = player.posX + 1.0f;
        if (!checkCollision(posX,posZ)){
            player.targetX = posX;
            player.speedX = +moveSpeed;
            player.moving = 1;
        }
        lightDirX = +1.0f; 
        lightDirZ = 0.0f;
    }
    else if ((key == 'd' || key == 'D') && !player.moving) {  // Move para a direita (no eixo X)
        posX = player.posX - 1.0f;
        if (!checkCollision(posX,posZ)){
            player.targetX = posX;
            player.speedX = -moveSpeed;
            player.moving = 1;
        }
        lightDirX = -1.0f; 
        lightDirZ = 0.0f; 
    }
    else if (key == 'f' || key == 'F') {
        player.flashlight = player.flashlight == 1 ? 0 : 1;
    }
    else if(key == '+') {
        if (batteryCharge < 90.0f) {
            batteryCharge += 5.0f; 
            batteryPercentage += 7.143f; // Aumenta o raio da luz
        }
    }
    else if(key == '-') {
        if (batteryCharge > 10.0f) {
            batteryCharge -= 5.0f; 
            batteryPercentage -= 7.143f; // Diminui o raio da luz
        }
    }
    else if(key == 'r' || key == 'R') {
        initMaze();
        generateMaze(1, 1);
        spawnPlayer();
        spawnDots();
        spawnBatteries();
        generateExit();
    }  
    else if(key == 27) {
        switch (game.currentState) {
            case MAIN_MENU:
                exit(0);
                break;
            case NEW_GAME_MENU:
                game.currentState = MAIN_MENU;
                game.selectedOption = 0;
                break;
            case LOAD_GAME_MENU:
                game.currentState = MAIN_MENU;
                game.selectedOption = 1;
                break;
            case RANKING_MENU:
                game.currentState = MAIN_MENU;
                game.selectedOption = 2;
                break;
            case OPTIONS_MENU:
                game.currentState = MAIN_MENU;
                game.selectedOption = 3;
                break;
            case PLAYING:
                break;
            default:
                break;
        }
    } // ESC para sair

    // Adiciona teclas para controle de volume
    if (key == 'v') { // Aumentar volume dos efeitos
        increaseEffectVolume();
    }
    else if (key == 'c') { // Diminuir volume dos efeitos
        decreaseEffectVolume();
    }

    if (key == 13) {  // Tecla Enter
        switch (game.currentState) {
            case MAIN_MENU:
                if (game.selectedOption == 0) {
                    game.currentState = NEW_GAME_MENU;
                    game.selectedOption = 0;
                } else if (game.selectedOption == 1) {
                    game.currentState = LOAD_GAME_MENU;
                    game.selectedOption = 0;
                } else if (game.selectedOption == 2) {
                    game.currentState = RANKING_MENU;
                    game.selectedOption = 0;
                } else if (game.selectedOption == 3) {
                    game.currentState = OPTIONS_MENU;
                    game.selectedOption = 0;
                } else if (game.selectedOption == 4) {
                    exit(0);  // Sair do jogo
                }
                break;
            case NEW_GAME_MENU:
                if (game.selectedOption == 0) {
                    game.currentState = PLAYING;  // Começar o jogo
                } else {
                    game.currentState = MAIN_MENU;
                    game.selectedOption = 0;
                }
                break;
            case LOAD_GAME_MENU:
                if (game.selectedOption == 0) {
                    // Lógica para carregar o jogo
                } else {
                    game.currentState = MAIN_MENU;
                    game.selectedOption = 1;
                }
                break;
            case RANKING_MENU:
                if (game.selectedOption == 0) {
                    // Lógica para carregar o jogo
                } else {
                    game.currentState = MAIN_MENU;
                    game.selectedOption = 2;
                }
            case OPTIONS_MENU:
                if (game.selectedOption == 0) {
                    // Lógica para carregar o jogo
                } else {
                    game.currentState = MAIN_MENU;
                    game.selectedOption = 3;
                }
                break;
            case PLAYING:
                break;
            default:
                break;
        }

        drawScene();
    }
    
    glutPostRedisplay();
}

// Função para capturar o evento de soltar a tecla e zerar a direção de movimento
void keyboardUp(unsigned char key, int x, int y) {
    // switch (key) {
    //     case 's': player.moveDirY = 0.0f; break;
    //     case 'w': player.moveDirY = 0.0f; break;
    //     case 'd': player.moveDirX = 0.0f; break;
    //     case 'a': player.moveDirX = 0.0f; break;
    //     case 'S': player.moveDirY = 0.0f; break;
    //     case 'W': player.moveDirY = 0.0f; break;
    //     case 'D': player.moveDirX = 0.0f; break;
    //     case 'A': player.moveDirX = 0.0f; break;
    // }
    glutPostRedisplay();
}

// Função para navegar entre os menus com as teclas de seta
void keyboardNavigation(int key, int x, int y) {
    if(game.currentState == MAIN_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 5) % 5;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 5;
        }
        drawScene();
    }
    else if(game.currentState == NEW_GAME_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 2) % 2;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 2;
        }
        drawScene();
    }
    else if(game.currentState == LOAD_GAME_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 2) % 2;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 2;
        }
        drawScene();
    }
    else if(game.currentState == RANKING_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 2) % 2;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 2;
        }
        drawScene();
    }
    else if(game.currentState == OPTIONS_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 2) % 2;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 2;
        }
        drawScene();
    }
}

// Função para lidar com o redimensionamento da janela
void reshape(int w, int h) {
    // Define a proporção da janela
    window_width = w;
    window_height = h;

    glViewport(0, 0, w, h);

    // Define a matriz de projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Mantém a proporção da cena
    GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
    
    // Ajusta a perspectiva de acordo com a proporção
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

// Função de callback do GLUT para o loop do jogo
void update(int value) {
    if(game.currentState == PLAYING) {
       if(player.moving) {
            movePlayer();            // Move o jogador de acordo com a direção e velocidade
       } 
        updateBattery();
        updatePlayerStatus();

        if(goalDots == 0) {
            updateGame();
        }
    }

    // Define o próximo loop de atualização (geralmente 16ms para 60fps)
    glutPostRedisplay();
    glutTimerFunc(4, update, 0); // Chama `update()` a cada 16ms (aproximadamente 60fps)
}

int main(int argc, char** argv) {
    srand(time(NULL));

    // Inicialização do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600); // Tamanho inicial da janela
    glutCreateWindow("Maze Game");

    // Configura o loop de atualização
    glutTimerFunc(4, update, 0); // Define o intervalo para 25ms (aproximadamente 40fps)

    // Funções de inicialização do OpenGL
    initGame();

    // Registra as funções de callback
    glutDisplayFunc(display);
    // Define callbacks de teclado
    glutSpecialFunc(keyboardNavigation);  // Função para as teclas de seta
    glutKeyboardFunc(keyboardDown);  // Para quando a tecla é pressionada
    glutKeyboardUpFunc(keyboardUp);   // Para quando a tecla é liberada
    glutReshapeFunc(reshape); // Registra a função de reshape
    
    // Loop principal do GLUT
    glutMainLoop();

    return 0;
}
