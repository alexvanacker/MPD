#include <stdio.h>
#include <stdlib.h>
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
void freeMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0; 

	int i1=0, i2=0, i3=0, j1=0, j2=0, j3=0;
	int n1=23, n2=43, n3=8, p1=56, p2=9, p3=38945;

	double *overlapMatrix=NULL, *eigenvectors=NULL, *eigenvalues=NULL;
	double **pOverlapMatrix=NULL, **pEigenvectors=NULL, **pEigenvalues=NULL;

	// Test starts
	time(&startTimer);
	printf("Testing freeMatrixMemory function.\n");

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	printf("pOverlapMatrix=%p pEigenvectors=%p pEigenvalues=%p\n",pOverlapMatrix,pEigenvectors,pEigenvalues);
	freeMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues);
	returnValue=1;
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pOverlapMatrix=&overlapMatrix;
	pEigenvectors=&eigenvectors;
	pEigenvalues=&eigenvalues;
	PRINT_TEST_START(counter,expectedValue);
	printf("pOverlapMatrix=%p pEigenvectors=%p pEigenvalues=%p\n",pOverlapMatrix,pEigenvectors,pEigenvalues);
	freeMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	for (i1=-n1; i1<=n1; i1+=n1)
	{
		pOverlapMatrix=(double**)calloc(i1,sizeof(double*));
		for (i2=-n2; i2<=n2; i2+=n2)
		{
			pEigenvectors=(double**)calloc(i2,sizeof(double*));
			for (i3=-n3; i3<=n3; i3+=n3)
			{
				pEigenvalues=(double**)calloc(i3,sizeof(double*));
				for (j1=-p1; j1<=p1; j1+=p1)
				{	
					for (j2=-p2; j2<=p2; j2+=p2)
					{
						for (j3=-p3; j3<=p3; j3+=p3)
						{
							if (i1>=0)
							{
								*pOverlapMatrix=(double*)calloc(j1,sizeof(double));
							}
							if (i2>=0)
							{
								*pEigenvectors=(double*)calloc(j2,sizeof(double));
							}
							if (i3>=0)
							{										
								*pEigenvalues=(double*)calloc(j3,sizeof(double));
							}
							returnValue=1;
							PRINT_TEST_START(counter,expectedValue);
							printf("pOverlapMatrix=%p pEigenvectors=%p pEigenvalues=%p",pOverlapMatrix,pEigenvectors,pEigenvalues);
							if (i1>=0)
							{
								printf(" overlapMatrix=%p",*pOverlapMatrix);
							}
							if (i2>=0)
							{
								printf(" eigenvectors=%p",*pEigenvectors);
							}
							if (i3>=0)
							{
								printf(" eigenvalues=%p",*pEigenvalues);
							}
							printf("\n");
							freeMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues);							
							if (pOverlapMatrix!=NULL)
							{
								if (*pOverlapMatrix!=NULL)
								{
									printf("Error: although freeMatrixMemor function ends normally, pOverlapMatrix was not freed up correctly.\n");
									returnValue=0;
								}
							}
							if (pEigenvectors!=NULL)
							{
								if (*pEigenvectors!=NULL)
								{
									printf("Error: although freeMatrixMemor function ends normally, pEigenvectors was not freed up correctly.\n");
									returnValue=0;
								}
							}
							if (pEigenvalues!=NULL)
							{
								if (*pEigenvalues!=NULL)
								{
									printf("Error: although freeMatrixMemor function ends normally, pEigenvalues was not freed up correctly.\n");
									returnValue=0;
								}
							}
							PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
						}
					}
				}
				free(pEigenvalues);
			}
			free(pEigenvectors);	
		}
		free(pOverlapMatrix);
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

/* *************************************************************************************************************************************************************************************************** */
// The function freeMatrixMemory free the pointers and memory allocated with calloc (malloc or realloc) for all vector structures (pOverlapMatrix, pEigenvectors, pEigenvalues) of the main function
// It has the three double** variables (pOverlapMatrix, pEigenvectors, pEigenvalues) as input arguments and it does not return any argument (void output)
/* *************************************************************************************************************************************************************************************************** */
void freeMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues)
{
	if (pOverlapMatrix!=NULL)
	{
		free(*pOverlapMatrix);	// free function does not return any value (void output)
		*pOverlapMatrix=NULL;
	}
	if (pEigenvectors!=NULL)
	{
		free(*pEigenvectors);
		*pEigenvectors=NULL;
	}
	if (pEigenvalues!=NULL)
	{
		free(*pEigenvalues);
		*pEigenvalues=NULL;
	}

	return;
}

