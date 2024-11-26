#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>  // Para usleep() ou sleep()
#include <dirent.h>  // Para listar arquivos na pasta
#include <math.h>
#include <windows.h>

#include "ObjLoader.h"
#include "UI.h"
#include "Game.h"
#include "Tempo.h"
#include "Sound.h"
#include "miniaudio.h"
#include "stb_image.h"
#include "SaveLoad.h"
#include "textureloader.h"

#define WIDTH 20                        // Largura do labirinto
#define HEIGHT 20                       // Altura do labirinto
#define DOT_COUNT 30                    // Quantidade de dots a serem coletados
#define BATTERY_COUNT 5                 // Quantidade de baterias a serem spawnadas
#define MAX_BATTERY_PERCENTAGE 100.0f   // Capacidade máxima da bateria (100%)
#define MAX_BATTERY 70.0f               // Capacidade máxima da lanterna
#define MAX_HEALTH 100.0f               // Capacidade máxima da vida
#define MAX_SANITY 100.0f               // Capacidade máxima da sanidade
#define BATTERY_DECREASE_RATE 0.01f     // Taxa de diminuição da bateria por atualização de frame
#define HEALTH_DECREASE_RATE 0.02f      // Taxa de diminuição da bateria por atualização de frame
#define SANITY_DECREASE_RATE 0.02f      // Taxa de diminuição da bateria por atualização de frame
#define NUM_MENU_OPTIONS 5              // Opções do Menu
#define M_PI 3.14159265358979323846
#define STB_IMAGE_IMPLEMENTATION
#define SOUND_POOL_SIZE 15
#define MAX_SAVES 4                     // Limite máximo de saves permitido

int maze_widht = WIDTH, maze_height = HEIGHT;
int window_width = 800, window_height = 600;

int goalDots;                           // Quantidade de dots que o jogador precisa coletar
int total_batteries = 5;

float lightDirX = 0.0f;
float lightDirZ = 1.0f;                 // Inicialmente apontando para "frente"
float maxDistance = 5.0f;               // Distância máxima para a lanterna

float batteryDecrease = BATTERY_DECREASE_RATE;

// Variáveis globais para o tempo
time_t timeStart;
time_t currentTime;
time_t lastSaveTime;  // Variável para armazenar o tempo do último save
time_t pausedTime;

int elapsedTime;
int elapsedSaveTime = 0;
int max_font_height = 35;
int med_font_height = 20;
int min_font_height = 14;

Player player;

ma_engine engine;
ma_sound soundAmbient;
ma_sound soundDotCollect;
ma_sound soundMenu;
ma_sound soundFlashlightClick;
ma_sound soundFlashlightChange;
ma_sound soundMenuSelectPool[SOUND_POOL_SIZE];
ma_sound soundMenuChangePool[SOUND_POOL_SIZE];
ma_sound soundMenuBackPool[SOUND_POOL_SIZE];

int isMainMenuMusicOn = 0;
int wasTheGameSaved = 0;
GLuint backgroundTexture, batteryTexture;

bool isGamePaused = false;

// Instância global do jogo
Game game;
GLuint icons[2];
char saveName[256];

int newGame = -1;

// Declaração dos ponteiros para as fontes
FTGLfont *maxFont;
FTGLfont *medFont;
FTGLfont *minFont;

ObjectList objectList;

// Pegue o primeiro objeto carregado como modelo do jogador
Object* batteryModel;

// Função para a câmera seguir o jogador
void cameraFollowPlayer() {
    float camX = player.posX;           // Posicionar a câmera na mesma linha X do jogador
    float camY = 8.0f;                  // Eleva a câmera para uma visão de cima (Y mais alto)
    float camZ = player.posZ - 4.0f;    // Posicionar a câmera atrás do jogador no eixo Z

    gluLookAt(camX, camY, camZ,                         // Posição da câmera
              player.posX, player.posY, player.posZ,    // Olhando para o jogador
              0.0f, 0.0f, 1.0f);                        // Up vector
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
    GLfloat lightDir[] = { lightDirX, -0.8f, lightDirZ }; 

    // Definindo características da luz ambiente, difusa e especular
    GLfloat ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightIntensity;

    if(player.flashlightCharge == 0 || player.flashlight == 0) {
        lightIntensity = 0.0f;
    }
    else {
        lightIntensity = player.flashlightCharge / 100.0f; // A intensidade é proporcional à carga da bateria
    }
    
    GLfloat diffuseLight[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };  // A luz fica mais fraca à medida que a bateria acaba
    GLfloat specularLight[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };

    // Ajuste do brilho do material
    GLfloat shininess = 100.0f;         
    GLfloat spotExponent = 10.0f;       
    GLfloat spotCutoff = 30.0f;       

    // Fatores de atenuação baseados em maxDistance
    float constantAttenuation = 1.0f;
    float linearAttenuation = 0.1f / maxDistance;                      
    float quadraticAttenuation = 0.05f / (maxDistance * maxDistance);   

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
    int gridX = (int)(newX);  
    int gridZ = (int)(newZ);  

    if (gridX >= 0 && gridX < WIDTH && gridZ >= 0 && gridZ < HEIGHT) {
        if (game.maze[gridX][gridZ] == 1) {  
            return true;              
        }
    } else {
        return true;    // Fora dos limites do mapa
    }
    return false;       // Sem colisão
}


