#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#include "Time.h"
#include "UI.h"

// Variáveis globais para o tempo
extern int startTime;
extern time_t currentTime;
extern int elapsedTime;
extern int font_height;
extern int window_width, window_height;

// Função para iniciar o tempo do jogo
void startGameTimer() {
    startTime = time(NULL);  // Tempo inicial
}

// Função para atualizar o tempo
void updateGameTime(int value) {
    currentTime = time(NULL);  // Tempo atual
    elapsedTime = (int)difftime(currentTime, startTime);  // Tempo decorrido em segundos

    glutPostRedisplay();  // Re-renderiza a tela para atualizar a UI
    glutTimerFunc(1000, updateGameTime, 0);  // Agenda próxima atualização em 1 segundo
}
