#include "meshCube.h"

/*
// Prototypes of non-static functions in meshCube.c
void initializeMeshStructure(Mesh* pMesh);
void freeMeshMemory(Mesh* pMesh);
int writingMeshFile(char* fileLocation, Mesh* pMesh);
int meshCube(char* fileLocation, Mesh *pMesh);
*/

// Prototypes of static functions in meshCube.c
static int copyFileLocation(char* fileLocation, char* fileLocationForCopy);
static int initializeCubeDiscretization(Mesh* pMesh);
static int allocateInitialMeshMemory(Mesh* pMesh);
static int labelPoint(int i, int j, int k);
static int discretizeCube(Mesh* pMesh);

/* ********************************************************************************************************************** */
// The function initializeMeshStructure initializes the mesh structure to zero for variables and NULL for pointers. 
// It has the pMesh structure (described in main.h) as input argument and it does not return any argument (void ouput)
/* ********************************************************************************************************************** */
void initializeMeshStructure(Mesh* pMesh)
{
	if (pMesh!=NULL)
	{
		pMesh->nver=0;
		pMesh->pver=NULL;
#ifndef LS_MODE
		pMesh->ncor=0;

		pMesh->nnorm=0;
		pMesh->pnorm=NULL;

		pMesh->ntan=0;
		pMesh->ptan=NULL;

		pMesh->nedg=0;
		pMesh->pedg=NULL;

		pMesh->ntri=0;
		pMesh->ptri=NULL;

		pMesh->ntet=0;
		pMesh->ptet=NULL;
#endif
#ifdef LS_MODE
		pMesh->nqua=0;
		pMesh->pqua=NULL;

		pMesh->nhex=0;
		pMesh->phex=NULL;

		pMesh->nadj=0;
		pMesh->padj=NULL;
#endif
	}

	return;
}

