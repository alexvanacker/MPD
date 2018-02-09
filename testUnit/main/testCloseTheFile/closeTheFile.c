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

// Testing
void closeTheFile(FILE** pFileToClose);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0; 

	FILE **pFileToClose=NULL, *fileToClose=NULL;

	// Test starts
	time(&startTimer);
	printf("Testing closeTheFile function.\n");

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	printf("pFileToClose=%p\n",pFileToClose);
	closeTheFile(pFileToClose);
	returnValue=1;
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pFileToClose=&fileToClose;
	PRINT_TEST_START(counter,expectedValue);
	printf("*pFileToClose=%p\n",*pFileToClose);
	closeTheFile(pFileToClose);
	returnValue=1;
	if (fileToClose!=NULL)
	{
		returnValue=0;
	}
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	system("touch test.txt");
	PRINT_TEST_START(counter,expectedValue);
	fileToClose=fopen("test.txt","r");
	printf("*pFileToClose=%p\n",*pFileToClose);
	closeTheFile(pFileToClose);
	returnValue=1;
	if (fileToClose!=NULL)
	{
		returnValue=0;
	}
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	fileToClose=fopen("test.txt","w");
	fprintf(fileToClose,"Hello World \n");
	printf("*pFileToClose=%p\n",*pFileToClose);
	closeTheFile(pFileToClose);
	closeTheFile(pFileToClose);
	returnValue=1;
	if (fileToClose!=NULL)
	{
		returnValue=0;
	}
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	system("rm test.txt");
	PRINT_TEST_START(counter,expectedValue);
	printf("*pFileToClose=%p\n",*pFileToClose);
	closeTheFile(pFileToClose);
	returnValue=1;
	if (fileToClose!=NULL)
	{
		returnValue=0;
	}
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	system("touch test.txt");
	PRINT_TEST_START(counter,expectedValue);
	fileToClose=fopen("test.txt","w");
	printf("*pFileToClose=%p\n",*pFileToClose);
	closeTheFile(pFileToClose);
	returnValue=1;
	if (fileToClose!=NULL)
	{
		returnValue=0;
	}
	system("rm test.txt");
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

