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

// Macro function to initialize diagnostic of an error
#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

void freeMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues);

// Testing
int allocateMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues, int size);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0; 

	int size=0, i1=0, i2=0, i3=0, i4=0;
	int n1=1, n2=1, n3=1, n4=123;

	double *overlapMatrix=NULL, *eigenvectors=NULL, *eigenvalues=NULL;
	double **pOverlapMatrix=NULL, **pEigenvectors=NULL, **pEigenvalues=NULL;

	// Test starts
	time(&startTimer);
	printf("Testing allocateMatrixMemory function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	printf("pOverlapMatrix=%p pEigenvectors=%p pEigenvalues=%p size=%d\n",pOverlapMatrix,pEigenvectors,pEigenvalues,size);
	returnValue=allocateMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues,size);
	freeMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pOverlapMatrix=&overlapMatrix;
	pEigenvectors=&eigenvectors;
	pEigenvalues=&eigenvalues;
	PRINT_TEST_START(counter,expectedValue);
	printf("pOverlapMatrix=%p pEigenvectors=%p pEigenvalues=%p size=%d\n",pOverlapMatrix,pEigenvectors,pEigenvalues,size);
	printf("overlapMatrix=%p eigenvectors=%p eigenvalues=%p\n",*pOverlapMatrix,*pEigenvectors,*pEigenvalues);
	returnValue=allocateMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues,size);
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
				for (i4=-n4; i4<=n4; i4+=n4)
				{
					size=i4;
					expectedValue=1;
					if (i1<0 || i2<0 || i3<0 || i4<1)
					{
						expectedValue=0;
					}
					PRINT_TEST_START(counter,expectedValue);
					printf("pOverlapMatrix=%p pEigenvectors=%p pEigenvalues=%p size=%d\n",pOverlapMatrix,pEigenvectors,pEigenvalues,size);
					returnValue=allocateMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues,size);
					freeMatrixMemory(pOverlapMatrix,pEigenvectors,pEigenvalues);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
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

/* ************************************************************************************************************************************************************************* */
// The function allocateMatrixMemory allocates memory for all vector structures: pOverlapMatrix(size x size), pEigenvectors(size x size), and pEigenvalues(size).
// It has the int size and the three double** variables (pOverlapMatrix, pMatrixToDiagonalize, pEigenvalues) as input arguments and it returns one on sucess, otherwise zero
/* ************************************************************************************************************************************************************************** */
int allocateMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues, int size)
{
	printf("\nAllocating memory for matrices.\n");	

	// Check if all the input pointer variables are not set to NULL
	if (pOverlapMatrix==NULL || pEigenvectors==NULL || pEigenvalues==NULL)
	{
		PRINT_ERROR();
		printf("In allocateMatrixMemory: one of the input variables pOverlapMatrix=%p, pEigenvectors=%p or pEigenvalues=%p is not a valid adress.\n",pOverlapMatrix,pEigenvectors,pEigenvalues);
		return 0;
	}

	// Check if the input size variable is positive
	if (size<1)
	{
		PRINT_ERROR();
		printf("In allocateMatrixMemory: the prescribed size=%d (input) variable is not positive.\n",size);
		return 0;
	}

	// Allocating memory for the OverlapMatrix, Eigenvectors and Eigenvalues
	*pOverlapMatrix=(double*)calloc(size*size,sizeof(double));
	*pEigenvectors=(double*)calloc(size*size,sizeof(double));
	*pEigenvalues=(double*)calloc(size,sizeof(double));
	if (*pOverlapMatrix==NULL || *pEigenvectors==NULL || *pEigenvalues==NULL)
	{
		PRINT_ERROR();
		printf("In allocateMatrixMemory: could not allocate memory for matrices.\n");
		return 0;
	}

	return 1;
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

