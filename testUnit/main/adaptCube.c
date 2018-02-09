#include "adaptCube.h"

/*
// Prototypes of non-static static functions in adaptCube.c
int readAndAllocateMesh(char* fileLocation, Mesh* pMesh);
int evaluatingMetricOnMesh(Mesh* pMesh, ChemicalSystem* pChemicalSystem);
int getLevelSetQuadrilaterals(Mesh* pMesh);
int initialLevelSetInMeshExists(Mesh* pMesh);
int writingSolFile(char* fileLocation, Mesh* pMesh);
int adaptCube(char* fileLocation, char* commandLineMmg3dIsotropic, char* commandLineMmg3dMetric, char* commandLineMmg3dLevelSet, Mesh* pMesh, ChemicalSystem* pChemicalSystem);
*/

// Prototypes of static functions in adaptCube.c
static double evaluatingHessianAtVertices(ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbital, Point* pPoint, double gradient[3], double hessian[6], int primitive);
static int getRootsFromCoefficients(double roots[3], double coefficients[3]);
static int findEigenvaluesOfSymmetricMatrix(double matrix[6], double roots[3]);
static int initializeLevelSet(Mesh* pMesh);
#ifdef LS_MODE
	static int initializeAdjacency(Mesh* pMesh);
#endif

/* ************************************************************************************************************************************************************************************ */
// The function readAndAllocateMesh reads the *.mesh file at fileLocation (warning: file must exists), checks the expected syntax, allocates and fills memory in the pMesh structure
// It has the char* and Mesh* variable (described in main.h) as input arguments and it returns zero if an error occurred, otherwise one
/* ************************************************************************************************************************************************************************************ */
int readAndAllocateMesh(char* fileLocation, Mesh* pMesh)
{
	char readStringIn[21]={'\0'}, *readStringOut=NULL, keyword[4]={'\0'};
	int readIntegerIn=0, readIntegerOut=0, i=0, j=0, k=0;
	double readDouble=0.;
	FILE *meshFile=NULL;

	// Testing if the input pMesh variable is pointing to NULL otherwise initialize its structure to zero for variables and NULL for pointers
	if (pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: the input pMesh variable is pointing to %p adress.\n",pMesh);
		return 0;	
	}
	initializeMeshStructure(pMesh);

	// Since the file must have been previously created, we check if it is the case 
	if(!initialFileExists(fileLocation))
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: initialFileExists function returned zero instead of one.\n");
		return 0;
	}

	// Testing if the file is well opened (warning: file must have been previously created)
	printf("\nOpening %s file. ",fileLocation);
	meshFile=fopen(fileLocation,"r");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned
	if (meshFile==NULL)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: could not read %s file.\n",fileLocation);
		return 0;
	}
	printf("Reading and saving mesh data. ");		

	// Reading precision (expecting MeshVersionFormatted 2): in general 1=single, 2=double, MeshVersionUnformatted for binary (.meshb) file (not considered here)
	readStringOut=fgets(readStringIn,21,meshFile);		// fgets function returns a null pointer if it failed, otherwise it returns the same readStringIn parameters
	readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn); 	// fscanf function returns the number of input items successfully matched and assigned, which can be fewer than provided for

	// Important remark: white space characters, such as blanks and newline characters, causes fscanf (scanf and sscanf) to read, but not to store, all consecutive white space characters in 
	//                   the input up to the next character that is not white space. One white space character in format-string matches any combination of white space characters in the input.

	if(readStringOut==NULL || readIntegerOut!=1) 	// strcmp function returns 0 if the two strings are equal, otherwise a >0 (resp. <0) value if In is greater (resp. less) than Out variable
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in MeshVersionFormatted.\n",readStringOut,readIntegerOut);
		closeTheFile(&meshFile);		
		return 0;
	}
	if (strcmp(readStringIn,"MeshVersionFormatted") || readIntegerIn!=2)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: expecting MeshVersionFormatted 2 instead of %s %d value.\n",readStringIn,readIntegerIn);
		closeTheFile(&meshFile);
		return 0;
	}

	// Reading space dimension (expecting Dimension 3)
	readStringOut=fgets(readStringIn,10,meshFile);
	readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
	if(readStringOut==NULL || readIntegerOut!=1)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Dimension.\n",readStringOut,readIntegerOut);
		closeTheFile(&meshFile);
		return 0;
	}
	if (strcmp(readStringIn,"Dimension") || readIntegerIn!=3)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: expecting Dimension 3 instead of %s %d value.\n",readStringIn,readIntegerIn);
		closeTheFile(&meshFile);
		return 0;
	}

	// Read number of Vertices and store it in pMesh->nver (we impose Vertices keyword first in order to check then the validity of integers referring to the position in the array of Vertices)
	readStringOut=fgets(readStringIn,9,meshFile);
	readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
	if(readStringOut==NULL || readIntegerOut!=1)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Vertices.\n",readStringOut,readIntegerOut);
		closeTheFile(&meshFile);
		return 0;
	}
	if (strcmp(readStringIn,"Vertices") || readIntegerIn<1)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: expecting Vertices Nver instead of %s %d (positive value).\n",readStringIn,readIntegerIn);
		closeTheFile(&meshFile);
		return 0;
	}
	pMesh->nver=readIntegerIn;

	// Allocating memory for Vertices and check if it works
	pMesh->pver=(Point*)calloc(readIntegerIn,sizeof(Point));
	if (pMesh->pver==NULL)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: could not allocate memory for Vertices.\n");
		closeTheFile(&meshFile);
		return 0;
	}

	// Saving Vertices in pMesh->pver and check if it works
	j=0;
	for (k=0; k<readIntegerIn; k++)
	{	
		j+=fscanf(meshFile,"%lf ",&readDouble);
		pMesh->pver[k].x=readDouble;
		j+=fscanf(meshFile,"%lf ",&readDouble);
		pMesh->pver[k].y=readDouble;					
		j+=fscanf(meshFile,"%lf ",&readDouble);
		pMesh->pver[k].z=readDouble;
		j+=fscanf(meshFile,"%d ",&readIntegerOut);
		pMesh->pver[k].label=readIntegerOut;
		pMesh->pver[k].value=1.;	// Important here: initialize value to one if metric is evaluated on mesh (Warning in adaptCube: metric=initial value if spin are different)
	}
	if (j!=4*readIntegerIn)
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: expecting %d Vertices instead of %d properly saved.\n",readIntegerIn,j/4);
		closeTheFile(&meshFile);
		return 0;
	}
	
	// Reading the next three letters and distinguishing the different cases until "End" keyword
	i=0;
	do {
		readStringOut=fgets(keyword,4,meshFile);
		if(readStringOut==NULL)
		{
			PRINT_ERROR();
			printf("In readAndAllocateMesh: wrong return of fgets (=%p) in %s file.\n",readStringOut,fileLocation);
			closeTheFile(&meshFile);
			return 0;
		}
		i++;
#ifdef LS_MODE
		if (!strcmp(keyword,"Qua"))
		{	
			// Read number of Quadrilaterals and store it in pMesh->nqua
			readStringOut=fgets(readStringIn,12,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Quadrilaterals.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"drilaterals") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Quadrilaterals Nqua instead of Qua%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			pMesh->nqua=readIntegerIn;

			// Allocating memory for Quadrilaterals and check if it works
			pMesh->pqua=(Quadrilateral*)calloc(readIntegerIn,sizeof(Quadrilateral));
			if (pMesh->pqua==NULL)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: could not allocate memory for Quadrilaterals.\n");
				closeTheFile(&meshFile);
				return 0;
			}

			// Saving Quadrilaterals in pMesh->pqua and check if it works
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{	
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: first point of Quadrilateral %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->pqua[k].p1=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: second point of Quadrilateral %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->pqua[k].p2=readIntegerOut;
					
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: third point of Quadrilateral %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->pqua[k].p3=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: fourth point of Quadrilateral %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->pqua[k].p4=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				pMesh->pqua[k].label=readIntegerOut;		
			}
			if (j!=5*readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Quadrilaterals instead of %d properly saved.\n",readIntegerIn,j/5);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (!strcmp(keyword,"Hex"))
		{	
			// Read number of Hexahedra and store it in pMesh->nhex
			readStringOut=fgets(readStringIn,7,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Hexahedra.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"ahedra") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Hexahedra Nhex instead of Hex%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			pMesh->nhex=readIntegerIn;

			// Allocating memory for Hexahedra and check if it works
			pMesh->phex=(Hexahedron*)calloc(readIntegerIn,sizeof(Hexahedron));
			if (pMesh->phex==NULL)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: could not allocate memory for Hexahedra.\n");
				closeTheFile(&meshFile);
				return 0;
			}

			// Saving Hexahedra in pMesh->phex and check if it works
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{	
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: first point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p1=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: second point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p2=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: third point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p3=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: fourth point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p4=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: fifth point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p5=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: sixth point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p6=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: seventh point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p7=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: eighth point of Hexahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->phex[k].p8=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				pMesh->phex[k].label=readIntegerOut;		
			}
			if (j!=9*readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Hexahedra instead of %d properly saved.\n",readIntegerIn,j/9);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (i>3)
		{
			i=11;	// Security to end the do..while loop if the LS_MODE (only four keywords should be read: Ver, Hex, Qua, End)
		}
#endif
#ifndef LS_MODE
		if (!strcmp(keyword,"Tri"))
		{	
			// Read number of Triangles and store it in pMesh->ntri
			readStringOut=fgets(readStringIn,7,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Triangles.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"angles") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Triangles Ntri instead of Tri%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			pMesh->ntri=readIntegerIn;

			// Allocating memory for Triangles and check if it works
			pMesh->ptri=(Triangle*)calloc(readIntegerIn,sizeof(Triangle));
			if (pMesh->ptri==NULL)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: could not allocate memory for Triangles.\n");
				closeTheFile(&meshFile);
				return 0;
			}

			// Saving Triangles in pMesh->ptri and check if it works
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{	
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: first point of Triangle %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptri[k].p1=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: second point of Triangle %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptri[k].p2=readIntegerOut;
	
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: third point of Triangle %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptri[k].p3=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				pMesh->ptri[k].label=readIntegerOut;		
			}
			if (j!=4*readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Triangles instead of %d properly saved.\n",readIntegerIn,j/4);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (!strcmp(keyword,"Edg"))
		{	
			// Read number of Edges and store it in pMesh->nedg
			readStringOut=fgets(readStringIn,3,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Edges.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"es") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Edges Nedg instead of Edg%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			if (pMesh->nedg!=0 && readIntegerIn!=pMesh->nedg)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: number of Ridges %d different from number of Edges %d in %s file.\n",pMesh->nedg,readIntegerIn,fileLocation);
				closeTheFile(&meshFile);
				return 0;
			}
			if (pMesh->nedg==0)
			{
				pMesh->nedg=readIntegerIn;
				// Allocating memory for Edges and check if it works			
				pMesh->pedg=(Edge*)calloc(readIntegerIn,sizeof(Edge));
				if (pMesh->pedg==NULL)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: could not allocate memory for Edges.\n");
					closeTheFile(&meshFile);
					return 0;
				}
			}

			// Saving Edges in pMesh->pedg and check if it works
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{	
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: first point of Edge %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->pedg[k].p1=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: second point of Edge %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->pedg[k].p2=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				pMesh->pedg[k].label=readIntegerOut;		
			}	
			if (j!=3*readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Edges instead of %d properly saved.\n",readIntegerIn,j/3);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (!strcmp(keyword,"Rid"))
		{	
			// Read number of Ridges and store it in pMesh->nedg
			readStringOut=fgets(readStringIn,4,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Ridges.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"ges") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Ridges Nrid instead of Rid%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			if (pMesh->nedg!=0&&readIntegerIn!=pMesh->nedg)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: number of Edges %d different from number of Ridges %d in %s file.\n",pMesh->nedg,readIntegerIn,fileLocation);
				closeTheFile(&meshFile);
				return 0;
			}
			if (pMesh->nedg==0)
			{
				pMesh->nedg=readIntegerIn;
				// Allocating memory for Ridges and check if it works			
				pMesh->pedg=(Edge*)calloc(readIntegerIn,sizeof(Edge));
				if (pMesh->pedg==NULL)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: could not allocate memory for Ridges.\n");
					closeTheFile(&meshFile);
					return 0;
				}
			}

			// Checking if Ridges are well stored in increasing order
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut!=k+1)
				{
					printf("\nWarning in readAndAllocateMesh function: Ridge %d is not numbered %d (in increasing order).\n",readIntegerOut,k+1);
				}
			}
			if (j!=readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Ridges instead of %d properly saved.\n",readIntegerIn,j);
				closeTheFile(&meshFile);
				return 0;
			}
		}
		else if (!strcmp(keyword,"Cor"))
		{	
			// Read number of Corners and store it in pMesh->ncor
			readStringOut=fgets(readStringIn,5,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Corners.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"ners") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Corners Ncor instead of Cor%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			if (readIntegerIn>pMesh->nver)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: number of Corners %d greater than number of Vertices %d in %s file.\n",readIntegerIn,pMesh->nver,fileLocation);
				closeTheFile(&meshFile);
				return 0;
			}
			if (pMesh->ncor!=0&&readIntegerIn!=pMesh->ncor)
			{
				printf("\nWarning: number of RequiredVertices %d different from number of Corners %d in %s file.\n",pMesh->ncor,readIntegerIn,fileLocation);
			}
			pMesh->ncor=readIntegerIn;

			// Check number of Corners
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
			}
			if (j!=readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Corners instead of %d properly read.\n",readIntegerIn,j);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (!strcmp(keyword,"Req"))
		{	
			// Read number of RequiredVertices and store it in pMesh->ncor
			readStringOut=fgets(readStringIn,14,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in RequiredVertices.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"uiredVertices") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting RequiredVertices Nreq instead of Req%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			if (readIntegerIn>pMesh->nver)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: number of RequiredVertices %d greater than number of Vertices %d in %s file.\n",readIntegerIn,pMesh->nver,fileLocation);
				closeTheFile(&meshFile);
				return 0;
			}
			if (pMesh->ncor!=0&&readIntegerIn!=pMesh->ncor)
			{
				printf("\nWarning: number of Corners %d different from number of RequiredVertices %d in %s file.\n",pMesh->ncor,readIntegerIn,fileLocation);
			}
			else if (pMesh->ncor==0)
			{
				pMesh->ncor=readIntegerIn;
			}

			// Check number of RequiredVertices
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
			}
			if (j!=readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d RequiredVertices instead of %d properly read.\n",readIntegerIn,j);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (!strcmp(keyword,"Tet"))
		{	
			// Read number of Tetrahedra and store it in pMesh->ntet
			readStringOut=fgets(readStringIn,8,meshFile);
			readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
			if(readStringOut==NULL || readIntegerOut!=1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in Tetrahedra.\n",readStringOut,readIntegerOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (strcmp(readStringIn,"rahedra") || readIntegerIn<1)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Tetrahedra Ntet instead of Tet%s %d (positive value).\n",readStringIn,readIntegerIn);
				closeTheFile(&meshFile);
				return 0;
			}
			pMesh->ntet=readIntegerIn;

			// Allocating memory for Tetrahedra and check if it works
			pMesh->ptet=(Tetrahedron*)calloc(readIntegerIn,sizeof(Tetrahedron));
			if (pMesh->ptet==NULL)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: could not allocate memory for Tetrahedra.\n");
				closeTheFile(&meshFile);
				return 0;
			}

			// Saving Tetrahedra in pMesh->ptet and check if it works
			j=0;
			for (k=0; k<readIntegerIn; k++)
			{	
				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: first point of Tetrahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptet[k].p1=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: second point of Tetrahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptet[k].p2=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: third point of Tetrahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptet[k].p3=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: fourth point of Tetrahedron %d is referred to as %d but there are only %d Vertices in the Mesh.\n",k+1,readIntegerOut,pMesh->nver);
					closeTheFile(&meshFile);
					return 0;
				}
				pMesh->ptet[k].p4=readIntegerOut;

				j+=fscanf(meshFile,"%d ",&readIntegerOut);
				pMesh->ptet[k].label=readIntegerOut;		
			}
			if (j!=5*readIntegerIn)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting %d Tetrahedra instead of %d properly saved.\n",readIntegerIn,j/5);
				closeTheFile(&meshFile);
				return 0;
			}	
		}
		else if (!strcmp(keyword,"Nor"))
		{	
			readStringOut=fgets(readStringIn,5,meshFile);
			if(readStringOut==NULL)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) in Normals/NormalAtVertices.\n",readStringOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (!strcmp(readStringIn,"malA"))
			{	
				// Read number of NormalAtVertices and store it in pMesh->nnorm
				readStringOut=fgets(readStringIn,10,meshFile);
				readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
				if(readStringOut==NULL || readIntegerOut!=1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in NormalAtVertices.\n",readStringOut,readIntegerOut);
					closeTheFile(&meshFile);
					return 0;
				}
				if (strcmp(readStringIn,"tVertices") || readIntegerIn<1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting NormalAtVertices Norv instead of NormalA%s %d (positive value).\n",readStringIn,readIntegerIn);
					closeTheFile(&meshFile);
					return 0;
				}
				if (readIntegerIn>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of NormalAtVertices %d greater than number of Vertices %d in %s file.\n",readIntegerIn,pMesh->nver,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->nnorm!=0&&readIntegerIn!=pMesh->nnorm)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of NormalAtVertices %d different from number of Normals %d in %s file.\n",readIntegerIn,pMesh->nnorm,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->nnorm==0)
				{
					pMesh->nnorm=readIntegerIn;
					// Allocating memory for NormalAtVertices and check if it works
					pMesh->pnorm=(Vector*)calloc(readIntegerIn,sizeof(Vector));
					if (pMesh->pnorm==NULL)
					{
						PRINT_ERROR();
						printf("In readAndAllocateMesh: could not allocate memory for NormalAtVertices.\n");
						closeTheFile(&meshFile);
						return 0;
					}
				}

				// Saving NormalAtVertices in pMesh->pnorm then check if it works and if it is stored in increasing order
				j=0;
				for (k=0; k<readIntegerIn; k++)
				{	
					j+=fscanf(meshFile,"%d ",&readIntegerOut);
					if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
					{
						PRINT_ERROR();
						printf("In readAndAllocateMesh: the point associated with (normal) Vector %d is referred to as ",k+1);
						printf("%d but there are only %d Vertices in the Mesh.\n",readIntegerOut,pMesh->nver);
						closeTheFile(&meshFile);
						return 0;
					}
					pMesh->pnorm[k].p=readIntegerOut;

					j+=fscanf(meshFile,"%d ",&readIntegerOut);
					if (readIntegerOut!=k+1)
					{
						printf("\nWarning: NormalAtVertice %d is not numbered %d (in increasing order).\n",readIntegerOut,k+1);
					}							
				}
				if (j!=2*readIntegerIn)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting %d NormalAtVertices instead of %d properly saved.\n",readIntegerIn,j/2);
					closeTheFile(&meshFile);
					return 0;
				}	
			}
			else if (!strcmp(readStringIn,"mals"))
			{
				// Read number of Normals and store it in pMesh->nnorm
				readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
				if(readIntegerOut!=1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: wrong return of fscanf (=%d) in Normals.\n",readIntegerOut);
					closeTheFile(&meshFile);
					return 0;
				}
				if (readIntegerIn<1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting Normals Norm instead of Normals %d (positive value).\n",readIntegerIn);
					closeTheFile(&meshFile);
					return 0;
				}
				if (readIntegerIn>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of Normals %d greater than number of Vertices %d in %s file.\n",readIntegerIn,pMesh->nver,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->nnorm!=0&&readIntegerIn!=pMesh->nnorm)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of Normals %d different from number of NormalAtVertices %d in %s file.\n",readIntegerIn,pMesh->nnorm,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->nnorm==0)
				{
					pMesh->nnorm=readIntegerIn;
					// Allocating memory for Normals and check if it works
					pMesh->pnorm=(Vector*)calloc(readIntegerIn,sizeof(Vector));
					if (pMesh->pnorm==NULL)
					{
						PRINT_ERROR();
						printf("In readAndAllocateMesh: could not allocate memory for Normals.\n");
						closeTheFile(&meshFile);
						return 0;
					}
				}
				// Saving Normals in pMesh->pnorm and check if it works
				j=0;
				for (k=0; k<readIntegerIn; k++)
				{	
					j+=fscanf(meshFile,"%lf ",&readDouble);
					pMesh->pnorm[k].x=readDouble;					
					j+=fscanf(meshFile,"%lf ",&readDouble);
					pMesh->pnorm[k].y=readDouble;
					j+=fscanf(meshFile,"%lf ",&readDouble);
					pMesh->pnorm[k].z=readDouble;							
				}
				if (j!=3*readIntegerIn)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting %d Normals instead of %d properly saved.\n",readIntegerIn,j/3);
					closeTheFile(&meshFile);
					return 0;
				}	
			}
			else
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Normals/NormalAtVertices instead of Nor%s.\n",readStringIn);
				closeTheFile(&meshFile);
				return 0;
			}
		}
		else if (!strcmp(keyword,"Tan"))
		{	
			readStringOut=fgets(readStringIn,6,meshFile);
			if(readStringOut==NULL)
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: wrong return of fgets (=%p) in Tangents/TangentAtVertices.\n",readStringOut);
				closeTheFile(&meshFile);
				return 0;
			}
			if (!strcmp(readStringIn,"gentA"))
			{
				// Read number of TangentAtVertices and store it in pMesh->ntan
				readStringOut=fgets(readStringIn,10,meshFile);
				readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
				if(readStringOut==NULL || readIntegerOut!=1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: wrong return of fgets (=%p) and/or fscanf (=%d) in TangentAtVertices.\n",readStringOut,readIntegerOut);
					closeTheFile(&meshFile);
					return 0;
				}
				if (strcmp(readStringIn,"tVertices") || readIntegerIn<1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting TangentAtVertices Ntav instead of TangentA%s %d (positive value).\n",readStringIn,readIntegerIn);
					closeTheFile(&meshFile);
					return 0;
				}
				if (readIntegerIn>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of TangentAtVertices %d greater than number of Vertices %d in %s file.\n",readIntegerIn,pMesh->nver,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->ntan!=0&&readIntegerIn!=pMesh->ntan)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of TangentAtVertices %d different from number of Tangents %d in %s file.\n",readIntegerIn,pMesh->ntan,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->ntan==0)
				{
					pMesh->ntan=readIntegerIn;
					// Allocating memory for TangentAtVertices and check if it works
					pMesh->ptan=(Vector*)calloc(readIntegerIn,sizeof(Vector));
					if (pMesh->ptan==NULL)
					{
						PRINT_ERROR();
						printf("In readAndAllocateMesh: could not allocate memory for TangentAtVertices.\n");
						closeTheFile(&meshFile);
						return 0;
					}
				}

				// Saving TangentAtVertices in pMesh->ptan then check if it works and if it is stored in increasing order
				j=0;
				for (k=0; k<readIntegerIn; k++)
				{	
					j+=fscanf(meshFile,"%d ",&readIntegerOut);
					if (readIntegerOut<1 || readIntegerOut>pMesh->nver)
					{
						PRINT_ERROR();
						printf("In readAndAllocateMesh: the point associated with (tangent) Vector %d is referred to as %d ",k+1,readIntegerOut);
						printf("but there are only %d Vertices in the Mesh.\n",pMesh->nver);
						closeTheFile(&meshFile);
						return 0;
					}
					pMesh->ptan[k].p=readIntegerOut;

					j+=fscanf(meshFile,"%d ",&readIntegerOut);
					if (readIntegerOut!=k+1)
					{
						printf("\nWarning in readAndAllocateMesh function: TangentAtVertice %d is not numbered %d (in increasing order).\n",readIntegerOut,k+1);
					}							
				}
				if (j!=2*readIntegerIn)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting %d TangentAtVertices instead of %d properly saved.\n",readIntegerIn,j/2);
					closeTheFile(&meshFile);
					return 0;
				}	
			}
			else if (!strcmp(readStringIn,"gents"))
			{
				// Read number of Tangents and store it in pMesh->ntan
				readIntegerOut=fscanf(meshFile," %d ",&readIntegerIn);
				if(readIntegerOut!=1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: wrong return of fscanf (=%d) in Tangents.\n",readIntegerOut);
					closeTheFile(&meshFile);
					return 0;
				}
				if (readIntegerIn<1)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting Tangents Ntan instead of Tangents %d (positive value).\n",readIntegerIn);
					closeTheFile(&meshFile);
					return 0;
				}
				if (readIntegerIn>pMesh->nver)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of Tangents %d greater than number of Vertices %d in %s file.\n",readIntegerIn,pMesh->nver,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->ntan!=0&&readIntegerIn!=pMesh->ntan)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: number of Tangents %d different from number of TangentAtVertices %d in %s file.\n",readIntegerIn,pMesh->ntan,fileLocation);
					closeTheFile(&meshFile);
					return 0;
				}
				if (pMesh->ntan==0)
				{
					pMesh->ntan=readIntegerIn;
					// Allocating memory for Tangents and check if it works
					pMesh->ptan=(Vector*)calloc(readIntegerIn,sizeof(Vector));
					if (pMesh->ptan==NULL)
					{
						PRINT_ERROR();
						printf("In readAndAllocateMesh: could not allocate memory for Tangents.\n");
						closeTheFile(&meshFile);
						return 0;
					}
				}

				// Saving Tangents in pMesh->ptan and check if it works
				j=0;
				for (k=0; k<readIntegerIn; k++)
				{	
					j+=fscanf(meshFile,"%lf ",&readDouble);
					pMesh->ptan[k].x=readDouble;
					j+=fscanf(meshFile,"%lf ",&readDouble);
					pMesh->ptan[k].y=readDouble;
					j+=fscanf(meshFile,"%lf ",&readDouble);
					pMesh->ptan[k].z=readDouble;							
				}
				if (j!=3*readIntegerIn)
				{
					PRINT_ERROR();
					printf("In readAndAllocateMesh: expecting %d Tangents instead of %d properly saved.\n",readIntegerIn,j/3);
					closeTheFile(&meshFile);
					return 0;
				}	
			}
			else
			{
				PRINT_ERROR();
				printf("In readAndAllocateMesh: expecting Tangents/TangentAtVertices instead of Tan%s.\n",readStringIn);
				closeTheFile(&meshFile);
				return 0;
			}
		}