/* **************************************************************************************************************************************************************************** */
// The function freeMeshMemory free the pointers and memory allocated with calloc (malloc or realloc) for pMesh (but the all variables except pointers are not reset to zero)
// It has the Mesh* variable (described in main.h) as input argument and it does not return any argument (void output)
/* **************************************************************************************************************************************************************************** */
void freeMeshMemory(Mesh* pMesh) 
{
	if (pMesh!=NULL)
	{
		free(pMesh->pver);	// free function does not return any value (void output)
		pMesh->pver=NULL;	
#ifndef LS_MODE
		free(pMesh->pnorm);
		pMesh->pnorm=NULL;

		free(pMesh->ptan);
		pMesh->ptan=NULL;

		free(pMesh->pedg);
		pMesh->pedg=NULL;

		free(pMesh->ptri);
		pMesh->ptri=NULL;

		free(pMesh->ptet);
		pMesh->ptet=NULL;
#endif
#ifdef LS_MODE
		free(pMesh->phex);
		pMesh->phex=NULL;

		free(pMesh->pqua);
		pMesh->pqua=NULL;

		free(pMesh->padj);
		pMesh->padj=NULL;
#endif
	}

	return;
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

/* *********************************************************************************************************************************************** */
// The function initializeCubeDiscretization loads some (preprocessor) parameters entered in meshCube.h and main.h files into the pMesh structure
// It has the Mesh* variable (described in main.h) as input arguments and it returns (+/-) one on success, otherwise zero (an error has occurred)
/* *********************************************************************************************************************************************** */
static int initializeCubeDiscretization(Mesh* pMesh)	
{
	int returnValue=0, readChar=0, sizeMemory=0, numberOfVertices=0;
#ifndef LS_MODE
	int numberOfNormals=0, numberOfTangents=0, numberOfEdges=0, numberOfTriangles=0, numberOfTetrahedra=0;
#endif
#ifdef LS_MODE
	int numberOfQuadrilaterals=0, numberOfHexahedra=0;
#endif

	// Test if the pMesh pointer (input variable) has a NULL value or not
	if (pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In initializeCubeDiscretization: the pMesh (input) variable is set to the %p adress.\n",pMesh);
		return 0;
	}

	// Computing number of points, tetrahedra, triangles, edges, normals and tangents (hexahedra and quadrilaterals for cubic algorithm in LS_MODE, cf. main.h)
	numberOfVertices=N_X*N_Y*N_Z;
	sizeMemory=sizeof(Mesh)+numberOfVertices*sizeof(Point);
#ifndef LS_MODE
	numberOfNormals=2*((N_X-2)*(N_Y-2)+(N_X-2)*(N_Z-2)+(N_Y-2)*(N_Z-2));
	numberOfTangents=4*((N_X-2)+(N_Y-2)+(N_Z-2));
	numberOfEdges=4*((N_X-1)+(N_Y-1)+(N_Z-1));
	numberOfTriangles=4*((N_X-1)*(N_Y-1)+(N_X-1)*(N_Z-1)+(N_Y-1)*(N_Z-1));		// (2 triangles per square) x 2 faces 
	numberOfTetrahedra=6*(N_X-1)*(N_Y-1)*(N_Z-1);					// 6 tetrahedra per cube (warning: respect adjacency between cube cells)
	sizeMemory+=(numberOfNormals+numberOfTangents)*sizeof(Vector)+numberOfEdges*sizeof(Edge)+numberOfTriangles*sizeof(Triangle)+numberOfTetrahedra*sizeof(Tetrahedron);
#endif
#ifdef LS_MODE
	numberOfQuadrilaterals=2*((N_X-1)*(N_Y-1)+(N_X-1)*(N_Z-1)+(N_Y-1)*(N_Z-1)); 	// 2 faces per square
	numberOfHexahedra=(N_X-1)*(N_Y-1)*(N_Z-1);
	sizeMemory+=numberOfQuadrilaterals*sizeof(Quadrilateral)+numberOfHexahedra*sizeof(Hexahedron);
#endif			

	// Recall the discretization that is going to be performed
	printf("\nThe cube [%lf,%lf]x[%lf,%lf]x[%lf,%lf] will have:\n",X_MIN,X_MAX,Y_MIN,Y_MAX,Z_MIN,Z_MAX);
#ifndef LS_MODE
	printf("%d points, %d tetrahedra, and %d boundary triangles (%d edges, 8 corners).\n",numberOfVertices,numberOfTetrahedra,numberOfTriangles,numberOfEdges);
#endif
#ifdef LS_MODE
	printf("%d points, %d hexahedra, and %d boundary quadrilaterals.\n",numberOfVertices,numberOfHexahedra,numberOfQuadrilaterals);
#endif
	printf("Hence, it will take %d,%d Mo in memory. WARNING: overwrite cube.mesh file if already exists.\n",sizeMemory/1000000,sizeMemory%1000000);
	printf("Do you want to continue (y/n)? ");

	// Manually confirm the discretization of the cube
	readChar=getchar();	// getchar function returns the character read (in stdin) as an unsigned char cast to an int or EOF on end of file or error.
	switch(readChar)
	{
		case 'y':
			printf("Ok, we mesh the cube!\n");
			pMesh->nver=numberOfVertices;
#ifndef LS_MODE
			pMesh->ncor=8;
			pMesh->nnorm=numberOfNormals;
			pMesh->ntan=numberOfTangents;
			pMesh->nedg=numberOfEdges;
			pMesh->ntri=numberOfTriangles;
			pMesh->ntet=numberOfTetrahedra;
#endif
#ifdef LS_MODE
			pMesh->nqua=numberOfQuadrilaterals;
			pMesh->nhex=numberOfHexahedra;
#endif
			returnValue=1;
			break;

		case 'n':
			printf("Ok, leaving program.\n");
			returnValue=-1;
			break;

		case EOF:
			PRINT_ERROR();
			printf("In initializeCubeDiscretization: expecting 'y' or 'n' instead of EOF (%d = end-of-file) in your answer.\n", readChar);
			returnValue=0;
			break;

		default:
			PRINT_ERROR();
			printf("In initializeCubeDiscretization: expecting 'y' or 'n' instead of '%c' in your answer.\n", readChar);
			returnValue=0;
			break;
	}		

	// Clean the input buffer (stdin) in case of buffer overflow (warning: be sure that it is not empty otherwise wait undefinitively for input)
	while (readChar!='\n' && readChar!=EOF)
	{
		readChar=getchar(); 
	}	

	return returnValue;
}

/* *********************************************************************************************************************************************************** */
// The function allocateInitialMeshMemory allocates memory in the pMesh structure thanks to the parameters loaded by initializeCubeDiscretization function
// It has the Mesh* variable (described in main.h) as input argument and it returns zero if an error occurred, otherwise one is returned
/* *********************************************************************************************************************************************************** */
static int allocateInitialMeshMemory(Mesh* pMesh)
{
	// Test if the pMesh pointer (input variable) has a NULL value or not
	if (pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In allocateInitialMeshMemory: the pMesh (input) variable is set to the %p adress.\n",pMesh);
		return 0;
	}

	// Allocating memory for the different elements and check if the allocation worked
	printf("\nAllocating memory.\n");
	pMesh->pver=(Point*)calloc(pMesh->nver,sizeof(Point));
#ifndef LS_MODE	
	pMesh->pnorm=(Vector*)calloc(pMesh->nnorm,sizeof(Vector));
	pMesh->ptan=(Vector*)calloc(pMesh->ntan,sizeof(Vector));
	pMesh->pedg=(Edge*)calloc(pMesh->nedg,sizeof(Edge));
	pMesh->ptri=(Triangle*)calloc(pMesh->ntri,sizeof(Triangle));
	pMesh->ptet=(Tetrahedron*)calloc(pMesh->ntet,sizeof(Tetrahedron));

	if (pMesh->pver==NULL || pMesh->pnorm==NULL || pMesh->ptan==NULL || pMesh->pedg==NULL || pMesh->ptri==NULL || pMesh->ptet==NULL)
	{
		PRINT_ERROR();
		printf("In allocateInitialMeshMemory: could not allocate memory for the initial mesh.\n");
		return 0;
	}
#endif
#ifdef LS_MODE
	pMesh->pqua=(Quadrilateral*)calloc(pMesh->nqua,sizeof(Quadrilateral));
	pMesh->phex=(Hexahedron*)calloc(pMesh->nhex,sizeof(Hexahedron));

	if (pMesh->pver==NULL || pMesh->pqua==NULL || pMesh->phex==NULL)
	{
		PRINT_ERROR();
		printf("In allocateInitialMeshMemory: could not allocate memory for the initial mesh.\n");
		return 0;
	}
#endif

	return 1;
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

/* ******************************************************************************************************************** */
// The function discretizeCube fills the pMesh structure memory allocated with the allocateInitialMeshMemory function 
// It has the Mesh* variable (described in main.h) as input argument and it returns one on success, otherwise zero 
/* ******************************************************************************************************************** */
static int discretizeCube(Mesh* pMesh)
{
	int i=0, j=0, k=0, counter=0;

	// Test if the pMesh pointer (input variable) has a NULL value or not
	if (pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In discretizeCube: the pMesh (input) variable is set to the %p adress.\n",pMesh);
		return 0;
	}
	
	// Test if the memory was allocated for the different types of elements
#ifndef LS_MODE	
	if (pMesh->pver==NULL || pMesh->pnorm==NULL || pMesh->ptan==NULL || pMesh->pedg==NULL || pMesh->ptri==NULL || pMesh->ptet==NULL)
	{
		PRINT_ERROR();
		printf("In discretizeCube: one of the following pointer\npMesh->pver=%p\npMesh->pnorm=%p\npMesh->ptan=%p\npMesh->pedg=%p\n",pMesh->pver,pMesh->pnorm,pMesh->ptan,pMesh->pedg);
		printf("pMesh->ptri=%p\npMesh->ptet=%p\ndoes not have a valid allocated adress.\n",pMesh->ptri,pMesh->ptet);
		return 0;
	}
#endif
#ifdef LS_MODE
	if (pMesh->pver==NULL || pMesh->pqua==NULL || pMesh->phex==NULL)
	{
		PRINT_ERROR();
		printf("In discretizeCube: one of the following pointer\npMesh->pver=%p\npMesh->pqua=%p\npMesh->phex=%p\ndoes not have a valid allocated adress.\n",pMesh->pver,pMesh->pqua,pMesh->phex);
		return 0;
	}
#endif

	// Saving the cube vertices P[(i*N_Y+j)*N_Z+k]=(X_MIN+i*DELTA_X,Y_MIN+j*DELTA_Y,Z_MIN+k*DELTA_Z) i=0...N_X-1,j=0...N_Y-1,k=0...N_Z-1
	// Points are labelled according to boundary face, edge (10*face1+face2 with face1<face2), corner (100*face1+10*face2+face3 with face1<face2<face3), otherwise zero
	printf("Saving points. ");
	counter=N_X*N_Y*N_Z;
	if (pMesh->nver!=counter)
	{
		PRINT_ERROR();
		printf("IndiscretizeCube: expecting %d instead of %d for the number of vertices.\n",counter,pMesh->nver);
		return 0;
	}
	counter=0;
	for (i=0; i<N_X; i++)
	{
		for (j=0; j<N_Y; j++)
		{ 
			for (k=0; k<N_Z; k++)
			{
				// Saving the current point
				pMesh->pver[counter].x=X_MIN+i*DELTA_X;
				pMesh->pver[counter].y=Y_MIN+j*DELTA_Y;
				pMesh->pver[counter].z=Z_MIN+k*DELTA_Z;
				pMesh->pver[counter].label=labelPoint(i,j,k);
				counter++;
			}
		}
	}
	
#ifdef LS_MODE
	// Saving the hexahedra corresponding to each cube cell. Point is saved as 1+(a*N_Y+b)*N_Z+c a=0...N_X-1, b=0...N_Y-1, c=0...N_Z-1
	// Point reference (a,b,c) in a cube cell: 1=(i-1,j-1,k-1) 2=(i,j-1,k-1) 3=(i,j,k-1) 4=(i-1,j,k-1) 5=(i-1,j-1,k) 6=(i,j-1,k) 7=(i,j,k) 8=(i-1,j,k) i=1..N_X-1, j=1..N_Y-1, k=1..N_Z-1 	
	printf("Saving hexahedra. ");
	counter=(N_X-1)*(N_Y-1)*(N_Z-1);
	if (pMesh->nhex!=counter)
	{
		PRINT_ERROR();
		printf("In discretizeCube: expecting %d instead of %d for the number of hexahedra.\n",counter,pMesh->nhex);
		return 0;
	}
	counter=0;
	for (i=1; i<N_X; i++)
	{
		for (j=1; j<N_Y; j++)
		{
			for (k=1; k<N_Z; k++)
			{
				pMesh->phex[counter].p1=1+((i-1)*N_Y+(j-1))*N_Z+(k-1);
				pMesh->phex[counter].p2=1+(i*N_Y+(j-1))*N_Z+(k-1);
				pMesh->phex[counter].p3=1+(i*N_Y+j)*N_Z+(k-1);
				pMesh->phex[counter].p4=1+((i-1)*N_Y+j)*N_Z+(k-1);
				pMesh->phex[counter].p5=1+((i-1)*N_Y+(j-1))*N_Z+k;
				pMesh->phex[counter].p6=1+(i*N_Y+(j-1))*N_Z+k;
				pMesh->phex[counter].p7=1+(i*N_Y+j)*N_Z+k;
				pMesh->phex[counter].p8=1+((i-1)*N_Y+j)*N_Z+k;
				pMesh->phex[counter].label=0;
				counter++;
			}
		}
	}

	// Saving the boundary quadrilaterals on each faces (trigonometric orientation in front of it looking from the exterior of the cube)
	printf("Saving boundary quadrilaterals.\n");
	counter=2*((N_X-1)*(N_Y-1)+(N_X-1)*(N_Z-1)+(N_Y-1)*(N_Z-1));
	if (pMesh->nqua!=counter)
	{
		PRINT_ERROR();
		printf("In discretizeCube: expecting %d instead of %d for the number of quadrilaterals.\n",counter,pMesh->nqua);
		return 0;
	}
	counter=0;
	// Face 1 (j=0) with reference (a,b,c) in each quadrilateral corresponding to square cell: 1=(i-1,k-1) 2=(i,k-1) 3=(i,k) 4=(i-1,k)
	for (i=1; i<N_X; i++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Square 1234
			pMesh->pqua[counter].p1=1+(i-1)*N_Y*N_Z+(k-1);
			pMesh->pqua[counter].p2=1+i*N_Y*N_Z+(k-1);
			pMesh->pqua[counter].p3=1+i*N_Y*N_Z+k;
			pMesh->pqua[counter].p4=1+(i-1)*N_Y*N_Z+k;
			pMesh->pqua[counter].label=1;	
			counter++;
		}
	}
	// Face 2 (i=N_X-1) with reference (a,b,c) in square cell: 1=(j-1,k-1) 2=(j,k-1) 3=(j,k) 4=(j-1,k)
	for (j=1; j<N_Y; j++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Square 1234
			pMesh->pqua[counter].p1=1+((N_X-1)*N_Y+(j-1))*N_Z+(k-1);
			pMesh->pqua[counter].p2=1+((N_X-1)*N_Y+j)*N_Z+(k-1);			
			pMesh->pqua[counter].p3=1+((N_X-1)*N_Y+j)*N_Z+k;
			pMesh->pqua[counter].p4=1+((N_X-1)*N_Y+(j-1))*N_Z+k;
			pMesh->pqua[counter].label=2;	
			counter++;
		}
	}
	// Face 3 (j=N_Y-1) with reference (a,b,c) in square cell: 1=(i-1,k-1) 2=(i,k-1) 3=(i,k) 4=(i-1,k)
	for (i=1; i<N_X; i++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Square 1432
			pMesh->pqua[counter].p1=1+((i-1)*N_Y+(N_Y-1))*N_Z+(k-1);
			pMesh->pqua[counter].p2=1+((i-1)*N_Y+(N_Y-1))*N_Z+k;
			pMesh->pqua[counter].p3=1+(i*N_Y+(N_Y-1))*N_Z+k;
			pMesh->pqua[counter].p4=1+(i*N_Y+(N_Y-1))*N_Z+(k-1);
			pMesh->pqua[counter].label=3;	
			counter++;
		}
	}
	// Face 4 (i=0) with reference (a,b,c) in square cell: 1=(j-1,k-1) 2=(j,k-1) 3=(j,k) 4=(j-1,k)
	for (j=1; j<N_Y; j++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Square 1432
			pMesh->pqua[counter].p1=1+(j-1)*N_Z+(k-1);
			pMesh->pqua[counter].p2=1+(j-1)*N_Z+k;
			pMesh->pqua[counter].p3=1+j*N_Z+k;
			pMesh->pqua[counter].p4=1+j*N_Z+(k-1);
			pMesh->pqua[counter].label=4;	
			counter++;
		}
	}
	// Face 5 (k=0) with reference (a,b,c) in square cell: 1=(i-1,j-1) 2=(i,j-1) 3=(i,j) 4=(i-1,j)
	for (i=1; i<N_X; i++)
	{
		for (j=1; j<N_Y; j++)
		{
			// Square 1432
			pMesh->pqua[counter].p1=1+((i-1)*N_Y+(j-1))*N_Z;
			pMesh->pqua[counter].p2=1+((i-1)*N_Y+j)*N_Z;
			pMesh->pqua[counter].p3=1+(i*N_Y+j)*N_Z;
			pMesh->pqua[counter].p4=1+(i*N_Y+(j-1))*N_Z;
			pMesh->pqua[counter].label=5;
			counter++;
		}
	}
	// Face 6 (k=N_Z-1) with reference (a,b,c) in square cell: 1=(i-1,j-1) 2=(i,j-1) 3=(i,j) 4=(i-1,j)
	for (i=1; i<N_X; i++)
	{
		for (j=1; j<N_Y; j++)
		{
			// Square 1234
			pMesh->pqua[counter].p1=1+((i-1)*N_Y+(j-1))*N_Z+(N_Z-1);
			pMesh->pqua[counter].p2=1+(i*N_Y+(j-1))*N_Z+(N_Z-1);
			pMesh->pqua[counter].p3=1+(i*N_Y+j)*N_Z+(N_Z-1);
			pMesh->pqua[counter].p4=1+((i-1)*N_Y+j)*N_Z+(N_Z-1);
			pMesh->pqua[counter].label=6;	
			counter++;
		}
	}
