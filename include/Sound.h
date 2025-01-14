#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>
#include "miniaudio.h"

#define SOUND_POOL_SIZE 15

extern ma_engine engine;
extern ma_sound soundAmbient;
extern ma_sound soundDotCollect;
extern ma_sound soundMenu;
extern ma_sound soundMenuSelectPool[SOUND_POOL_SIZE];
extern ma_sound soundMenuChangePool[SOUND_POOL_SIZE];
extern ma_sound soundMenuBackPool[SOUND_POOL_SIZE];
extern bool isGamePaused;

void initAudio();
void playAmbientMusic();
void playMenuMusic();
void stopMenuMusic();
void playMenuSelectSound();
void playMenuChangeSound();
void playMenuBackSound();
void playDotCollectSound();
void setEffectVolume(float volume);
void increaseEffectVolume();
void decreaseEffectVolume();
void cleanupAudio();

#endif