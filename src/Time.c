#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#include "Time.h"
#include "UI.h"

// Função para iniciar o tempo do jogo
void startGameTimer() {
    startTime = time(NULL);  // Tempo inicial
    glutTimerFunc(1000, updateGameTime, 0);  // Agenda próxima atualização em 1 segundo
}

// Função para atualizar o tempo
void updateGameTime(int value) {
    currentTime = time(NULL);  // Tempo atual
    elapsedTime = (int)difftime(currentTime, startTime);  // Tempo decorrido em segundos

    glutPostRedisplay();  // Re-renderiza a tela para atualizar a UI
    glutTimerFunc(1000, updateGameTime, 0);  // Agenda próxima atualização em 1 segundo
}
