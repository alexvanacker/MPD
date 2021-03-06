#include "writingMeshFile.h"

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
void initializeMeshStructure(Mesh* pMesh);
void freeMeshMemory(Mesh* pMesh); 
int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);
int readAndAllocateMesh(char* fileLocation, Mesh* pMesh);
*/

// testing
int writingMeshFile(char* fileLocation, Mesh* pMesh);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int i=0, iver=0, readCharRef=0, readCharTest=0, counterChar=0;
	int nver=1210;
#ifndef LS_MODE
	int icor=0, inorm=0, itan=0, iedg=0, itri=0, itet=0;
	int ncor=8, nnorm=534, ntan=128, nedg=140, ntri=1336,  ntet=5040;
#endif
#ifdef LS_MODE
	int iqua=0, ihex=0, iadj=0;
	int nqua=668, nhex=840, nadj=10;
#endif
	FILE *refFile=NULL, *testFile=NULL;
	Mesh *pMesh=NULL, mesh;

	// Initializing the mesh variable
	mesh.nver=0;	
	mesh.pver=NULL;
#ifndef LS_MODE
	mesh.ncor=0;

	mesh.nnorm=0;
	mesh.pnorm=NULL;

	mesh.ntan=0;
	mesh.ptan=NULL;

	mesh.nedg=0;
	mesh.pedg=NULL;

	mesh.ntri=0;
	mesh.ptri=NULL;

	mesh.ntet=0;
	mesh.ptet=NULL;
#endif
#ifdef LS_MODE
	mesh.nqua=0;
	mesh.pqua=NULL;

	mesh.nhex=0;
	mesh.phex=NULL;

	mesh.nadj=0;
	mesh.padj=NULL;
#endif

	// Test starts on a real cube example [-3.25,4.25]x[10.2,10.8]x[-0.21,0.0] with (11)x(5)x(22) grid and precision (0.75)x(0.15)x(0.01)
	time(&startTimer);
	printf("Testing writingMeshFile function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingMeshFile(NULL,pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingMeshFile("test.mesh",pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pMesh=&mesh;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingMeshFile(NULL,pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=writingMeshFile("test.mesh",pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	for (iver=-nver; iver<=nver; iver+=nver)
	{
#ifndef LS_MODE
		for (icor=-ncor; icor<=ncor; icor+=ncor)
		{		
			for (inorm=-nnorm; inorm<=nnorm; inorm+=nnorm)
			{	
				for (itan=-ntan; itan<=ntan; itan+=ntan)
				{	
					for (iedg=-nedg; iedg<=nedg; iedg+=nedg)
					{	
						for (itri=-ntri; itri<=ntri; itri+=ntri)
						{	
							for (itet=-ntet; itet<=ntet; itet+=ntet)
							{	
								pMesh->nver=iver;
								pMesh->pver=(Point*)calloc(iver,sizeof(Point));

								pMesh->ncor=icor;

								pMesh->nnorm=inorm;
								pMesh->pnorm=(Vector*)calloc(inorm,sizeof(Vector));

								pMesh->ntan=itan;
								pMesh->ptan=(Vector*)calloc(itan,sizeof(Vector));

								pMesh->nedg=iedg;
								pMesh->pedg=(Edge*)calloc(iedg,sizeof(Edge));

								pMesh->ntri=itri;
								pMesh->ptri=(Triangle*)calloc(itri,sizeof(Triangle));

								pMesh->ntet=itet;
								pMesh->ptet=(Tetrahedron*)calloc(itet,sizeof(Tetrahedron));

								if (pMesh->nver<=0 || pMesh->ncor<=0 || pMesh->nnorm<=0 || pMesh->ntan<=0 || pMesh->nedg<=0 || pMesh->ntri<=0 || pMesh->ntet<=0)
								{
									expectedValue=0;	
								}
								else
								{
									expectedValue=1;
									for (i=0; i<8; i++)
									{
										pMesh->pver[i].label=101;
									}
								}
								PRINT_TEST_START(counter,expectedValue);
								printf("nver=%d pver=%p ncor=%d nnorm=%d pnorm=%p ",pMesh->nver,pMesh->pver,pMesh->ncor,pMesh->nnorm,pMesh->pnorm);
								printf("ntan=%d ptan=%p nedg=%d pedg=%p ",pMesh->ntan,pMesh->ptan,pMesh->nedg,pMesh->pedg);
								printf("ntri=%d ptri=%p ntet=%d ptet=%p\n",pMesh->ntri,pMesh->ptri,pMesh->ntet,pMesh->ptet);
								returnValue=writingMeshFile("test0.mesh",pMesh);
								freeMeshMemory(pMesh);
								PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
							}
						}
					}
				}
			}
		}
#endif
#ifdef LS_MODE
		for (iqua=-nqua; iqua<=nqua; iqua+=nqua)
		{			
			for (ihex=-nhex; ihex<=nhex; ihex+=nhex)
			{		
				for (iadj=-nadj; iadj<=nadj; iadj+=nadj)
				{
					pMesh->nver=iver;
					pMesh->pver=(Point*)calloc(iver,sizeof(Point));
	
					pMesh->nqua=iqua;
					pMesh->pqua=(Quadrilateral*)calloc(iqua,sizeof(Quadrilateral));	

					pMesh->nhex=ihex;
					pMesh->phex=(Hexahedron*)calloc(ihex,sizeof(Hexahedron));

					pMesh->nadj=iadj;
					pMesh->padj=(Adjacency*)calloc(iadj,sizeof(Adjacency));

					if (pMesh->nver<=0 || pMesh->nqua<=0 || pMesh->nhex<=0)
					{
						expectedValue=0;	
					}
					else
					{
						expectedValue=1;
						for (i=0; i<8; i++)
						{
							pMesh->pver[i].label=101;
						}	
					}
					PRINT_TEST_START(counter,expectedValue);
					printf("nver=%d pver=%p nqua=%d pqua=%p ",pMesh->nver,pMesh->pver,pMesh->nqua,pMesh->pqua);
					printf("nhex=%d phex=%p nadj=%d padj=%p\n",pMesh->nhex,pMesh->phex,pMesh->nadj,pMesh->padj);
					returnValue=writingMeshFile("test1.mesh",pMesh);
					freeMeshMemory(pMesh);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
			}
		}
#endif
	}

	// The last test compares with the result of writingMeshFile function with a testRef01.mesh file
	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
#ifndef LS_MODE
	printf("readAndAllocateMesh returns %d value.\n",readAndAllocateMesh("testRef0.mesh",pMesh));
	returnValue=writingMeshFile("test0.mesh",pMesh);
	freeMeshMemory(pMesh);
	refFile=fopen("testRef0.mesh","r");
	testFile=fopen("test0.mesh","r");
#endif
#ifdef LS_MODE
	printf("readAndAllocateMesh returns %d value.\n",readAndAllocateMesh("testRef1.mesh",pMesh));
	returnValue=writingMeshFile("test1.mesh",pMesh);
	freeMeshMemory(pMesh);
	refFile=fopen("testRef1.mesh","r");
	testFile=fopen("test1.mesh","r");
#endif
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
	system("rm test0.mesh test1.mesh");

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

