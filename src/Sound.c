#include <stdio.h>
#include <stdlib.h>

#include "miniaudio.h"
#include "Sound.h"

extern float volumeEffects;

void initAudio() {
    // Inicializa o motor de áudio
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Falha ao inicializar o motor de áudio.\n");
        exit(1);
    }

    // Carrega os arquivos de áudio
    if (ma_sound_init_from_file(&engine, "assets/Ambient.mp3", 0, NULL, NULL, &soundAmbient) != MA_SUCCESS) {
        printf("Falha ao carregar a música ambiente.\n");
    }
    if (ma_sound_init_from_file(&engine, "assets/Dot.mp3", 0, NULL, NULL, &soundDotCollect) != MA_SUCCESS) {
        printf("Falha ao carregar o som de coleta de dot.\n");
    }

    // Configura o volume inicial
    ma_sound_set_volume(&soundDotCollect, volumeEffects);
    ma_sound_set_volume(&soundAmbient, 0.5f); // Volume ambiente inicial menor
}

void playAmbientMusic() {
    // Configura o callback para tocar a música ambiente quando a introdução terminar
    ma_sound_start(&soundAmbient);
    ma_sound_set_looping(&soundAmbient, MA_TRUE); // Define a música ambiente para repetir em loop
}

void playDotCollectSound() {
    ma_sound_stop(&soundDotCollect);  // Para o som atual, caso esteja tocando
    ma_sound_start(&soundDotCollect); // Recomeça o som do início
}

void setEffectVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;  // Impede valores negativos
    if (volume > 1.0f) volume = 1.0f;  // Limita o volume ao máximo permitido
    volumeEffects = volume;
    ma_sound_set_volume(&soundDotCollect, volumeEffects);
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
}
