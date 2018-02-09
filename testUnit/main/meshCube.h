#ifndef DEF_MESH_CUBE
#define DEF_MESH_CUBE

#include "main.h"

// Size of the cube
#define X_MIN -3.5	// WARNING: we must have real XYZ_MIN < real XYZ_MAX
#define Y_MIN -3.5
#define Z_MIN -3.5

#define X_MAX 3.5
#define Y_MAX 3.5
#define Z_MAX 3.5

// Prototypes of non-static functions in meshCube.c
void initializeMeshStructure(Mesh* pMesh);
void freeMeshMemory(Mesh* pMesh);
int writingMeshFile(char* fileLocation, Mesh* pMesh);
int meshCube(char* fileLocation, Mesh *pMesh);

/*
// Prototypes of static functions in meshCube.c
static int copyFileLocation(char* fileLocation, char* fileLocationForCopy);
static int initializeCubeDiscretization(Mesh* pMesh);
static int allocateInitialMeshMemory(Mesh* pMesh);
static int labelPoint(int i, int j, int k);
static int discretizeCube(Mesh* pMesh);
*/

#endif