#endif
		else if (!strcmp(keyword,"End"))
		{
			i=11;
		}
		else
		{
			PRINT_ERROR();
			printf("In readAndAllocateMesh: unexpected (*.mesh) syntax in %s file.\n",fileLocation);
			closeTheFile(&meshFile);
			return 0;
		}
	} while (i<11);

	// Testing if file is well closed
	if(fclose(meshFile))
	{
		PRINT_ERROR();
		printf("In readAndAllocateMesh: could not close %s file properly.\n",fileLocation);
		meshFile=NULL;
		return 0;
	}
	meshFile=NULL;
	printf("Closing file.\n");

	return 1;
}

/* ******************************************************************************************************************************************************************* */
// The function initialLevelSetInMeshExists checks if the pMesh variable is encoding a level-set structure (i.e. faces (resp. quadrilaterals in LS_MODE) labelled 10)
// It has the Mesh* variable (described in main.h) as input argument and returns one if it is the case, otherwise zero is returned
/* ******************************************************************************************************************************************************************* */
int initialLevelSetInMeshExists(Mesh* pMesh)
{
	int i=0, iMax=0; 

	// Testing if the input pMesh variable is pointing to NULL
	if (pMesh==NULL)
	{	
		printf("\nWarning in initialLevelSetInMeshExists function: the input pMesh variable is pointing to %p adress.\n",pMesh);
		return 0;
	}

#ifndef LS_MODE
	iMax=pMesh->ntri;
	if (iMax<1)
	{
		printf("\nWarning in initialLevelSetInMeshExists function: %d boundary Triangles in the mesh.\n",iMax);
		return 0;
	}
	if (pMesh->ptri==NULL)
	{	
		printf("\nWarning in initialLevelSetInMeshExists function: the pMesh->ptri variable is pointing to %p adress.\n",pMesh->ptri);
		return 0;
	}
	for (i=0; i<iMax; i++)
	{
		if (pMesh->ptri[i].label==10)
		{
			return 1;
		}
	}
#endif

#ifdef LS_MODE
	iMax=pMesh->nqua;
	if (iMax<1)
	{
		printf("\nWarning in initialLevelSetInMeshExists function: %d boundary Quadrilaterals in the mesh.\n",iMax);
		return 0;
	}
	if (pMesh->pqua==NULL)
	{	
		printf("\nWarning in initialLevelSetInMeshExists function: the pMesh->pqua variable is pointing to %p adress.\n",pMesh->pqua);
		return 0;
	}
	for (i=0; i<iMax; i++)
	{
		if (pMesh->pqua[i].label==10)
		{
			return 1;
		}
	}
#endif

	return 0;
}

