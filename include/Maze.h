#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <GL/glut.h>

#define WIDTH 20 // Largura do labirinto
#define HEIGHT 20 // Altura do labirinto
#define DOT_COUNT 30 // Quantidade de dots a serem coletados

extern int maze[WIDTH][HEIGHT];
extern int playerX, playerY; // Posição do jogador
extern int goalDots; // Quantidade de dots que o jogador precisa coletar
extern float lightDirX;
extern float lightDirZ; // Inicialmente apontando para "frente"
extern float spotCutoff; // Ângulo da lanterna ajustável
extern float maxDistance; // Distância máxima para a lanterna

typedef struct {
    int x, y;
    bool collected; // Indica se o dot já foi coletado
} Dot;

extern Dot dots[DOT_COUNT];

// Função para inicializar o labirinto com paredes
void initMaze();

// Algoritmo para geração procedural do labirinto
void generateMaze(int x, int y);

// Função para posicionar o jogador no ponto inicial
void spawnPlayer();

// Função para gerar dots aleatoriamente em posições válidas no labirinto
void spawnDots();

// Função para configurar o material de uma superfície
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess);

// Função para renderizar o labirinto em 3D usando materiais
void renderMaze();

// Função para verificar se um dot está visível pela lanterna do jogador
bool isDotVisible(int dotX, int dotY);

// Função para renderizar o jogador e os dots usando materiais
void renderPlayerAndDots();

#endif // MAZE_H
