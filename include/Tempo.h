#ifndef TEMPO_H
#define TEMPO_H

#include <time.h>  // Adicione esta linha

// Variáveis globais para o tempo
extern time_t timeStart;
extern time_t currentTime;
extern time_t lastSaveTime;
extern int elapsedTime;
extern int font_height;
extern int window_width, window_height;

void startGameTimer();
void updateGameTime(int value);

#endif

