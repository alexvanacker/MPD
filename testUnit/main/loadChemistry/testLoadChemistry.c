#include "loadChemistry.h"
#include "testLoadChemistry.h"

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

/*
int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);
*/

// Testing int loadChemistry(ChemicalSystem *pChemicalSystem, int nuElectrons);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int nuElectrons=0;
	ChemicalSystem *pChemicalSystem=NULL, chemicalSystem;

	// Initializing the chemical structure
	chemicalSystem.nmorb=0;
	chemicalSystem.pmorb=NULL;

	chemicalSystem.nnucl=0;
	chemicalSystem.pnucl=NULL;

	chemicalSystem.ngauss=0;

	// Test starts
	time(&startTimer);
	printf("Testing loadChemistry function.\n");

	for (nuElectrons=-1; nuElectrons<10; nuElectrons++)
	{
		printf("nuElectrons=%d\n",nuElectrons);
		pChemicalSystem=NULL;

		expectedValue=0;
		PRINT_TEST_START(counter,expectedValue);
		returnValue=loadChemistry(NULL,pChemicalSystem,nuElectrons);
		freeChemicalMemory(pChemicalSystem);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

		PRINT_TEST_START(counter,expectedValue);
		returnValue=loadChemistry("not_here.wfn",pChemicalSystem,nuElectrons);
		freeChemicalMemory(pChemicalSystem);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

		system("cp ../inputFiles/chemicalSystem.wfn test.wfn");
		PRINT_TEST_START(counter,expectedValue);
		returnValue=loadChemistry("test.wfn",pChemicalSystem,nuElectrons);
		freeChemicalMemory(pChemicalSystem);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

		pChemicalSystem=&chemicalSystem;
		expectedValue=1;
#if (ORB_SPIN)
		if (nuElectrons<1||nuElectrons>8)
		{
			expectedValue=0;
		}
#else
		if (nuElectrons<1||nuElectrons>4)
		{
			expectedValue=0;
		}		
#endif
		PRINT_TEST_START(counter,expectedValue);
		returnValue=loadChemistry("test.wfn",pChemicalSystem,nuElectrons);		
		freeChemicalMemory(pChemicalSystem);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

		expectedValue=1;
#if (ORB_SPIN)
		if (nuElectrons<1||nuElectrons>8)
		{
			expectedValue=0;
		}
#else
		if (nuElectrons<1||nuElectrons>4)
		{
			expectedValue=0;
		}		
#endif
		PRINT_TEST_START(counter,expectedValue);
		returnValue=loadChemistry("chemicalOut.txt",pChemicalSystem,nuElectrons);
		freeChemicalMemory(pChemicalSystem);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
		system("rm chemicalOut.txt");
		system("rm test.wfn");
	
		expectedValue=0;
		PRINT_TEST_START(counter,expectedValue);
		returnValue=loadChemistry("test.wfn",pChemicalSystem,nuElectrons);
		freeChemicalMemory(pChemicalSystem);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
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

/* **************************************************************************************************** */
// The function initialFileExists checks if a file exists at fileLocation (you need to add string.h) 
// It has the char* fileLocation variable as input and returns one if the file exists otherwise zero
/* **************************************************************************************************** */
int initialFileExists(char* fileLocation)
{
	FILE *nameFile=NULL;
	
	if (fileLocation==NULL)
	{	
		printf("\nWarning in initialFileExists function: the input file is pointing to %p adress.\n",fileLocation);
		return 0;
	}
	nameFile=fopen(fileLocation,"r");	// fopen function returns a FILE pointer. Otherwise, NULL is returned (file must have been previously created in reading mode)
	if (nameFile==NULL)
	{
		printf("\nWarning in initialFileExists function: could not find %s file.\n",fileLocation);
		return 0;
	}
	if(fclose(nameFile))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		printf("\nWarning in initialFileExists function: could not close %s file properly.\n",fileLocation);
		nameFile=NULL;
		return 0;
	}
	nameFile=NULL;

	return 1;
}

/* *************************************************************************************************************************** */
// The function closeTheFile tries to properly close the FILE* variable via its associated FILE** pointer named pFileToClose 
// It has the FILE** variable (don't forget to add string.h) as input argument and do not return any value (void output)
/* *************************************************************************************************************************** */
void closeTheFile(FILE** pFileToClose)
{
	if (pFileToClose!=NULL)
	{
		if (*pFileToClose!=NULL)
		{
			// Testing closure of the file
			if(fclose(*pFileToClose))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
			{
				printf("\nWarning in closeTheFile function: in addition, we could not close the file properly.\n");	
			}
			else
			{	
				printf("Closing file.\n");
			}
			*pFileToClose=NULL;
		}
	}	

	return;
}