/* ******************************************************************************************************************************************************************************************************* */
// The function evaluatingHessianAtVertices evaluates a (gaussian function) primitive of pMolecularOrbital described in pChemicalSystem at pPoint, and it also calculates its gradient and hessian matrix  
// It has the ChemicalSystem*, MolecularOrbital*, Point* variables (described in main.h), and the integer characterizing the gaussian as input arguments 
// It returns the value of the gaussian primitive, fills gradient[3] with its first-order derivatives, and hessian[6] (stored as 0->xx 1->yy 2->zz 3->xy,yx 4->xz,zx 5->yz,zy) with its second-order ones
/* ******************************************************************************************************************************************************************************************************* */
static double evaluatingHessianAtVertices(ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbital, Point* pPoint, double gradient[3], double hessian[6], int primitive)
{
	// First, if coefficient is zero, return zero to avoid waist of time (warning: gradient and hessian must have been correctly initialized to zero)
	double coefficient=pMolecularOrbital->coeff[primitive];
	if (coefficient==0.)
	{
		return 0.;
	}
	double exponent=pMolecularOrbital->exp[primitive];
	int type=pMolecularOrbital->type[primitive], iNucleus=pMolecularOrbital->nucl[primitive]; 
	Nucleus* pCenter=&pChemicalSystem->pnucl[iNucleus-1]; 	// Warning: we stored Nuclei ref as in chemicalSystem.txt so do not forget to remove one from these int when calling ->pnucl
	double dx=(pPoint->x)-(pCenter->x), dy=(pPoint->y)-(pCenter->y), dz=(pPoint->z)-(pCenter->z), distanceSquared=dx*dx+dy*dy+dz*dz;
	double exponent2=2.*exponent, dxx=exponent2*dx*dx, dyy=exponent2*dy*dy, dzz=exponent2*dz*dz,  dxy=exponent2*dx*dy, dxz=exponent2*dx*dz, dyz=exponent2*dy*dz;
	double value=coefficient*exp(-exponent*distanceSquared), function=value;
	
	// Evaluate function, gradient and hessian according of the type of the gaussian primitive function (parameters are defined in main.h)
	switch(type)
	{
		default:	
			printf("\nWarning in evaluatingHessianAtVertices function: type %d should be positive and less than 20 here. Treated as one (s-type orbital).\n",type);
		case ORB_1S:			// These parameters are defined in main.h (common file in all *.h files) bacause it is also used in shapeDerivative.c functions
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[1]-=value*dy;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-1.);
			hessian[3]+=value*dxy;
			hessian[4]+=value*dxz;
			hessian[5]+=value*dyz;
			break;

		case ORB_2PX:
			function*=(dxx-1.);		
			gradient[0]-=function;
			function*=exponent2;
			hessian[3]+=function*dy;
			hessian[4]+=function*dz;
			value*=dx;
			function=value;
			value*=exponent2;
			gradient[1]-=value*dy;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-3.);
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-1.);
			hessian[5]+=value*dyz;
			break;

		case ORB_2PY:
			function*=(dyy-1.);		
			gradient[1]-=function;
			function*=exponent2;
			hessian[3]+=function*dx;
			hessian[5]+=function*dz;
			value*=dy;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-3.);
			hessian[2]+=value*(dzz-1.);
			hessian[4]+=value*dxz;
			break;

		case ORB_2PZ:
			function*=(dzz-1.);		
			gradient[2]-=function;
			function*=exponent2;
			hessian[4]+=function*dx;
			hessian[5]+=function*dy;
			value*=dz;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[1]-=value*dy;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-3.);
			hessian[3]+=value*dxy;
			break;

		case ORB_3DXX:
			hessian[0]+=value*(2.+dxx*(dxx-5.));
			function*=dx*(dxx-2.);
			gradient[0]-=function;
			function*=exponent2;
			hessian[3]+=function*dy;
			hessian[4]+=function*dz;
			value*=dx*dx;
			function=value;
			value*=exponent2;
			gradient[1]-=value*dy;
			gradient[2]-=value*dz;
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-1.);
			hessian[5]+=value*dyz;
			break;

		case ORB_3DYY:
			hessian[1]+=value*(2.+dyy*(dyy-5.));
			function*=dy*(dyy-2.);
			gradient[1]-=function;
			function*=exponent2;
			hessian[3]+=function*dx;
			hessian[5]+=function*dz;
			value*=dy*dy;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-1.);
			hessian[2]+=value*(dzz-1.);
			hessian[4]+=value*dxz;
			break;

		case ORB_3DZZ:
			hessian[2]+=value*(2.+dzz*(dzz-5.));
			function*=dz*(dzz-2.);
			gradient[2]-=function;
			function*=exponent2;
			hessian[4]+=function*dx;
			hessian[5]+=function*dy;
			value*=dz*dz;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[1]-=value*dy;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-1.);
			hessian[3]+=value*dxy;
			break;

		case ORB_3DXY:
			hessian[3]+=value*(dyy-1.)*(dxx-1.);
			function*=dy*(dxx-1.);
			gradient[0]-=function;	
			hessian[4]+=function*exponent2*dz;
			value*=dx;
			function=value*(dyy-1.);
			gradient[1]-=function;
			hessian[5]+=function*exponent2*dz;
			value*=dy;
			function=value;
			value*=exponent2;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-3.);
			hessian[1]+=value*(dyy-3.);
			hessian[2]+=value*(dzz-1.);
			break;

		case ORB_3DXZ:
			hessian[4]+=value*(dxx-1.)*(dzz-1.);
			function*=dz*(dxx-1.);
			gradient[0]-=function;
			hessian[3]+=function*exponent2*dy;
			value*=dx;
			function=value*(dzz-1.);
			gradient[2]-=function;	
			hessian[5]+=function*exponent2*dy;
			value*=dz;		
			function=value;
			value*=exponent2;
			gradient[1]-=value*dy;
			hessian[0]+=value*(dxx-3.);
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-3.);
			break;

		case ORB_3DYZ:
			hessian[5]+=value*(dyy-1.)*(dzz-1.);
			function*=dz*(dyy-1.);
			gradient[1]-=function;
			hessian[3]+=function*exponent2*dx;
			value*=dy;
			function=value*(dzz-1.);
			gradient[2]-=function;
			hessian[4]+=function*exponent2*dx;
			value*=dz;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-3.);
			hessian[2]+=value*(dzz-3.);
			break;

		case ORB_4FXXX:
			hessian[0]+=value*dx*(6.+dxx*(dxx-7.));
			value*=dx*dx;
			function=value*(dxx-3.);
			gradient[0]-=function;
			hessian[3]+=function*exponent2*dy;
			hessian[4]+=function*exponent2*dz;
			value*=dx;
			function=value;
			value*=exponent2;
			gradient[1]-=value*dy;
			gradient[2]-=value*dz;
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-1.);
			hessian[5]+=value*dyz;
			break;

		case ORB_4FYYY:
			hessian[1]+=value*dy*(6.+dyy*(dyy-7.));
			value*=dy*dy;
			function=value*(dyy-3.);
			gradient[1]-=function;
			hessian[3]+=function*exponent2*dx;
			hessian[5]+=function*exponent2*dz;
			value*=dy;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-1.);
			hessian[2]+=value*(dzz-1.);
			hessian[4]+=value*dxz;
			break;

		case ORB_4FZZZ:
			hessian[2]+=value*dz*(6.+dzz*(dzz-7.));
			value*=dz*dz;
			function=value*(dzz-3.);
			gradient[2]-=function;
			hessian[4]+=function*exponent2*dx;
			hessian[5]+=function*exponent2*dy;
			value*=dz;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			gradient[1]-=value*dy;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-1.);
			hessian[3]+=value*dxy;
			break;

		case ORB_4FXXY:
			hessian[0]+=value*dy*(2.+dxx*(dxx-5.));
			function*=dx*(dxx-2.);
			hessian[3]+=function*(dyy-1.);
			function*=dy;
			gradient[0]-=function;
			hessian[4]+=function*exponent2*dz;
			value*=dx*dx;
			function=value*(dyy-1.);
			gradient[1]-=function;
			hessian[5]+=function*exponent2*dz;
			value*=dy;
			function=value;
			value*=exponent2;
			gradient[2]-=value*dz;
			hessian[1]+=value*(dyy-3.);
			hessian[2]+=value*(dzz-1.);
			break;

		case ORB_4FXXZ:
			hessian[0]+=value*dz*(2.+dxx*(dxx-5.));
			function*=dx*(dxx-2.);
			hessian[4]+=function*(dzz-1.);
			function*=dz;
			gradient[0]-=function;
			hessian[3]+=function*exponent2*dy;
			value*=dx*dx;
			function=value*(dzz-1.);
			gradient[2]-=function;
			hessian[5]+=function*exponent2*dy;
			value*=dz;
			function=value;
			value*=exponent2;
			gradient[1]-=value*dy;
			hessian[1]+=value*(dyy-1.);
			hessian[2]+=value*(dzz-3.);
			break;

		case ORB_4FYYZ:
			hessian[1]+=value*dz*(2.+dyy*(dyy-5.));
			function*=dy*(dyy-2.);
			hessian[5]+=function*(2.*exponent*dz*dz-1.);
			function*=dz;
			gradient[1]-=function;
			hessian[3]+=function*exponent2*dx;
			value*=dy*dy;			
			function=value*(dzz-1.);
			gradient[2]-=function;
			hessian[4]+=function*exponent2*dx;
			value*=dz;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			hessian[0]+=value*(dxx-1.);
			hessian[2]+=value*(dzz-3.);
			break;

		case ORB_4FXYY:
			hessian[1]+=value*dx*(2.+dyy*(dyy-5.));
			function*=dy*(dyy-2.);
			hessian[3]+=function*(dxx-1.);
			function*=dx;
			gradient[1]-=function;
			hessian[5]+=function*exponent2*dz;
			value*=dy*dy;
			function=value*(dxx-1.);
			gradient[0]-=function;
			hessian[4]+=function*exponent2*dz;
			value*=dx;
			function=value;
			value*=exponent2;
			gradient[2]-=value*dz;
			hessian[0]+=value*(dxx-3.);
			hessian[2]+=value*(dzz-1.);
			break;

		case ORB_4FXZZ:
			hessian[2]+=value*dx*(2.+dzz*(dzz-5.));
			function*=dz*(dzz-2.);
			hessian[4]+=function*(dxx-1.);
			function*=dx;
			gradient[2]-=function;
			hessian[5]+=function*exponent2*dy;
			value*=dz*dz;
			function=value*(dxx-1.);
			gradient[0]-=function;
			hessian[3]+=function*exponent2*dy;
			value*=dx;
			function=value;
			value*=exponent2;
			gradient[1]-=value*dy;
			hessian[0]+=value*(dxx-3.);
			hessian[1]+=value*(dyy-1.);
			break;

		case ORB_4FYZZ:
			hessian[2]+=value*dy*(2.+dzz*(dzz-5.));
			function*=dz*(dzz-2.);
			hessian[5]+=function*(dyy-1.);
			function*=dy;
			gradient[2]-=function;
			hessian[4]+=function*exponent2*dx;
			value*=dz*dz;
			function=value*(dyy-1.);
			gradient[1]-=function;
			hessian[3]+=function*exponent2*dx;
			value*=dy;
			function=value;
			value*=exponent2;
			gradient[0]-=value*dx;
			hessian[0]+=value*(dxx-1.);
			hessian[1]+=value*(dyy-3.);
			break;

		case ORB_4FXYZ:	
			function*=dx*dy*dz;
			gradient[0]+=value*dy*dz*(1.-dxx);
			gradient[1]+=value*dx*dz*(1.-dyy);
			gradient[2]+=value*dx*dy*(1.-dzz);
			hessian[3]+=value*dz*(dxx-1.)*(dyy-1.);
			hessian[4]+=value*dy*(dxx-1.)*(dzz-1.);
			hessian[5]+=value*dx*(dyy-1.)*(dzz-1.);
			value*=exponent2*dx*dy*dz;
			hessian[0]+=value*(dxx-3.);
			hessian[1]+=value*(dyy-3.);
			hessian[2]+=value*(dzz-3.);
			break;
	}

	return function;
}