// Função para mover o jogador gradualmente em direção à direção definida
void movePlayer() {
    if (player.moving) {
        if (player.posX != player.targetX) {
            player.posX += player.speedX;
            if ((player.speedX > 0 && player.posX >= player.targetX) || (player.speedX < 0 && player.posX <= player.targetX)) {
                player.posX = player.targetX;   // Chegou ao destino
                player.speedX = 0.0f;           // Parar movimento no eixo X
                player.x = (int)(player.posX);
            }
        }

        if (player.posZ != player.targetZ) {
            player.posZ += player.speedZ;
            if ((player.speedZ > 0 && player.posZ >= player.targetZ) || (player.speedZ < 0 && player.posZ <= player.targetZ)) {
                player.posZ = player.targetZ;   // Chegou ao destino
                player.speedZ = 0.0f;           // Parar movimento no eixo Z
                player.y = (int)(player.posZ);
            }
        }

        if (player.speedX == 0.0f && player.speedZ == 0.0f) {
            player.moving = 0;  // Interrompe o movimento quando ambos os eixos atingem o destino
        }

        if(game.maze[player.x][player.y] == 2 || game.maze[player.x][player.y] == 3) {
            checkObjectCollision(&game, &player);
        }
        glutPostRedisplay();
    }
}

// Configurações de inicialização do OpenGL para 3D
void initPlaying() {
    if (game.currentState == PLAYING) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_LIGHTING);  // Ativa a iluminação
        glEnable(GL_LIGHT0);    // Ativa a luz 0
        initializeRendering();  
        if(newGame == 1) {
            initGame(&game, &player);
            game.selectedOption = 0;
        }
        startGameTimer();  // Inicia o tempo no começo do jogo
        playAmbientMusic();
    }
}

// Função para desenhar a tela dependendo do estado atual
void drawScene() {
    switch (game.currentState) {
        case MAIN_MENU:
            drawMainMenu(&game);
            break;
        case NEW_GAME_MENU:
            drawNewGameMenu(&game);
            break;
        case LOAD_GAME_MENU:
            drawLoadGameMenu(&game);
            break;
        case RANKING_MENU:
            drawRankingMenu(&game);
            break;
        case OPTIONS_MENU:
            drawOptionsMenu(&game);
            break;
        case PLAYING:
            initPlaying();
            break;
        case FINISHED:
            drawGameOver(&game);
        default:
            break;
    }
    
    glutPostRedisplay();
}

// Função para renderizar a cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();     // Salva o estado atual da transformação

    if(game.currentState != PLAYING) {
        setup2DProjection();
        drawScene();
        if(isMainMenuMusicOn == 0) {
            playMenuMusic();
            isMainMenuMusicOn = 1;
        }
    }
    else if(game.currentState == PLAYING) {
        if(isMainMenuMusicOn == 1) {
            stopMenuMusic();
            isMainMenuMusicOn = 0;

        } 

        // Configura a projeção 3D para renderizar o jogo
        setup3DProjection();  // Configura a projeção 3D para o jogo
        cameraFollowPlayer(); // Move a câmera para seguir o jogador
        
        if(player.moving) {
            movePlayer();     // Move o jogador de acordo com a direção e velocidade
        } 

        updateLighting();                   // Atualiza a iluminação de acordo com o jogador
        renderScene(&game, &player, batteryModel);        // Renderiza o jogador e os dots

        // Configura a projeção 2D para renderizar a UI
        setup2DProjection();
        if(!isGamePaused) {
            renderDotCount();           // Renderiza o contador de dots
            renderGameTime();           // Renderiza o tempo de jogo
            renderLevel(&player);
            updateBattery(&player);
            renderBatteryUI(&player);
            renderSanityUI(&player);
            renderHealthUI(&player);
        }
        else {
            drawOptionsPauseMenu(&game);
        }
        glPopMatrix();
    }

    glutSwapBuffers();
}

