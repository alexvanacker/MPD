#include "readWfnFileAndAllocateChemicalSystem.h"

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

#define FREQUENCY_TEST 1000
#define NUMBER_OF_CHARS 2
#define NUMBER_OF_STRINGS 8
#define MAXIMUM_LENGTH_OF_STRINGS 7

/*
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem);
void freeChemicalMemory(ChemicalSystem* pChemicalSystem);
int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);
*/

// Testing
static int readWfnFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	char inserChar[NUMBER_OF_CHARS]={'?','\n'}, insertString[NUMBER_OF_STRINGS][MAXIMUM_LENGTH_OF_STRINGS]={"??","\n??"," 1.2 "," 203 "," 0 "," 0.0 "," -1.0 "," -10 "};
	int i=0, iMax=0, iMaxIni=0, s=0, sizeFile=0, frequencyTest=0;
	ChemicalSystem *pChemicalSystem=NULL, chemicalSystem;
	FILE *wfnFile=NULL, *refFile=NULL;

	// Initializing the chemical structure
	chemicalSystem.nmorb=0;
	chemicalSystem.pmorb=NULL;

	chemicalSystem.nnucl=0;
	chemicalSystem.pnucl=NULL;

	chemicalSystem.ngauss=0;

	// Test starts
	time(&startTimer);
	printf("Testing readWfnFileAndAllocateChemicalSystem function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem("test.wfn",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem("ref.wfn",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem(NULL,pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem("not_here.wfn",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pChemicalSystem=&chemicalSystem;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem(NULL,pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem("not_here.wfn",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	system("touch test.wfn");
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem("test.wfn",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	// First get the size of the reference file and determine the frequency in test loops
	i=0;
	refFile=fopen("ref.wfn","r");
	do {
		readChar=fgetc(refFile);
		i++;
	} while (readChar!='\n' && readChar!=EOF);
	iMaxIni=i;
	do {
		readChar=fgetc(refFile);
		i++;
	} while (readChar!=EOF);
	fclose(refFile);
	sizeFile=i-60;	// Warning: since there is a line after END DATA be carefull where to end so that returnValue does not reach one value (text added after END KEYWORD is not taken into account)
	frequencyTest=sizeFile/FREQUENCY_TEST;

	for (s=0; s<NUMBER_OF_CHARS; s++)
	{
		for (iMax=iMaxIni; iMax<sizeFile; iMax+=frequencyTest)
		{
			i=0;
			refFile=fopen("ref.wfn","r");
			wfnFile=fopen("test.wfn","w+");
			do {
				i++;
				readChar=fgetc(refFile);
				if (readChar!=EOF)
				{
					fputc(readChar,wfnFile);	
				}
			} while (readChar!=EOF && i<iMax);
			fputc(inserChar[s],wfnFile);
			fclose(wfnFile);
			fclose(refFile);	
			PRINT_TEST_START(counter,expectedValue);
			returnValue=readWfnFileAndAllocateChemicalSystem("test.wfn",pChemicalSystem);
			freeChemicalMemory(pChemicalSystem);
			PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);	
		}
	}

	for (s=0; s<NUMBER_OF_STRINGS; s++)
	{
		for (iMax=iMaxIni; iMax<sizeFile; iMax+=frequencyTest)
		{
			i=0;
			refFile=fopen("ref.wfn","r");
			wfnFile=fopen("test.wfn","w+");
			do {
				i++;
				readChar=fgetc(refFile);
				if (readChar!=EOF)
				{
					fputc(readChar,wfnFile);	
				}
			} while (readChar!=EOF && i<iMax);
			fputs(insertString[s],wfnFile);
			do {
				readChar=fgetc(refFile);
				if (readChar!=EOF)
				{
					fputc(readChar,wfnFile);	
				}
			} while (readChar!=EOF);
			fclose(wfnFile);
			fclose(refFile);	
			PRINT_TEST_START(counter,expectedValue);
			returnValue=readWfnFileAndAllocateChemicalSystem("test.wfn",pChemicalSystem);
			freeChemicalMemory(pChemicalSystem);
			PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);	
		}
	}

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=readWfnFileAndAllocateChemicalSystem("ref.wfn",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	system("rm test.wfn");
	printf("\nsizeFile=%d and frequencyTest=%d\n",sizeFile,frequencyTest);

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

/* ************************************************************************************************************************************************************************************************** */
// The function readWfnFileAndAllocateChemicalSystem reads file at fileLocation (file must have been previously created), checks wfn syntax, allocates and fills memory in pChemicalSystem structure
// It has the char* fileLocation and ChemicalSystem* variable (described in main.h) as input arguments and it returns zero if an error occurred, otherwise one
/* ************************************************************************************************************************************************************************************************** */
static int readWfnFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem)
{
	char readStringIn[18]={'\0'}, *readStringOut=NULL;
	int readIntegerIn=0, readIntegerOut=0, readChar=0, i=0, iMax=0, j=0, k=0, kMax=0;
	double readDouble=0.;
	MolecularOrbital *pMolecularOrbital=NULL, *pMolecularOrbitalI=NULL;
	FILE *wfnFile=NULL;

	// Testing if the input pChemicalSystem variable is pointing to NULL otherwise initialize its structure to zero for variables and NULL for pointers
	if (pChemicalSystem==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: the input pChemicalSystem variable is pointing to %p adress.\n",pChemicalSystem);
		return 0;	
	}
	initializeChemicalStructure(pChemicalSystem);

	// Since the file must have been previously created, we check if it is the case 
	if(!initialFileExists(fileLocation))
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: initialFileExists function returned zero instead of one.\n");
		return 0;
	}

	// Testing if the file is well opened (warning: file must have been previously created)
	printf("\nOpening %s file. ",fileLocation);
	wfnFile=fopen(fileLocation,"r");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned
	if (wfnFile==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: could not read %s file.\n",fileLocation);
		return 0;
	}
	printf("Reading and saving chemical data. ");		

	// Skip the first line of the file
	do {
		readChar=fgetc(wfnFile);	// fgetc function returns the character read as an unsigned char cast to an int or EOF on end of file or error (because EOF can't be stored as char)	
	} while (readChar!='\n' && readChar!=EOF);
	if (readChar==EOF)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: problem with fgetc or end-of-file reached without any data entered in %s file.\n",fileLocation);
		closeTheFile(&wfnFile);
		return 0;
	}

	// Read the expected world GAUSSIAN in the second line
	readStringOut=fgets(readStringIn,9,wfnFile);	// fgets function returns a null pointer if it failed, otherwise it returns the same readStringIn parameters
	if (readStringOut==NULL || strcmp(readStringIn,"GAUSSIAN")) // strcmp function returns 0 if the two strings are equal, otherwise >0 (resp. <0) if In is greater (resp. less) than Out
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting GAUSSIAN instead of %s (and fgets returns %p adress).\n",readStringIn,readStringOut);
		closeTheFile(&wfnFile);
		return 0;
	}

	// Reading number of MolecularOrbitals and store it in pChemicalSystem->nmorb
	readIntegerOut=fscanf(wfnFile," %d ",&readIntegerIn);	// fscanf function returns the number of input items successfully matched and assigned, which can be fewer than provided for
	readStringOut=fgets(readStringIn,13,wfnFile);

	// Important remark: white space characters, such as blanks and newline characters, causes fscanf (scanf and sscanf) to read, but not to store, all consecutive white space characters in 
	//                   the input up to the next character that is not white space. One white space character in format-string matches any combination of white space characters in the input.

	if(readIntegerOut!=1 || readStringOut==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fscanf (=%d) and/or fgets (=%p) in Molecular Orbitals.\n",readIntegerOut,readStringOut);
		closeTheFile(&wfnFile);
		return 0;
	}
	if (readIntegerIn<1 || strcmp(readStringIn,"MOL ORBITALS"))
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting Nmorb MOL ORBITALS instead of %d %s (positive value).\n",readIntegerIn,readStringIn);
		closeTheFile(&wfnFile);
		return 0;
	}
	pChemicalSystem->nmorb=readIntegerIn;

	// Allocating memory for MolecularOrbitals and check if it works
	pChemicalSystem->pmorb=(MolecularOrbital*)calloc(readIntegerIn,sizeof(MolecularOrbital));
	if (pChemicalSystem->pmorb==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: could not allocate memory for %d Molecular Orbitals.\n",readIntegerIn);
		closeTheFile(&wfnFile);
		return 0;
	}

	// Reading number of Primitives and store it in pChemicalSystem->ngauss
	readIntegerOut=fscanf(wfnFile," %d ",&readIntegerIn);
	readStringOut=fgets(readStringIn,11,wfnFile);
	if(readIntegerOut!=1 || readStringOut==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fscanf (=%d) and/or fgets (out=%p) in Primitives.\n",readIntegerOut,readStringOut);		
		closeTheFile(&wfnFile);
		return 0;
	}
	if (readIntegerIn<1 || strcmp(readStringIn,"PRIMITIVES"))
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting Ngauss PRIMITIVES instead of %d %s (positive value).\n",readIntegerIn,readStringIn);		
		closeTheFile(&wfnFile);
		return 0;
	}
	pChemicalSystem->ngauss=readIntegerIn;
	iMax=pChemicalSystem->nmorb;
	for (i=0; i<iMax; i++)
	{
		// Initializing MolecularOrbital structure
		pMolecularOrbital=&pChemicalSystem->pmorb[i];
		pMolecularOrbital->spin=0;
		pMolecularOrbital->coeff=NULL;
		pMolecularOrbital->exp=NULL;
		pMolecularOrbital->nucl=NULL;
		pMolecularOrbital->type=NULL;

		// Allocating memory for Primitives in each orbitals and check if it works			
		pMolecularOrbital->coeff=(double*)calloc(readIntegerIn,sizeof(double));
		pMolecularOrbital->exp=(double*)calloc(readIntegerIn,sizeof(double));
		pMolecularOrbital->nucl=(int*)calloc(readIntegerIn,sizeof(int));
		pMolecularOrbital->type=(int*)calloc(readIntegerIn,sizeof(int));
		if (pMolecularOrbital->coeff==NULL || pMolecularOrbital->exp==NULL || pMolecularOrbital->nucl==NULL || pMolecularOrbital->type==NULL)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: could not allocate memory for %d Primitives in %d-th Molecular Orbital.\n",readIntegerIn,i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}	
	}

	// Reading number of Nuclei and store it in pChemicalSystem->nnucl
	readIntegerOut=fscanf(wfnFile," %d ",&readIntegerIn);
	readStringOut=fgets(readStringIn,7,wfnFile);	
	if(readIntegerOut!=1 || readStringOut==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fscanf (=%d) and/or fgets (=%p) in Nuclei.\n",readIntegerOut,readStringOut);		
		closeTheFile(&wfnFile);
		return 0;
	}
	if (readIntegerIn<1 || strcmp(readStringIn,"NUCLEI"))
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting Nnucl NUCLEI instead of %d %s (positive value).\n",readIntegerIn,readStringIn);		
		closeTheFile(&wfnFile);
		return 0;
	}
	pChemicalSystem->nnucl=readIntegerIn;

	// Allocating memory for Nuclei and check if it works
	pChemicalSystem->pnucl=(Nucleus*)calloc(readIntegerIn,sizeof(Nucleus));
	if (pChemicalSystem->pnucl==NULL)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: could not allocate memory for %d Nuclei.\n",readIntegerIn);		
		closeTheFile(&wfnFile);
		return 0;
	}

	// Saving Nuclei in pMesh->pnucl and check if it works
	fscanf(wfnFile," ");
	iMax=readIntegerIn;
	j=0;
	for (i=0; i<iMax; i++)
	{
		// Read the name of the molecule and check its number
		readStringOut=fgets(readStringIn,3,wfnFile);
		readIntegerOut=fscanf(wfnFile," %d (",&readIntegerIn);
		if (readStringOut==NULL || readIntegerOut!=1 || readIntegerIn!=i+1)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fgets (in=%s, out=%p) and/or fscanf (in=%d, out=%d) ",readStringIn,readStringOut,readIntegerIn,readIntegerOut);
			printf("in %d-th Nucleus.\n",i+1);
			closeTheFile(&wfnFile);
			return 0;
		}

		// Read the centre reference and store its number in readIntegerIn
		readStringOut=fgets(readStringIn,7,wfnFile);
		readIntegerOut=fscanf(wfnFile," %d) ",&readIntegerIn);
		if(readStringOut==NULL || readIntegerOut!=1)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fgets (=%p) and/or fscanf (=%d) in %d-th Centre.\n",readStringOut,readIntegerOut,i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}
		if (strcmp(readStringIn,"CENTRE") || readIntegerIn>iMax || readIntegerIn<1)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting CENTRE %d instead of %s %d in %d-th Nucleus ",i+1,readStringIn,readIntegerIn,i+1);
			printf("(positive value less than %d).\n",iMax);		
			closeTheFile(&wfnFile);
			return 0;
		}
		if (readIntegerIn!=i+1)
		{
			printf("\nWarning in readWfnFileAndAllocateChemicalSystem function: Nucleus %d is not numbered %d (in increasing order).\n",readIntegerIn,i+1);
		}
		j+=fscanf(wfnFile,"%lf ",&readDouble);	
		pChemicalSystem->pnucl[readIntegerIn-1].x=readDouble;
		j+=fscanf(wfnFile,"%lf ",&readDouble);	
		pChemicalSystem->pnucl[readIntegerIn-1].y=readDouble;
		j+=fscanf(wfnFile,"%lf ",&readDouble);	
		pChemicalSystem->pnucl[readIntegerIn-1].z=readDouble;

		// Read the number of charge of the current nucleus (not used here)
		readStringOut=fgets(readStringIn,7,wfnFile);
		readIntegerOut=fscanf(wfnFile," = %lf ",&readDouble);
		if(readStringOut==NULL || readIntegerOut!=1)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fgets (=%p) and/or fscanf (=%d) in the charge of %d-th Nucleus.\n",readStringOut,readIntegerOut,i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}
		if (strcmp(readStringIn,"CHARGE") || readDouble>100. || readDouble<1.)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting CHARGE = Rcharge instead of %s = %lf in %d-th Nucleus ",readStringIn,readDouble,i+1);
			printf("(positive value less than 100).\n");		
			closeTheFile(&wfnFile);
			return 0;
		}
	}

	// Check that all coordinates of nuclei have been saved
	if (j!=3*iMax)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting %d Nuclei instead of %d properly saved.\n",iMax,j/3);		
		closeTheFile(&wfnFile);
		return 0;
	}
	
	// Save the centre assignments in pChemicalSystem->nmorb[0].nucl and copy them in all pChemicalSystem->nmorb[i].nucl structure i=1..pChemicalSystem->nmorb-1
	kMax=pChemicalSystem->ngauss;
	pMolecularOrbital=&pChemicalSystem->pmorb[0];
	j=0;
	for (k=0; k<kMax; k++)
	{
		readChar=fgetc(wfnFile);
		if (readChar==EOF)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: problem with fgetc or end-of-file reached without all data saved in %d-th Centre Assignment.\n",k+1);
			closeTheFile(&wfnFile);
			return 0;
		}
		else if (readChar=='C')
		{
			readStringOut=fgets(readStringIn,18,wfnFile);
			if(readStringOut==NULL || strcmp(readStringIn,"ENTRE ASSIGNMENTS"))	
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: expecting CENTRE ASSIGNMENTS instead of C%s (and fgets returns %p adress) ",readStringIn,readStringOut);
				printf("in %d-th Centre Assignment.\n",k+1);		
				closeTheFile(&wfnFile);
				return 0;
			}
			fscanf(wfnFile," ");
		}
		else
		{
			if(fseek(wfnFile,-1,SEEK_CUR)) 
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fseek function in %d-th Centre Assignment.\n",k+1);
				closeTheFile(&wfnFile);
				return 0;
			}
		}
		j+=fscanf(wfnFile,"%d ",&readIntegerOut);
		if (readIntegerOut<1 || readIntegerOut>iMax)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting positive integer less than %d instead of %d in %d-th Centre Assignment.\n",iMax,readIntegerOut,k+1);
			closeTheFile(&wfnFile);
			return 0;
		}
		pMolecularOrbital->nucl[k]=readIntegerOut;
	}
	if (j!=kMax)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting %d Centre Assignments instead of %d properly saved.\n",kMax,j);		
		closeTheFile(&wfnFile);
		return 0;
	}
	iMax=pChemicalSystem->nmorb;
	if (iMax>1)
	{
		for (i=1; i<iMax; i++)
		{
			pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
			for (k=0; k<kMax; k++)
			{
				pMolecularOrbitalI->nucl[k]=pMolecularOrbital->nucl[k];
			}
		}
	}

	// Save the type assignments in pChemicalSystem->nmorb[0].type and copy them in all pChemicalSystem->nmorb[i].type structure i=1..pChemicalSystem->nmorb-1
	kMax=pChemicalSystem->ngauss;
	pMolecularOrbital=&pChemicalSystem->pmorb[0];
	j=0;
	for (k=0; k<kMax; k++)
	{
		readChar=fgetc(wfnFile);
		if (readChar==EOF)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: problem with fgetc or end-of-file reached without all data saved in %d-th Type Assignment.\n",k+1);
			closeTheFile(&wfnFile);
			return 0;
		}
		else if (readChar=='T')
		{
			readStringOut=fgets(readStringIn,16,wfnFile);
			if(readStringOut==NULL || strcmp(readStringIn,"YPE ASSIGNMENTS"))	
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem:  expecting TYPE ASSIGNMENTS instead of T%s (and fgets returns %p adress) ",readStringIn,readStringOut);
				printf("in %d-th Type Assignment.\n",k+1);		
				closeTheFile(&wfnFile);
				return 0;
			}
			fscanf(wfnFile," ");
		}
		else
		{
			if(fseek(wfnFile,-1,SEEK_CUR))
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fseek function in %d-th Type Assignment.\n",k+1);
				closeTheFile(&wfnFile);
				return 0;
			}
		}
		j+=fscanf(wfnFile,"%d ",&readIntegerOut);
		if (readIntegerOut<1 || readIntegerOut>20)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting positive integer less than 20 instead of %d in %d-th Type Assignment.\n",readIntegerOut,k+1);
			closeTheFile(&wfnFile);
			return 0;
		}		
		pMolecularOrbital->type[k]=readIntegerOut;
	}
	if (j!=kMax)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting %d Type Assignements instead of %d properly saved.\n",kMax,j);		
		closeTheFile(&wfnFile);
		return 0;
	}
	iMax=pChemicalSystem->nmorb;
	if (iMax>1)
	{
		for (i=1; i<iMax; i++)
		{
			pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
			for (k=0; k<kMax; k++)
			{
				pMolecularOrbitalI->type[k]=pMolecularOrbital->type[k];
			}
		}
	}

	// Save the exponents in pChemicalSystem->nmorb[0].exp and copy them in all pChemicalSystem->nmorb[i].exp structure i=1..pChemicalSystem->nmorb-1
	kMax=pChemicalSystem->ngauss;
	pMolecularOrbital=&pChemicalSystem->pmorb[0];
	j=0;
	for (k=0; k<kMax; k++)
	{
		readChar=fgetc(wfnFile);
		if (readChar==EOF)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: problem with fgetc or end-of-file reached without all data saved in %d-th Exponent.\n",k+1);
			closeTheFile(&wfnFile);
			return 0;
		}
		else if (readChar=='E')
		{
			readStringOut=fgets(readStringIn,9,wfnFile);
			if(readStringOut==NULL || strcmp(readStringIn,"XPONENTS"))	
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: expecting EXPONENTS instead of E%s (and fgets returns %p adress) ",readStringIn,readStringOut);
				printf("in %d-th Exponent.\n",k+1);		
				closeTheFile(&wfnFile);
				return 0;
			}
			fscanf(wfnFile," ");
		}
		else
		{
			if(fseek(wfnFile,-1,SEEK_CUR))
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fseek function in %d-th Exponent.\n",k+1);
				closeTheFile(&wfnFile);
				return 0;
			}
		}
		j+=fscanf(wfnFile,"%lf ",&readDouble);
		if (readDouble<=0.)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting positive value instead of %lf in %d-th Exponent.\n",readDouble,k+1);
			closeTheFile(&wfnFile);
			return 0;
		}		
		pMolecularOrbital->exp[k]=readDouble;
	}
	if (j!=kMax)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting %d Exponents instead of %d properly saved.\n",kMax,j);		
		closeTheFile(&wfnFile);
		return 0;
	}
	iMax=pChemicalSystem->nmorb;
	if (iMax>1)
	{
		for (i=1; i<iMax; i++)
		{
			pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
			for (k=0; k<kMax; k++)
			{
				pMolecularOrbitalI->exp[k]=pMolecularOrbital->exp[k];
			}
		}
	}

	// Save in pChemicalSystem->nmorb[i-1].coeff i=1..pChemicalSystem->nmorb, the different primitives coefficients associated with the different molecular orbitals 
	for (i=0; i<iMax; i++)
	{
		// Read the first line associated with the i-th orbital
		readStringOut=fgets(readStringIn,3,wfnFile);
		if(readStringOut==NULL || strcmp(readStringIn,"MO"))	
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting MO instead of %s (and fgets returns %p adress) ",readStringIn,readStringOut);
			printf("in %d-th Molecular Orbital.\n",i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}
		readIntegerOut=fscanf(wfnFile," %d ",&readIntegerIn);
		if (readIntegerOut!=1 || readIntegerIn<1 || readIntegerIn>iMax)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fscanf (in=%d, out=%d) in %d-th Molecular Orbital ",readIntegerIn,readIntegerOut,i+1);
			printf("(positive value less than %d.\n",iMax);		
			closeTheFile(&wfnFile);
			return 0;
		}
		if (readIntegerIn!=i+1)
		{
			printf("\nWarning in readWfnFileAndAllocateChemicalSystem function: Molecular Orbital %d is not numbered %d (in increasing order).\n",readIntegerIn,i+1);	
		}
		readStringOut=fgets(readStringIn,3,wfnFile);
		readIntegerOut=fscanf(wfnFile," %lf ",&readDouble);
		if(readStringOut==NULL || strcmp(readStringIn,"MO") || readIntegerOut!=1 || readDouble!=0.)	
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting MO 0.0 instead of %s %lf ",readStringIn,readDouble);
			printf("(and fgets returns %p adress while fscanf returns %d) in %d-th Molecular Orbital (zero value).\n",readStringOut,readIntegerOut,i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}
		readStringOut=fgets(readStringIn,7,wfnFile);
		if(readStringOut==NULL || strcmp(readStringIn,"OCC NO"))	
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting OCC NO instead of %s (and fgets returns %p adress) ",readStringIn,readStringOut);
			printf("in %d-th Molecular Orbital.\n",i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}
		readIntegerOut=fscanf(wfnFile," = %lf ",&readDouble);
		if (readIntegerOut!=1)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: wrong return of fscanf (=%d) in occupation number of %d-th Molecular Orbital.\n",readIntegerOut,i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}

		// Temporarily save the occupation number in pChemicalSystem->pmorb[readIntegerIn-1].spin to potentially duplicate the Molecular Orbital (2 spins) later in chemicalSystem.txt file
		pMolecularOrbitalI=&pChemicalSystem->pmorb[readIntegerIn-1];
		if (readDouble==2.)
		{
			pMolecularOrbitalI->spin=2;
		}
		else if (readDouble==1.)
		{
			pMolecularOrbitalI->spin=1;
		}
		else
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting 1.0 or 2.0 instead of %lf as occupation number in %d-th Molecular Orbital.\n",readDouble,readIntegerIn);		
			closeTheFile(&wfnFile);
			return 0;
		}
		readStringOut=fgets(readStringIn,12,wfnFile);
		readIntegerOut=fscanf(wfnFile," = %lf ",&readDouble);
		if(readStringOut==NULL || strcmp(readStringIn,"ORB. ENERGY") || readIntegerOut!=1)	
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting ORB. ENERGY = value instead of %s = %lf ",readStringIn,readDouble);
			printf("(and fgets returns %p adress while fscanf return %d) in %d-th Molecular Orbital.\n",readStringOut,readIntegerOut,i+1);		
			closeTheFile(&wfnFile);
			return 0;
		}

		// Save the coefficients of the primitives in pMolecularOrbitalI->coeff[k-1] k=1...pChemicalSystem->ngauss
		kMax=pChemicalSystem->ngauss;
		j=0;
		for (k=0; k<kMax; k++)
		{
			j+=fscanf(wfnFile,"%lf ",&readDouble);
			pMolecularOrbitalI->coeff[k]=readDouble;
		}
		if (j!=kMax)
		{
			PRINT_ERROR();
			printf("In readWfnFileAndAllocateChemicalSystem: expecting %d Primitive instead of %d properly saved in %d-th Molecular Orbital.\n",kMax,j,readIntegerIn);		
			closeTheFile(&wfnFile);
			return 0;
		}
	}

	// Check that the END DATA keyword follows (transformed into ENe eATA for reading scientific notation by readAndConvertFile function)
	readStringOut=fgets(readStringIn,9,wfnFile);
	if(readStringOut==NULL || strcmp(readStringIn,"ENe eATA"))	
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: expecting ENe eATA instead of %s (and fgets returns %p adress) for the END DATA keyword.\n",readStringIn,readStringOut);		
		closeTheFile(&wfnFile);
		return 0;
	}
	
	// Testing if file is well closed
	if(fclose(wfnFile))		// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		PRINT_ERROR();
		printf("In readWfnFileAndAllocateChemicalSystem: could not close %s file properly.\n",fileLocation);
		wfnFile=NULL;
		return 0;
	}
	printf("Closing file.\n");
	wfnFile=NULL;	

	// Check the number of Molecular Orbitals that have a spin number equals to two, and duplicate them in the pChemicalSystem structure (if the ORB_SPIN parameter is set to 1, see loadChemistry.h)
	iMax=pChemicalSystem->nmorb;
	if (ORB_SPIN)
	{
		printf("Duplicating Molecular Orbitals whose occupation number equals two.\n");
		// First count the number of Molecular Orbitals that have an occupation number equals to two (saved in the spin variable for the moment) 
		readIntegerIn=0;
		for (i=0; i<iMax; i++)
		{
			pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
			if (pMolecularOrbitalI->spin==2)
			{
				readIntegerIn++;
			}
		}

		// Allocating memory for the extra molecular orbitals and checks if it works (realloc function returns a pointer to the newly allocated memory, or NULL if the request fails)
		if (readIntegerIn>0)
		{
			pMolecularOrbital=(MolecularOrbital*)realloc(pChemicalSystem->pmorb,(iMax+readIntegerIn)*sizeof(MolecularOrbital));
			if (pMolecularOrbital==NULL)
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: could not reallocate memory for %d Molecular Orbitals.\n",readIntegerIn+iMax);		
				return 0;
			}
			pChemicalSystem->pmorb=(MolecularOrbital*)pMolecularOrbital;

			// Update the new number of Molecular Orbitals
			pChemicalSystem->nmorb=iMax+readIntegerIn;

			// Duplicate the Molecular Orbitals that have an occupation number equals to two
			readIntegerIn=iMax-1;
			kMax=pChemicalSystem->ngauss;
			for (i=0; i<iMax; i++)
			{
				pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
				if (pMolecularOrbitalI->spin==2)
				{
					pMolecularOrbitalI->spin=1;
					readIntegerIn++;
					// Initializing MolecularOrbital structure
					pMolecularOrbital=&pChemicalSystem->pmorb[readIntegerIn];
					pMolecularOrbital->spin=0;
					pMolecularOrbital->coeff=NULL;
					pMolecularOrbital->exp=NULL;
					pMolecularOrbital->nucl=NULL;
					pMolecularOrbital->type=NULL;

					// Change the spin of the duplicated Molecular Orbital
					pMolecularOrbital->spin=-pMolecularOrbitalI->spin;

					// Allocating memory for Primitives of the duplicated Molecular Orbital	
					pMolecularOrbital->coeff=(double*)calloc(kMax,sizeof(double));
					pMolecularOrbital->exp=(double*)calloc(kMax,sizeof(double));
					pMolecularOrbital->nucl=(int*)calloc(kMax,sizeof(int));
					pMolecularOrbital->type=(int*)calloc(kMax,sizeof(int));
					// Check if the allocation worked
					if (pMolecularOrbital->coeff==NULL || pMolecularOrbital->exp==NULL || pMolecularOrbital->nucl==NULL || pMolecularOrbital->type==NULL)
					{
						PRINT_ERROR();
						printf("In readWfnFileAndAllocateChemicalSystem: could not reallocate memory for %d Primitives in %d-th Molecular Orbital.\n",kMax,i+1);		
						return 0;
					}
					for (k=0; k<kMax; k++)
					{
						pMolecularOrbital->coeff[k]=pMolecularOrbitalI->coeff[k];
						pMolecularOrbital->exp[k]=pMolecularOrbitalI->exp[k];
						pMolecularOrbital->nucl[k]=pMolecularOrbitalI->nucl[k];
						pMolecularOrbital->type[k]=pMolecularOrbitalI->type[k];
					}
				}
				else if (pMolecularOrbitalI->spin==1)
				{
					continue;
				}
				else
				{
					PRINT_ERROR();
					printf("In readWfnFileAndAllocateChemicalSystem: expecting 1 or 2 occupation number instead of %d ",pMolecularOrbitalI->spin);
					printf("in %d-th Molecular Orbital.\n",i+1);		
					return 0;
				}
			}	
		}		
	}
	else
	{
		printf("Warning in readWfnFileAndAllocateChemicalSystem function: not duplicating Molecular Orbitals whose occupation number equals two.\n");
		for (i=0; i<iMax; i++)
		{
			pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
			if (pMolecularOrbitalI->spin==2)
			{
				pMolecularOrbitalI->spin=1;
			}
			else if (pMolecularOrbitalI->spin==1)
			{
				continue;
			}
			else
			{
				PRINT_ERROR();
				printf("In readWfnFileAndAllocateChemicalSystem: expecting 1 or 2 occupation number instead of %d in %d-th Molecular Orbital.\n",pMolecularOrbitalI->spin,i+1);		
				return 0;
			}
		}
	}

	return 1;
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

