#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>
#include "miniaudio.h"

extern ma_engine engine;
extern ma_sound soundAmbient;
extern ma_sound soundDotCollect;
extern bool isGamePaused;

void initAudio();
void playAmbientMusic();
void playDotCollectSound();
void setEffectVolume(float volume);
void increaseEffectVolume();
void decreaseEffectVolume();
void cleanupAudio();

#endif