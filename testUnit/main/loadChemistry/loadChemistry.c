#include "loadChemistry.h"

/*
// Prototypes of non-static functions in loadChemistry.c
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem);
void freeChemicalMemory(ChemicalSystem* pChemicalSystem);
int loadChemistry(char* fileLocation, ChemicalSystem *pChemicalSystem, int nuElectrons);
*/

// Prototypes of static functions in loadChemistry.c
static int getChemicalFormat(char* fileLocation);
static int readAndConvertWfnFile(char* fileLocation, char charToRemove, char charToPutInstead);
static int readWfnFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem);
static int writingChemicalFile(char* fileLocation, ChemicalSystem* pChemicalSystem);
static int readTxtFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem);

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

/* *********************************************************************************************************************************************************************************************** */
// The function writingChemicalFile writes data stored in the pChemicalSystem structure into a file located at fileLocation (warning: reset and overwrite file at fileLocation if already exists)
// It has the char* fileLocation and the ChemicalSystem* variable (described in main.h) as input arguments and it returns zero if an error occurred, otherwise one
/* *********************************************************************************************************************************************************************************************** */
static int writingChemicalFile(char* fileLocation, ChemicalSystem* pChemicalSystem)
{
	int i=0, j=0, numberOfMolecularOrbitals=0, numberOfNuclei=0, numberOfPrimitives=0;
	MolecularOrbital *pMolecularOrbital=NULL;
	FILE *chemicalFile=NULL;

	// Testing if the input pChemicalSystem variable is pointing to NULL
	if (pChemicalSystem==NULL)
	{
		PRINT_ERROR();
		printf("In writingChemicalFile: the input pChemicalSystem variable is pointing to %p adress.\n",pChemicalSystem);
		return 0;	
	}
	numberOfMolecularOrbitals=pChemicalSystem->nmorb; 
	numberOfNuclei=pChemicalSystem->nnucl;
	numberOfPrimitives=pChemicalSystem->ngauss;

	// Testing if the input fileLocation variable is pointing to NULL
	if (fileLocation==NULL)
	{	
		PRINT_ERROR();
		printf("In writingChemicalFile: the input fileLocation variable is pointing to %p adress.\n",fileLocation);
		return 0;
	}

	// Test if file is well opened (warning: reset and overwrite file at fileLocation if already exists)
	printf("\nOpening %s file. ",fileLocation);
	chemicalFile=fopen(fileLocation,"w+");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned
	if (chemicalFile==NULL)
	{
		PRINT_ERROR();
		printf("In writingChemicalFile: could not write in %s file.\n",fileLocation);
		return 0;
	}
	printf("Writing chemical data. ");

	// Writing file at fileLocation. Start with number of MolecularOrbitals
	if (numberOfMolecularOrbitals<1)
	{
		PRINT_ERROR();
		printf("In writingChemicalFile: we cannot have %d Molecular Orbitals.\n",numberOfMolecularOrbitals);
		fprintf(chemicalFile,"\nEnd");
		closeTheFile(&chemicalFile);
		return 0;
	}
	fprintf(chemicalFile,"MolecularOrbitals %d\n",numberOfMolecularOrbitals);

	// Writing number of Primitives	
	if (numberOfPrimitives<1)
	{
		PRINT_ERROR();
		printf("In writingChemicalFile: we cannot have %d Primitives.\n",numberOfPrimitives);
		fprintf(chemicalFile,"\nEnd");
		closeTheFile(&chemicalFile);
		return 0;
	}
	fprintf(chemicalFile,"\nPrimitives %d\n",numberOfPrimitives);

        // Number and description of Nuclei
	if (numberOfNuclei<1)
	{
		PRINT_ERROR();
		printf("In writingChemicalFile: we cannot have %d Nuclei.\n",numberOfNuclei);
		fprintf(chemicalFile,"\nEnd");
		closeTheFile(&chemicalFile);
		return 0;
	}
	fprintf(chemicalFile,"\nNuclei\n%d\n",numberOfNuclei);
	for (i=0; i<numberOfNuclei; i++)
	{
		fprintf(chemicalFile,"%le %le %le \n",pChemicalSystem->pnucl[i].x,pChemicalSystem->pnucl[i].y,pChemicalSystem->pnucl[i].z);
	}

	// Description of Orbitals
	fprintf(chemicalFile,"\nCoefficient Exponent Center Type");
	for (i=0; i<numberOfMolecularOrbitals; i++)
	{
		fprintf(chemicalFile,"\n%d\n",i+1);
		pMolecularOrbital=&pChemicalSystem->pmorb[i];
		if (abs(pMolecularOrbital->spin)!=1)
		{
			PRINT_ERROR();
			printf("In writingChemicalFile: Spin should be (+/-) 1 instead of %d value.\n",pMolecularOrbital->spin);
			fprintf(chemicalFile,"\nEnd");
			closeTheFile(&chemicalFile);
			return 0;
		}
		fprintf(chemicalFile,"Spin %d\n",pMolecularOrbital->spin);

		// Check if no error occurs for the pointers in pMolecularOrbital structure
		if (pMolecularOrbital->coeff==NULL || pMolecularOrbital->exp==NULL || pMolecularOrbital->nucl==NULL || pMolecularOrbital->type==NULL)
		{
			PRINT_ERROR();
			printf("In writingChemicalFile: coeff=%p exp=%p nucl=%p or type=%p ",pMolecularOrbital->coeff,pMolecularOrbital->exp,pMolecularOrbital->nucl,pMolecularOrbital->type);
			printf("is not a valid adress in %d-th MolecularOrbital.\n",i+1);
			fprintf(chemicalFile,"\nEnd");
			closeTheFile(&chemicalFile);
			return 0;					
		}
		for (j=0; j<numberOfPrimitives; j++)
		{

			fprintf(chemicalFile,"%le ",pMolecularOrbital->coeff[j]);
			// Check exponent values
			if (pMolecularOrbital->exp[j]<=0.)
			{
				PRINT_ERROR();
				printf("In writingChemicalFile: %d-th Exponent of %d-th MolecularOrbital should be positive instead of %lf value.\n",j+1,i+1,pMolecularOrbital->exp[j]);
				fprintf(chemicalFile,"\nEnd");
				closeTheFile(&chemicalFile);
				return 0;	
			}
			fprintf(chemicalFile,"%le ",pMolecularOrbital->exp[j]);

			// Check center assignments 
			if (pMolecularOrbital->nucl[j]<1||pMolecularOrbital->nucl[j]>numberOfNuclei)
			{
				PRINT_ERROR();
				printf("In writingChemicalFile: %d-th Center of %d-th MolecularOrbital should be a positive ",j+1,i+1);
				printf("integer less than %d instead of %d value.\n",numberOfNuclei,pMolecularOrbital->nucl[j]);
				fprintf(chemicalFile,"\nEnd");
				closeTheFile(&chemicalFile);
				return 0;	
			}
			fprintf(chemicalFile,"%d ",pMolecularOrbital->nucl[j]);	

			// Check type assignments			
			if (pMolecularOrbital->type[j]<1||pMolecularOrbital->type[j]>20)
			{
				PRINT_ERROR();
				printf("In writingChemicalFile: %d-th Type of %d-th MolecularOrbital should be a positive integer less than 20 instead of %d value.\n",j+1,i+1,pMolecularOrbital->type[j]);
				fprintf(chemicalFile,"\nEnd");
				closeTheFile(&chemicalFile);
				return 0;	
			}
			fprintf(chemicalFile,"%d \n",pMolecularOrbital->type[j]);
		}		
	}
	fprintf(chemicalFile,"\nEnd");

	// Testing if file is well closed
	if(fclose(chemicalFile))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		PRINT_ERROR();
		printf("In writingChemicalFile: could not close %s file properly.\n",fileLocation);
		chemicalFile=NULL;
		return 0;
	}
	printf("Closing file.\n");
	chemicalFile=NULL;

	return 1;
}