#endif
#ifndef LS_MODE
	// Saving the six tetrahedra (positively oriented) inside each cube cell. Point is saved as 1+(a*N_Y+b)*N_Z+c a=0...N_X-1, b=0...N_Y-1, c=0...N_Z-1
	// Point reference (a,b,c) in cube cell: 1=(i-1,j-1,k-1) 2=(i,j-1,k-1) 3=(i,j,k-1) 4=(i-1,j,k-1) 5=(i-1,j-1,k) 6=(i,j-1,k) 7=(i,j,k) 8=(i-1,j,k) i=1..N_X-1, j=1..N_Y-1, k=1..N_Z-1 	
	printf("Saving tetrahedra. ");	// Warning: tetrahedra must respect adjacency between different cube cells (symmetric cutting inside each cube cell)
	counter=6*(N_X-1)*(N_Y-1)*(N_Z-1);
	if (pMesh->ntet!=counter)
	{
		PRINT_ERROR();
		printf("In discretizeCube: expecting %d instead of %d for the number of tetrahedra.\n",counter,pMesh->ntet);
		return 0;
	}
	counter=0;
	for (i=1; i<N_X; i++)
	{
		for (j=1; j<N_Y; j++)
		{
			for (k=1; k<N_Z; k++)
			{
				// Tetrahedron 1524 
				pMesh->ptet[counter].p1=1+((i-1)*N_Y+(j-1))*N_Z+(k-1);
				pMesh->ptet[counter].p2=1+((i-1)*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].p3=1+(i*N_Y+(j-1))*N_Z+(k-1);
				pMesh->ptet[counter].p4=1+((i-1)*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].label=0;
				counter++;
				// Tetrahedron 6254
				pMesh->ptet[counter].p1=1+(i*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].p2=1+(i*N_Y+(j-1))*N_Z+(k-1);
				pMesh->ptet[counter].p3=1+((i-1)*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].p4=1+((i-1)*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].label=0;
				counter++;
				// Tetrahedron 2346 
				pMesh->ptet[counter].p1=1+(i*N_Y+(j-1))*N_Z+(k-1);
				pMesh->ptet[counter].p2=1+(i*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].p3=1+((i-1)*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].p4=1+(i*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].label=0;
				counter++;
				// Tetrahedron 7836 
				pMesh->ptet[counter].p1=1+(i*N_Y+j)*N_Z+k;
				pMesh->ptet[counter].p2=1+((i-1)*N_Y+j)*N_Z+k;
				pMesh->ptet[counter].p3=1+(i*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].p4=1+(i*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].label=0;
				counter++;
				// Tetrahedron 6483
				pMesh->ptet[counter].p1=1+(i*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].p2=1+((i-1)*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].p3=1+((i-1)*N_Y+j)*N_Z+k;
				pMesh->ptet[counter].p4=1+(i*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].label=0;
				counter++;
				// Tetrahedron 8654
				pMesh->ptet[counter].p1=1+((i-1)*N_Y+j)*N_Z+k;
				pMesh->ptet[counter].p2=1+(i*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].p3=1+((i-1)*N_Y+(j-1))*N_Z+k;
				pMesh->ptet[counter].p4=1+((i-1)*N_Y+j)*N_Z+(k-1);
				pMesh->ptet[counter].label=0;
				counter++;
			}
		}
	}

	// Saving the boundary triangles on each faces (trigonometric orientation in front of it looking from the exterior of the cube)
	printf("Saving boundary triangles. ");
	counter=4*((N_X-1)*(N_Y-1)+(N_X-1)*(N_Z-1)+(N_Y-1)*(N_Z-1));
	if (pMesh->ntri!=counter)
	{
		PRINT_ERROR();
		printf("In discretizeCube: expecting %d instead of %d for the number of triangles.\n",counter,pMesh->ntri);
		return 0;
	}
	counter=0;
	// Face 1 (j=0) with reference (a,b,c) in square cell: 1=(i-1,k-1) 2=(i,k-1) 3=(i,k) 4=(i-1,k)
	for (i=1; i<N_X; i++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Triangle 124
			pMesh->ptri[counter].p1=1+(i-1)*N_Y*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+i*N_Y*N_Z+(k-1);
			pMesh->ptri[counter].p3=1+(i-1)*N_Y*N_Z+k;
			pMesh->ptri[counter].label=1;	
			counter++;
			// Triangle 234
			pMesh->ptri[counter].p1=1+i*N_Y*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+i*N_Y*N_Z+k;
			pMesh->ptri[counter].p3=1+(i-1)*N_Y*N_Z+k;
			pMesh->ptri[counter].label=1;	
			counter++;
		}
	}
	// Face 2 (i=N_X-1) with reference (a,b,c) in square cell: 1=(j-1,k-1) 2=(j,k-1) 3=(j,k) 4=(j-1,k)
	for (j=1; j<N_Y; j++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Triangle 124
			pMesh->ptri[counter].p1=1+((N_X-1)*N_Y+(j-1))*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+((N_X-1)*N_Y+j)*N_Z+(k-1);
			pMesh->ptri[counter].p3=1+((N_X-1)*N_Y+(j-1))*N_Z+k;
			pMesh->ptri[counter].label=2;	
			counter++;
			// Triangle 234
			pMesh->ptri[counter].p1=1+((N_X-1)*N_Y+j)*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+((N_X-1)*N_Y+j)*N_Z+k;
			pMesh->ptri[counter].p3=1+((N_X-1)*N_Y+(j-1))*N_Z+k;
			pMesh->ptri[counter].label=2;	
			counter++;
		}
	}
	// Face 3 (j=N_Y-1) with reference (a,b,c) in square cell: 1=(i-1,k-1) 2=(i,k-1) 3=(i,k) 4=(i-1,k)
	for (i=1; i<N_X; i++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Triangle 142
			pMesh->ptri[counter].p1=1+((i-1)*N_Y+(N_Y-1))*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+((i-1)*N_Y+(N_Y-1))*N_Z+k;
			pMesh->ptri[counter].p3=1+(i*N_Y+(N_Y-1))*N_Z+(k-1);
			pMesh->ptri[counter].label=3;	
			counter++;
			// Triangle 243
			pMesh->ptri[counter].p1=1+(i*N_Y+(N_Y-1))*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+((i-1)*N_Y+(N_Y-1))*N_Z+k;
			pMesh->ptri[counter].p3=1+(i*N_Y+(N_Y-1))*N_Z+k;
			pMesh->ptri[counter].label=3;	
			counter++;
		}
	}
	// Face 4 (i=0) with reference (a,b,c) in square cell: 1=(j-1,k-1) 2=(j,k-1) 3=(j,k) 4=(j-1,k)
	for (j=1; j<N_Y; j++)
	{
		for (k=1; k<N_Z; k++)
		{
			// Triangle 142
			pMesh->ptri[counter].p1=1+(j-1)*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+(j-1)*N_Z+k;
			pMesh->ptri[counter].p3=1+j*N_Z+(k-1);
			pMesh->ptri[counter].label=4;	
			counter++;
			// Triangle 243
			pMesh->ptri[counter].p1=1+j*N_Z+(k-1);
			pMesh->ptri[counter].p2=1+(j-1)*N_Z+k;
			pMesh->ptri[counter].p3=1+j*N_Z+k;
			pMesh->ptri[counter].label=4;	
			counter++;
		}
	}
	// Face 5 (k=0) with reference (a,b,c) in square cell: 1=(i-1,j-1) 2=(i,j-1) 3=(i,j) 4=(i-1,j)
	for (i=1; i<N_X; i++)
	{
		for (j=1; j<N_Y; j++)
		{
			// Triangle 142
			pMesh->ptri[counter].p1=1+((i-1)*N_Y+(j-1))*N_Z;
			pMesh->ptri[counter].p2=1+((i-1)*N_Y+j)*N_Z;
			pMesh->ptri[counter].p3=1+(i*N_Y+(j-1))*N_Z;
			pMesh->ptri[counter].label=5;
			counter++;
			// Triangle 243
			pMesh->ptri[counter].p1=1+(i*N_Y+(j-1))*N_Z;
			pMesh->ptri[counter].p2=1+((i-1)*N_Y+j)*N_Z;
			pMesh->ptri[counter].p3=1+(i*N_Y+j)*N_Z;
			pMesh->ptri[counter].label=5;	
			counter++;
		}
	}
	// Face 6 (k=N_Z-1) with reference (a,b,c) in square cell: 1=(i-1,j-1) 2=(i,j-1) 3=(i,j) 4=(i-1,j)
	for (i=1; i<N_X; i++)
	{
		for (j=1; j<N_Y; j++)
		{
			// Triangle 124
			pMesh->ptri[counter].p1=1+((i-1)*N_Y+(j-1))*N_Z+(N_Z-1);
			pMesh->ptri[counter].p2=1+(i*N_Y+(j-1))*N_Z+(N_Z-1);
			pMesh->ptri[counter].p3=1+((i-1)*N_Y+j)*N_Z+(N_Z-1);
			pMesh->ptri[counter].label=6;	
			counter++;
			// Triangle 234
			pMesh->ptri[counter].p1=1+(i*N_Y+(j-1))*N_Z+(N_Z-1);
			pMesh->ptri[counter].p2=1+(i*N_Y+j)*N_Z+(N_Z-1);
			pMesh->ptri[counter].p3=1+((i-1)*N_Y+j)*N_Z+(N_Z-1);
			pMesh->ptri[counter].label=6;	
			counter++;
		}
	}

	// Saving the boundary edges called ridges and labelled 10*face1+ face2 (with label face1<face2)
	printf("Saving geometry (ridges, corners).\n");
	counter=4*((N_X-1)+(N_Y-1)+(N_Z-1));
	if (pMesh->nedg!=counter)
	{
		PRINT_ERROR();
		printf("IndiscretizeCube: expecting %d instead of %d for the number of triangles.\n",counter,pMesh->nedg);
		return 0;
	}
	counter=0;
	for (i=1; i<N_X; i++)
	{
		// Edge 1 (j=0,k=0)
		pMesh->pedg[counter].p1=1+(i-1)*N_Y*N_Z;
		pMesh->pedg[counter].p2=1+i*N_Y*N_Z;
		pMesh->pedg[counter].label=15;
		counter++;	
		// Edge 2 (j=N_Y-1,k=0) 
		pMesh->pedg[counter].p1=1+((i-1)*N_Y+(N_Y-1))*N_Z;
		pMesh->pedg[counter].p2=1+(i*N_Y+(N_Y-1))*N_Z;
		pMesh->pedg[counter].label=35;
		counter++;	
		// Edge 3 (j=N_Y-1,k=N_Z-1) 
		pMesh->pedg[counter].p1=1+((i-1)*N_Y+(N_Y-1))*N_Z+(N_Z-1);
		pMesh->pedg[counter].p2=1+(i*N_Y+(N_Y-1))*N_Z+(N_Z-1);
		pMesh->pedg[counter].label=36;
		counter++;	
		// Edge 4 (j=0,k=N_Z-1) 
		pMesh->pedg[counter].p1=1+(i-1)*N_Y*N_Z+(N_Z-1);
		pMesh->pedg[counter].p2=1+i*N_Y*N_Z+(N_Z-1);
		pMesh->pedg[counter].label=16;
		counter++;	
	} 
	for (j=1; j<N_Y; j++)
	{
		// Edge 5 (i=0, k=0)
		pMesh->pedg[counter].p1=1+(j-1)*N_Z;
		pMesh->pedg[counter].p2=1+j*N_Z;
		pMesh->pedg[counter].label=45;
		counter++;	
		// Edge 6 (i=N_X-1, k=0) 
		pMesh->pedg[counter].p1=1+((N_X-1)*N_Y+(j-1))*N_Z;
		pMesh->pedg[counter].p2=1+((N_X-1)*N_Y+j)*N_Z;
		pMesh->pedg[counter].label=25;
		counter++;	
		// Edge 7 (i=N_X-1, k=N_Z-1) 
		pMesh->pedg[counter].p1=1+((N_X-1)*N_Y+(j-1))*N_Z+(N_Z-1);
		pMesh->pedg[counter].p2=1+((N_X-1)*N_Y+j)*N_Z+(N_Z-1);
		pMesh->pedg[counter].label=26;
		counter++;	
		// Edge 8 (i=0, k=N_Z-1) 
		pMesh->pedg[counter].p1=1+(j-1)*N_Z+(N_Z-1);
		pMesh->pedg[counter].p2=1+j*N_Z+(N_Z-1);
		pMesh->pedg[counter].label=46;
		counter++;	
	}
	for (k=1; k<N_Z; k++)
	{
		// Edge 9 (i=0,j=0) 
		pMesh->pedg[counter].p1=1+(k-1);
		pMesh->pedg[counter].p2=1+k;
		pMesh->pedg[counter].label=14;
		counter++;	
		// Edge 10 (i=N_X-1,j=0) 
		pMesh->pedg[counter].p1=1+(N_X-1)*N_Y*N_Z+(k-1);
		pMesh->pedg[counter].p2=1+(N_X-1)*N_Y*N_Z+k;
		pMesh->pedg[counter].label=12;
		counter++;	
		// Edge 11 (i=N_X-1, j=N_Y-1) 
		pMesh->pedg[counter].p1=1+((N_X-1)*N_Y+(N_Y-1))*N_Z+(k-1);
		pMesh->pedg[counter].p2=1+((N_X-1)*N_Y+(N_Y-1))*N_Z+k;
		pMesh->pedg[counter].label=23;
		counter++;	
		// Edge 12 (i=0, j=N_Y-1) 
		pMesh->pedg[counter].p1=1+(N_Y-1)*N_Z+(k-1);
		pMesh->pedg[counter].p2=1+(N_Y-1)*N_Z+k;
		pMesh->pedg[counter].label=34;
		counter++;	
	}
	// Saving the normals (pointing outwards the cube)
	counter=2*((N_X-2)*(N_Y-2)+(N_X-2)*(N_Z-2)+(N_Y-2)*(N_Z-2));
	if (pMesh->nnorm!=counter)
	{
		PRINT_ERROR();
		printf("In discretizeCube: expecting %d instead of %d for the number of normal vectors.\n",counter,pMesh->nnorm);
		return 0;
	}
	counter=0;
	// Face 1 (j=0)
	for (i=1;i<N_X-1;i++)
	{
		for (k=1;k<N_Z-1;k++)
		{
			pMesh->pnorm[counter].x=0.;
			pMesh->pnorm[counter].y=-1.;
			pMesh->pnorm[counter].z=0.;
			pMesh->pnorm[counter].p=1+i*N_Y*N_Z+k;
			counter++;
		}
	} 
	// Face 2 (i=N_X-1)
	for (j=1;j<N_Y-1;j++)
	{
		for (k=1;k<N_Z-1;k++)
		{
			pMesh->pnorm[counter].x=1.;
			pMesh->pnorm[counter].y=0.;
			pMesh->pnorm[counter].z=0.;
			pMesh->pnorm[counter].p=1+((N_X-1)*N_Y+j)*N_Z+k;
			counter++;
		}
	}
	// Face 3 (j=N_Y-1)
	for (i=1;i<N_X-1;i++)
	{
		for (k=1;k<N_Z-1;k++)
		{
			pMesh->pnorm[counter].x=0.;
			pMesh->pnorm[counter].y=1.;
			pMesh->pnorm[counter].z=0.;
			pMesh->pnorm[counter].p=1+(i*N_Y+(N_Y-1))*N_Z+k;
			counter++;
		}
	}
	// Face 4 (i=0)
	for (j=1;j<N_Y-1;j++)
	{
		for (k=1;k<N_Z-1;k++)
		{
			pMesh->pnorm[counter].x=-1.;
			pMesh->pnorm[counter].y=0.;
			pMesh->pnorm[counter].z=0.;
			pMesh->pnorm[counter].p=1+j*N_Z+k;
			counter++;
		}
	}
	// Face 5 (k=0)
 	for (i=1;i<N_X-1;i++)
	{
		for (j=1;j<N_Y-1;j++)
		{
			pMesh->pnorm[counter].x=0.;
			pMesh->pnorm[counter].y=0.;
			pMesh->pnorm[counter].z=-1.;
			pMesh->pnorm[counter].p=1+(i*N_Y+j)*N_Z;
			counter++;
		}
	} 
	// Face 6 (k=N_Z-1)
 	for (i=1;i<N_X-1;i++)
	{
		for (j=1;j<N_Y-1;j++)
		{
			pMesh->pnorm[counter].x=0.;
			pMesh->pnorm[counter].y=0.;
			pMesh->pnorm[counter].z=1.;
			pMesh->pnorm[counter].p=1+(i*N_Y+j)*N_Z+(N_Z-1);
			counter++;
		}
	} 
	// Saving the tangents
	counter=4*((N_X-2)+(N_Y-2)+(N_Z-2));
	if (pMesh->ntan!=counter)
	{
		PRINT_ERROR();
		printf("In discretizeCube: expecting %d instead of %d for the number of tangent vectors.\n",counter,pMesh->ntan);
		return 0;
	}
	counter=0;
	for (i=1; i<N_X-1; i++)
	{
		// Edge 1 (j=0,k=0)
		pMesh->ptan[counter].x=1.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+i*N_Y*N_Z;
		counter++;	
		// Edge 2 (j=N_Y-1,k=0) 
		pMesh->ptan[counter].x=1.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+(i*N_Y+(N_Y-1))*N_Z;
		counter++;	
		// Edge 3 (j=N_Y-1,k=N_Z-1) 
		pMesh->ptan[counter].x=1.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+(i*N_Y+(N_Y-1))*N_Z+(N_Z-1);
		counter++;	
		// Edge 4 (j=0,k=N_Z-1) 
		pMesh->ptan[counter].x=1.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+i*N_Y*N_Z+(N_Z-1);
		counter++;	
	} 
	for (j=1; j<N_Y-1; j++)
	{
		// Edge 5 (i=0, k=0)
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=1.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+j*N_Z;
		counter++;	
		// Edge 6 (i=N_X-1, k=0) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=1.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+((N_X-1)*N_Y+j)*N_Z;
		counter++;	
		// Edge 7 (i=N_X-1, k=N_Z-1) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=1.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+((N_X-1)*N_Y+j)*N_Z+(N_Z-1);
		counter++;	
		// Edge 8 (i=0, k=N_Z-1) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=1.;
		pMesh->ptan[counter].z=0.;
		pMesh->ptan[counter].p=1+j*N_Z+(N_Z-1);
		counter++;	
	}
	for (k=1; k<N_Z-1; k++)
	{
		// Edge 9 (i=0,j=0) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=1.;
		pMesh->ptan[counter].p=1+k;
		counter++;	
		// Edge 10 (i=N_X-1,j=0) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=1.;
		pMesh->ptan[counter].p=1+(N_X-1)*N_Y*N_Z+k;
		counter++;	
		// Edge 11 (i=N_X-1, j=N_Y-1) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=1.;
		pMesh->ptan[counter].p=1+((N_X-1)*N_Y+(N_Y-1))*N_Z+k;
		counter++;	
		// Edge 12 (i=0, j=N_Y-1) 
		pMesh->ptan[counter].x=0.;
		pMesh->ptan[counter].y=0.;
		pMesh->ptan[counter].z=1.;
		pMesh->ptan[counter].p=1+(N_Y-1)*N_Z+k;
		counter++;	
	}