/* ******************************************************************************************************************************************************************************************** */
// The function findEigenvaluesOfSymmetricMatrix computes the 3 real eigenvalues of a symmetric (3x3)-matrix (stored as 0=(1,1) 1=(2,2) 2=(3,3) 3=(1,2)=(2,1) 4=(1,3)=(3,1) and 5=(2,3)=(3,2)
// It has a matrix[6] to diagonalize and roots[3] for eigenvalues as input arguments, and it returns the number of distinct eigenvalues, otherwise zero if an error is encountered
/* ******************************************************************************************************************************************************************************************** */
static int findEigenvaluesOfSymmetricMatrix(double matrix[6], double roots[3])
{
	double coefficients[3];		// Coefficients of the characteristic polynomial P(x) = determinant(x*Id-S) = x^3 - trace(S)*x^2 + trace(Comatrix(S))*x - determinant(S)
	coefficients[2]=matrix[0]+matrix[1]+matrix[2];
	coefficients[1]=(matrix[0]*matrix[1]+matrix[0]*matrix[2]+matrix[1]*matrix[2])-(matrix[3]*matrix[3]+matrix[4]*matrix[4]+matrix[5]*matrix[5]);
	coefficients[0]=matrix[0]*(matrix[1]*matrix[2]-matrix[5]*matrix[5])-matrix[3]*(matrix[3]*matrix[2]-matrix[5]*matrix[4])+matrix[4]*(matrix[3]*matrix[5]-matrix[1]*matrix[4]);
	return getRootsFromCoefficients(roots,coefficients); 	
}