/* ****************************************************************************************************************************************************************************************** */
// The function readTxtFileAndAllocateChemicalSystem reads file at fileLocation (file must have been previously created), checks txt syntax, allocates and fills memory in pChemicalSystem 
// It has the char* fileLocation and ChemicalSystem* variable (described in main.h) as input arguments and it returns zero if an error occurred, otherwise one
/* ****************************************************************************************************************************************************************************************** */
static int readTxtFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem)
{
	char readStringIn[33]={'\0'}, *readStringOut=NULL;
	int readIntegerIn=0, readIntegerOut=0, i=0, iMax=0, j=0, k=0, kMax=0;
	double readDouble=0.;
	MolecularOrbital *pMolecularOrbital=NULL;
	FILE *chemicalFile=NULL;

	// Testing if the input pChemicalSystem variable is pointing to NULL otherwise initialize its structure to zero for variables and NULL for pointers
	if (pChemicalSystem==NULL)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: the input pChemicalSystem variable is pointing to %p adress.\n",pChemicalSystem);
		return 0;	
	}
	initializeChemicalStructure(pChemicalSystem);

	// Since the file must have been previously created, we check if it is the case 
	if(!initialFileExists(fileLocation))
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: initialFileExists function returned zero instead of one.\n");
		return 0;
	}

	// Testing if the file is well opened (warning: file must have been previously created)
	printf("\nOpening %s file. ",fileLocation);
	chemicalFile=fopen(fileLocation,"r");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned
	if (chemicalFile==NULL)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: could not read %s file.\n",fileLocation);
		return 0;
	}
	printf("Reading and saving chemical data. ");		

	// Reading number of MolecularOrbitals and store it in pChemicalSystem->nmorb
	readStringOut=fgets(readStringIn,18,chemicalFile);	  // fgets function returns a null pointer if it failed, otherwise it returns the same readStringIn parameters
	readIntegerOut=fscanf(chemicalFile," %d ",&readIntegerIn);// fscanf function returns the number of input items successfully matched and assigned, which can be fewer than provided for

	// Important remark: white space characters, such as blanks and newline characters, causes fscanf (scanf and sscanf) to read, but not to store, all consecutive white space characters in 
	//                   the input up to the next character that is not white space. One white space character in format-string matches any combination of white space characters in the input.

	if(readStringOut==NULL || readIntegerOut!=1) 
	{
		PRINT_ERROR();
		printf("In readAndAllocChemicalSystem: wrong return of fgets (=%p) and/or fscanf (=%d) in MolecularOrbitals.\n",readStringOut,readIntegerOut);
		closeTheFile(&chemicalFile);
		return 0;
	}
	if (strcmp(readStringIn,"MolecularOrbitals") || readIntegerIn<1)	// strcmp function returns 0 if the two strings are equal, otherwise a non-zero value
	{
		PRINT_ERROR();
		printf("In readAndAllocChemicalSystem: expecting MolecularOrbitals Nmorb instead of %s %d (positive value).\n",readStringIn,readIntegerIn);
		closeTheFile(&chemicalFile);
		return 0;
	}
	pChemicalSystem->nmorb=readIntegerIn;

	// Allocating memory for MolecularOrbitals and check if it works
	pChemicalSystem->pmorb=(MolecularOrbital*)calloc(readIntegerIn,sizeof(MolecularOrbital));
	if (pChemicalSystem->pmorb==NULL)
	{
		PRINT_ERROR();
		printf("In readAndAllocChemicalSystem: could not allocate memory for %d Molecular Orbitals.\n",readIntegerIn);
		closeTheFile(&chemicalFile);
		return 0;
	}

	// Reading number of Primitives and store it in pChemicalSystem->ngauss
	readStringOut=fgets(readStringIn,11,chemicalFile);
	readIntegerOut=fscanf(chemicalFile," %d ",&readIntegerIn);
	if(readStringOut==NULL || readIntegerOut!=1)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: wrong return of fgets (=%p) and/or fscanf (=%d) in Primitives.\n",readStringOut,readIntegerOut);		
		closeTheFile(&chemicalFile);
		return 0;
	}
	if (strcmp(readStringIn,"Primitives") || readIntegerIn<1)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: expecting Primitives Ngauss instead of %s %d (positive value).\n",readStringIn,readIntegerIn);		
		closeTheFile(&chemicalFile);
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
			printf("In readTxtFileAndAllocateChemicalSystem: could not allocate memory for %d Primitives in %d-th Molecular Orbital.\n",readIntegerIn,i+1);		
			closeTheFile(&chemicalFile);
			return 0;
		}	
	}

	// Reading number of Nuclei and store it in pChemicalSystem->nnucl
	readStringOut=fgets(readStringIn,7,chemicalFile);
	readIntegerOut=fscanf(chemicalFile," %d ",&readIntegerIn);
	if(readStringOut==NULL || readIntegerOut!=1)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: wrong return of fgets (=%p) and/or fscanf (=%d) in Nuclei.\n",readStringOut,readIntegerOut);		
		closeTheFile(&chemicalFile);
		return 0;
	}
	if (strcmp(readStringIn,"Nuclei") || readIntegerIn<1)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: expecting Nuclei Nnucl instead of %s %d (positive value).\n",readStringIn,readIntegerIn);		
		closeTheFile(&chemicalFile);
		return 0;
	}
	pChemicalSystem->nnucl=readIntegerIn;

	// Allocating memory for Nuclei and check if it works
	pChemicalSystem->pnucl=(Nucleus*)calloc(readIntegerIn,sizeof(Nucleus));
	if (pChemicalSystem->pnucl==NULL)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: could not allocate memory for %d Nuclei.\n",readIntegerIn);		
		closeTheFile(&chemicalFile);
		return 0;
	}

	// Saving Nuclei in pMesh->pnucl and check if it works
	j=0;
	for (i=0; i<readIntegerIn; i++)
	{
		j+=fscanf(chemicalFile,"%lf ",&readDouble);	
		pChemicalSystem->pnucl[i].x=readDouble;	
		j+=fscanf(chemicalFile,"%lf ",&readDouble);	
		pChemicalSystem->pnucl[i].y=readDouble;
		j+=fscanf(chemicalFile,"%lf ",&readDouble);	
		pChemicalSystem->pnucl[i].z=readDouble;	
	}
	if (j!=3*readIntegerIn)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: expecting %d Nuclei instead of %d properly saved.\n",readIntegerIn,j/3);		
		closeTheFile(&chemicalFile);
		return 0;
	}
	
	// Read and save successively the spin (1 or -1) followed by enumeration of gaussian coefficients, gaussian exponents and references of associated centers and types, respectively for each orbital
	readStringOut=fgets(readStringIn,33,chemicalFile);
	if(readStringOut==NULL || strcmp(readStringIn,"Coefficient Exponent Center Type"))
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: expecting Coefficient Exponent Center Type instead of %s (and fgets returns %p adress).\n",readStringIn,readStringOut);		
		closeTheFile(&chemicalFile);
		return 0;
	}
	iMax=pChemicalSystem->nmorb;
	for (i=0; i<iMax;i++)
	{
		readIntegerOut=fscanf(chemicalFile," %d ",&readIntegerIn);
		if (readIntegerOut!=1 || readIntegerIn!=i+1)
		{
			PRINT_ERROR();
			printf("In readTxtFileAndAllocateChemicalSystem: wrong return of fscanf (in=%d, out=%d) in %d-th Molecular Orbital.\n",readIntegerIn,readIntegerOut,i+1);		
			closeTheFile(&chemicalFile);
			return 0;
		}
		pMolecularOrbital=&pChemicalSystem->pmorb[i];
		readStringOut=fgets(readStringIn,5,chemicalFile);
		readIntegerOut=fscanf(chemicalFile," %d ",&readIntegerIn);
		if(readStringOut==NULL || readIntegerOut!=1)
		{
			PRINT_ERROR();
			printf("In readTxtFileAndAllocateChemicalSystem: wrong return of fgets (=%p) and/or fscanf (=%d) in Spin of %d-th Molecular Orbital.\n",readStringOut,readIntegerOut,i+1);
			closeTheFile(&chemicalFile);
			return 0;
		}
		if (strcmp(readStringIn,"Spin") || abs(readIntegerIn)!=1)
		{
			PRINT_ERROR();
			printf("In readTxtFileAndAllocateChemicalSystem: expecting Spin (+/-)1 instead of %s %d (unitary value).\n",readStringIn,readIntegerIn);		
			closeTheFile(&chemicalFile);
			return 0;
		}
		pMolecularOrbital->spin=readIntegerIn;
		j=0;
		kMax=pChemicalSystem->ngauss;
		for (k=0; k<kMax; k++)
		{
			j+=fscanf(chemicalFile,"%lf ",&readDouble);
			pMolecularOrbital->coeff[k]=readDouble;

			// Check exponent values
			j+=fscanf(chemicalFile,"%lf ",&readDouble);
			if (readDouble<=0.)
			{
				PRINT_ERROR();
				printf("In readTxtFileAndAllocateChemicalSystem: %d-th Exponent of %d-th Molecular Orbital should be positive instead of %lf value.\n",k+1,i+1,readDouble);
				closeTheFile(&chemicalFile);
				return 0;	
			}
			pMolecularOrbital->exp[k]=readDouble;

			// Check center assignments 
			j+=fscanf(chemicalFile,"%d ",&readIntegerOut);
			if (readIntegerOut<1 || readIntegerOut>pChemicalSystem->nnucl)
			{
				PRINT_ERROR();
				printf("In readTxtFileAndAllocateChemicalSystem: %d-th Center of %d-th Molecular Orbital should be a positive integer ",k+1,i+1);
				printf("less than %d instead of %d value.\n",pChemicalSystem->nnucl,readIntegerOut);
				closeTheFile(&chemicalFile);
				return 0;	
			}
			pMolecularOrbital->nucl[k]=readIntegerOut;

			// Check type assignments
			j+=fscanf(chemicalFile,"%d ",&readIntegerOut);	
			if (readIntegerOut<1 || readIntegerOut>20)
			{
				PRINT_ERROR();
				printf("In readTxtFileAndAllocateChemicalSystem: %d-th Center of %d-th Molecular Orbital should be a positive integer ",k+1,i+1);
				printf("less than 20 instead of %d value.\n",readIntegerOut);
				closeTheFile(&chemicalFile);
				return 0;	
			}	
			pMolecularOrbital->type[k]=readIntegerOut;
		}	
		if (j!=4*kMax)
		{
			PRINT_ERROR();
			printf("In readTxtFileAndAllocateChemicalSystem: expecting %d Primitives in %d-th orbitals instead of %d properly saved.\n",kMax,i+1,j/4);		
			closeTheFile(&chemicalFile);
			return 0;
		}

	}

	// Check that "End" keyword is ending the file
	readStringOut=fgets(readStringIn,4,chemicalFile);	
	if(readStringOut==NULL || strcmp(readStringIn,"End"))
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: expecting End instead of %s (and fgets returns %p adress).\n",readStringIn,readStringOut);		
		closeTheFile(&chemicalFile);
		return 0;
	}

	// Testing if file is well closed
	if(fclose(chemicalFile))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		PRINT_ERROR();
		printf("In readTxtFileAndAllocateChemicalSystem: could not close %s file properly.\n",fileLocation);
		chemicalFile=NULL;
		return 0;
	}
	printf("Closing file.\n");
	chemicalFile=NULL;

	return 1;
}

