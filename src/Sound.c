#include <stdio.h>
#include <stdlib.h>

#include "miniaudio.h"
#include "Sound.h"
#include "UI.h"

#define SOUND_POOL_SIZE 15

extern float volumeEffects;
extern float volumeMusic;
extern float volumeAmbient;
extern Game game;

void initAudio() {
    // Inicializa o motor de áudio
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Falha ao inicializar o motor de áudio.\n");
        exit(1);
    }

    // Carrega os arquivos de áudio
    if (ma_sound_init_from_file(&engine, "assets/Sounds/Ambient.mp3", 0, NULL, NULL, &soundAmbient) != MA_SUCCESS) {
        printf("Falha ao carregar a música ambiente.\n");
    }
    if (ma_sound_init_from_file(&engine, "assets/Sounds/Dot.mp3", 0, NULL, NULL, &soundDotCollect) != MA_SUCCESS) {
        printf("Falha ao carregar o som de coleta de dot.\n");
    }
    if (ma_sound_init_from_file(&engine, "assets/Sounds/MainMenu.mp3", 0, NULL, NULL, &soundMenu) != MA_SUCCESS) {
        printf("Falha ao carregar a música do menu.\n");
    }

    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        ma_sound_init_from_file(&engine, "assets/Sounds/MainMenuSelectOption.mp3", 0, NULL, NULL, &soundMenuSelectPool[i]);
        ma_sound_init_from_file(&engine, "assets/Sounds/MainMenuChangeOption.mp3", 0, NULL, NULL, &soundMenuChangePool[i]);
        ma_sound_init_from_file(&engine, "assets/Sounds/MainMenuBackOption.mp3", 0, NULL, NULL, &soundMenuBackPool[i]);
    }

    // Configura o volume inicial
    ma_sound_set_volume(&soundDotCollect, volumeEffects);
    ma_sound_set_volume(&soundAmbient, volumeAmbient);
    ma_sound_set_volume(&soundMenu, volumeMusic); 
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        ma_sound_set_volume(&soundMenuSelectPool[i], volumeEffects);
        ma_sound_set_volume(&soundMenuChangePool[i], volumeEffects);
        ma_sound_set_volume(&soundMenuBackPool[i], volumeEffects);
    }
}

void playAmbientMusic() {
    ma_sound_start(&soundAmbient);
    ma_sound_set_looping(&soundAmbient, MA_TRUE); // Define a música ambiente para repetir em loop
}

void playMenuMusic() {
    ma_sound_start(&soundMenu);
    ma_sound_set_looping(&soundMenu, MA_TRUE);
}

void stopMenuMusic() {
    ma_sound_stop(&soundMenu);  // Para o som atual, caso esteja tocando
}

void playMenuSelectSound() {
    ma_sound_start(&soundMenuSelectPool[game.selectedOption]);
}

void playMenuChangeSound() {
    ma_sound_start(&soundMenuChangePool[game.selectedOption]);
}

void playMenuBackSound() {
    ma_sound_start(&soundMenuBackPool[game.selectedOption]);
}

void playDotCollectSound() {
    ma_sound_stop(&soundDotCollect);  
    ma_sound_start(&soundDotCollect);
}

void setEffectVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;  // Impede valores negativos
    if (volume > 1.0f) volume = 1.0f;  // Limita o volume ao máximo permitido
    volumeEffects = volume;
    ma_sound_set_volume(&soundDotCollect, volumeEffects);

    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        // Para cada som no pool, libere os recursos
        ma_sound_set_volume(&soundMenuSelectPool[i], volumeEffects);
        ma_sound_set_volume(&soundMenuChangePool[i], volumeEffects);
        ma_sound_set_volume(&soundMenuBackPool[i], volumeEffects);
    }
}

void increaseEffectVolume() {
    setEffectVolume(volumeEffects + 0.1f); // Aumenta o volume em 0.1
    printf("Volume dos efeitos aumentado para %.1f\n", volumeEffects);
}

void decreaseEffectVolume() {
    setEffectVolume(volumeEffects - 0.1f); // Diminui o volume em 0.1
    printf("Volume dos efeitos reduzido para %.1f\n", volumeEffects);
}

void cleanupAudio() {
    ma_sound_uninit(&soundAmbient);
    ma_sound_uninit(&soundDotCollect);
    ma_engine_uninit(&engine);
    ma_sound_uninit(&soundMenu);
    
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        // Para cada som no pool, libere os recursos
        ma_sound_uninit(&soundMenuSelectPool[i]);
        ma_sound_uninit(&soundMenuChangePool[i]);
        ma_sound_uninit(&soundMenuBackPool[i]);
    }
}
