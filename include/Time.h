#ifndef TIME_H
#define TIME_H

// Variáveis globais para o tempo
extern time_t startTime;
extern time_t currentTime;
extern time_t lastSaveTime;
extern int elapsedTime;
extern int font_height;
extern int window_width, window_height;

void startGameTimer();
void updateGameTime(int value);

#endif

