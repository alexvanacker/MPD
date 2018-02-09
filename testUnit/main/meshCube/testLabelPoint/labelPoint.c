#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_X 11				
#define N_Y 5
#define N_Z 22

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
static int labelPoint(int i, int j, int k);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int i=0, j=0, k=0;
	FILE *meshFile=NULL;

	// The theoretical mesh is the one given by nx=11, ny=5 and nz=22 on a cube [0.0,10.0]x[0.0,4.0]x[0.0,21.0] with a discretization (1.0)x(1.0)x(1.0)
	// Medit software (warning: must have been previously installed) better vizualizes labels of mesh elements than the ones of mesh vertices (here, the return value of labelPoint function)
	// Hence the idea of the test consists in creating another mesh so that each point of the previous cube mesh is the center of the tetrahedra in the new mesh with corresponding label    
	// Moreover, the total resulting mesh is covered with an extra layer of tetrahedra to see how labelPoint function reacts if we do not respect the original size of the cube

	// Test starts 
	time(&startTimer);
	printf("Testing labelPoint function.\n");

	meshFile=fopen("cube.mesh","w+");
	fprintf(meshFile,"MeshVersionFormatted 2\n\nDimension 3\n\nVertices\n%d\n",(N_X+3)*(N_Y+3)*(N_Z+3));

	for (i=0; i<N_X+3; i++)
	{
		for (j=0; j<N_Y+3; j++)
		{ 
			for (k=0; k<N_Z+3; k++)
			{
				fprintf(meshFile,"%le %le %le %d \n",-1.5+i,-1.5+j,-1.5+k,0);
			}
		}
	}

	fprintf(meshFile,"\nHexahedra\n%d\n",(N_X+2)*(N_Y+2)*(N_Z+2));
	for (i=1; i<N_X+3; i++)
	{
		for (j=1; j<N_Y+3; j++)
		{
			for (k=1; k<N_Z+3; k++)
			{
				fprintf(meshFile,"%d %d %d %d ",1+((i-1)*(N_Y+3)+(j-1))*(N_Z+3)+(k-1),1+(i*(N_Y+3)+(j-1))*(N_Z+3)+(k-1),1+(i*(N_Y+3)+j)*(N_Z+3)+(k-1),1+((i-1)*(N_Y+3)+j)*(N_Z+3)+(k-1));
				fprintf(meshFile,"%d %d %d %d ",1+((i-1)*(N_Y+3)+(j-1))*(N_Z+3)+k,1+(i*(N_Y+3)+(j-1))*(N_Z+3)+k,1+(i*(N_Y+3)+j)*(N_Z+3)+k,1+((i-1)*(N_Y+3)+j)*(N_Z+3)+k);
				fprintf(meshFile,"%d \n",labelPoint(i-2,j-2,k-2));
			}
		}
	}

//*	
	fprintf(meshFile,"\nQuadrilaterals\n%d\n",2*((N_X+2)*(N_Y+2)+(N_X+2)*(N_Z+2)+(N_Y+2)*(N_Z+2)));
	for (i=1; i<N_X+3; i++)
	{
		for (k=1; k<N_Z+3; k++)
		{
			fprintf(meshFile,"%d %d %d %d %d \n",1+(i-1)*(N_Y+3)*(N_Z+3)+(k-1),1+i*(N_Y+3)*(N_Z+3)+(k-1),1+i*(N_Y+3)*(N_Z+3)+k,1+(i-1)*(N_Y+3)*(N_Z+3)+k,0);
			fprintf(meshFile,"%d %d %d %d ",1+((i-1)*(N_Y+3)+(N_Y+2))*(N_Z+3)+(k-1),1+(i*(N_Y+3)+(N_Y+2))*(N_Z+3)+(k-1),1+(i*(N_Y+3)+(N_Y+2))*(N_Z+3)+k,1+((i-1)*(N_Y+3)+(N_Y+2))*(N_Z+3)+k);
			fprintf(meshFile,"%d \n",0);
		}
	}
	for (j=1; j<N_Y+3; j++)
	{
		for (k=1; k<N_Z+3; k++)
		{
			fprintf(meshFile,"%d %d %d %d %d \n",1+(j-1)*(N_Z+3)+(k-1),1+j*(N_Z+3)+(k-1),1+j*(N_Z+3)+k,1+(j-1)*(N_Z+3)+k,0);
			fprintf(meshFile,"%d %d %d %d ",1+((N_X+2)*(N_Y+3)+(j-1))*(N_Z+3)+(k-1),1+((N_X+2)*(N_Y+3)+j)*(N_Z+3)+(k-1),1+((N_X+2)*(N_Y+3)+j)*(N_Z+3)+k,1+((N_X+2)*(N_Y+3)+(j-1))*(N_Z+3)+k);
			fprintf(meshFile,"%d \n",0);
		}
	}
	for (i=1; i<N_X+3; i++)
	{
		for (j=1; j<N_Y+3; j++)
		{
			fprintf(meshFile,"%d %d %d %d %d \n",1+((i-1)*(N_Y+3)+(j-1))*(N_Z+3),1+((i-1)*(N_Y+3)+j)*(N_Z+3),1+(i*(N_Y+3)+j)*(N_Z+3),1+(i*(N_Y+3)+(j-1))*(N_Z+3),0);
			fprintf(meshFile,"%d %d %d %d ",1+((i-1)*(N_Y+3)+(j-1))*(N_Z+3)+(N_Z+2),1+((i-1)*(N_Y+3)+j)*(N_Z+3)+(N_Z+2),1+(i*(N_Y+3)+j)*(N_Z+3)+(N_Z+2),1+(i*(N_Y+3)+(j-1))*(N_Z+3)+(N_Z+2));
			fprintf(meshFile,"%d \n",0);
		}
	}
