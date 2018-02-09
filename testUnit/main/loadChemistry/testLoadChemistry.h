#ifndef DEF_TEST_LOAD_CHEMISTRY
#define DEF_TEST_LOAD_CHEMISTRY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Macro function to initialize diagnostic of an error
#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

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

int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);

#endif

