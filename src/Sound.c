#include <stdio.h>
#include <stdlib.h>

#include "miniaudio.h"
#include "Sound.h"
#include "UI.h"

#define SOUND_POOL_SIZE 15

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
    ma_sound_set_volume(&soundDotCollect, game.volumeEffects);
    ma_sound_set_volume(&soundAmbient, game.volumeAmbient);
    ma_sound_set_volume(&soundMenu, game.volumeMusic); 
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        ma_sound_set_volume(&soundMenuSelectPool[i], game.volumeEffects);
        ma_sound_set_volume(&soundMenuChangePool[i], game.volumeEffects);
        ma_sound_set_volume(&soundMenuBackPool[i], game.volumeEffects);
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
    game.volumeEffects = volume;
    ma_sound_set_volume(&soundDotCollect, game.volumeEffects);

    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        // Para cada som no pool, libere os recursos
        ma_sound_set_volume(&soundMenuSelectPool[i], game.volumeEffects);
        ma_sound_set_volume(&soundMenuChangePool[i], game.volumeEffects);
        ma_sound_set_volume(&soundMenuBackPool[i], game.volumeEffects);
    }
}

void setMusicVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;  // Impede valores negativos
    if (volume > 1.0f) volume = 1.0f;  // Limita o volume ao máximo permitido
    game.volumeMusic = volume;
    ma_sound_set_volume(&soundMenu, game.volumeMusic);
}

void setAmbientVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;  // Impede valores negativos
    if (volume > 1.0f) volume = 1.0f;  // Limita o volume ao máximo permitido
    game.volumeAmbient = volume;
    ma_sound_set_volume(&soundAmbient, game.volumeAmbient);
}

void increaseEffectVolume() {
    setEffectVolume(game.volumeEffects + 0.1f); // Aumenta o volume em 0.1
    printf("Volume dos efeitos aumentado para %.1f\n", game.volumeEffects);
}

void decreaseEffectVolume() {
    setEffectVolume(game.volumeEffects - 0.1f); // Diminui o volume em 0.1
    printf("Volume dos efeitos reduzido para %.1f\n", game.volumeEffects);
}

void increaseMusicVolume() {
    setMusicVolume(game.volumeMusic + 0.1f); // Aumenta o volume em 0.1
    printf("Volume da música aumentado para %.1f\n", game.volumeMusic);
}

void decreaseMusicVolume() {
    setMusicVolume(game.volumeMusic - 0.1f); // Diminui o volume em 0.1
    printf("Volume da música reduzido para %.1f\n", game.volumeMusic);
}

void increaseAmbientVolume() {
    setAmbientVolume(game.volumeAmbient + 0.1f); // Aumenta o volume em 0.1
    printf("Volume ambiente aumentado para %.1f\n", game.volumeAmbient);
}

void decreaseAmbientVolume() {
    setAmbientVolume(game.volumeAmbient - 0.1f); // Diminui o volume em 0.1
    printf("Volume ambiente reduzido para %.1f\n", game.volumeAmbient);
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