/* ********************************************************************************************************************************************************************************************* */
// The function getRootsFromCoefficients finds the three real roots of the characteristic polynomial P(x) = x^3 - b*x^2 + c*x - d associated with a symmetric matrix thanks to Newton's method
// It has a coefficients[3] and roots[3] (for storing roots in increasing order) as entry arguments, and it returns returns the number of distinct roots (1, 2, or 3) otherwise 0 (error)
/* ********************************************************************************************************************************************************************************************* */
static int getRootsFromCoefficients(double roots[3], double coefficients[3])
{	
	double a=3., b=coefficients[2], c=coefficients[1], d=coefficients[0], delta=b*b-a*c;		// we use the reduced discriminant for P'(x) = a*x^2 - 2*b*x + c
	double x0=(b-sqrt(delta))/a, x1=(b+sqrt(delta))/a, f0=c+x0*(a*x0-2.*b), f1=c+x1*(a*x1-2.*b);	// we use Hörner's formula     
	double df0=2.*(a*x0-b), df1=2.*(a*x1-b), dx0=f0/df0, dx1=f1/df1; 
	int counter=0;

	if (delta>TOL_MAX)
	{
		// We perform Newton's method starting from the roots of P'(x) (convex function) to avoid numerical errors
		counter=0;
		while (counter<ITER_MAX)
		{
			counter++;
			x0-=dx0;
			x1-=dx1;
			if (fmax(fabs(f0),fabs(dx0/x0))<TOL_MAX || fmax(fabs(f1),fabs(dx1/x1))<TOL_MAX)
			{
				break;
			}
			f0=c+x0*(a*x0-2.*b);	
			f1=c+x1*(a*x1-2.*b);
			df0=2.*(a*x0-b);	
			df1=2.*(a*x1-b);
			dx0=f0/df0;
			dx1=f1/df1;
		}
		if (counter==ITER_MAX)
		{
			
		}

		f0=x0*(c+x0*(x0-b))-d;
		f1=x1*(c+x1*(x1-b))-d;
		if (fabs(f0)<TOL_MAX)
		{
			// Case where x0 is a double root: we perform Newton's method on the single root of P(x) to avoid numerical errors
			x1=b-2.*x0;
			f1=x1*(c+x1*(x1-b))-d;
			df1=c+x1*(a*x1-2.*b);
			dx1=f1/df1;
			counter=0;	
			while (counter<ITER_MAX)
			{
				counter++;
				x1-=dx1;	
				if (fabs(f1)<TOL_MAX && fabs(dx1/x1)<TOL_MAX)
				{
					break;
				}
				f1=x1*(c+x1*(x1-b))-d;
				df1=c+x1*(a*x1-2.*b);
				dx1=f1/df1;
			}
			if (counter==ITER_MAX)
			{
				printf("\nWarning in getRootsFromCoefficients function: maximum iteration reached for double root x0.\n");
			}
			// Roots ordered in increasing order
			roots[0]=x0;
			roots[1]=x0;
			roots[2]=x1;
			return 2;
		}
		else if (fabs(f1)<TOL_MAX)
		{
			// Case where x1 is a double root: we perform Newton's method on the single root of P(x) to avoid numerical errors
			x0=b-2.*x1;
			f0=x0*(c+x0*(x0-b))-d;
			df0=c+x0*(a*x0-2.*b);
			dx0=f0/df0;
			counter=0;
			while (counter<ITER_MAX)
			{
				counter++;
				x0-=dx0;	
				if (fabs(f0)<TOL_MAX && fabs(dx0/x0)<TOL_MAX)
				{
					break;
				}
				f0=x0*(c+x0*(x0-b))-d;
				df0=c+x0*(a*x0-2.*b);
				dx0=f0/df0;
			}
			if (counter==ITER_MAX)
			{
				printf("\nWarning in getRootsFromCoefficients function: maximum iteration reached for double root x1.\n");
			}
			// Roots ordered in increasing order
			roots[0]=x0;
			roots[1]=x1;
			roots[2]=x1;
			return 2;
		}
		else
		{
			// Case where there are three distincts roots: we firstly find the middle root from the starting point P"(x1)=0 (Newton's method again)
			x1=b/a;
			f1=x1*(c+x1*(x1-b))-d;
			df1=c+x1*(a*x1-2.*b); 
			dx1=f1/df1;
			counter=0;	
			while (counter<ITER_MAX)
			{
				counter++;
				x1-=dx1;
				if (fabs(f1)<TOL_MAX && fabs(dx1/x1)<TOL_MAX)
				{
					break;
				}
				f1=x1*(c+x1*(x1-b))-d;
				df1=c+x1*(a*x1-2.*b); 
				dx1=f1/df1;
			} 
			if (counter==ITER_MAX)
			{
				printf("\nWarning in getRootsFromCoefficients function: maximum iteration reached for middle root.\n");
			}
			roots[1]=x1;

			// Solve the remaining second order equation P(x)=(x-x1)(x^2-dx0*x+dx1) with dx0=b-x1 and dx1=c-dx0*x1
			dx0=b-x1;
			dx1=c-dx0*x1;
			delta=dx0*dx0-4.*dx1;
			if (delta<=0.)
			{
				PRINT_ERROR();
				printf("In getRootsFromCoefficients: discriminant of remaining 2nd-order polynomial is not positive.\n");
				return 0;
			}
			// We perform Newton's method starting from the two other roots of P(x) to avoid numerical errors
			x0=.5*(dx0-sqrt(delta));
			x1=.5*(dx0+sqrt(delta));
			f0=x0*(c+x0*(x0-b))-d;
			f1=x1*(c+x1*(x1-b))-d; 
			df0=c+x0*(a*x0-2.*b);
			df1=c+x1*(a*x1-2.*b);
			dx0=f0/df0;
			dx1=f1/df1;
			counter=0;	
			while (counter<ITER_MAX)
			{
				counter++;
				x0-=dx0;
				x1-=dx1;
				if (fmax(fabs(f0),fabs(dx0/x0))<TOL_MAX && fmax(fabs(f1),fabs(dx1/x1))<TOL_MAX)
				{
					break;
				}
				f0=x0*(c+x0*(x0-b))-d;
				f1=x1*(c+x1*(x1-b))-d;
				df0=c+x0*(a*x0-2.*b);
				df1=c+x1*(a*x1-2.*b);
				dx0=f0/df0; 
				dx1=f1/df1;
			} 
			if (counter==ITER_MAX)
			{
				printf("\nWarning in getRootsFromCoefficients function: maximum iteration reached for extremal roots.\n");	
			}
			// Roots ordered in increasing order
			roots[0]=x0;
			roots[2]=x1;
			return 1;
		}
	}
	else if (fabs(delta)<TOL_MAX)
	{
		// Case where there is a triple root given by P"(x)=2*(a*x-b)=0, a=3
		x0=b/a;
		f0=x0*(c+x0*(x0-b))-d;
		if (fabs(f0)>=TOL_MAX)
		{
			printf("\nWarning in getRootsFromCoefficients function: triple root expected but it does not seem so.\n");
		}
		roots[0]=x0;
		roots[1]=x0;
		roots[2]=x0;
		return 3;		
	}
	else
	{
		PRINT_ERROR();
		printf("In getRootsFromCoefficients: discriminant of derivative is negative and complex roots are not expected here (diagonalizing symmetric matrix).\n");
		return 0;
	}
}

/* ******************************************************************************************************************************************************************************************************* */
// The function evaluatingMetricOnMesh calculates at the vertices of pMesh struture (.value parameter) the metric associated to every product of orbitals given in the pChemicalSystem structure
// It has the Mesh* and ChemicalSystem* variable (described in main.h) as input arguments and returns one on sucess, otherwise zero is returned as an error
/* ******************************************************************************************************************************************************************************************************* */
int evaluatingMetricOnMesh(Mesh* pMesh, ChemicalSystem* pChemicalSystem)
{
	int i=0, j=0, k=0, l=0, numberOfVertices=pMesh->nver, numberOfMolecularOrbitals=pChemicalSystem->nmorb, numberOfPrimitives=pChemicalSystem->ngauss;
	double value=0., lambda=0., hessian[6]={0.}, eigenvalues[3]={0.};
	double functionI=0., functionJ=0., gradientI[3]={0.}, gradientJ[3]={0.}, hessianI[6]={0.}, hessianJ[6]={0.};
	MolecularOrbital *pMolecularOrbitalI=NULL, *pMolecularOrbitalJ=NULL;
	Point* pPoint;

	printf("\nEvaluating the mesh metric of the molecular orbitals.\n");
	for (i=0; i<numberOfMolecularOrbitals; i++)
	{
		pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
		for (j=0; j<i; j++)
		{
			pMolecularOrbitalJ=&pChemicalSystem->pmorb[j];
			if(pMolecularOrbitalI->spin!=pMolecularOrbitalJ->spin)
			{
				continue;	// Case where Orbitals i and j have different spin (warning: don't forget to initialize value to one in readAndAllocMesh function)
			}
			// Pointwise evaluation of the hessian matrix of Orbital(i)*Orbital(j) (stored as 0->xx 1->yy 2->zz 3->xy,yx 4->xz,zx 5->yz,zy)
			for (k=0; k<numberOfVertices; k++)
			{	
				// Re-initialization of the funtion, gradient and Hessian matrix 	
				functionI=0.;
				functionJ=0.;
				for (l=0; l<3; l++)
				{
					gradientI[l]=0.;
					gradientJ[l]=0.;
					hessianI[l]=0.;
					hessianJ[l]=0.;
					hessian[l]=0.;
					eigenvalues[l]=0.;
				}
				for (l=3; l<6; l++)
				{
					hessianI[l]=0.;
					hessianJ[l]=0.;
					hessian[l]=0.;
				}
								
				pPoint=&pMesh->pver[k];
				// Evaluating function, gradient and Hessian for Orbital i and j depending of the type of primitives encountered
				for (l=0; l<numberOfPrimitives; l++)
				{
					functionI+=evaluatingHessianAtVertices(pChemicalSystem,pMolecularOrbitalI,pPoint,gradientI,hessianI,l);
					functionJ+=evaluatingHessianAtVertices(pChemicalSystem,pMolecularOrbitalJ,pPoint,gradientJ,hessianJ,l);
				}
				// Evaluating Hessian of Orb(i)*Orb(j) for computing metric
				hessian[0]=hessianI[0]*functionJ+2.*gradientI[0]*gradientJ[0]+functionI*hessianJ[0];
				hessian[1]=hessianI[1]*functionJ+2.*gradientI[1]*gradientJ[1]+functionI*hessianJ[1];
				hessian[2]=hessianI[2]*functionJ+2.*gradientI[2]*gradientJ[2]+functionI*hessianJ[2];
				hessian[3]=hessianI[3]*functionJ+gradientI[0]*gradientJ[1]+gradientI[1]*gradientJ[0]+functionI*hessianJ[3];
				hessian[4]=hessianI[4]*functionJ+gradientI[0]*gradientJ[2]+gradientI[2]*gradientJ[0]+functionI*hessianJ[4];
				hessian[5]=hessianI[5]*functionJ+gradientI[1]*gradientJ[2]+gradientI[2]*gradientJ[1]+functionI*hessianJ[5];
				// Diagonalize the Hessian matrix
				if(!findEigenvaluesOfSymmetricMatrix(hessian,eigenvalues))
				{
					PRINT_ERROR();
					printf("In evaluatingMetricOnMesh: findEigenvaluesOfSymmetricMatrix function returned zero instead of one.\n");
					return 0;
				}
				// Get the normalized highest eigenvalues 
				lambda=1./(MET_MAX*MET_MAX);
				for (l=0; l<3; l++)
				{	
					value=MET_CST*fabs(eigenvalues[l])/MET_ERR;
					if (value>lambda)
					{
						lambda=value;
					}
				} 
				// Computing the metric (note that the values were initialized at one in readAndAllocateMesh function
				value=fmax(1./sqrt(lambda),MET_MIN); 	// Upper bound already imposed when initiliazing lambda
				if (value<(pPoint->value))
				{
					pPoint->value=value;					
				}
			}			
		}
		// Here we now treat the case where i=j 
		for (k=0; k<numberOfVertices; k++)
		{	
			// Re-initialization of the funtion, gradient and Hessian matrix 	
			functionI=0.;
			for (l=0; l<3; l++)
			{
				gradientI[l]=0.;
				hessianI[l]=0.;
				hessian[l]=0.;
				eigenvalues[l]=0.;
			}
			for (l=3; l<6; l++)
			{
				hessianI[l]=0.;
				hessian[l]=0.;
			}
							
			pPoint=&pMesh->pver[k];
			for (l=0; l<numberOfPrimitives; l++)
			{
				// Evaluating function, gradient and Hessian for Orbital i (hessian matrix stored as 0->xx 1->yy 2->zz 3->xy,yx 4->xz,zx 5->yz,zy)
				functionI+=evaluatingHessianAtVertices(pChemicalSystem,pMolecularOrbitalI,pPoint,gradientI,hessianI,l);
			}
			// Evaluating Hessian of Orb(i)^2 for computing metric
			hessian[0]=2.*(hessianI[0]*functionI+gradientI[0]*gradientI[0]);
			hessian[1]=2.*(hessianI[1]*functionI+gradientI[1]*gradientI[1]);
			hessian[2]=2.*(hessianI[2]*functionI+gradientI[2]*gradientI[2]);
			hessian[3]=2.*(hessianI[3]*functionI+gradientI[0]*gradientI[1]);
			hessian[4]=2.*(hessianI[4]*functionI+gradientI[0]*gradientI[2]);
			hessian[5]=2.*(hessianI[5]*functionI+gradientI[1]*gradientI[2]);
			// Diagonalize matrix
			if(!findEigenvaluesOfSymmetricMatrix(hessian,eigenvalues))
			{
				PRINT_ERROR();
				printf("In evaluatingMetricOnMesh: findEigenvaluesOfSymmetricMatrix function returned zero instead of one.\n");
				return 0;
			}
			// Get the normalized highest eigenvalues 
			lambda=1./(MET_MAX*MET_MAX);
			for (l=0; l<3; l++)
			{	
				value=MET_CST*fabs(eigenvalues[l])/MET_ERR;
				if (value>lambda)
				{
					lambda=value;
				}
			} 
			// Computing the metric (note that the values were initialized at one in readAndAllocMesh function
			value=fmax(1./sqrt(lambda),MET_MIN); 	// Upper bound already imposed when initiliazing lambda
			if (value<(pPoint->value))
			{
				pPoint->value=value;
			}
		}
	}

	return 1;
}