#endif

	return 1;
}

/* *************************************************************************************************************************************************************************************************** */
// The function writingMeshFile writes data (discretization of the cube) saved in the pMesh structure into a file located at fileLocation (warning: reset and overwrite file if already exists)
// It has the Mesh* variable (described in main.h ) as input argument and it returns zero if an error occurred, otherwise one is returned
/* *************************************************************************************************************************************************************************************************** */
int writingMeshFile(char* fileLocation, Mesh* pMesh)
{
	int i=0, iMax=0;
	Point *pVertex=NULL;
#ifndef LS_MODE 
	Vector *pNormal=NULL, *pTangent=NULL;
	Edge *pEdge=NULL;
	Triangle *pTriangle=NULL;
	Tetrahedron *pTetrahedron=NULL;
#endif
#ifdef LS_MODE
	Hexahedron *pHexahedron=NULL;
	Quadrilateral *pQuadrilateral=NULL;
#endif
	FILE *meshFile=NULL;

	// Testing if the input fileLocation variable is pointing to NULL
	if (fileLocation==NULL)
	{	
		PRINT_ERROR();
		printf("In writingMeshFile: the input fileLocation variable is pointing to %p adress.\n",fileLocation);
		return 0;
	}

	// Testing if the input pMesh variable is pointing to NULL
	if (pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: the pMesh (input) variable is set to the %p adress.\n",pMesh);
		return 0;
	}

	// Test if the memory was allocated for the different types of elements
#ifndef LS_MODE	
	if (pMesh->pver==NULL || pMesh->pnorm==NULL || pMesh->ptan==NULL || pMesh->pedg==NULL || pMesh->ptri==NULL || pMesh->ptet==NULL)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: one of the pointer\npMesh->pver=%p\npMesh->pnorm=%p\npMesh->ptan=%p\npMesh->pedg=%p\n",pMesh->pver,pMesh->pnorm,pMesh->ptan,pMesh->pedg);
		printf("pMesh->ptri=%p\npMesh->ptet=%p\ndoes not have a valid allocated adress.\n",pMesh->ptri,pMesh->ptet);
		return 0;
	}
