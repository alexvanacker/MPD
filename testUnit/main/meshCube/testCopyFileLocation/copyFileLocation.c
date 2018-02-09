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

#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

int initialFileExists(char* fileLocation);

// Testing
static int copyFileLocation(char* fileLocation, char* fileLocationForCopy);

int main (void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0; 

	char nameFile[150]="azertyuiopmlkjhgfdsqwxcvbnopmlmlkjhgfdsqazertyuiopmlkjhgfdsqazért-yèuiiiiklmjgreeeeesdgùmiyhjjjjgthyuuuuu";
	FILE *txtFile=NULL;

	// Test starts
	time(&startTimer);
	printf("Testing copyFileLocation function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation(NULL,NULL);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation(NULL,"testCopy.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt",NULL);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt","testCopy.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt","test.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	txtFile=fopen("test.txt","w+");
	fputs("Hello world!\n",txtFile);
	fclose(txtFile);
	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation(NULL,NULL);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation(NULL,"testCopy.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt",NULL);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt","test.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt","testCopy.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("testCopy.txt","test.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	
	system("touch azertyuiopmlkjhgfdsqwxcvbnopmlmlkjhgfdsqazertyuiopmlkjhgfdsqazért-yèuiiiiklmjgreeeeesdgùmiyhjjjjgthyuuuuu");
	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt",nameFile);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation(nameFile,"not_here.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm azertyuiopmlkjhgfdsqwxcvbnopmlmlkjhgfdsqazertyuiopmlkjhgfdsqazért-yèuiiiiklmjgreeeeesdgùmiyhjjjjgthyuuuuu");

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation("test.txt",nameFile);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=copyFileLocation(nameFile,"testCopy.txt");
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm test.txt testCopy.txt");

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

/* ********************************************************************************************************************************************************** */
// The function copyFileLocation tries to copy a file (using system function) located at fileLocation into the fileLocationForCopy (must not already exist)
// It has two char* variables (fileLocation, fileLocationForCopy) variables as input arguments and it returns one on success, otherwise zero 
/* ********************************************************************************************************************************************************** */
static int copyFileLocation(char* fileLocation, char* fileLocationForCopy)
{
	char lineCommand[205]={'\0'};
	int lengthTest=0;

	// Test if the input fileLocation variable refers to a valid file
	if (!initialFileExists(fileLocation))
	{
		PRINT_ERROR();
		printf("In copyFileLocation: initialFileExists function returned zero instead of one.\n");
		return 0;
	}

	// Test if the input fileLocationForCopy is not a NULL pointer
	if (fileLocationForCopy==NULL)
	{
		PRINT_ERROR();
		printf("In copyFileLocation: fileLocationForCopy input variable is pointing to %p adress.\n",fileLocationForCopy);
		return 0;
	}

	// Test if the input fileLocationForCopy variable does not already refer to a valid file (if the two input variables fileLocation and fileLocationForCopy are identical, an error is displayed here)
	if (initialFileExists(fileLocationForCopy))
	{
		PRINT_ERROR();
		printf("In copyFileLocation: %s file already exists and may be overwritten when copying %s file.\n",fileLocationForCopy,fileLocation);
		return 0;
	}

	// Test if the input fileLocation variable has not a length greater than 100
	lengthTest=strlen(fileLocation);	// strlen function returns the length of the string fileLocation, but not including the terminating nul character '\0'
	if (lengthTest<1 || lengthTest>100)
	{
		PRINT_ERROR();
		printf("In copyFileLocation: the length %d of string %s is not positive or exceeds 100 which is the maximum length allowed here.\n",lengthTest,fileLocation);
		return 0;
	}

	// Test if the input fileLocationForCopy variable has not a length greater than 100
	lengthTest=strlen(fileLocationForCopy);
	if (lengthTest<1 || lengthTest>100)
	{
		PRINT_ERROR();
		printf("In copyFileLocation: the length %d of string %s is not positive or exceeds 100 which is the maximum length allowed here.\n",lengthTest,fileLocationForCopy);
		return 0;
	}

	// Build the line command "cp fileLocation fileLocationForCopy" (input of system function)
	strcpy(lineCommand,"cp ");
	strcat(lineCommand,fileLocation);
	lengthTest=strlen(lineCommand);
	lineCommand[lengthTest+1]='\0';
	lineCommand[lengthTest]=' ';
	strcat(lineCommand,fileLocationForCopy);
	printf("%s\n",lineCommand);
	system(lineCommand);

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

