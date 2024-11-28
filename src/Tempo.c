#include <time.h>
#include <GL/glut.h>
#include <stdio.h>

#include "Tempo.h"
#include "UI.h"

extern bool isGamePaused;

// Função para iniciar o tempo do jogo
void startGameTimer() {
    timeStart = time(NULL);  // Tempo inicial
    pausedTime = 0;  // Reseta o tempo de pausa
    lastSaveTime = timeStart;  // Inicializa o tempo de último save como o tempo inicial do jogo
    updateGameTime(0);
}

// Função para pausar o jogo
void pauseGame() {
    if (!isGamePaused) {
        isGamePaused = true;
    }
}

// Função para retomar o jogo
void resumeGame() {
    if (isGamePaused) {
        isGamePaused = false;
    }
}

// Função para atualizar o tempo
void updateGameTime(int value) {
    if(isGamePaused) {
        elapsedSaveTime = elapsedTime;  // Captura o tempo atual quando o jogo é pausado
        timeStart = time(NULL);
    }
    else {
        currentTime = time(NULL);  // Tempo atual
        elapsedTime = (int)difftime(currentTime, timeStart) + elapsedSaveTime;  // Tempo decorrido em segundos
    }

    glutPostRedisplay();  // Re-renderiza a tela para atualizar a UI
    glutTimerFunc(1000, updateGameTime, 0);  // Agenda próxima atualização em 1 segundo
}

