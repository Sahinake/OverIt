#include <stdio.h>
#include <stdlib.h>

#include "ObjLoader.h"

// Função para inicializar a lista de objetos
void initObjectList(ObjectList *lde) {
    lde->head = NULL;
    lde->tail = NULL;
}

// Função para adicionar um objeto à lista
void addObjectList(ObjectList *lde, Object *obj) {
    if (lde->head == NULL) {
        lde->head = lde->tail = obj;
    } else {
        lde->tail->next = obj;
        obj->prev = lde->tail;
        lde->tail = obj;
    }
}

// Função para pegar um objeto da lista
Object* getObjectList(ObjectList *lde, int index) {
    Object *current = lde->head;
    int i = 0;

    // Percorre a lista até encontrar o objeto no índice desejado
    while (current != NULL) {
        if (i == index) {
            return current;  // Retorna o objeto no índice desejado
        }
        current = current->next;
        i++;
    }

    // Se o índice não for encontrado, retorna NULL
    return NULL;
}

// Função para remover um objeto da lista
void removeObjectList(ObjectList *lde, Object *obj) {
    if (obj->prev) {
        obj->prev->next = obj->next;
    } else {
        lde->head = obj->next;  // Caso o objeto seja o primeiro
    }

    if (obj->next) {
        obj->next->prev = obj->prev;
    } else {
        lde->tail = obj->prev;  // Caso o objeto seja o último
    }

    free(obj->faces);  // Libera o array de faces
    free(obj);         // Libera o objeto
}

// Função para limpar a lista e liberar a memória
void clearObjectList(ObjectList *lde) {
    Object *current = lde->head;
    Object *nextNode;

    while (current != NULL) {
        nextNode = current->next;
        removeObjectList(lde, current);  // Remove e libera cada objeto
        current = nextNode;
    }

    lde->head = lde->tail = NULL;  // A lista fica vazia
}

Face createFace(Vector normaA, Vector normaB, Vector normaC, Vector vertexA, Vector vertexB, Vector vertexC, Vector textureA, Vector textureB, Vector textureC, int index) {
    Face face;

    face.normaA = normaA;
    face.normaB = normaB;
    face.normaC = normaC;

    face.vertexA = vertexA;
    face.vertexB = vertexB;
    face.vertexC = vertexC;

    face.textureA = textureA;
    face.textureB = textureB;
    face.textureC = textureC;

    face.index = index;
    return face;
}

Vector createVector3D(float x, float y, float z) {
    Vector result;

    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

void addFace(Face** face, int* index, int* size, Face newFace) {
    if (*index+1 >= *size) {
        *size *= 2;
        *face = (Face *)realloc(*face, *size * sizeof(Face));
        if (*face == NULL) {
            fprintf(stderr, "Erro ao realocar memória\n");
            exit(1);
        }else{
            printf("Vetor de faces realocado para o tamanho: %d \n", *size);
        }
    }
    
    (*face)[*index] = newFace;
    *index = *index + 1;
}

void addVector(Vector** vector, int* index, int* size, Vector newVector) {
    if (*index+1 >= *size) {
        *size *= 2;
        *vector = (Vector *)realloc(*vector, *size * sizeof(Vector));
        if (*vector == NULL) {
            fprintf(stderr, "Erro ao realocar memória\n");
            exit(1);
        }else{
            printf("Vetor realocado para o tamanho: %d \n", *size);
        }
    }
    
    (*vector)[*index] = newVector;
    *index = *index + 1;
}

void loadObjectFile(ObjectList* lde, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", filename);
        return;
    }

    int sizeF = 128;
    int sizeV = 128;
    int sizeN = 128;
    int sizeT = 128;

    Face *faces = (Face *)malloc(sizeF * sizeof(Face));
    Vector *vertices = (Vector *)malloc(sizeV * sizeof(Vector));
    Vector *normals = (Vector *)malloc(sizeN * sizeof(Vector));
    Vector *textures = (Vector *)malloc(sizeT * sizeof(Vector));

    int numFaces = 0, numVertices = 0, numNormals = 0, numTextures = 0;
    float x, y, z;
    char ch;

    while (!feof(file)) {
        ch = fgetc(file);
        if (ch == 'v') {
            ch = fgetc(file);
            if (ch == ' '){
                fscanf(file, "%f %f %f", &x, &y, &z);
                addVector(&vertices, &numVertices, &sizeV, createVector3D(x, y, z));
            } else if (ch == 't') {
                fscanf(file, "%f %f", &x, &y);
                addVector(&textures, &numTextures, &sizeT, createVector3D(x, y, 0.0f));
            } else if (ch == 'n') {
                fscanf(file, "%f %f %f", &x, &y, &z);
                addVector(&normals, &numNormals, &sizeN, createVector3D(x, y, z));
            }
        } else if (ch == 'f') {
            int v1, t1, n1, v2, t2, n2, v3, t3, n3;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
            // Ajusta os índices para começarem de 0
            v1--; t1--; n1--;
            v2--; t2--; n2--;
            v3--; t3--; n3--;
            addFace(&faces, &numFaces, &sizeF,
            createFace(normals[n1], normals[n2], normals[n3],
                    vertices[v1], vertices[v2], vertices[v3],
                    textures[t1], textures[t2], textures[t3],
                    numFaces));
        } else if (ch == 'n') {
            fscanf(file, "%f %f %f", &x, &y, &z);
            addVector(&normals, &numNormals, &sizeN, createVector3D(x, y, z));
        }
    }

    //Cria o objeto com as faces carregadas
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->faces = faces;
    obj->size = numFaces;
    obj->prev = obj->next = NULL;

    //Adiciona o objeto à lista
    addObjectList(lde, obj);

    // Fecha o arquivo
    fclose(file);
}

// Função para imprimir todos os objetos e suas faces
void printObjectList(ObjectList* lde) {
    Object *current = lde->head;
    while (current != NULL) {
        printf("Objeto encontrado com %d faces.\n", current->size);
        current = current->next;
    }
}