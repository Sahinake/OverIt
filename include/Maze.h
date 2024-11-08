#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <GL/glut.h>

#define WIDTH 20 // Largura do labirinto
#define HEIGHT 20 // Altura do labirinto
#define DOT_COUNT 30 // Quantidade de dots a serem coletados
#define BATTERY_COUNT 5 // Quantidade de baterias a serem spawnadas

typedef struct {
    int x, y;
    bool collected; // Indica se o dot já foi coletado
} Dot;

// Estrutura de Bateria
typedef struct {
    int x, y;
    bool collected; // Indica se a bateria já foi coletada
} Battery;

void initMaze();
void generateMaze(int x, int y);
void spawnPlayer();
void spawnDots();
void spawnBatteries();
void setMaterial(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shininess);
void renderMaze();
bool isObjectVisible(int dotX, int dotY);
void renderPlayerAndObjects();
bool checkObjectCollision(int playerX, int playerY);

#endif // MAZE_H
