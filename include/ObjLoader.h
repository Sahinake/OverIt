#ifndef OBJLOADER_H
#define OBJLOADER_H

// Estrutura do ponto
typedef struct {
    float x, y, z;
} Vector;

// Estrutura da face
typedef struct {
    Vector normaA;
    Vector normaB;
    Vector normaC;
    Vector vertexA;
    Vector vertexB;
    Vector vertexC;
    Vector textureA;
    Vector textureB;
    Vector textureC;
    int index;
} Face;

// Estrutura do nó da lista duplamente encadeada
typedef struct Object {
    Face* faces;      // Ponteiro para um array de faces
    int size;     // Número de faces no objeto
    struct Object* prev;
    struct Object* next;
} Object;

// Estrutura da lista duplamente encadeada
typedef struct {
    // Ponteiro para o primeiro nó
    Object *head;
    // Ponteiro para o último nó
    Object *tail;
} ObjectList;

extern ObjectList objectList;

void initObjectList(ObjectList *lde);
void addObjectList(ObjectList *lde, Object *obj);
Object* getObjectList(ObjectList *lde, int index);
void removeObjectList(ObjectList *lde, Object *obj);
void printObjectList(ObjectList* lde);
void clearObjectList(ObjectList *lde);
void loadObjectFile(ObjectList* lde, const char* filename);
Vector createVector3D(float x, float y, float z);
Face createFace(Vector normaA, Vector normaB, Vector normaC, Vector vertexA, Vector vertexB, Vector vertexC, Vector textureA, Vector textureB, Vector textureC, int index);

#endif