/* *************************************************************************************************************************************************************************************************** */
// The function loadChemistry extracts chemical data from file (warning: reset and overwrite chemicalOut.txt file if already exists) located at fileLocation (file must be in *.wfn or *.txt format)
// It has the char* fileLocation, ChemicalSystem* variable (described in main.h) and int nuElectrons as input arguments and it returns zero if an error occurred, otherwise one
/* *************************************************************************************************************************************************************************************************** */
int loadChemistry(char* fileLocation, ChemicalSystem *pChemicalSystem, int nuElectrons)
{
	// Reading file at fileLocation if it exists and returns the format (one is *.txt, -1 is *.wfn, zero refers to an error)
	switch (getChemicalFormat(fileLocation))
	{
		case 1:
			// Reading *.txt file
			if(!readTxtFileAndAllocateChemicalSystem(fileLocation,pChemicalSystem))
			{
				PRINT_ERROR();
				printf("In loadChemistry: readTxtFileAndAllocateChemicalSystem function returned zero instead of one.\n");
				return 0;
			}
			break;

		case -1:
			// Converting the Fortran scientific notation (1.0D+01) into the C format (1.0e+01) in the *.wfn file		
			if(!readAndConvertWfnFile(fileLocation,'D','e'))
			{
				PRINT_ERROR();
				printf("In loadChemistry: readAndConvertWfnFile function returned zero instead of one.\n");
				return 0;	
			}

			// Reading *.wfn file
			if(!readWfnFileAndAllocateChemicalSystem(fileLocation,pChemicalSystem))
			{
				PRINT_ERROR();
				printf("In loadChemistry: readWfnFileAndAllocateChemicalSystem function returned zero instead of one.\n");
				// Reconverting the *.wfn file as it was
				if(!readAndConvertWfnFile(fileLocation,'e','D'))
				{
					PRINT_ERROR();
					printf("In loadChemistry: readAndConvertWfnFile function returned zero instead of one.\n");
				}
				return 0;
			}

			// Reconverting the *.wfn file as it was
			if(!readAndConvertWfnFile(fileLocation,'e','D'))
			{
				PRINT_ERROR();
				printf("In loadChemistry: readAndConvertWfnFile function returned zero instead of one.\n");
				return 0;
			}

			// Writing chemicalOut.txt file (warning: reset and overwrite chemicalOut.txt file if already exists)
			if(!writingChemicalFile("chemicalOut.txt",pChemicalSystem))
			{
				PRINT_ERROR();
				printf("In loadChemistry: writingChemicalFile function returned zero instead of one.\n");
				return 0;
			}
			break;

		default:
			PRINT_ERROR();
			printf("In loadChemistry: getChemicalFormat function returned zero instead of (+/-) one.\n");
			return 0;
			break;
	}
	printf("Chemical system loaded: %d Molecular Orbitals, %d Primitives, and %d Nuclei.\n",pChemicalSystem->nmorb,pChemicalSystem->ngauss,pChemicalSystem->nnucl);

	// Checking the nuElectrons parameter
	if (nuElectrons<1||nuElectrons>pChemicalSystem->nmorb)
	{
		PRINT_ERROR();
		printf("In loadChemistry: nuElectrons=%d should be positive and not greater than %d (total number of electrons).\n",nuElectrons,pChemicalSystem->nmorb);
		return 0;
	}

	return 1;
}

