#ifndef DEF_INITIALIZE_CHEMICAL_STRUCTURE
#define DEF_INITIALIZE_CHEMICAL_STRUCTURE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ********************************************************************** */
// Definitions of the different chemical structures used in the program
/* ********************************************************************** */
typedef struct {
	double x;			// First coordinate of the nucleus center
	double y;			// Second coordinate of the nucleus center
	double z;			// Third coordinate of the nucleus center
} Nucleus;

typedef struct {
	int spin;			// Integer used to define the spin associated with the molecular orbital (+1=up, -1=down)
	double* coeff;			// Pointer used to define the array containing the coefficients in the expansion of the molecular orbital into a sum of (Gaussian-type) primitives
	double* exp;			// Pointer used to define the array containing the exponents in the expansion of the molecular orbital into a sum of (Gaussian-type) primitives
	int* nucl;			// Pointer used to define the array containing the center reference of the nuclei in the expansion of the molecular orbital into a sum of (Gaussian-type) primitives
	int* type;			// Pointer used to define the array containing the type of Gaussian in the expansion of the molecular orbital into a sum of primitives
} MolecularOrbital;

typedef struct {
	int nmorb;			// Integer used to store the number of molecular orbitals in the chemical system considered
	MolecularOrbital* pmorb;	// Pointer used to define the array containing the molecular orbitals of the chemical system considered

	int nnucl;			// Integer used to store the number of nuclei in the chemical system considered
	Nucleus* pnucl;			// Pointer used to define the array containing the nuclei centers of the chemical system considered
	
	int ngauss;			// Integer used to store the number of (Gaussian-type) primitives used in the expansion of the molecular orbitals
} ChemicalSystem;

/*
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem);
*/

#endif

