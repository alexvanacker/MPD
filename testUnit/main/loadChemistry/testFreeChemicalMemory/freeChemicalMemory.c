#include "freeChemicalMemory.h"

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
void freeChemicalMemory(ChemicalSystem* pChemicalSystem);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int i=0, imorb=0, inucl=0, igauss=0, pspin=0, pcoeff=0, pexp=0, pnucl=0, ptype=0;
	int nmorb=8, nnucl=3, ngauss=128, nspin=1, ncoeff=128, nexp=128, npnucl=128, ntype=128;

	MolecularOrbital *pMolecularOrbital=NULL;
	ChemicalSystem *pChemicalSystem=NULL, chemicalSystem;

	// Initializing the chemical structure
	chemicalSystem.nmorb=-1;
	chemicalSystem.pmorb=(MolecularOrbital*)calloc(6,sizeof(MolecularOrbital));

	chemicalSystem.nnucl=0;
	chemicalSystem.pnucl=NULL;

	chemicalSystem.ngauss=0;

	// Test starts
	time(&startTimer);
	printf("Testing freeChemicalMemory function.\n");

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	printf("pChemicalSystem=%p\n",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	returnValue=1;
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pChemicalSystem=&chemicalSystem;
	PRINT_TEST_START(counter,expectedValue);
	printf("pChemicalSystem=%p\n",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	chemicalSystem.nmorb=0;
	PRINT_TEST_START(counter,expectedValue);
	printf("pChemicalSystem=%p\n",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	for (imorb=-nmorb; imorb<=nmorb; imorb+=nmorb)
	{
		for (inucl=-nnucl; inucl<=nnucl; inucl+=nnucl)
		{
			for (igauss=-ngauss; igauss<=ngauss; igauss+=ngauss)
			{	
				if (imorb>0)
				{
					for (pspin=-nspin; pspin<=nspin; pspin+=nspin)
					{
						for (pcoeff=-ncoeff; pcoeff<=ncoeff; pcoeff+=ncoeff)
						{
							for (pexp=-nexp; pexp<=nexp; pexp+=nexp)
							{
								for (pnucl=-npnucl; pnucl<=npnucl; pnucl+=npnucl)
								{
									for (ptype=-ntype; ptype<=ntype; ptype+=ntype)
									{
										pChemicalSystem->nmorb=imorb;
										pChemicalSystem->pmorb=(MolecularOrbital*)calloc(imorb,sizeof(MolecularOrbital));

										pChemicalSystem->nnucl=inucl;
										pChemicalSystem->pnucl=(Nucleus*)calloc(inucl,sizeof(Nucleus));
										
										pChemicalSystem->ngauss=igauss;

										for (i=0; i<imorb; i++)
										{
											pMolecularOrbital=&pChemicalSystem->pmorb[i];
											pMolecularOrbital->spin=pspin;										
											pMolecularOrbital->coeff=(double*)calloc(pcoeff,sizeof(double));
											pMolecularOrbital->exp=(double*)calloc(pexp,sizeof(double));
											pMolecularOrbital->nucl=(int*)calloc(pnucl,sizeof(int));
											pMolecularOrbital->type=(int*)calloc(ptype,sizeof(int));
										}

										returnValue=1;
										PRINT_TEST_START(counter,expectedValue);
										printf("nmorb=%d pmorb=%p nnucl=%d ",pChemicalSystem->nmorb,pChemicalSystem->pmorb,pChemicalSystem->nnucl);
										printf("pnucl=%p ngauss=%d\n",pChemicalSystem->pnucl,pChemicalSystem->ngauss);
										for (i=0; i<imorb; i++)
										{
											pMolecularOrbital=&pChemicalSystem->pmorb[i];
											printf("Orbital %d: spin=%d coeff=%p ",i+1,pMolecularOrbital->spin,pMolecularOrbital->coeff);
											printf("exp=%p nucl=%p type=%p\n",pMolecularOrbital->exp,pMolecularOrbital->nucl,pMolecularOrbital->type);
										}
										freeChemicalMemory(pChemicalSystem);
										if (pChemicalSystem->pmorb!=NULL || pChemicalSystem->pnucl!=NULL)
										{
											printf("Error: although freeChemicalMemory function ends normally, ");
											printf("the chemical structure was not freed up properly.\n");
											printf("Expecting pmorb=NULL pnucl=NULL instead of ");
											printf("pmorb=%p pnucl=%p\n",pChemicalSystem->pmorb,pChemicalSystem->pnucl);
											returnValue=0;
										}
										PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
									}
								}
							}
						}
					}
				}
				else
				{
					pChemicalSystem->nmorb=imorb;
					pChemicalSystem->pmorb=(MolecularOrbital*)calloc(imorb,sizeof(MolecularOrbital));

					pChemicalSystem->nnucl=inucl;
					pChemicalSystem->pnucl=(Nucleus*)calloc(inucl,sizeof(Nucleus));
					
					pChemicalSystem->ngauss=igauss;

					returnValue=1;
					PRINT_TEST_START(counter,expectedValue);
					printf("nmorb=%d pmorb=%p nnucl=%d ",pChemicalSystem->nmorb,pChemicalSystem->pmorb,pChemicalSystem->nnucl);
					printf("pnucl=%p ngauss=%d\n",pChemicalSystem->pnucl,pChemicalSystem->ngauss);
					freeChemicalMemory(pChemicalSystem);
					if (pChemicalSystem->pmorb!=NULL || pChemicalSystem->pnucl!=NULL)
					{
						printf("Error: although freeChemicalMemory function ends normally, the chemical structure was not freed up properly.\n");
						printf("Expecting pmorb=NULL pnucl=NULL instead of pmorb=%p pnucl=%p\n",pChemicalSystem->pmorb,pChemicalSystem->pnucl);
						returnValue=0;
					}
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
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

/* ****************************************************************************************************************************************************************************************** */
// The function freeChemicalMemory free the pointers and memory allocated with calloc (malloc or realloc) for pChemicalSystem (but the all variables except pointers are not reset to zero)
// It has the ChemicalSystem* variable (described in main.h) as input argument and it does not return any argument (void ouput)
/* ****************************************************************************************************************************************************************************************** */
void freeChemicalMemory(ChemicalSystem* pChemicalSystem)
{
	int i=0, iMax=0;
	
	if (pChemicalSystem!=NULL)
	{
		if (pChemicalSystem->pmorb!=NULL)
		{
			iMax=pChemicalSystem->nmorb;
			if (iMax<1)
			{
				printf("\nWarning in freeChemicalMemory function: %d molecular orbitals saved in pChemicalSystem. Some allocated pointers may not have been freed up correctly.\n",iMax);
			}
			for (i=0; i<iMax; i++)
			{
				free(pChemicalSystem->pmorb[i].coeff);	// free function does not return any value (void output)
				pChemicalSystem->pmorb[i].coeff=NULL;

				free(pChemicalSystem->pmorb[i].exp);
				pChemicalSystem->pmorb[i].exp=NULL;

				free(pChemicalSystem->pmorb[i].nucl);
				pChemicalSystem->pmorb[i].nucl=NULL;

				free(pChemicalSystem->pmorb[i].type);
				pChemicalSystem->pmorb[i].type=NULL;
			}
		}

		free(pChemicalSystem->pmorb);
		pChemicalSystem->pmorb=NULL;

		free(pChemicalSystem->pnucl);
		pChemicalSystem->pnucl=NULL;
	}

	return;
}

