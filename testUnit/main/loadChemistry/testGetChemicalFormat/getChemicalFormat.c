#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

// Macro function to initialize diagnostic of an error
#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

int initialFileExists(char* fileLocation);

// Testing 
static int getChemicalFormat(char* fileLocation);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	// Test starts
	time(&startTimer);
	printf("Testing getChemicalFormat function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=getChemicalFormat(NULL);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=getChemicalFormat("not_here.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	system("touch wfn");
	PRINT_TEST_START(counter,expectedValue);
	returnValue=getChemicalFormat("wfn");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm wfn");

	system("touch test.txt");
	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=getChemicalFormat("test.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm test.txt");

	system("touch test.wfn");
	expectedValue=-1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=getChemicalFormat("test.wfn");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm test.wfn");

	system("touch test.wwfn");
	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=getChemicalFormat("test.wwfn");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm test.wwfn");

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

/* ********************************************************************************************************************************************************************** */
// The function getChemicalFormat determines if the file located at fileLocation has the *.wfn format or the *.txt format concerning the chemical data   
// It has the char* fileLocation variable as input argument, and it return zero if an error occurs, otherwise one (resp. -1) is returned for *.txt (resp. *.wfn) format 
/* ********************************************************************************************************************************************************************** */
static int getChemicalFormat(char* fileLocation)
{
	size_t nameLength=0;
	int returnValue=0;
	
	// First test if the file exists at fileLocation
	if (!initialFileExists(fileLocation))
	{
		PRINT_ERROR();
		printf("In getChemicalFormat: initialFileExists function returned zero instead of one.\n");
		return returnValue;	
	}

	// Get the length of the name and check its size
	nameLength=strlen(fileLocation);	// strlen function returns the length of the string fileLocation, but not including the terminating nul character '\0'
	if (nameLength<=5)
	{
		PRINT_ERROR();
		printf("In getChemicalFormat: the input FileLocation variable (%s) should be at least of size 4 ",fileLocation);
		printf("instead of %d since it must end at least with .wfn or .txt format.\n",(int)nameLength);
		return returnValue;
	}

	// Distinguish the *.wfn from *.txt format
	if (fileLocation[nameLength-4]=='.' && fileLocation[nameLength-3]=='w' &&fileLocation[nameLength-2]=='f' && fileLocation[nameLength-1]=='n' && fileLocation[nameLength]=='\0')
	{
		returnValue=-1;	
	}
	else if (fileLocation[nameLength-4]=='.' && fileLocation[nameLength-3]=='t' &&fileLocation[nameLength-2]=='x' && fileLocation[nameLength-1]=='t' && fileLocation[nameLength]=='\0')
	{
		returnValue=1;	
	}
	else
	{
		PRINT_ERROR();
		printf("In getChemicalFormat: the input FileLocation variable (%s) does not end with .wfn or .txt format.\n", fileLocation);
		returnValue=0;
	}

	return returnValue;
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

