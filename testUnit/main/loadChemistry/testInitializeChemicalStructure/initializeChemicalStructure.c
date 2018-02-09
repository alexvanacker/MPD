#include "initializeChemicalStructure.h"

#define PRINT_TEST_START(counter,expectedValue) do {									\
	(counter)++;													\
	printf("\nTest %d starts: expecting %d as returnValue.\n",(counter),(expectedValue));				\
} while(0)

#define PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar) do {			\
	if ((returnValue)==(expectedValue))										\
	{														\
		printf("Test %d succeeded: we got returnValue=%d as expected.\n",(counter),(returnValue));		\
		(counterSuccess)++;											\
	}														\
	else														\
	{														\
		printf("Test %d failed: returnValue=%d was not the expected value.\n",(counter),(returnValue));		\
		(counterFail)++;											\
		(readChar)=getchar();											\
		while ((readChar)!='\n' && (readChar)!=EOF)								\
		{													\
			(readChar)=getchar(); 										\
		} 													\
	}														\
} while (0)

// Testing
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int imorb=0, jmorb=0, inucl=0, jnucl=0, igauss=0;
	int nmorb=8, nnucl=3, ngauss=128;

	ChemicalSystem *pChemicalSystem=NULL, chemicalSystem;

	// Initializing the chemical structure
	chemicalSystem.nmorb=0;
	chemicalSystem.pmorb=NULL;

	chemicalSystem.nnucl=0;
	chemicalSystem.pnucl=NULL;

	chemicalSystem.ngauss=0;

	// Test starts
	time(&startTimer);
	printf("Testing initializeChemicalStructure function.\n");	 

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	printf("pChemicalSystem=%p\n",pChemicalSystem);
	initializeChemicalStructure(pChemicalSystem);
	returnValue=1;
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pChemicalSystem=&chemicalSystem;
	PRINT_TEST_START(counter,expectedValue);
	printf("pChemicalSystem=%p\n",pChemicalSystem);
	initializeChemicalStructure(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	for (imorb=-nmorb; imorb<=nmorb; imorb+=nmorb)
	{
		for (jmorb=-nmorb; jmorb<=nmorb; jmorb+=nmorb)
		{
			for (inucl=-nnucl; inucl<=nnucl; inucl+=nnucl)
			{
				for (jnucl=-nnucl; jnucl<=nnucl; jnucl+=nnucl)
				{
					for (igauss=-ngauss; igauss<=ngauss; igauss+=ngauss)
					{
						pChemicalSystem->nmorb=imorb;
						pChemicalSystem->pmorb=(MolecularOrbital*)calloc(jmorb,sizeof(MolecularOrbital));
						free(pChemicalSystem->pmorb);

						pChemicalSystem->nnucl=inucl;
						pChemicalSystem->pnucl=(Nucleus*)calloc(jnucl,sizeof(Nucleus));
						free(pChemicalSystem->pnucl);

						pChemicalSystem->ngauss=igauss;

						returnValue=1;
						PRINT_TEST_START(counter,expectedValue);
						printf("nmorb=%d pmorb=%p nnucl=%d ",pChemicalSystem->nmorb,pChemicalSystem->pmorb,pChemicalSystem->nnucl);
						printf("pnucl=%p ngauss=%d\n",pChemicalSystem->pnucl,pChemicalSystem->ngauss);
						initializeChemicalStructure(pChemicalSystem);
						if (pChemicalSystem->nmorb!=0 || pChemicalSystem->pmorb!=NULL || pChemicalSystem->nnucl!=0 || pChemicalSystem->pnucl!=NULL || pChemicalSystem->ngauss!=0)
						{
							printf("Error: although initializeChemicalStructure function ends normally, the chemical structure was not properly initialized.\n");
							printf("Expecting nmorb=0 pmorb=NULL nnucl=0 pnucl=NULL ngauss=0 instead of nmorb=%d ",pChemicalSystem->nmorb);
							printf("pmorb=%p nnucl=%d pnucl=%p ngauss=%d\n",pChemicalSystem->pmorb,pChemicalSystem->nnucl,pChemicalSystem->pnucl,pChemicalSystem->ngauss);
							returnValue=0;
						}
						PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
					}
				}
			}
		}
	}

	// End of the tests
	printf("\nTotal: %d tests (%d succeeded, %d failed) done in ",counter,counterSuccess,counterFail);
	time(&endTimer);
	if (difftime(endTimer,startTimer)<60.)				
	{									
		 printf("%lf seconds.\n",difftime(endTimer,startTimer));	
	}										
	else 									
	{											
		printf("%lf minutes.\n",difftime(endTimer,startTimer)/60.);				
	}									

	return EXIT_SUCCESS;
}

/* ****************************************************************************************************************************** */
// The function initializeChemicalStructure initializes the chemical structure to zero for variables and NULL for pointers. 
// It has the pChemicalSystem structure (described in main.h) as input argument and it does not return any argument (void ouput)
/* ****************************************************************************************************************************** */
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem)
{
	if (pChemicalSystem!=NULL)
	{
		pChemicalSystem->nmorb=0;
		pChemicalSystem->pmorb=NULL;

		pChemicalSystem->nnucl=0;
		pChemicalSystem->pnucl=NULL;
		
		pChemicalSystem->ngauss=0;
	}

	return;
}

