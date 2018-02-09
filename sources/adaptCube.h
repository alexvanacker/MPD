#ifndef DEF_ADAPT_CUBE
#define DEF_ADAPT_CUBE

#include "main.h"
#include "meshCube.h"

#define MET_CST 0.28125		// Dimensional constant for computing metric 9/32 in 3d, 2/9 in 2d, and (d/(d+1))^2/2 in dimension d>=2
#define MET_ERR 0.01		// Error allowed on the computed metric (Warning: MET_ERR must be positive here)
#define MET_MIN 0.01		// Warning: MET_MIN must be positive here
#define MET_MAX 1.0		// Warning: MET_MAX > MET_MIN here
#define ITER_MAX 100000		// Maximum iteration in the search of eigenvalues (find zero of third-order polynomial with Newton's method)
#define TOL_MAX 1.e-10		// Tolerance allowed on computation of the zeros (eigenvalues) of the characteristic polynomial associated with a symmetric (3x3)-matrix

// Prototypes of non-static static functions in adaptCube.c
int readAndAllocateMesh(char* fileLocation, Mesh* pMesh);
int evaluatingMetricOnMesh(Mesh* pMesh, ChemicalSystem* pChemicalSystem);
int getLevelSetQuadrilaterals(Mesh* pMesh);
int initialLevelSetInMeshExists(Mesh* pMesh);
int writingSolFile(char* fileLocation, Mesh* pMesh);
int adaptCube(char* fileLocation, char* commandLineMmg3dIsotropic, char* commandLineMmg3dMetric, char* commandLineMmg3dLevelSet, Mesh* pMesh, ChemicalSystem* pChemicalSystem);

/*
// Prototypes of static functions in adaptCube.c
static double evaluatingHessianAtVertices(ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbital, Point* pPoint, double gradient[3], double hessian[6], int primitive);
static int getRootsFromCoefficients(double roots[3], double coefficients[3]);
static int findEigenvaluesOfSymmetricMatrix(double matrix[6], double roots[3]);
static int initializeLevelSet(Mesh* pMesh);
#ifdef LS_MODE
	static int initializeAdjacency(Mesh* pMesh);
#endif
*/

#endif

