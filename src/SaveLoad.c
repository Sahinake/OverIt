#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>  // Para mkdir
#include <dirent.h>
#include <ctype.h>

#include "SaveLoad.h"
#include "Game.h"
#include "UI.h"

extern int goalDots;
extern bool inputingSaveName;
extern char saveName[256];

#define MAX_SAVES 4

// Função para criar a pasta saves caso não exista
void createSavesDirectory() {
    // Tenta criar a pasta 'saves' se ela não existir
    struct stat st = {0};
    if (stat("saves", &st) == -1) {
        mkdir("saves", 0700);  // Cria a pasta com permissão 0700
    }
}

void removeFinishedSaves(const char* directoryPath) {
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        printf("Erro: Não foi possível abrir o diretório '%s'.\n", directoryPath);
        return;
    }

    struct dirent* entry;
    char filePath[512];

    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Montar o caminho completo do arquivo
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        // Abrir o arquivo para verificar o estado
        FILE* file = fopen(filePath, "rb");
        if (file != NULL) {
            SavedGame savedGame;
            fread(&savedGame, sizeof(SavedGame), 1, file);
            fclose(file);

            // Verificar o estado do jogo
            if (savedGame.currentState == FINISHED) {
                if (remove(filePath) == 0) {
                    printf("Save '%s' com estado FINISHED foi removido.\n", entry->d_name);
                } else {
                    printf("Erro ao tentar remover o save '%s'.\n", entry->d_name);
                }
            }
        }
    }

    closedir(dir);
}

// Função de salvar o estado do jogo
void saveGame(const char* filename, Player* player, Game* game, int elapsedTime) {
    const char* savesDirectory = "./saves";

    // Criar a pasta saves caso não exista
    createSavesDirectory();

    // Remover saves com estado FINISHED
    removeFinishedSaves(savesDirectory);

    // Verificar a quantidade de saves existentes
    int saveCount = countFilesInDirectory(savesDirectory);
    if (saveCount < 0) {
        // Erro ao acessar o diretório
        return;
    }

    if (saveCount >= MAX_SAVES) {
        printf("Limite de %d saves atingido. Apague um save antes de criar outro.\n", MAX_SAVES);
        return;
    }

    char filePath[512];
    snprintf(filePath, sizeof(filePath), "saves/%s", filename); 

    FILE* file = fopen(filePath, "wb");
    if (file != NULL) {
        // Salvar o estado do jogo
        SavedGame savedGame;
        
        snprintf(savedGame.saveName, sizeof(savedGame.saveName), "%s", filename);  // Nome do save
        savedGame.lastPlayed = time(NULL);  // Salvar a data atual

        // Passando as variáveis como parâmetros
        for (int i = 0; i < 30; i++) {
            savedGame.dots[i] = game->dots[i];
        }
        for (int i = 0; i < 5; i++) {
            savedGame.batteries[i] = game->batteries[i];
        }  
        
        savedGame.player = *player;  
        savedGame.elapsedSaveTime = elapsedTime + elapsedSaveTime;
        savedGame.exitDoor = game->exitDoor;  
        
        // Copiar o labirinto para o save
        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                savedGame.maze[i][j] = game->maze[i][j];
            }
        }
        strncpy(saveName, filename, sizeof(saveName) - 1);
        saveName[sizeof(saveName) - 1] = '\0';  // Garante que o nome seja NULL-terminated

        savedGame.currentState = game->currentState;   
        fwrite(&savedGame, sizeof(SavedGame), 1, file);
        fclose(file);
        printf("Jogo salvo com sucesso em: %s\n", filePath);
    } else {
        printf("Erro ao abrir o arquivo para salvar o jogo.\n");
    }
}