/* ******************************************************************************************************************************************************************************** */
// The function initializeLevelSet evaluate the initial level set function on mesh (sphere of center (LS_X,LS_Y,LS_Z) and radius LS_R defined in main.h)
// It has the Mesh* variable (described in main.h) as input argument and returns one on sucess, otherwise zero is returned
/* ******************************************************************************************************************************************************************************** */
static int initializeLevelSet(Mesh* pMesh)
{
	int l=0, lMax=0;
	double value=0., dx=0., dy=0., dz=0.;
	Point *pPoint=NULL;

	printf("\nInitializing the signed distance function of a sphere of radius %lf and center (%lf, %lf, %lf).\n",LS_R,LS_X,LS_Y,LS_Z);
	// Save the initial level-set function in pMesh->pver[l].value of the Point structure
	lMax=pMesh->nver;
	for (l=0; l<lMax; l++)
	{	
		pPoint=&pMesh->pver[l];
		dx=(pPoint->x)-LS_X;
		dy=(pPoint->y)-LS_Y;
		dz=(pPoint->z)-LS_Z;
		value=dx*dx+dy*dy+dz*dz;
		pPoint->value=sqrt(value)-LS_R;
	}

#ifdef LS_MODE
	int lPoint=0;
	Hexahedron *pHexahedron=NULL;

	// Labelling the exterior hexahedra by 2 (positive level-set function) while the interior ones are labelled 3 (negative level-set function)
	lMax=pMesh->nhex;
	for (l=0; l<lMax; l++)
	{
		pHexahedron=&pMesh->phex[l];
		lPoint=pHexahedron->p1; 	// Warning here: point are stored thanks to the .mesh file so do not forget to remove one from these int when calling a point in the pMesh structure
		pPoint=&pMesh->pver[lPoint-1];
		dx=pPoint->x+.5*DELTA_X-LS_X;
		dy=pPoint->y+.5*DELTA_Y-LS_Y;
		dz=pPoint->z+.5*DELTA_Z-LS_Z;
		value=dx*dx+dy*dy+dz*dz;
		value=sqrt(value)-LS_R;
		if (value>=0.)
		{
			pHexahedron->label=2;
		}
		else
		{
			pHexahedron->label=3;
		}
	}
#endif
	return 1;
}

#ifdef LS_MODE
/* *********************************************************************************************************************************************************************************************** */
// The function getLevelSetQuadrilaterals modfies the pMesh structure so that it contains the cube faces and quadrilaterals corresponding to the MPDomain (and update adjacency table accordingly)
// It has the Mesh* variable (described in the readAndSaveFile.h file, included in all *.h files) as input argument and returns one on sucess, otherwise zero is returned
/* *********************************************************************************************************************************************************************************************** */
int getLevelSetQuadrilaterals(Mesh* pMesh)
{
	int i=0, j=0, k=0, l=0, lMax=0, p1=0, p2=0, p3=0, p4=0, p5=0, p6=0, p7=0, p8=0, label=0, lLabel=0, counter=0, lPoint=0;
	int initialNumberOfQuadrilaterals=2*((N_X-1)*(N_Y-1)+(N_X-1)*(N_Z-1)+(N_Y-1)*(N_Z-1)); // 2 faces per square
	Quadrilateral *pQuadrilateral=NULL;
	Hexahedron *pHexahedron=NULL;

	// Counting the number of boundary quadrilaterals corresponding to the initial sphere inside the cube
	lMax=pMesh->nhex;
	counter=0;
	for (l=0; l<lMax; l++)
	{		
		label=pMesh->phex[l].label;
		k=l%(N_Z-1);
		lPoint=l/(N_Z-1);
		j=lPoint%(N_Y-1);
		i=lPoint/(N_Y-1);
		if (i>0)
		{
			lPoint=((i-1)*(N_Y-1)+j)*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				counter++;
			}
		}
		if (i<N_X-2)
		{
			lPoint=((i+1)*(N_Y-1)+j)*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				counter++;
			}
		}
		if (j>0)
		{
			lPoint=(i*(N_Y-1)+(j-1))*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				counter++;
			}
		}
		if (j<N_Y-2)
		{
			lPoint=(i*(N_Y-1)+(j+1))*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				counter++;
			}
		}
		if (k>0)
		{
			lPoint=(i*(N_Y-1)+j)*(N_Z-1)+(k-1);
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				counter++;
			}
		}
		if (k<N_Z-2)
		{
			lPoint=(i*(N_Y-1)+j)*(N_Z-1)+(k+1);
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				counter++;
			}
		}
	}
	pMesh->nadj=counter;

	// Reallocating memory for the quadrilaterals and check if it works and allocating memory for the adjacency table
	pQuadrilateral=(Quadrilateral*)realloc(pMesh->pqua,(initialNumberOfQuadrilaterals+counter)*sizeof(Quadrilateral));
	pMesh->padj=(Adjacency*)calloc(counter,sizeof(Adjacency));
	if (pQuadrilateral==NULL || pMesh->padj==NULL)
	{
		PRINT_ERROR();
		printf("In getLevelSetQuadrilaterals: could not reallocate memory for Quadrilaterals Adjacency.\n");
		return 0;
	}
	pMesh->pqua=(Quadrilateral*)pQuadrilateral;

	// Save the boundary quadrilaterals that are not faces of the cube (we assume that initially, the discretized (pixelized) initial sphere is embedded strictly in the cube)
	counter=initialNumberOfQuadrilaterals;
	
	for (l=0; l<lMax; l++)
	{
		pHexahedron=&pMesh->phex[l];
		p1=pHexahedron->p1;
		p2=pHexahedron->p2;
		p3=pHexahedron->p3;
		p4=pHexahedron->p4;
		p5=pHexahedron->p5;
		p6=pHexahedron->p6;
		p7=pHexahedron->p7;
		p8=pHexahedron->p8;
		label=pHexahedron->label;
		k=l%(N_Z-1);
		lPoint=l/(N_Z-1);
		j=lPoint%(N_Y-1);
		i=lPoint/(N_Y-1);
		// In hexahedra corresponding to each cube cell, point is saved as (a*N_Y+b)*N_Z+c a=0...N_X-1, b=0...N_Y-1, c=0...N_Z-1
		// Point reference (a,b,c) in cube cell: 1=(i-1,j-1,k-1) 2=(i,j-1,k-1) 3=(i,j,k-1) 4=(i-1,j,k-1) 5=(i-1,j-1,k) 6=(i,j-1,k) 7=(i,j,k) 8=(i-1,j,k) i=1..N_X-1; j=1..N_Y-1; k=1..N_Z-1 
		if (i>0)
		{
			lPoint=((i-1)*(N_Y-1)+j)*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior)
			{
				// Square 1584
				pQuadrilateral=&pMesh->pqua[counter];
				pQuadrilateral->p1=p1;
				pQuadrilateral->p2=p5;
				pQuadrilateral->p3=p8;
				pQuadrilateral->p4=p4;
				pQuadrilateral->label=10;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].quad=counter+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexout=l+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexin=lPoint+1;
				counter++;
			}
		}
		if (i<N_X-2)
		{
			lPoint=((i+1)*(N_Y-1)+j)*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior)
			{
				// Square 2376
				pQuadrilateral=&pMesh->pqua[counter];
				pQuadrilateral->p1=p2;
				pQuadrilateral->p2=p3;
				pQuadrilateral->p3=p7;
				pQuadrilateral->p4=p6;
				pQuadrilateral->label=10;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].quad=counter+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexout=l+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexin=lPoint+1;
				counter++;
			}
		}
		if (j>0)
		{
			lPoint=(i*(N_Y-1)+(j-1))*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior) 
			{
				// Square 1265
				pQuadrilateral=&pMesh->pqua[counter];
				pQuadrilateral->p1=p1;
				pQuadrilateral->p2=p2;
				pQuadrilateral->p3=p6;
				pQuadrilateral->p4=p5;
				pQuadrilateral->label=10;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].quad=counter+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexout=l+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexin=lPoint+1;
				counter++;
			}
		}
		if (j<N_Y-2)
		{
			lPoint=(i*(N_Y-1)+(j+1))*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior) 
			{
				// Square 3487
				pQuadrilateral=&pMesh->pqua[counter];
				pQuadrilateral->p1=p3;
				pQuadrilateral->p2=p4;
				pQuadrilateral->p3=p8;
				pQuadrilateral->p4=p7;
				pQuadrilateral->label=10;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].quad=counter+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexout=l+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexin=lPoint+1;
				counter++;
			}
		}
		if (k>0)
		{
			lPoint=(i*(N_Y-1)+j)*(N_Z-1)+(k-1);
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior)
			{
				// Square 1432
				pQuadrilateral=&pMesh->pqua[counter];
				pQuadrilateral->p1=p1;
				pQuadrilateral->p2=p4;
				pQuadrilateral->p3=p3;
				pQuadrilateral->p4=p2;
				pQuadrilateral->label=10;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].quad=counter+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexout=l+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexin=lPoint+1;
				counter++;
			}
		}
		if (k<N_Z-2)
		{
			lPoint=(i*(N_Y-1)+j)*(N_Z-1)+(k+1);
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2&&abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior)
			{
				// Square 5678
				pQuadrilateral=&pMesh->pqua[counter];
				pQuadrilateral->p1=p5;
				pQuadrilateral->p2=p6;
				pQuadrilateral->p3=p7;
				pQuadrilateral->p4=p8;
				pQuadrilateral->label=10;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].quad=counter+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexout=l+1;
				pMesh->padj[counter-initialNumberOfQuadrilaterals].hexin=lPoint+1;
				counter++;
			}
		}
	}
	// Updating the number of quadrilaterals 
	pMesh->nqua=initialNumberOfQuadrilaterals+pMesh->nadj;
	if (pMesh->nqua!=counter)
	{
		PRINT_ERROR();
		printf("In getLevelSetQuadrilaterals: expecting %d instead of %d for the number of Quadrilaterals.\n",pMesh->nqua,counter);
		return 0;
	}

	return 1;
}

