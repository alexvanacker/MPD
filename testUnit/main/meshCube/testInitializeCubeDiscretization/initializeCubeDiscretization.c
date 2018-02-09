#include "initializeCubeDiscretization.h"

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
static int initializeCubeDiscretization(Mesh* pMesh);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;
 	
	int iver=0, jver=0;
	int nver=1210;
#ifndef LS_MODE
	int icor=0, inorm=0, jnorm=0, itan=0, jtan=0, iedg=0, jedg=0, itri=0, jtri=0, itet=0, jtet=0;
	int ncor=8, nnorm=534, ntan=128, nedg=140, ntri=1336, ntet=5040;
#endif
#ifdef LS_MODE
	int iqua=0, jqua=0, ihex=0, jhex=0, iadj=0, jadj=0;
	int nqua=668, nhex=840, nadj=10;
#endif
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

	// Test starts on a real mesh: results given by nx=11, ny=5 and nz=22 on a cube [-3.25,4.25]x[10.2,10.8]x[-0.21,0] with a discretization (0.75)x(0.15)x(0.01)
	time(&startTimer);
	printf("Testing initializeCubeDiscretization function.\n");

	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=initializeCubeDiscretization(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
//*
	// To test with 'y' answer to the question (and also with or without LS_MODE)
	pMesh=&mesh;
	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=initializeCubeDiscretization(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
//*/
/*	
	for (iver=-nver; iver<=nver; iver+=nver)
	{
		for (jver=-nver; jver<=nver; jver+=nver)
		{
#ifndef LS_MODE
			for (icor=-ncor; icor<=ncor; icor+=ncor)
			{	
				for (inorm=-nnorm; inorm<=nnorm; inorm+=nnorm)
				{
					for (jnorm=-nnorm; jnorm<=nnorm; jnorm+=nnorm)
					{				
						for (itan=-ntan; itan<=ntan; itan+=ntan)
						{	
							for (jtan=-ntan; jtan<=ntan; jtan+=ntan)
							{	
								for (iedg=-nedg; iedg<=nedg; iedg+=nedg)
								{	
									for (jedg=-nedg; jedg<=nedg; jedg+=nedg)
									{
										for (itri=-ntri; itri<=ntri; itri+=ntri)
										{
											for (jtri=-ntri; jtri<=ntri; jtri+=ntri)
											{	
												for (itet=-ntet; itet<=ntet; itet+=ntet)
												{	
													for (jtet=-ntet; jtet<=ntet; jtet+=ntet)
													{
														pMesh->nver=iver;
														pMesh->pver=(Point*)calloc(jver,sizeof(Point));
														free(pMesh->pver);

														pMesh->ncor=icor;

														pMesh->nnorm=inorm;
														pMesh->pnorm=(Vector*)calloc(jnorm,sizeof(Vector));
														free(pMesh->pnorm);

														pMesh->ntan=itan;
														pMesh->ptan=(Vector*)calloc(jtan,sizeof(Vector));
														free(pMesh->ptan);

														pMesh->nedg=iedg;
														pMesh->pedg=(Edge*)calloc(jedg,sizeof(Edge));
														free(pMesh->pedg);

														pMesh->ntri=itri;
														pMesh->ptri=(Triangle*)calloc(jtri,sizeof(Triangle));
														free(pMesh->ptri);

														pMesh->ntet=itet;
														pMesh->ptet=(Tetrahedron*)calloc(jtet,sizeof(Tetrahedron));
														free(pMesh->ptet);

														PRINT_TEST_START(counter,expectedValue);
														printf("nver=%d pver=%p ncor=%d ",pMesh->nver,pMesh->pver,pMesh->ncor);
														printf("nnorm=%d pnorm=%p ntan=%d ",pMesh->nnorm,pMesh->pnorm,pMesh->ntan);
														printf("ptan=%p nedg=%d pedg=%p ",pMesh->ptan,pMesh->nedg,pMesh->pedg);
														printf("ntri=%d ptri=%p ntet=%d ptet=%p\n",pMesh->ntri,pMesh->ptri,pMesh->ntet,pMesh->ptet);
														returnValue=initializeCubeDiscretization(pMesh);
														if (pMesh->nver!=nver || pMesh->ncor!=ncor || pMesh->nnorm!=nnorm || pMesh->ntan!=ntan || pMesh->nedg!=nedg || pMesh->ntri!=ntri || pMesh->ntet!=ntet)
														{
															printf("Error: even if returnValue=%d (expectedValue=%d",returnValue,expectedValue);
															printf("), at least one of the mesh parameters does not have a correct value.\n");
															printf("Expecting nver=%d ncor=%d nnorm=%d ntan=%d ",nver,ncor,nnorm,ntan);
															printf("nedg=%d ntri=%d ntet=%d instead of nver=%d ",nedg,ntri,ntet,pMesh->nver);
															printf("ncor=%d nnorm=%d ntan=%d ",pMesh->ncor,pMesh->nnorm,pMesh->ntan);
															printf("nedg=%d ntri=%d ntet=%d\n",pMesh->nedg,pMesh->ntri,pMesh->ntet);
															returnValue=0;	
														}
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
#endif
#ifdef LS_MODE
			for (iqua=-nqua; iqua<=nqua; iqua+=nqua)
			{
				for (jqua=-nqua; jqua<=nqua; jqua+=nqua)
				{	
					for (ihex=-nhex; ihex<=nhex; ihex+=nhex)
					{
						for (jhex=-nhex; jhex<=nhex; jhex+=nhex)
						{			
							for (iadj=-nadj; iadj<=nadj; iadj+=nadj)
							{
								for (jadj=-nadj; jadj<=nadj; jadj+=nadj)
								{	
									pMesh->nver=iver;
									pMesh->pver=(Point*)calloc(jver,sizeof(Point));						
									free(pMesh->pver);

									pMesh->nqua=iqua;
									pMesh->pqua=(Quadrilateral*)calloc(jqua,sizeof(Quadrilateral));	
									free(pMesh->pqua);					

									pMesh->nhex=ihex;
									pMesh->phex=(Hexahedron*)calloc(jhex,sizeof(Hexahedron));
									free(pMesh->phex);

									pMesh->nadj=iadj;
									pMesh->padj=(Adjacency*)calloc(jadj,sizeof(Adjacency));
									free(pMesh->padj);

									PRINT_TEST_START(counter,expectedValue);
									printf("nver=%d pver=%p nqua=%d pqua=%p ",pMesh->nver,pMesh->pver,pMesh->nqua,pMesh->pqua);
									printf("nhex=%d phex=%p nadj=%d padj=%p\n",pMesh->nhex,pMesh->phex,pMesh->nadj,pMesh->padj);
									returnValue=initializeCubeDiscretization(pMesh);
									if (pMesh->nver!=nver || pMesh->nqua!=nqua || pMesh->nhex!=nhex)
									{
										printf("Error: althought returnValue=%d (expectedValue=%d), ",returnValue,expectedValue);
										printf("at least one of the mesh parameters does not have a correct value.\n");
										printf("Expecting nver=%d nqua=%d nhex=%d instead of ",nver,nqua,nhex);
										printf("nver=%d nqua=%d nhex=%d (nadj=%d)\n",pMesh->nver,pMesh->nqua,pMesh->nhex,pMesh->nadj);
										returnValue=0;	
									}
									PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);	
								}
							}
						}
					}
				}
			}	
#endif
		}
	}
*/
//*
	// To test with 'n' answer to the question (and also with or without LS_MODE)
	pMesh=&mesh;
	expectedValue=-1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=initializeCubeDiscretization(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
//*/
/*	
	for (iver=-nver; iver<=nver; iver+=nver)
	{
		for (jver=-nver; jver<=nver; jver+=nver)
		{
#ifndef LS_MODE
			for (icor=-ncor; icor<=ncor; icor+=ncor)
			{	
				for (inorm=-nnorm; inorm<=nnorm; inorm+=nnorm)
				{
					for (jnorm=-nnorm; jnorm<=nnorm; jnorm+=nnorm)
					{				
						for (itan=-ntan; itan<=ntan; itan+=ntan)
						{	
							for (jtan=-ntan; jtan<=ntan; jtan+=ntan)
							{	
								for (iedg=-nedg; iedg<=nedg; iedg+=nedg)
								{	
									for (jedg=-nedg; jedg<=nedg; jedg+=nedg)
									{
										for (itri=-ntri; itri<=ntri; itri+=ntri)
										{
											for (jtri=-ntri; jtri<=ntri; jtri+=ntri)
											{	
												for (itet=-ntet; itet<=ntet; itet+=ntet)
												{	
													for (jtet=-ntet; jtet<=ntet; jtet+=ntet)
													{
														pMesh->nver=iver;
														pMesh->pver=(Point*)calloc(jver,sizeof(Point));
														free(pMesh->pver);

														pMesh->ncor=icor;

														pMesh->nnorm=inorm;
														pMesh->pnorm=(Vector*)calloc(jnorm,sizeof(Vector));
														free(pMesh->pnorm);

														pMesh->ntan=itan;
														pMesh->ptan=(Vector*)calloc(jtan,sizeof(Vector));
														free(pMesh->ptan);

														pMesh->nedg=iedg;
														pMesh->pedg=(Edge*)calloc(jedg,sizeof(Edge));
														free(pMesh->pedg);

														pMesh->ntri=itri;
														pMesh->ptri=(Triangle*)calloc(jtri,sizeof(Triangle));
														free(pMesh->ptri);

														pMesh->ntet=itet;
														pMesh->ptet=(Tetrahedron*)calloc(jtet,sizeof(Tetrahedron));
														free(pMesh->ptet);

														PRINT_TEST_START(counter,expectedValue);
														printf("nver=%d pver=%p ncor=%d ",pMesh->nver,pMesh->pver,pMesh->ncor);
														printf("nnorm=%d pnorm=%p ntan=%d ",pMesh->nnorm,pMesh->pnorm,pMesh->ntan);
														printf("ptan=%p nedg=%d pedg=%p ",pMesh->ptan,pMesh->nedg,pMesh->pedg);
														printf("ntri=%d ptri=%p ntet=%d ptet=%p\n",pMesh->ntri,pMesh->ptri,pMesh->ntet,pMesh->ptet);
														returnValue=initializeCubeDiscretization(pMesh);
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
#endif
#ifdef LS_MODE
			for (iqua=-nqua; iqua<=nqua; iqua+=nqua)
			{
				for (jqua=-nqua; jqua<=nqua; jqua+=nqua)
				{	
					for (ihex=-nhex; ihex<=nhex; ihex+=nhex)
					{
						for (jhex=-nhex; jhex<=nhex; jhex+=nhex)
						{			
							for (iadj=-nadj; iadj<=nadj; iadj+=nadj)
							{
								for (jadj=-nadj; jadj<=nadj; jadj+=nadj)
								{	
									pMesh->nver=iver;
									pMesh->pver=(Point*)calloc(jver,sizeof(Point));						
									free(pMesh->pver);

									pMesh->nqua=iqua;
									pMesh->pqua=(Quadrilateral*)calloc(jqua,sizeof(Quadrilateral));	
									free(pMesh->pqua);					

									pMesh->nhex=ihex;
									pMesh->phex=(Hexahedron*)calloc(jhex,sizeof(Hexahedron));
									free(pMesh->phex);

									pMesh->nadj=iadj;
									pMesh->padj=(Adjacency*)calloc(jadj,sizeof(Adjacency));
									free(pMesh->padj);

									PRINT_TEST_START(counter,expectedValue);
									printf("nver=%d pver=%p nqua=%d pqua=%p ",pMesh->nver,pMesh->pver,pMesh->nqua,pMesh->pqua);
									printf("nhex=%d phex=%p nadj=%d padj=%p\n",pMesh->nhex,pMesh->phex,pMesh->nadj,pMesh->padj);
									returnValue=initializeCubeDiscretization(pMesh);
									PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);	
								}
							}
						}
					}
				}
			}	
#endif
		}
	}
*/
//*
	// To test with '\n' or any answer other than 'y' or 'n' to the question (and also with or without LS_MODE)
	pMesh=&mesh;
	expectedValue=0;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=initializeCubeDiscretization(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=initializeCubeDiscretization(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	PRINT_TEST_START(counter,expectedValue);
	returnValue=initializeCubeDiscretization(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
//*/
/*	
	for (iver=-nver; iver<=nver; iver+=nver)
	{
		for (jver=-nver; jver<=nver; jver+=nver)
		{
#ifndef LS_MODE
			for (icor=-ncor; icor<=ncor; icor+=ncor)
			{	
				for (inorm=-nnorm; inorm<=nnorm; inorm+=nnorm)
				{
					for (jnorm=-nnorm; jnorm<=nnorm; jnorm+=nnorm)
					{				
						for (itan=-ntan; itan<=ntan; itan+=ntan)
						{	
							for (jtan=-ntan; jtan<=ntan; jtan+=ntan)
							{	
								for (iedg=-nedg; iedg<=nedg; iedg+=nedg)
								{	
									for (jedg=-nedg; jedg<=nedg; jedg+=nedg)
									{
										for (itri=-ntri; itri<=ntri; itri+=ntri)
										{
											for (jtri=-ntri; jtri<=ntri; jtri+=ntri)
											{	
												for (itet=-ntet; itet<=ntet; itet+=ntet)
												{	
													for (jtet=-ntet; jtet<=ntet; jtet+=ntet)
													{
														pMesh->nver=iver;
														pMesh->pver=(Point*)calloc(jver,sizeof(Point));
														free(pMesh->pver);

														pMesh->ncor=icor;

														pMesh->nnorm=inorm;
														pMesh->pnorm=(Vector*)calloc(jnorm,sizeof(Vector));
														free(pMesh->pnorm);

														pMesh->ntan=itan;
														pMesh->ptan=(Vector*)calloc(jtan,sizeof(Vector));
														free(pMesh->ptan);

														pMesh->nedg=iedg;
														pMesh->pedg=(Edge*)calloc(jedg,sizeof(Edge));
														free(pMesh->pedg);

														pMesh->ntri=itri;
														pMesh->ptri=(Triangle*)calloc(jtri,sizeof(Triangle));
														free(pMesh->ptri);

														pMesh->ntet=itet;
														pMesh->ptet=(Tetrahedron*)calloc(jtet,sizeof(Tetrahedron));
														free(pMesh->ptet);

														PRINT_TEST_START(counter,expectedValue);
														printf("nver=%d pver=%p ncor=%d ",pMesh->nver,pMesh->pver,pMesh->ncor);
														printf("nnorm=%d pnorm=%p ntan=%d ",pMesh->nnorm,pMesh->pnorm,pMesh->ntan);
														printf("ptan=%p nedg=%d pedg=%p ",pMesh->ptan,pMesh->nedg,pMesh->pedg);
														printf("ntri=%d ptri=%p ntet=%d ptet=%p\n",pMesh->ntri,pMesh->ptri,pMesh->ntet,pMesh->ptet);
														returnValue=initializeCubeDiscretization(pMesh);
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
#endif
#ifdef LS_MODE
			for (iqua=-nqua; iqua<=nqua; iqua+=nqua)
			{
				for (jqua=-nqua; jqua<=nqua; jqua+=nqua)
				{	
					for (ihex=-nhex; ihex<=nhex; ihex+=nhex)
					{
						for (jhex=-nhex; jhex<=nhex; jhex+=nhex)
						{			
							for (iadj=-nadj; iadj<=nadj; iadj+=nadj)
							{
								for (jadj=-nadj; jadj<=nadj; jadj+=nadj)
								{	
									pMesh->nver=iver;
									pMesh->pver=(Point*)calloc(jver,sizeof(Point));						
									free(pMesh->pver);

									pMesh->nqua=iqua;
									pMesh->pqua=(Quadrilateral*)calloc(jqua,sizeof(Quadrilateral));	
									free(pMesh->pqua);					

									pMesh->nhex=ihex;
									pMesh->phex=(Hexahedron*)calloc(jhex,sizeof(Hexahedron));
									free(pMesh->phex);

									pMesh->nadj=iadj;
									pMesh->padj=(Adjacency*)calloc(jadj,sizeof(Adjacency));
									free(pMesh->padj);

									PRINT_TEST_START(counter,expectedValue);
									printf("nver=%d pver=%p nqua=%d pqua=%p ",pMesh->nver,pMesh->pver,pMesh->nqua,pMesh->pqua);
									printf("nhex=%d phex=%p nadj=%d padj=%p\n",pMesh->nhex,pMesh->phex,pMesh->nadj,pMesh->padj);
									returnValue=initializeCubeDiscretization(pMesh);
									PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);	
								}
							}
						}
					}
				}
			}	
#endif
		}
	}
*/

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

