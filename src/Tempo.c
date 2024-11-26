#include <time.h>
#include <GL/glut.h>
#include <stdio.h>

#include "Tempo.h"
#include "UI.h"

// Função para iniciar o tempo do jogo
void startGameTimer() {
    timeStart = time(NULL);  // Tempo inicial
    lastSaveTime = timeStart;  // Inicializa o tempo de último save como o tempo inicial do jogo
    glutTimerFunc(1000, updateGameTime, 0);  // Agenda próxima atualização em 1 segundo
}

// Função para atualizar o tempo
void updateGameTime(int value) {
    currentTime = time(NULL);  // Tempo atual
    elapsedTime = (int)difftime(currentTime, timeStart);  // Tempo decorrido em segundos

    glutPostRedisplay();  // Re-renderiza a tela para atualizar a UI
    glutTimerFunc(1000, updateGameTime, 0);  // Agenda próxima atualização em 1 segundo
}