//*/	
	fprintf(meshFile,"\nEnd");
	fclose(meshFile);

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	system("medit cube.mesh");
	printf("\nCheck with medit software that labelPoint function is giving the correct label on vertices (center of each Hexahedra in the cube.mesh file).\nIs it correct (y/n)? ");
	readChar=getchar();
	returnValue=0;
	if (readChar=='y')
	{
		returnValue=1;
	}			
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

/* ******************************************************************************************************************************************************************************************** */
// The function labelPoint label the cube vertices: 0 on internal points; 1,2,3,4,5,6 on faces; 12,14,15,16,23,25,26,34,35,36,45,46 on ridges; 125,126,145,146,235,236,345,346 on corners.
// It has the point coordinates (i,j,k)=(X_MIN+i*DELTA_X,Y_MIN+j*DELTA_Y,Z_MIN+k*DELTA_Z) as input arguments and it returns the point label (warning: be sure i,j,k lie between 0 and N_XYZ-1)
/* ******************************************************************************************************************************************************************************************** */
static int labelPoint(int i, int j, int k)
{	
	int returnValue=0;	

	switch(i)
	{
		case 0:
			switch(j)
			{
				case 0:
					switch(k)
					{
						case 0:
							returnValue=145;
							break;

						case N_Z-1:
							returnValue=146;
							break;

						default:
							returnValue=14;
							break;
					}
					break;

				case N_Y-1:
					switch(k)
					{
						case 0:
							returnValue=345;
							break;

						case N_Z-1:
							returnValue=346;
							break;

						default:
							returnValue=34;
							break;
					}
					break;

				default:
					switch(k)
					{
						case 0:
							returnValue=45;
							break;

						case N_Z-1:
							returnValue=46;
							break;

						default:
							returnValue=4;
							break;
					}
					break;
			}		
			break;

		case N_X-1:
			switch(j)
			{
				case 0:
					switch(k)
					{
						case 0:
							returnValue=125;
							break;

						case N_Z-1:
							returnValue=126;
							break;

						default:
							returnValue=12;
							break;
					}
					break;

				case N_Y-1:
					switch(k)
					{
						case 0:
							returnValue=235;
							break;

						case N_Z-1:
							returnValue=236;
							break;

						default:
							returnValue=23;
							break;
					}
					break;

				default:
					switch(k)
					{
						case 0:
							returnValue=25;
							break;

						case N_Z-1:
							returnValue=26;
							break;

						default:
							returnValue=2;
							break;
					}
					break;
			}
			break;

		default:
			switch(j)
			{
				case 0:
					switch(k)
					{
						case 0:
							returnValue=15;
							break;

						case N_Z-1:
							returnValue=16;
							break;

						default:
							returnValue=1;
							break;
					}
					break;

				case N_Y-1:
					switch(k)
					{
						case 0:
							returnValue=35;
							break;

						case N_Z-1:
							returnValue=36;
							break;

						default:
							returnValue=3;
							break;
					}
					break;

				default:
					switch(k)
					{
						case 0:
							returnValue=5;
							break;

						case N_Z-1:
							returnValue=6;
							break;

						default:
							returnValue=0;
							break;
					}
					break;
			}
			break;
	}

	return returnValue;
}