#endif
#ifdef LS_MODE
	if (pMesh->pver==NULL || pMesh->pqua==NULL || pMesh->phex==NULL)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: one of the pointer\npMesh->pver=%p\npMesh->pqua=%p\npMesh->phex=%p\ndoes not have a valid allocated adress.\n",pMesh->pver,pMesh->pqua,pMesh->phex);
		return 0;
	}
#endif

	// Testing if the file is well opened (warning: reset and overwrite on *.mesh file if already exists)
	printf("Opening %s file. ",fileLocation); 
	meshFile=fopen(fileLocation,"w+");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned
	if (meshFile==NULL)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: could not write in %s file.\n",fileLocation);
		return 0;
	}
	printf("Start writing. ");	
	fprintf(meshFile,"MeshVersionFormatted 2\n\nDimension 3\n");

	// Writing *.mesh file starting by Vertices
	iMax=pMesh->nver;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Vertices in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nVertices\n%d\n",iMax);	
	for (i=0; i<iMax; i++)
	{
		pVertex=&pMesh->pver[i];
		fprintf(meshFile,"%le %le %le %d \n",pVertex->x,pVertex->y,pVertex->z,pVertex->label);
	}
#ifdef LS_MODE
	// Quadrilaterals
	iMax=pMesh->nqua;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Quadrilaterals in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nQuadrilaterals\n%d\n",iMax);	
	for (i=0; i<iMax; i++)
	{
		pQuadrilateral=&pMesh->pqua[i];
		fprintf(meshFile,"%d %d %d %d %d \n",pQuadrilateral->p1,pQuadrilateral->p2,pQuadrilateral->p3,pQuadrilateral->p4,pQuadrilateral->label);
	}

	// Hexahedra
	iMax=pMesh->nhex;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Hexahedra in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nHexahedra\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pHexahedron=&pMesh->phex[i];
		fprintf(meshFile,"%d %d %d %d ",pHexahedron->p1,pHexahedron->p2,pHexahedron->p3,pHexahedron->p4);
		fprintf(meshFile,"%d %d %d %d %d \n",pHexahedron->p5,pHexahedron->p6,pHexahedron->p7,pHexahedron->p8,pHexahedron->label);
	}
