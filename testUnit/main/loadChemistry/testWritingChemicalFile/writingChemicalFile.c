#include "writingChemicalFile.h"

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
void initializeChemicalStructure(ChemicalSystem* pChemicalSystem);
void freeChemicalMemory(ChemicalSystem* pChemicalSystem);
int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);
*/
static int readTxtFileAndAllocateChemicalSystem(char* fileLocation, ChemicalSystem* pChemicalSystem);

// Testing
static int writingChemicalFile(char* fileLocation, ChemicalSystem* pChemicalSystem);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int i=0, imorb=0, inucl=0, igauss=0, j=0, jcoeff=0, jexp=0, jnucl=0, jtype=0, pspin=0, pcoeff=0, pexp=0, pnucl=0, ptype=0;
	int nmorb=8, nnucl=3, ngauss=128, nspin=1, ncoeff=128, nexp=128, npnucl=128, ntype=128, counterChar=0, readCharTest=0,readCharRef=0;
	FILE *refFile=NULL, *testFile=NULL;
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
	printf("Testing writingChemicalFile function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingChemicalFile(NULL,pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingChemicalFile("test.txt",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pChemicalSystem=&chemicalSystem;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingChemicalFile(NULL,pChemicalSystem);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingChemicalFile("test.txt",pChemicalSystem);
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

										expectedValue=0;
										PRINT_TEST_START(counter,expectedValue);
										printf("nmorb=%d pmorb=%p nnucl=%d ",pChemicalSystem->nmorb,pChemicalSystem->pmorb,pChemicalSystem->nnucl);
										printf("pnucl=%p ngauss=%d\n",pChemicalSystem->pnucl,pChemicalSystem->ngauss);
										for (i=0; i<imorb; i++)
										{
											pMolecularOrbital=&pChemicalSystem->pmorb[i];
											printf("Orbital %d: spin=%d coeff=%p ",i+1,pMolecularOrbital->spin,pMolecularOrbital->coeff);
											printf("exp=%p nucl=%p type=%p\n",pMolecularOrbital->exp,pMolecularOrbital->nucl,pMolecularOrbital->type);
										}
										returnValue=writingChemicalFile("test.txt",pChemicalSystem);
										freeChemicalMemory(pChemicalSystem);
										PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
										if (pcoeff>0 && pexp>0 && pnucl>0 && ptype>0)
										{
											for (jcoeff=-ncoeff; jcoeff<=ncoeff; jcoeff+=ncoeff)
											{
												for (jexp=-nexp; jexp<=nexp; jexp+=nexp)
												{
													for (jnucl=-3; jnucl<=3; jnucl+=3)
													{
														for (jtype=-10; jtype<=10; jtype+=10)
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
																for (j=0; j<pcoeff; j++)
																{
																	pMolecularOrbital->coeff[j]=(double)jcoeff;
																}
																pMolecularOrbital->exp=(double*)calloc(pexp,sizeof(double));
																for (j=0; j<pexp; j++)
																{
																	pMolecularOrbital->exp[j]=(double)jexp;
																}
																pMolecularOrbital->nucl=(int*)calloc(pnucl,sizeof(int));
																for (j=0; j<pnucl; j++)
																{
																	pMolecularOrbital->nucl[j]=jnucl;
																}
																pMolecularOrbital->type=(int*)calloc(ptype,sizeof(int));
																for (j=0; j<ptype; j++)
																{
																	pMolecularOrbital->type[j]=jtype;
																}
															}
															expectedValue=0;
															if (inucl>0 && pspin!=0 && igauss>0 && jexp>0 && jnucl>0 && jtype>0)
															{
																expectedValue=1;
															}
															PRINT_TEST_START(counter,expectedValue);
															printf("nmorb=%d pmorb=%p ",pChemicalSystem->nmorb,pChemicalSystem->pmorb);
															printf("nnucl=%d pnucl=%p ",pChemicalSystem->nnucl,pChemicalSystem->pnucl);
															printf("ngauss=%d\n",pChemicalSystem->ngauss);
															for (i=0; i<imorb; i++)
															{
																pMolecularOrbital=&pChemicalSystem->pmorb[i];
																printf("Orbital %d: spin=%d ",i+1,pMolecularOrbital->spin);
																printf("coeff=%p exp=%p ",pMolecularOrbital->coeff,pMolecularOrbital->exp);
																printf("nucl=%p type=%p\n",pMolecularOrbital->nucl,pMolecularOrbital->type);
															}
															returnValue=writingChemicalFile("test.txt",pChemicalSystem);
															freeChemicalMemory(pChemicalSystem);
															PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
														}
													}
												}	
											}				
										}
									}
								}
							}
						}
					}
				}
				else
				{
					expectedValue=0;
					PRINT_TEST_START(counter,expectedValue);
					printf("nmorb=%d pmorb=%p nnucl=%d ",pChemicalSystem->nmorb,pChemicalSystem->pmorb,pChemicalSystem->nnucl);
					printf("pnucl=%p ngauss=%d\n",pChemicalSystem->pnucl,pChemicalSystem->ngauss);
					returnValue=writingChemicalFile("test.txt",pChemicalSystem);
					freeChemicalMemory(pChemicalSystem);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
			}
		}
	}

	// The last test compares with the result of writingChemicalFile function with a chemicalRef.txt file
	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	printf("readTxtFileAndAllocateChemicalSystem function returns %d value.\n",readTxtFileAndAllocateChemicalSystem("chemicalRef.txt",pChemicalSystem));
	returnValue=writingChemicalFile("test.txt",pChemicalSystem);
	freeChemicalMemory(pChemicalSystem);
	refFile=fopen("chemicalRef.txt","r");
	testFile=fopen("test.txt","r");

	counterChar=0;
	do {
		counterChar++;
		readCharRef=fgetc(refFile);
		readCharTest=fgetc(testFile);
		if (readCharRef!=readCharTest)
		{
			printf("Letter %d is different: %c (test) != %c (ref)\n",counterChar,readCharTest,readCharRef);
			for (i=0; i<10; i++)
			{
				printf("%c",fgetc(testFile));
			}
			printf("\n(test) vs (ref)\n");
			for (i=0; i<10; i++)
			{
				printf("%c",fgetc(refFile));
			}
			getchar();
			returnValue=0;
		}
	} while (readCharRef!=EOF && readCharTest!=EOF);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	fclose(refFile);
	fclose(testFile);
	system("rm test.txt");

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
				printf("\nWarning in freeChemicalMemory function: %d molecular orbitals saved in chemicalSystem. Some allocated pointers may not have been freed up correctly.\n",iMax);
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