// Função para capturar o pressionamento do teclado e definir a direção de movimento
void keyboardDown(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;  // Velocidade de movimento por delta de tempo (por segundo)
    float posX = player.posX;
    float posZ = player.posZ;
    
    if(game.currentState == PLAYING) {
        if(!isGamePaused) {
            if ((key == 'w' || key == 'W') && !player.moving) {  // Move para frente (no eixo Z)
                posZ = player.posZ + 1.0f;
                if (!checkCollision(posX,posZ)){
                    player.targetZ = posZ;
                    player.speedZ = +moveSpeed;
                    player.moving = 1;
                }
                lightDirX = 0.0f; 
                lightDirZ = 1.0f;
                player.rotation = 90.0f;
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
                player.rotation = -90.0f;
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
                player.rotation = 180.0f;
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
                player.rotation = 0.0f;
            }
            else if (key == 'f' || key == 'F') {
                player.flashlight = player.flashlight == 1 ? 0 : 1;
                playFlashlightClickSound();
            }
            else if(key == '+') {
                if (player.flashlightCharge < 90.0f) {
                    player.flashlightCharge += 5.0f; 
                    player.flashlightPercentage += 7.143f; // Aumenta o raio da luz
                }
            }
            else if(key == '-') {
                if (player.flashlightCharge > 10.0f) {
                    player.flashlightCharge -= 5.0f; 
                    player.flashlightPercentage -= 7.143f; // Diminui o raio da luz
                }
            }
            else if(key == 'r' || key == 'R') {
                initializeRendering();
                initMaze(&game);
                generateMaze(&game, 1, 1);
                spawnPlayer(&game, &player);
                spawnDots(&game);
                spawnBatteries(&game);
                generateExit(&game);
            }  

            // Adiciona teclas para controle de volume
            else if (key == 'v' || key == 'V') { // Aumentar volume dos efeitos
                adjustBrightness(&game, 1.1f);  // Aumenta o brilho em 10%();
            }
            else if (key == 'c' || key == 'C') { // Diminuir volume dos efeitos
                adjustBrightness(&game, 0.9f);  // Aumenta o brilho em 10%();
            }
            else if (key == 't' || key == 'T') {
                //saveGame(saveName, &player, &game, elapsedTime);
                player.health = 0.0f;
            }
        }
        if(key == 27) { 
            isGamePaused = !isGamePaused;
            if(isGamePaused) {
                pauseGame();
            }
            else {
                resumeGame();
            }
        }
    }
    if(game.currentState != PLAYING) {
        if(key == 27) {
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
                case FINISHED:
                    break;
                default:
                    break;
            }
        } // ESC para sair

        if (key == 127) {
            deleteSelectedSave(&game);
        } // Delete

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
                    playMenuSelectSound();
                    break;
                case NEW_GAME_MENU:
                    newGame = 1;
                    saveSelectedGame(&game, &player, elapsedTime);
                    //game.currentState = PLAYING;  // Começar o jogo
                    playMenuSelectSound();
                    game.selectedOption = 0;
                    break;
                case LOAD_GAME_MENU:
                    newGame = 0;
                    loadSelectedGame(&game, &player);
                    playMenuSelectSound();
                    game.selectedOption = 0;
                    //printSave(saveName);
                    break;
                case RANKING_MENU:
                    break;
                case OPTIONS_MENU:
                    if (game.selectedOption == 0) {
                        playMenuSelectSound();
                    } else {
                        game.currentState = MAIN_MENU;
                        game.selectedOption = 3;
                        playMenuBackSound();
                    }
                    break;
                case FINISHED:
                    game.currentState = MAIN_MENU;
                    game.selectedOption = 0;
                    wasTheGameSaved = 0;
                    stopAmbientMusic();
                    removeFinishedSaves("./saves");
                    break;
                default:
                    break;
            }
            drawScene();
        }
    }
}