// Função de carregar o estado do jogo
void loadGame(const char* filename, Player* player, Game* game) {
    // Montar o caminho completo para o arquivo de save na pasta saves
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "saves/%s", filename);

    FILE* file = fopen(filePath, "rb");
    
    if (file != NULL) {
        SavedGame savedGame;
        
        // Ler os dados do jogo salvo
        if (fread(&savedGame, sizeof(SavedGame), 1, file) != 1) {
            printf("Erro ao ler o arquivo de save.\n");
            fclose(file);
            return;
        }
        fclose(file);

        // Restaurar o estado do jogo
        game->currentState = PLAYING;  // Supondo que o jogo deve começar com a tela de jogo
        game->selectedOption = 0;      

        // Restaurar outros estados do jogo (dots, batteries, player, etc.)
        for (int i = 0; i < 30; i++) {
            game->dots[i] = savedGame.dots[i];  // Copia os elementos de dots
        }
        
        for (int i = 0; i < 5; i++) {
            game->batteries[i] = savedGame.batteries[i];  // Copia os elementos de batteries
        } 
        *player = savedGame.player;  
        elapsedSaveTime = savedGame.elapsedSaveTime;
        
        game->exitDoor = savedGame.exitDoor; 
                
        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                game->maze[i][j] = savedGame.maze[i][j];
            }
        }
        goalDots = 0;
        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                if (game->maze[i][j] == 2) {
                    goalDots++;  // Incrementa a quantidade de dots não coletados
                }
            }
        }

        strncpy(saveName, filename, sizeof(saveName) - 1);
        saveName[sizeof(saveName) - 1] = '\0';  // Garante que o nome seja NULL-terminated
        
        // Mostrar uma mensagem de sucesso ou algum tipo de confirmação
        printf("Jogo carregado com sucesso de: %s\n", filePath);
    } else {
        // Se não houver um arquivo de jogo, mostre uma mensagem de erro
        printf("Erro ao carregar o jogo. Arquivo não encontrado em: %s\n", filePath);
    }
}

int countFilesInDirectory(const char* directoryPath) {
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        printf("Erro: Não foi possível abrir o diretório '%s'.\n", directoryPath);
        return -1;
    }

    struct dirent* entry;
    int fileCount = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        fileCount++;
    }

    closedir(dir);
    return fileCount;
}

int isSlotEmpty(Game* game, int slotIndex) {
    if (slotIndex < 0 || slotIndex >= MAX_SAVES) {
        printf("Índice do slot fora do intervalo permitido.\n");
        return 1;  // Tratar slots fora do intervalo como vazios
    }
    return game->slotFiles[slotIndex] == NULL;
}

void saveSelectedGame(Game* game, Player* player, int elapsedTime) {
    if (game->selectedOption >= 0 && game->selectedOption < MAX_SAVES) {
        // Verifica se o slot já possui um save
        if (!isSlotEmpty(game, game->selectedOption)) {
            // Exibe mensagem de sobrescrever
            printf("O slot %d já possui um save.\n", game->selectedOption);
            return;
        }

        // Nome do arquivo com base no slot selecionado
        snprintf(saveName, sizeof(saveName), "Oliver_%d", game->selectedOption + 1); // Gera o nome do arquivo baseado no slot

        // Salva o jogo com o nome gerado
        saveGame(saveName, player, game, elapsedTime);
        loadSaveSlots(game);  // Recarrega os slots para atualizar o estado
        game->currentState = PLAYING;
    } else {
        printf("Opção selecionada inválida.\n");
    }
}

// Função para extrair o número do slot a partir do nome do arquivo
int getSlotFromSaveName(const char* filename) {
    // Supondo que o formato do nome seja "Oliver_##"
    int slot = -1;
    while (*filename) {
        if (isdigit(*filename)) {
            slot = *filename - '0';  // Extrai o número, assume que é um único dígito
            break;
        }
        filename++;
    }
    return slot;  // Retorna o número do slot ou -1 se não encontrar
}

