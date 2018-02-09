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
void closeTheFile(FILE** pFileToClose);

// Testing
static int readAndConvertWfnFile(char* fileLocation, char charToRemove, char charToPutInstead);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0; 

	// Test starts
	time(&startTimer);
	printf("Testing readAndConvertWfnFile function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readAndConvertWfnFile(NULL,'\0','a');
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readAndConvertWfnFile("notHere.wfn",'D','e');
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readAndConvertWfnFile("test.txt",'D','e');
	printf("In test.txt, does all 'D' character were converted into 'e' character (y/n)? \n");
	readChar=getchar();
	if (readChar!='y')
	{
		expectedValue=0;
	}
	while (readChar!='\n' && readChar!=EOF)
	{	
		readChar=getchar(); 	
	} 
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readAndConvertWfnFile("test.txt",'e','D');
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readAndConvertWfnFile("test2.txt",'q','t');
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

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

/* ****************************************************************************************************************************************************************************************************** */
// The function readAndConvertWfnFile reads a *.wfn file at fileLocation (file must have been previously created), except the first line, and converts all charToRemove encountered into charToPutInstead
// It has a char* variable (fileLocation) and two char variables (charToRemove, charToPutInstead) as input arguments and it returns zero if an error occurred, otherwise one
/* ****************************************************************************************************************************************************************************************************** */
static int readAndConvertWfnFile(char* fileLocation, char charToRemove, char charToPutInstead)
{
	int readChar=0;
	FILE *wfnFile=NULL;

	// Since the file must have been previously created, we check if it is the case 
	if(!initialFileExists(fileLocation))
	{
		PRINT_ERROR();
		printf("In readAndConvertWfnFile: initialFileExists function returned zero instead of one.\n");
		return 0;
	}

	// Testing if the file is well opened (warning: file must have been previously created)
	printf("\nOpening %s file. ",fileLocation);
	wfnFile=fopen(fileLocation,"r+");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned
	if (wfnFile==NULL)
	{
		PRINT_ERROR();
		printf("In readAndConvertWfnFile: could not read and/or write in %s file.\n",fileLocation);
		return 0;
	}
	printf("Converting data (any '%c' into '%c'). ",charToRemove,charToPutInstead);
	// Skip the first line
	do {
		readChar=fgetc(wfnFile);	// fgetc function returns the character read as an unsigned char cast to an int or EOF on end of file or error (because EOF can't be stored as char)
	} while (readChar!='\n' && readChar!=EOF);
	if (readChar==EOF)
	{
		PRINT_ERROR();
		printf("In readAndConvertWfnFile: problem with fgetc or end-of-file reached without any data written in %s file.\n",fileLocation);
		closeTheFile(&wfnFile);
		return 0;
	}
	// Remark:'D' symbol now only appears in *.wfn at fileLocation either for scientific notation (Fortran 1.0D+01) or for the END DATA keyword so it can be easily converted into C syntax (1.0e+01)
	do {
		readChar=fgetc(wfnFile);	
		if (readChar==(int)charToRemove)
		{
			if(fseek(wfnFile,-1,SEEK_CUR)) // fseek function returns zero on success, otherwise a non-zero value
			{
				PRINT_ERROR();
				printf("In readAndConvertWfnFile: wrong return of fseek function in %s file.\n",fileLocation);
				closeTheFile(&wfnFile);
				return 0;
			}
			if(fputc(charToPutInstead,wfnFile)==EOF) // fputc function returns the same character on success, otherwise EOF value (end of file)
			{
				PRINT_ERROR();
				printf("In readAndConvertWfnFile: wrong return of fputc function in %s file.\n",fileLocation);
				closeTheFile(&wfnFile);
				return 0;
			}
		}
	} while (readChar!=EOF);
	// Testing if file is well closed
	if(fclose(wfnFile))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		PRINT_ERROR();
		printf("In readAndConvertWfnFile: could not close %s file properly.\n",fileLocation);
		wfnFile=NULL;
		return 0;
	}
	wfnFile=NULL;
	printf("Closing file.\n");

	return 1;
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

