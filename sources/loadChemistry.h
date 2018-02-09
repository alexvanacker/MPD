#ifndef DEF_LOAD_CHEMISTRY
#define DEF_LOAD_CHEMISTRY

#include "main.h"

#define ORB_SPIN 1	// it equals one to allow duplication of the molecular orbitals with occupation number equals to two in *.wfn file, otherwise it must be set to zero (other values are forbidden)

// Prototypes of non-static functions in loadChemistry.c
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem);
void freeChemicalMemory(ChemicalSystem* pChemicalSystem);
int loadChemistry(char* fileLocation, ChemicalSystem *pChemicalSystem, int nuElectrons);

/*
// Prototypes of static functions in loadChemistry.c
static int getChemicalFormat(char* fileLocation);
static int readAndConvertWfnFile(char* fileLocation, char charToRemove, char charToPutInstead);
static int readWfnFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem);
static int writingChemicalFile(char* fileLocation, ChemicalSystem* pChemicalSystem);
static int readTxtFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem);
*/

#endif