#endif
#ifndef LS_MODE
	// Triangles
	iMax=pMesh->ntri;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Triangles in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nTriangles\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pTriangle=&pMesh->ptri[i];
		fprintf(meshFile,"%d %d %d %d \n",pTriangle->p1,pTriangle->p2,pTriangle->p3,pTriangle->label);
	}

	// Edges
	iMax=pMesh->nedg;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Edges/Ridges in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nEdges\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pEdge=&pMesh->pedg[i];
		fprintf(meshFile,"%d %d %d \n",pEdge->p1,pEdge->p2,pEdge->label);
	}
	// Ridges
	fprintf(meshFile,"\nRidges\n%d\n",iMax);
	for (i=0;i<iMax;i++)
	{
		fprintf(meshFile,"%d \n",i+1);
	}

	// Corners
	iMax=pMesh->ncor;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Corners/RequiredVertices in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	else if (iMax!=8)
	{
		printf("\nWarning in writingMeshFile function: the number of Corners/RequiredVertices should be 8 instead of %d in %s file.\n",iMax,fileLocation);
	}
	fprintf(meshFile,"\nCorners\n%d\n",iMax);
	iMax=pMesh->nver;
	for (i=0; i<iMax; i++)
	{
		pVertex=&pMesh->pver[i];
		if (pVertex->label>100)
		{
			fprintf(meshFile,"%d \n",i+1);
		}
	}
	// RequiredVertices
	fprintf(meshFile,"\nRequiredVertices\n%d\n",pMesh->ncor);
	for (i=0; i<iMax; i++)
	{
		pVertex=&pMesh->pver[i];
		if (pVertex->label>100)
		{
			fprintf(meshFile,"%d \n",i+1);
		}
	}

	// Tetrahedra	
	iMax=pMesh->ntet;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Tetrahedra in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nTetrahedra\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pTetrahedron=&pMesh->ptet[i];
		fprintf(meshFile,"%d %d %d %d %d \n",pTetrahedron->p1,pTetrahedron->p2,pTetrahedron->p3,pTetrahedron->p4,pTetrahedron->label);
	}

	// Normals 
	iMax=pMesh->nnorm;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Normals/NormalAtVertices in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nNormals\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pNormal=&pMesh->pnorm[i];
		fprintf(meshFile,"%le %le %le \n",pNormal->x,pNormal->y,pNormal->z);
	}
	// NormalAtVertices
	fprintf(meshFile,"\nNormalAtVertices\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pNormal=&pMesh->pnorm[i];
		fprintf(meshFile,"%d %d \n",pNormal->p,i+1);
	}

	// Tangents 
	iMax=pMesh->ntan;
	if (iMax<1)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: expecting a positive value instead of %d for the number of Tangents/TangentAtVertices in %s file.\n",iMax,fileLocation);
		fprintf(meshFile,"\nEnd");
		closeTheFile(&meshFile);		
		return 0;
	}
	fprintf(meshFile,"\nTangents\n%d\n",iMax);
	for (i=0; i<iMax; i++)
	{
		pTangent=&pMesh->ptan[i];
		fprintf(meshFile,"%le %le %le \n",pTangent->x,pTangent->y,pTangent->z);
	}
	// TangentAtVertices
	fprintf(meshFile,"\nTangentAtVertices\n%d\n",iMax);
	for (i=0;i<iMax;i++)
	{
		pTangent=&pMesh->ptan[i];
		fprintf(meshFile,"%d %d \n",pTangent->p,i+1);
	}