void loadSaveSlots(Game* game) {
    // Abre a pasta "saves"
    DIR* dir = opendir("./saves");
    if (dir) {
        struct dirent* entry;
        
        // Inicializa os slotFiles
        for (int i = 0; i < MAX_SAVES; i++) {
            game->slotFiles[i] = NULL;  // Inicializa os slots como NULL
        }

        // Lê o conteúdo da pasta de saves
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;  // Ignora "." e ".."
            }

            // Extrai o slot a partir do nome do arquivo
            int slot = getSlotFromSaveName(entry->d_name) - 1;
            if (slot >= 0 && slot < MAX_SAVES) {
                // Aloca e copia o nome do arquivo para o slot correspondente
                game->slotFiles[slot] = strdup(entry->d_name);  // Aloca e copia o nome
            }
        }
        closedir(dir);
    }
}

void loadSelectedGame(Game* game, Player* player) {
    if (game->selectedOption >= 0 && game->selectedOption < MAX_SAVES) {
        char* selectedSaveFile = game->slotFiles[game->selectedOption];
        if (selectedSaveFile != NULL) {
            loadGame(selectedSaveFile, player, game);  // Carrega o jogo
        } else {
            printf("Nenhum save encontrado nesse slot.\n");
        }
    }
}

// Função para apagar o slot de save selecionado
void deleteSelectedSave(Game* game) {
    if (game->selectedOption >= 0 && game->selectedOption < MAX_SAVES) {
        // Verifica se o slot possui um save
        char* selectedSaveFile = game->slotFiles[game->selectedOption];
        if (selectedSaveFile != NULL) {
            // Monta o caminho completo do arquivo de save
            char filePath[512];
            snprintf(filePath, sizeof(filePath), "saves/%s", selectedSaveFile);

            // Tenta remover o arquivo de save
            if (remove(filePath) == 0) {
                printf("Save '%s' apagado com sucesso.\n", selectedSaveFile);
                
                // Libera a memória alocada para o nome do arquivo
                free(game->slotFiles[game->selectedOption]);
                game->slotFiles[game->selectedOption] = NULL;

                // Recarrega os slots de save
                loadSaveSlots(game);
            } else {
                printf("Erro ao tentar apagar o save '%s'.\n", selectedSaveFile);
            }
        } else {
            printf("Nenhum save encontrado neste slot.\n");
        }
    } else {
        printf("Índice do slot inválido.\n");
    }
}

void cleanupSaveSlots(Game* game) {
    for (int i = 0; i < MAX_SAVES; i++) {
        if (game->slotFiles[i] != NULL) {
            free(game->slotFiles[i]);  // Libera a memória alocada para o nome do arquivo
            game->slotFiles[i] = NULL;
        }
    }
}

void printSave(const char* filename) {
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "saves/%s", filename);

    FILE* file = fopen(filePath, "rb");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo '%s'.\n", filePath);
        return;
    }

    SavedGame savedGame;
    fread(&savedGame, sizeof(SavedGame), 1, file);
    fclose(file);

    printf("=== Conteúdo do Save: %s ===\n", filename);
    printf("Nome do Save: %s\n", savedGame.saveName);
    printf("Data do Último Jogo: %s", ctime(&savedGame.lastPlayed));
    printf("Tempo de Jogo Salvo: %d segundos\n", savedGame.elapsedSaveTime);

    printf("Estado Atual do Jogo: %d\n", savedGame.currentState);
    printf("Porta de Saída: (%d, %d)\n", savedGame.exitDoor.x, savedGame.exitDoor.y);

    printf("Posição do Jogador: (%d, %d)\n", savedGame.player.x, savedGame.player.y);

    printf("Labirinto:\n");
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            printf("%c ", savedGame.maze[i][j]);
        }
        printf("\n");
    }

    printf("Dots:\n");
    for (int i = 0; i < 30; i++) {
        printf("Dot %d: (%d, %d) - Coletado: %d\n",
               i, savedGame.dots[i].x, savedGame.dots[i].y, savedGame.dots[i].collected);
    }

    printf("Baterias:\n");
    for (int i = 0; i < 5; i++) {
        printf("Bateria %d: (%d, %d) - Energia: %d\n",
               i, savedGame.batteries[i].x, savedGame.batteries[i].y, savedGame.batteries[i].collected);
    }
    printf("========================\n");
}