/* ******************************************************************************************************************************************************************************** */
// The function initializeAdjacency generates the Adjacency table of Quadrilaterals that are labelled 10 on mesh (only used in when LS_MODE is defined)
// It has the Mesh* variable (described in the main.h file, included in all *.h files) as input argument and returns one on sucess, otherwise zero is returned (error)
/* ******************************************************************************************************************************************************************************** */
static int initializeAdjacency(Mesh* pMesh)
{
	int i=0, j=0, k=0, l=0, lMax=0, lPoint=0, lLabel=0, label=0, counter=0, initialNumberOfQuadrilaterals=2*((N_X-1)*(N_Y-1)+(N_X-1)*(N_Z-1)+(N_Y-1)*(N_Z-1)); // 2 faces per square

	// Since the file come from previous calculations, initialize label of hexahedra
	lMax=pMesh->nhex;
	for (l=0; l<lMax; l++)
	{
		pMesh->phex[l].label=abs(pMesh->phex[l].label);
	}

	// Counting the number of boundary quadrilaterals corresponding to the initial sphere inside the cube and save it in pMesh->nadj
	lMax=pMesh->nqua;
	if (lMax<=initialNumberOfQuadrilaterals)
	{
		PRINT_ERROR();
		printf("In initializeAdjacency: %d boundary Quadrilaterals in the mesh while there should be strictly more than %d.\n",lMax,initialNumberOfQuadrilaterals);
		return 0;
	}
	counter=0;
	for (l=0; l<lMax; l++)
	{
		if(pMesh->pqua[l].label==10)
		{
			counter++;
		}
	}
	pMesh->nadj=counter;

	// Check the number of quadrilaterals concerned by counter
	if (initialNumberOfQuadrilaterals!=pMesh->nqua-pMesh->nadj)
	{
		PRINT_ERROR();
		printf("In initializeAdjacency: expecting %d initial Quadrilaterals instead of %d, and %d for Adjacency.\n",initialNumberOfQuadrilaterals,pMesh->nqua-pMesh->nadj,counter);
		return 0;	
	}
	// Allocating memory for the quadrilaterals and check if it works and allocating memory for the adjacency table
	pMesh->padj=(Adjacency*)calloc(pMesh->nadj,sizeof(Adjacency));
	if (pMesh->padj==NULL)
	{
		PRINT_ERROR();
		printf("In initializeAdjacency: could not allocate memory for Quadrilateral Adjacency.\n");
		return 0;
	}

	// Save the boundary quadrilaterals adjacency that are not faces of the cube
	lMax=pMesh->nhex;
	counter=0;
	for (l=0; l<lMax; l++)
	{
		label=pMesh->phex[l].label;
		k=l%(N_Z-1);
		lPoint=l/(N_Z-1);
		j=lPoint%(N_Y-1);
		i=lPoint/(N_Y-1);
		// In hexahedra corresponding to each cube cell, point is saved as (a*N_Y+b)*N_Z+c a=0...N_X-1, b=0...N_Y-1, c=0...N_Z-1
		// Point reference (a,b,c) in cube cell: 1=(i-1,j-1,k-1) 2=(i,j-1,k-1) 3=(i,j,k-1) 4=(i-1,j,k-1) 5=(i-1,j-1,k) 6=(i,j-1,k) 7=(i,j,k) 8=(i-1,j,k) i=1..N_X-1; j=1..N_Y-1; k=1..N_Z-1 
		if (i>0)
		{
			lPoint=((i-1)*(N_Y-1)+j)*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2 && abs(lLabel)==3)	// Test not to count two times the quadrilaterals (save with trigonometric orientation when looking from the exterior)
			{
				pMesh->padj[counter].quad=initialNumberOfQuadrilaterals+counter;
				pMesh->padj[counter].hexout=l+1;
				pMesh->padj[counter].hexin=lPoint+1;
				counter++;	
			}
		}
		if (i<N_X-2)
		{
			lPoint=((i+1)*(N_Y-1)+j)*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2 && abs(lLabel)==3)	// Test not to count two times the quadrilaterals 
			{
				pMesh->padj[counter].quad=initialNumberOfQuadrilaterals+counter;
				pMesh->padj[counter].hexout=l+1;
				pMesh->padj[counter].hexin=lPoint+1;
				counter++;
			}
		}
		if (j>0)
		{
			lPoint=(i*(N_Y-1)+(j-1))*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2 && abs(lLabel)==3)	// Test not to count two times the quadrilaterals
			{
				pMesh->padj[counter].quad=initialNumberOfQuadrilaterals+counter;
				pMesh->padj[counter].hexout=l+1;
				pMesh->padj[counter].hexin=lPoint+1;
				counter++;
			}
		}
		if (j<N_Y-2)
		{
			lPoint=(i*(N_Y-1)+(j+1))*(N_Z-1)+k;
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2 && abs(lLabel)==3)	// Test not to count two times the quadrilaterals 
			{
				pMesh->padj[counter].quad=initialNumberOfQuadrilaterals+counter;
				pMesh->padj[counter].hexout=l+1;
				pMesh->padj[counter].hexin=lPoint+1;
				counter++;
			}
		}
		if (k>0)
		{
			lPoint=(i*(N_Y-1)+j)*(N_Z-1)+(k-1);
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2 && abs(lLabel)==3)	// Test not to count two times the quadrilaterals 
			{
				pMesh->padj[counter].quad=initialNumberOfQuadrilaterals+counter;
				pMesh->padj[counter].hexout=l+1;
				pMesh->padj[counter].hexin=lPoint+1;
				counter++;
			}
		}
		if (k<N_Z-2)
		{
			lPoint=(i*(N_Y-1)+j)*(N_Z-1)+(k+1);
			lLabel=pMesh->phex[lPoint].label;
			if (abs(label)==2 && abs(lLabel)==3)	// Test not to count two times the quadrilaterals 
			{
				pMesh->padj[counter].quad=initialNumberOfQuadrilaterals+counter;
				pMesh->padj[counter].hexout=l+1;
				pMesh->padj[counter].hexin=lPoint+1;
				counter++;
			}
		}
	}
	// Checking the number of Quadrilaterals in Adjacency
	if (pMesh->nadj!=counter)
	{
		PRINT_ERROR();
		printf("In initializeAdjacency: expecting %d instead of %d for the number of Quadrilateral Adjacency.\n",pMesh->nadj,counter);
		return 0;
	}

	return 1;
}

#endif

/* ******************************************************************************************************************************************************************************************************* */
// The function writingSolFile writes the vertices values stored in pMesh->pver[i].value, into a file at fileLocatoin (warning: reset/overwrite file if already exists) according to the *.sol syntax
// It has the Mesh* variable (described in the main.h file, included in all *.h files) as input argument and it returns zero if an error occurred, otherwise one
/* ******************************************************************************************************************************************************************************************************* */
int writingSolFile(char* fileLocation, Mesh* pMesh)
{
	int i=0, iMax=0;
	FILE *solFile=NULL;

	// Testing if the input fileLocation variable is pointing to NULL
	if (fileLocation==NULL)
	{	
		PRINT_ERROR();
		printf("In writingSolFile: the input fileLocation variable is pointing to %p adress.\n",fileLocation);
		return 0;
	}

	// Testing if file is well opened  (warning: reset and overwrite on cube.sol file if already exists)
	printf("\nOpening %s file. ",fileLocation);
	solFile=fopen(fileLocation,"w+");		// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned	
	if (solFile==NULL)
	{
		PRINT_ERROR();	
		printf("In writingSolFile: could not write in %s file.\n",fileLocation);
		return 0;
	}
	printf("Start writing solution. ");	

	// Writing cube.sol file: MeshVersionFormated (1=single 2=double precision), Dimension (2 or 3), SolAtVertices numberOfSolution typeOfSolution (1=scalar, 2=vector, 3=symmetric tensor) 
	iMax=pMesh->nver;
	if (iMax>0)
	{	
		fprintf(solFile,"MeshVersionFormatted 2\n\nDimension 3\n\nSolAtVertices\n%d\n1 1 \n\n",pMesh->nver);
		for (i=0;i<iMax;i++)
		{
			fprintf(solFile,"%le \n",pMesh->pver[i].value);
		}
		fprintf(solFile,"\nEnd");
	}
	else
	{
		PRINT_ERROR();	
		printf("In writingSolFile: %d SolAtVertices.\n",iMax);
		fprintf(solFile,"\nEnd");
		closeTheFile(&solFile);		
		return 0;
	}
	// Testing if file is well closed
	if(fclose(solFile))	// fclose function returns zero if the input FILE* variable is successfully closed, otherwise EOF is returned (pointer to the end of the file)
	{
		PRINT_ERROR();	
		printf("In writingSolFile: could not close %s file properly.\n",fileLocation);
		solFile=NULL;
		return 0;
	}
	else
	{
		solFile=NULL;
		printf("Close file.\n");
	}

	return 1;
}