// Função para navegar entre os menus com as teclas de seta
void keyboardNavigation(int key, int x, int y) {
    if(game.currentState == MAIN_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 5) % 5;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 5;
        }
        playMenuChangeSound();
        drawScene();
    }
    else if(game.currentState == NEW_GAME_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 4) % 4;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 4;
        }
        playMenuChangeSound();
        drawScene();
    }
    else if (game.currentState == LOAD_GAME_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 4) % 4;
        } else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 4;
        }
        playMenuChangeSound();
        drawScene();
    }
    else if(game.currentState == OPTIONS_MENU) {
        if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 4) % 4;
        } 
        else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 4;
        }
        else if (key == GLUT_KEY_LEFT) {
            if(game.selectedOption == 0) {
                decreaseEffectVolume();
            }
            else if(game.selectedOption == 1) {
                decreaseMusicVolume();
            }
            else if(game.selectedOption == 2) {
                decreaseAmbientVolume();
            }
            else if(game.selectedOption == 3) {
                adjustBrightness(&game, 0.9f);  // Diminui o brilho em 10%
            } 
        }
        else if (key == GLUT_KEY_RIGHT) {
            if(game.selectedOption == 0) {
                increaseEffectVolume();
            }
            else if(game.selectedOption == 1) {
                increaseMusicVolume();
            }
            else if(game.selectedOption == 2) {
                increaseAmbientVolume();
            }
            else if(game.selectedOption == 3) {
                adjustBrightness(&game, 1.1f);  // Aumenta o brilho em 10%
            }
        }
        playMenuChangeSound();
        drawScene();
    }
    else if(game.currentState == PLAYING && isGamePaused) {
         if (key == GLUT_KEY_UP) {
            game.selectedOption = (game.selectedOption - 1 + 4) % 4;
        } 
        else if (key == GLUT_KEY_DOWN) {
            game.selectedOption = (game.selectedOption + 1) % 4;
        }
        else if (key == GLUT_KEY_LEFT) {
            if(game.selectedOption == 0) {
                adjustBrightness(&game, 0.9f);  // Aumenta o brilho em 10%
            } 
        }
        else if (key == GLUT_KEY_RIGHT) {
            if(game.selectedOption == 0) {
                adjustBrightness(&game, 1.1f);  // Aumenta o brilho em 10%
            }
        }
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

    if(game.currentState != PLAYING) {
        drawScene();
    }
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    // Inicialização do OpenGL
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Cor de fundo (preto)

    setup2DProjection();
    
    game.currentState = MAIN_MENU;
    game.selectedOption = 0;
    game.volumeEffects = 0.6f;
    game.volumeMusic = 1.0f;
    game.volumeAmbient = 0.4f;
    game.brightness = 1.0f;

    // Carregar a imagem de fundo
    backgroundTexture = loadTexture("C:/Users/Maluzinha/OneDrive/Documentos/PacMan3d/assets/Images/Background.png");
    batteryTexture = loadTexture("C:/Users/Maluzinha/OneDrive/Documentos/PacMan3d/assets/Objects/battery/RGB_74f40f51e22244c68872b66bf27b6403_cj_dianchi01.png");

    initMaxFont("C:/Users/Maluzinha/OneDrive/Documentos/PacMan3d/assets/fonts/Rexlia.ttf");  
    initMedFont("C:/Users/Maluzinha/OneDrive/Documentos/PacMan3d/assets/fonts/Rexlia.ttf");  
    initMinFont("C:/Users/Maluzinha/OneDrive/Documentos/PacMan3d/assets/fonts/Rexlia.ttf");
    initAudio();
    loadIcons();

    initObjectList(&objectList);
    loadObjectFile(&objectList, "C:/Users/Maluzinha/OneDrive/Documentos/PacMan3d/assets/Objects/battery/battery.obj");
    batteryModel = getObjectList(&objectList, 0);
    
    // Carrega os arquivos de save nos slots
    loadSaveSlots(&game);
    // Carrega a lista de ranking
    loadRankingFromFile(&game, "./ranking.dat");
}

// Função de callback do GLUT para o loop do jogo
void update(int value) {
    if(game.currentState == PLAYING && !isGamePaused && game.currentState != FINISHED) {
        updatePlayerStatus(&game, &player);

        if(goalDots == 0) {
            if(updateGame(&game, &player) == 1) {
                wasTheGameSaved = 0;
            }
        }
    }

    // Define o próximo loop de atualização (geralmente 16ms para 60fps)
    glutPostRedisplay();
    glutTimerFunc(8, update, 0); // Chama `update()` a cada 16ms (aproximadamente 60fps)
}

int main(int argc, char** argv) {
    srand(time(NULL));

    // Inicialização do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600); // Tamanho inicial da janela
    glutCreateWindow("Maze Game");

    init();

    // Configura o loop de atualização
    glutTimerFunc(8, update, 0); 

    // Registra as funções de callback
    glutDisplayFunc(display);
    glutSpecialFunc(keyboardNavigation);  
    glutKeyboardFunc(keyboardDown);  
    //glutKeyboardUpFunc(keyboardUp);   // Para quando a tecla é liberada
    glutReshapeFunc(reshape); 
    
    // Loop principal do GLUT
    glutMainLoop();

    return 0;
}