#endif	
	fprintf(meshFile,"\nEnd");

	// Testing if file is well closed
	if(fclose(meshFile))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		PRINT_ERROR();
		printf("In writingMeshFile: could not close %s file properly.\n",fileLocation);
		meshFile=NULL;
		return 0;
	}
	meshFile=NULL;
	printf("File closed.\n");

	return 1;
}

/* ********************************************************************************************************************************************************************************** */
// The function meshCube checks if mesh data can be copied from fileLocation otherwise (over)write a cube.mesh file discretize an initial cube thanks to preprocessor parameters  
// It has the char* fileLocation and Mesh* variable (described in main.h) as input arguments and it returns (+/-) one on success, otherwise zero
/* ********************************************************************************************************************************************************************************** */
int meshCube(char* fileLocation, Mesh *pMesh)
{
	int outPut=0;

	// Test if the pMesh pointer (input variable) has a NULL value or not
	if (pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In meshCube: the pMesh (input) variable is set to the %p adress.\n",pMesh);
		return 0;
	}

	if (initialFileExists(fileLocation))
	{
		// copy the initial mesh file into the one named cube.mesh (name imposed here, important for adaptCube function)
		if(!copyFileLocation(fileLocation,"cube.mesh"))
		{
			PRINT_ERROR();
			printf("In meshCube: copyFileLocation function returned zero instead of one.\n");
			return 0;
		}
	}
	else
	{
		outPut=initializeCubeDiscretization(pMesh);

		// Load parameters of main.h and meshCube.h files
		if (outPut==-1)
		{
			return -1;
		}
		else if (outPut==0)
		{
			PRINT_ERROR();
			printf("In meshCube: initializeCubeDiscretization function returned zero instead of (+/-) one.\n");
			return 0;
		}
	
		// Allocating memory
		if(!allocateInitialMeshMemory(pMesh))
		{
			PRINT_ERROR();
			printf("In meshCube: allocateInitialMeshMemory function returned zero instead of one.\n");
			return 0;
		}

		// Meshing the cube according to the properly loaded parameters
		if(!discretizeCube(pMesh))
		{
			PRINT_ERROR();
			printf("In meshCube: discretizeCube function returned zero instead of one.\n");
			return 0;
		}

		// Saving the mesh in cube.mesh file
		if(!writingMeshFile("cube.mesh",pMesh))
		{
			PRINT_ERROR();
			printf("In meshCube: writingMeshFile function returned zero instead of one.\n");
			return 0;
		}
		
	}

	return 1;
}