/* ***************************************************************************************************************************************************************************************** */
// The function adaptCube prepare the mesh (adaption to the metric and level-set function initialized) for the optimization loop depending on the different modes of the algorithm
// It has the char* fileLocation, three char* commmandLine, the Mesh* and ChemicalSystem* (described in main.h) as input arguments and it returns zero if an error occurred, otherwise one
/* ***************************************************************************************************************************************************************************************** */
int adaptCube(char* fileLocation, char* commandLineMmg3dIsotropic, char* commandLineMmg3dMetric, char* commandLineMmg3dLevelSet, Mesh* pMesh, ChemicalSystem* pChemicalSystem)
{
#ifndef LS_MODE
	int sizeMemory=0;
#endif

	// Test that some input variables are not pointing to NULL
	if (commandLineMmg3dIsotropic==NULL || commandLineMmg3dMetric==NULL || commandLineMmg3dLevelSet==NULL || pMesh==NULL)
	{
		PRINT_ERROR();
		printf("In adaptCube: one of the (input) commandLine variables (commandLineMmg3dIsotropic=%p commandLineMmg3dMetric=%p ",commandLineMmg3dIsotropic,commandLineMmg3dMetric);
		printf("commandLineMmg3dLevelSet=%p, pMesh=%p) is not a valid adress.\n",commandLineMmg3dLevelSet,pMesh);
		return 0;
	} 

#ifndef LS_MODE
	// Perform a preliminary mesh adaptation if the *.mesh file located at fileLocation was not given in the command line of the main function (argc=3 and fileLocation=argv[3] is not NULL)
	if (!initialFileExists(fileLocation))
	{
		// Free the memory allocated for the mesh since we are going to adapt it
		sizeMemory=sizeof(Mesh)+(pMesh->nver)*sizeof(Point)+(pMesh->nnorm+pMesh->ntan)*sizeof(Vector)+(pMesh->nedg)*sizeof(Edge)+(pMesh->ntri)*sizeof(Triangle)+(pMesh->ntet)*sizeof(Tetrahedron);
		printf("\nCleaning allocated memory (%d,%d Mo) for a preliminary isotropic mesh adaption.\n",sizeMemory/1000000,sizeMemory%1000000);
		freeMeshMemory(pMesh);

		// Preliminary isotropic mesh adaptation (Warning: file names are imposed and mmg3d software must have been previously installed in ../bin/ directory)
		system(commandLineMmg3dIsotropic);	// commandLineMmg3dIsotropic = "./../bin/mmg3d_O3 -in cube.mesh -out cubeAdapt.mesh -hmin 0.1 -hmax 0.2 -hausd 0.01 -hgrad 2.0"		
		system("rm cube.mesh cubeAdapt.sol");	// better to choose hmin of order (DELTA_X+DELTA_Y+DELTA_Z)/3 given in main.h and hmax a little higher		
		system("mv cubeAdapt.mesh cube.mesh");	// mmg3d commandLines are input arguments so as to change parameters (hmin,...) directly in main function (easier than going into each files)
	}

	// Read file entitled cube.mesh containing the mesh data
	if(!readAndAllocateMesh("cube.mesh",pMesh))
	{
		PRINT_ERROR();
		printf("In adaptCube: readAndAllocateMesh function returned zero instead of one.\n");
		return 0;
	}
	printf("Mesh data loaded: %d Vertices, %d Tetrahedra, %d Triangles.\n",pMesh->nver,pMesh->ntet,pMesh->ntri);
	printf("Geometry loaded: %d Ridges, %d Corners, %d Normals, %d Tangents.\n",pMesh->nedg,pMesh->ncor,pMesh->nnorm,pMesh->ntan);	

	// If a level-set structure is not encoded in the mesh, adapt the mesh to the orbital, initialize the level-set function with a sphere (LS_XYZR in main.h), and finally adapt the mesh to it
	if(!initialLevelSetInMeshExists(pMesh))
	{
		// Evaluating on each mesh vertices the metric associated with the orbitals of the pChemicalSystem structure  
		if (!evaluatingMetricOnMesh(pMesh,pChemicalSystem))
		{
			PRINT_ERROR();
			printf("In adaptCube: evaluatingMetricOnMesh function returned zero instead of one.\n");
			return 0;
		}

		// Saving metric values in a file entitled cube.sol	
		if (!writingSolFile("cube.sol",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: writingSolFile function returned zero instead of one.\n");
			return 0;
		}
		system("medit cube.mesh");		// Warning: medit software must have been previously installed

		// Free the memory allocated for the mesh since we are going to adapt it again
		sizeMemory=sizeof(Mesh)+(pMesh->nver)*sizeof(Point)+(pMesh->nnorm+pMesh->ntan)*sizeof(Vector)+(pMesh->nedg)*sizeof(Edge)+(pMesh->ntri)*sizeof(Triangle)+(pMesh->ntet)*sizeof(Tetrahedron);
		printf("\nCleaning allocated memory (%d,%d Mo) for a first mesh adaption to the metric of molecular orbitals.\n",sizeMemory/1000000,sizeMemory%1000000);
		freeMeshMemory(pMesh);

		// First isotropic mesh adaptation according to the metric associated with the orbitals (Warning: mmg3d software must have been previously installed in ../bin/ directory)
		system(commandLineMmg3dMetric);			// commandLineMmg3dMetric = "./../bin/mmg3d_O3 -in cube.mesh -sol cube.sol -out cubeAdapt.mesh -hmin 0.01 -hmax 1.0 -hausd 0.01 -hgrad 2.0"
		system("rm cube.mesh cube.sol cubeAdapt.sol");	// mmg3d commandLines are input arguments so as to change parameters (hmin,...) directly in main function (but file names are imposed)
		system("mv cubeAdapt.mesh cube.mesh");

		//  Read file entitled cube.mesh containing the new adapted mesh to the orbitals
		if(!readAndAllocateMesh("cube.mesh",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: readAndAllocateMesh function returned zero instead of one.\n");
			return 0;
		}
		printf("New mesh data loaded: %d Vertices, %d Tetrahedra, %d Triangles.\n",pMesh->nver,pMesh->ntet,pMesh->ntri);
		printf("New geometry loaded: %d Ridges, %d Corners, %d Normals, %d Tangents.\n",pMesh->nedg,pMesh->ncor,pMesh->nnorm,pMesh->ntan);

		// Evaluating on each mesh vertices the metric associated with the orbitals in order to avoid interpolation error with the previous mesh
		if (!evaluatingMetricOnMesh(pMesh,pChemicalSystem))
		{
			PRINT_ERROR();
			printf("In adaptCube: evaluatingMetricOnMesh function returned zero instead of one.\n");
			return 0;
		}

		// Saving values in a file entitled metric.sol (Warning: file name "metric.sol" is completely imposed here, used for personal modified version of mmg3d)
		if (!writingSolFile("metric.sol",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: writingSolFile function returned zero instead of one.\n");
			return 0;
		}
		system("mv cube.mesh metric.mesh");
		system("medit metric.mesh");		// Warning: medit software must have been previously installed
		system("mv metric.mesh cube.mesh");

		// Evaluate the initial level-set function on mesh (sphere of center (LS_X,LS_Y,LS_Z) and radius LS_R defined in main.h)
		if (!initializeLevelSet(pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: initializeLevelSet function returned zero instead of one.\n");
			return 0;
		}

		// Saving level-set values in a file entitled cube.sol	
		if (!writingSolFile("cube.sol",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: writingSolFile function returned zero instead of one.\n");
			return 0;
		}
		system("medit cube.mesh");		// Warning: medit software must have been previously installed

		// Free the memory allocated for the mesh since we are going to adapt it again
		sizeMemory=sizeof(Mesh)+(pMesh->nver)*sizeof(Point)+(pMesh->nnorm+pMesh->ntan)*sizeof(Vector)+(pMesh->nedg)*sizeof(Edge)+(pMesh->ntri)*sizeof(Triangle)+(pMesh->ntet)*sizeof(Tetrahedron);
		printf("\nCleaning allocated memory (%d,%d Mo) for a second mesh adaption to the level-set interface geometry.\n",sizeMemory/1000000,sizeMemory%1000000);
		freeMeshMemory(pMesh);

		// Second isotropic mesh adaptation according to both level-set and orbitals metric (Warning: modified version of mmg3d software must have been previously installed in ../bin/ directory)
		system(commandLineMmg3dLevelSet);	// commandLineMmg3dLevelSet = "./../bin/mmg3d_O3 -in cube.mesh -ls -sol cube.sol -out cubeAdapt.mesh -nr -hmin 0.05 -hmax 1.0 -hausd 0.01 -hgrad 2.0"
		system("rm cube.mesh cube.sol cubeAdapt.sol metric.sol");// mmg3d commandLines are input arguments so as to change parameters (hmin,...) directly in main function (file names are imposed)
		system("mv cubeAdapt.mesh cube.mesh");

		//  Read file entitled cube.mesh containing the new adapted mesh to the orbitals and the level-set interface geometry
		if(!readAndAllocateMesh("cube.mesh",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: readAndAllocateMesh function returned zero instead of one.\n");
			return 0;
		}
		printf("New mesh data loaded: %d Vertices, %d Tetrahedra, %d Triangles.\n",pMesh->nver,pMesh->ntet,pMesh->ntri);
		printf("New geometry loaded: %d Ridges, %d Corners, %d Normals, %d Tangents.\n",pMesh->nedg,pMesh->ncor,pMesh->nnorm,pMesh->ntan);
	}	
#endif

#ifdef LS_MODE
	if (initialFileExists(fileLocation))
	{
		// Read file entitled cube.mesh containing the mesh data
		if(!readAndAllocateMesh("cube.mesh",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: readAndAllocateMesh function returned zero instead of one.\n");
			return 0;
		}
	}
	printf("Mesh data loaded: %d Vertices, %d Hexahedra, %d Quadrilaterals.\n",pMesh->nver,pMesh->nhex,pMesh->nqua);

	// If a level-set structure is encoded in cube.mesh file (quadrilaterals labelled 10), initialize the adjacency table for Quadrilaterals.
	if(initialLevelSetInMeshExists(pMesh))
	{	
		printf("\nInternal domain detected. Setting up adjacency table for Quadrilaterals. ");
		if (!initializeAdjacency(pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: initializeAdjacency function returned zero instead of one.\n");
			return 0;
		}
		printf("Procedure end normally.\n");
	}
	else
	{
		// Evaluate the initial level-set function on mesh (sphere of center (LS_X,LS_Y,LS_Z) and radius LS_R defined in main.h)
		if (!initializeLevelSet(pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: initializeLevelSet function returned zero instead of one.\n");
			return 0;
		}
	
		// Modifying the pMesh structure so that it contains the quadrilaterals corresponding to the level-set interface
		if (!getLevelSetQuadrilaterals(pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: getLevelSetQuadrilaterals function returned zero instead of one.\n");
			return 0;
		}

		// Since we have added some quadrialterals with getLevelSetQuadrilaterals function, we must save the new mesh in a file entitle cube.mesh
		if (!writingMeshFile("cube.mesh",pMesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: writingMeshFile function returned zero instead of one.\n");
			return 0;
		}
		system("medit cube.mesh");	// Warning: medit software must have been previously installed
	}
#endif

	return 1;
}

