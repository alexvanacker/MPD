#ifndef DEF_SHAPE_DERIVATIVE
#define DEF_SHAPE_DERIVATIVE

#include "main.h"

// Constants defined in the article of Grundmann and Moller (april 1978) for numerical integration (formula used by default in Freefem++ software for three-dimensional tetrahedral meshes)
// The 14-point formula (cf. (4.5), p. 288) is exact on tetrahedra for 5th-order polynomials (sum x^a*y^b*z^c with a+b+c<=5)
// Otherwise int_tet(f)=vol(tet)*sum_i [w_i*f(a1i,a2i,a3i,a4i)] + Error (of order 6) 
#define CST_A 0.007091003462846911	// weights
#define CST_B 0.018781320953002642
#define CST_C 0.012248840519393658

#define CST_a 0.454496295874350351	// barycentric coordinates
#define CST_b 0.310885919263300610
#define CST_c 0.092735250310891226
             
#define CST_aa 0.045503704125649649 	// (1/2-CST_a)
#define CST_bb 0.067342242210098170	// (1-3*CST_b)
#define CST_cc 0.721794249067326322	// (1-3*CST_c)


// Prototypes of non-static functions in shapeDerivative.c
void freeMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues);
int allocateMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues, int size);
int shapeDerivative(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, double* eigenvectors, double* eigenvalues, int nuElectrons, double* pProbability, double* pProbabilityOld, int trick, int i);
#ifdef LS_MODE
	int AddOrRemoveHexahedronToMesh(Mesh* pMesh, int j, int addOrRemove);
#endif

#ifndef LS_MODE
	int writingShapeSolFile(char* fileLocation, Mesh* pMesh);
#endif

/*
// Prototypes of static functions in shapeDerivative.c
#ifdef LS_MODE
	static int convertingType(int type, int oriention);
	static double evaluateGaussianIntegral(int exp0, double t0, double t1);
	static double evaluateOneIntegralIJ(int expI, int expJ, double cI, double cJ, double t0, double t1);
	static double evaluateTripleIntegralIJ(Mesh *pMesh, ChemicalSystem *pChemicalSystem, MolecularOrbital *pMolecularOrbitalI, MolecularOrbital *pMolecularOrbitalJ, int primitiveI, int primitiveJ, int kHexahedron);
	static int computeOverlapMatrixOnGrid(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, int label);
#endif

#ifndef LS_MODE
	static double computeVolumeTetrahedron(Point* pp1, Point* pp2, Point* pp3, Point* pp4);	
	static int computeOverlapMatrix(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, int label);
#endif

	static int diagonalizeOverlapMatrix(double* overlapMatrix, double** pEigenvectors, double** pEigenvalues, int sizeMatrix);
	static double computeProbability(double* eigenvalues, int numberElectrons, int nuElectrons);
	static double evaluatingPrimitiveAtVertices(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbital, int primitive);
	static double evaluateOrbitalsIJ(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbitalI, MolecularOrbital* pMolecularOrbitalJ);
	static double evaluateOrbitalsII(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbitalI);
	static int computeShapeDerivative(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* eigenvalues, double* eigenvectors, int nuElectrons, int trick);

#endif
*/

#endif

