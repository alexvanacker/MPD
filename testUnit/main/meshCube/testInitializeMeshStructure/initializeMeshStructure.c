#include "initializeMeshStructure.h"

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
void initializeMeshStructure(Mesh* pMesh);

int main(void)
{

	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int iver=0, jver=0;
	int nver=1210;
#ifndef LS_MODE
	int icor=0, inorm=0, jnorm=0, itan=0, jtan=0, iedg=0, jedg=0, itri=0, jtri=0, itet=0, jtet=0;
	int ncor=8, nnorm=534, ntan=128, nedg=140, ntri=1336,  ntet=5040;
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
	printf("Testing initializeMeshStructure function.\n");

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	printf("pMesh=%p\n",pMesh);
	initializeMeshStructure(pMesh);
	returnValue=1;
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	pMesh=&mesh;
	PRINT_TEST_START(counter,expectedValue);
	printf("pMesh=%p\n",pMesh);
	initializeMeshStructure(pMesh);
	PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

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

														returnValue=1;
														PRINT_TEST_START(counter,expectedValue);
														printf("nver=%d pver=%p ncor=%d ",pMesh->nver,pMesh->pver,pMesh->ncor);
														printf("nnorm=%d pnorm=%p ntan=%d ",pMesh->nnorm,pMesh->pnorm,pMesh->ntan); 
														printf("ptan=%p nedg=%d pedg=%p ",pMesh->ptan,pMesh->nedg,pMesh->pedg);
											 			printf("ntri=%d ptri=%p ntet=%d ptet=%p\n",pMesh->ntri,pMesh->ptri,pMesh->ntet,pMesh->ptet);

														initializeMeshStructure(pMesh);
														if (pMesh->nver!=0 || pMesh->pver!=NULL || pMesh->ncor!=0 || pMesh->nnorm!=0 || pMesh->pnorm!=NULL || pMesh->ntan!=0 || pMesh->ptan!=NULL || pMesh->nedg!=0 || pMesh->pedg!=NULL || pMesh->ntri!=0 || pMesh->ptri!=NULL || pMesh->ntet!=0 || pMesh->ptet!=NULL)
														{
															printf("Error: although initializeMeshStructure function ends normally, ");
															printf("the mesh structure was not properly initialized.\n");
															printf("Expecting nver=0 pver=NULL ncor=0 nnorm=0 pnorm=NULL ntan=0 ptan=NULL ");
															printf("nedg=0 pedg=NULL ntri=0 ptri=NULL ntet=0 ptet=NULL\ninstead of ");
															printf("nver=%d pver=%p ncor=%d ",pMesh->nver,pMesh->pver,pMesh->ncor);
															printf("nnorm=%d pnorm=%p ntan=%d ",pMesh->nnorm,pMesh->pnorm,pMesh->ntan); 
															printf("ptan=%p nedg=%d pedg=%p ",pMesh->ptan,pMesh->nedg,pMesh->pedg);
											 				printf("ntri=%d ptri=%p ntet=%d ",pMesh->ntri,pMesh->ptri,pMesh->ntet);
															printf("ptet=%p\n",pMesh->ptet);
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

									returnValue=1;
									PRINT_TEST_START(counter,expectedValue);
									initializeMeshStructure(pMesh);
									if (pMesh->nver!=0 || pMesh->pver!=NULL || pMesh->nqua!=0 || pMesh->pqua!=NULL || pMesh->nhex!=0 || pMesh->phex!=NULL || pMesh->nadj!=0 || pMesh->padj!=NULL)
									{
										printf("Error: although initializeMeshStructure function ends normally, the mesh structure was not properly initialized.\n");
										printf("Expecting nver=0 pver=NULL nqua=0 pqua=NULL nhex=0 phex=NULL nadj=0 padj=NULL\ninstead of ");
										printf("nver=%d pver=%p nqua=%d pqua=%p ",pMesh->nver,pMesh->pver,pMesh->nqua,pMesh->pqua);
										printf("nhex=%d phex=%p nadj=%d padj=%p\n",pMesh->nhex,pMesh->phex,pMesh->nadj,pMesh->padj);
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

