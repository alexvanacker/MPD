#include "main.h"

#include "loadChemistry.h"
#include "meshCube.h"
#include "adaptCube.h"
#include "shapeDerivative.h"

#define STR_PHASE 	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
#define STR_RELEASE 	"January 1st, 2018"
#define STR_VERSION	"1.1a"
#define STR_COPYRIGHT	"2018, ISCD-Sorbonne Universités"
#define STR_AUTHOR	"Jeremy DALPHIN"

// Macro functions to initialize and end the main function
#define INITIALIZE_MAIN_STRUCTURES(mesh,chemicalSystem,startTimer,argc,argv) do {										\
	time(startTimer); 																	\
	printf("\n%s\nMPD program starts. Released version %s (%s).\n",STR_PHASE,STR_VERSION,STR_RELEASE);							\
	printf("Copyright (c) %s. Author: %s.\n%s\n",STR_COPYRIGHT,STR_AUTHOR,STR_PHASE);									\
	switch(argc)																		\
	{																			\
		case 3:																		\
			printf("\nChemical data will be loaded from %s file.\n",(argv)[1]);									\
			printf("We look for optimal domain maximizing the probability to find exactly %s electrons.\n",(argv)[2]);				\
			break;																	\
		case 4:																		\
			printf("\nChemical data will be loaded from %s file.\n",(argv)[1]);									\
			printf("We look for the domain maximizing the probability to find exactly %s electrons.\n",(argv)[2]);					\
			printf("Mesh data will be loaded from %s file.\n",(argv)[3]);										\
			break;																	\
		default:																	\
			printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));		\
			printf("In main: the %s program only accepts two or three arguments (instead of %d).\n",(argv)[0],(argc)-1);				\
			printf("Expected syntax for arguments in command line (in this order): the *.wfn/*.txt (chemical data) file, ");			\
			printf("the (positive) number of electrons to look for, and optionally the *.mesh (mesh data) file to start with.\n");			\
			break;																	\
	}																			\
	initializeChemicalStructure(chemicalSystem);														\
	initializeMeshStructure(mesh);																\
} while (0)

#define FREE_AND_RETURN(mesh,chemicalSystem,startTimer,endTimer,overlapMatrix,eigenvectors,eigenvalues,valueToReturn) do {					\
	freeMeshMemory(mesh);																	\
	freeMatrixMemory((overlapMatrix),(eigenvectors),(eigenvalues));												\
	freeChemicalMemory(chemicalSystem);															\
	printf("\n%s\nTotal memory freed up. MPD program ends. Total execution time: ",STR_PHASE);								\
	time(endTimer);																		\
	if (difftime(*(endTimer),(startTimer))<60.)														\
	{																			\
		 printf("%lf seconds.\n%s\n\n",difftime(*(endTimer),(startTimer)),STR_PHASE);									\
	}																			\
	else 																			\
	{																			\
		printf("%lf minutes.\n%s\n\n",difftime(*(endTimer),(startTimer))/60.,STR_PHASE);								\
	}																			\
	return(valueToReturn);																	\
} while(0)

#define PRINT_LOCAL_TIME(integerToPrint,stringToPrint,startLocalTimer,endLocalTimer) do {									\
	time(endLocalTimer);																	\
	if (difftime(*(endLocalTimer),(startLocalTimer))<60.)													\
	{																			\
		 printf("\n%s\nITERATION %d COMPLETED IN %lf SECONDS: ",(stringToPrint),(integerToPrint),difftime(*(endLocalTimer),(startLocalTimer)));		\
	}																			\
	else 																			\
	{																			\
		printf("\n\n%s\nITERATION %d COMPLETED IN %lf MINUTES: ",(stringToPrint),(integerToPrint),difftime(*(endLocalTimer),(startLocalTimer))/60.);	\
	}																			\
} while (0)

/*
// Prototypes of non-static functions in main.c
int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);
*/

// Prototypes of static functions in main.c
static int checkAllPreprocessorConstants(int lsmode, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int nx, int ny, int nz, double deltax, double deltay, double deltaz, double lsr, double lsx, double lsy, double lsz, int orb1, int orb2, int orb3, int orb4, int orb5, int orb6, int orb7, int orb8, int orb9, int orb10, int orb11, int orb12, int orb13, int orb14, int orb15, int orb16, int orb17, int orb18, int orb19, int orb20, int orbspin, double metcst, double meterr, double metmin, double metmax, int itermax, double tolmax, double cstA, double cstB, double cstC, double csta, double cstb, double cstc, double cstaa, double cstbb, double cstcc);

/* ************************************************************************************************************************************************************************************** */
// The function main performs on an initial sphere (LS_XYZR) in a cube (mesh variable) iMax deformations to maximize the probability to find nuElectrons in a chemicalSystem structure.
// It has the usual int argc, char *argv[] command line as input arguments and returns EXIT_FAILURE if an error occurs in the optimization process, otherwise EXIT_SUCCESS is returned.
// Expected syntax for command line (in this order): the *.wfn/*.txt (chemical data) file, the (positive) number of electrons to look for, and optionally the *.mesh file to start with.
/* ************************************************************************************************************************************************************************************** */
int main(int argc, char *argv[])
{	
	char commandLineMmg3dIsotropic[150]="./../bin/mmg3d_O3 -in cube.mesh -out cubeAdapt.mesh -hmin 0.1 -hmax 0.2 -hausd 0.01 -hgrad 2.0";
	char commandLineMmg3dMetric[150]="./../bin/mmg3d_O3 -in cube.mesh -sol cube.sol -out cubeAdapt.mesh -hmin 0.01 -hmax 1.0 -hausd 0.01 -hgrad 2.0";
	char commandLineMmg3dLevelSet[150]="./../bin/mmg3d_O3 -in cube.mesh -ls -sol cube.sol -out cubeAdapt.mesh -nr -hmin 0.05 -hmax 1.0 -hausd 0.01 -hgrad 2.0";

	time_t startTimer=0, endTimer=0, startLocalTimer=0, endLocalTimer=0;
	int lsmode=0, nuElectrons=0, sizeMemory=0, i=0, iMax=1000;
	double *overlapMatrix=NULL, *eigenvectors=NULL, *eigenvalues=NULL, probability=0., probabilityOld=0.;
#ifdef LS_MODE
	int trick=1;
	#if (!LS_MODE)
	int j=0, k=0;
	#endif
#endif
	Mesh mesh;
	ChemicalSystem chemicalSystem;
		
	// Initialize Mesh and Chemical structure, then check number of arguments in command line and set up nuElectrons variable given as input argv[2] variable of main function
	INITIALIZE_MAIN_STRUCTURES(&mesh,&chemicalSystem,&startTimer,argc,argv);
	if (argc<3 || argc >4)
	{
		FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
	}
	nuElectrons=atoi(argv[2]);	// Remark: if argv[2] is not a positive integer, atoi function returns zero which will be considered as an error in loadchemistry function 

	// Initialize rand() function thanks to srand and time functions (must be performed only one time) if LS_MODE parameter is defined)
#ifdef LS_MODE
	lsmode=LS_MODE;
	#if (!LS_MODE)
	srand(time(NULL));
	#endif
#endif
	// Check the preprocessor parameters
	if (!checkAllPreprocessorConstants(lsmode,X_MIN,X_MAX,Y_MIN,Y_MAX,Z_MIN,Z_MAX,N_X,N_Y,N_Z,DELTA_X,DELTA_Y,DELTA_Z,LS_R,LS_X,LS_Y,LS_Z,ORB_1S,ORB_2PX,ORB_2PY,ORB_2PZ, ORB_3DXX,ORB_3DYY,ORB_3DZZ,ORB_3DXY,ORB_3DXZ,ORB_3DYZ,ORB_4FXXX,ORB_4FYYY,ORB_4FZZZ,ORB_4FXXY,ORB_4FXXZ,ORB_4FYYZ,ORB_4FXYY,ORB_4FXZZ,ORB_4FYZZ,ORB_4FXYZ,ORB_SPIN,MET_CST,MET_ERR,MET_MIN,MET_MAX, ITER_MAX,TOL_MAX,CST_A,CST_B,CST_C,CST_a,CST_b,CST_c,CST_aa,CST_bb,CST_cc))
	{
		PRINT_ERROR();
		printf("In main: checkAllPreprocessorConstants function returned zero instead of one.\n");
		FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
	}

	// Cleaning the working directory from eventual file names used in the algorithm // Remark (not used here): system function returns -1 if an error occurred, otherwise the return command status
	printf("\nCleaning working directory from eventual file names used in the algorithm.\n");
	system("rm cube.mesh cube.sol cubeOut.mesh cubeOut.sol cubeAdapt.mesh cubeAdapt.sol cube.chi.sol cube.new.sol metric.sol chemicalOut.txt"); 

	// Loading chemicalSystem data thanks to the *.txt/*.wfn file given as input argv[1] variable of main function
	if (!loadChemistry(argv[1],&chemicalSystem,nuElectrons))
	{
		PRINT_ERROR();
		printf("In main: loadChemistry function returned zero instead of one.\n");
		FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);	
	}

	// Allocating memory for matrices depending on the number of Molecular Orbitals just loaded
	if (!allocateMatrixMemory(&overlapMatrix,&eigenvectors,&eigenvalues,chemicalSystem.nmorb))
	{
		PRINT_ERROR();
		printf("In main: allocateMatrixMemory function returned zero instead of one.\n");
		FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);	
	}

	// Loading the Mesh data thanks to *.mesh file if given as input argv[3] variable of main function, otherwise build an initial mesh of the cube thanks to preprocessor parameters
	switch(meshCube(argv[3],&mesh))
	{
		case 0:
			PRINT_ERROR();
			printf("In main: meshCube function returned zero instead of one.\n");
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
			break;

		case -1:
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_SUCCESS);
			break;

		default:
			PRINT_LOCAL_TIME(-1,STR_PHASE,startTimer,&endLocalTimer);
			printf("CHEMICAL DATA AND MESH DATA INITIALIZED.\n%s\n",STR_PHASE);
			system("medit cube.mesh"); 	// Vizualize the mesh of the initial cube with medit (Warning: the medit software must have been previously installed)
			break;
	}

	// Prepare the mesh for the optimization loop (initialization of level-set function and perform some mesh adaptations if necessary)
	time(&startLocalTimer);
	if (!adaptCube(argv[3],commandLineMmg3dIsotropic,commandLineMmg3dMetric,commandLineMmg3dLevelSet,&mesh,&chemicalSystem))
	{
		PRINT_ERROR();
		printf("In main: adaptCube function returned zero instead of one.\n");
		FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);	
	}
	PRINT_LOCAL_TIME(0,STR_PHASE,startLocalTimer,&endLocalTimer);
	printf("MESH ADAPTED AND READY FOR THE OPTIMIZATION LOOP.\n%s\n",STR_PHASE);	

	// Starting the optimization loop
	for (i=0; i<iMax; i++)
	{
		printf("\nStarting iteration %d\n",i+1);
		time(&startLocalTimer);

#ifndef LS_MODE 
		// Evaluating on each mesh vertices the metric associated with the orbitals in order to avoid interpolation error with the previous mesh
		if (!evaluatingMetricOnMesh(&mesh,&chemicalSystem))
		{
			PRINT_ERROR();
			printf("In adaptCube: evaluatingMetricOnMesh function returned zero instead of one.\n");
			return 0;
		}

		// Saving values in a file entitled metric.sol (Warning: file name "metric.sol" is completely imposed here, used for personal modified version of mmg3d)
		if (!writingSolFile("metric.sol",&mesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: writingSolFile function returned zero instead of one.\n");
			return 0;
		}
		system("mv cube.mesh metric.mesh");
		//system("medit metric.mesh");		// Warning: medit software must have been previously installed
		system("mv metric.mesh cube.mesh");

		// Redistanciate the leve-set function thanks to mshdist algorithm (Warning: software mshdist must have been previously installed)
		system("mshdist cube.mesh -dom -ncpu 4"); // -it 100");
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed
		system("mv cube.sol cube.chi.sol");	// Advect software (must have been previously installed) imposes the file name cube.chi.sol for the level-set function 

		// COmpute the overlapMatrix, the probability and the shapeDerivative 
		if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,2,0))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}

		// Saving the vectorial shape derivative (theta) in cubeOut.sol file
		if (!writingShapeSolFile("cube.sol",&mesh))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed
		system("rm cube.mesh");

		// Saving the mesh in cube.mesh file with the right label on vertices (same mesh, only label were modified, no need to reload it, just useful for elastic algorithm with Dirichlet BC)
		if(!writingMeshFile("cube.mesh",&mesh))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}		
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed

		//  Extend the velocity field of shape derivative thanks to elasticity algorithm (must have been previously installed, also need parameters in cube.elas of ../inputFiles directory)
		system("elastic cube.mesh -p ../inputFiles/cube.elas -s cube.sol -o cube.new.sol");
		system("rm cube.sol");	
		system("mv cube.new.sol cube.sol");
		//system("medit cube.mesh");

		// Advecting the level-set function using the extended vector field (advect sofware must have been previously installed)
		system("advect -dt 1.0 -nocfl cube.mesh -c cube.chi.sol -s cube.sol -o cube.new.sol");
		system("rm cube.sol cube.chi.sol");
		system("mv cube.new.sol cube.sol");
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed

		// Free the memory allocated for the mesh since we are going to adapt it again
		sizeMemory=sizeof(Mesh)+(mesh.nver)*sizeof(Point)+(mesh.nnorm+mesh.ntan)*sizeof(Vector)+(mesh.nedg)*sizeof(Edge)+(mesh.ntri)*sizeof(Triangle)+(mesh.ntet)*sizeof(Tetrahedron);
		printf("\nCleaning allocated memory (%d,%d Mo) for a mesh adaption to the new level-set interface geometry.\n",sizeMemory/1000000,sizeMemory%1000000);
		freeMeshMemory(&mesh);

		// Isotropic mesh adaptation according to both level-set and orbitals metric (Warning: modified version of mmg3d software must have been previously installed in ../bin/ directory)
		system(commandLineMmg3dLevelSet); // commandLineMmg3dLevelSet = "./../bin/mmg3d_O3 -in cube.mesh -ls -sol cube.sol -out cubeAdapt.mesh -nr -hmin 0.05 -hmax 1.0 -hausd 0.01 -hgrad 2.0"
		system("rm cube.mesh cube.sol cubeAdapt.sol metric.sol"); // mmg3d commandLines are input arguments so as to change parameters (hmin,...) directly in main function (file names are imposed)
		system("mv cubeAdapt.mesh cube.mesh");
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed

		//  Read file entitled cube.mesh containing the new adapted mesh to the orbitals and the level-set interface geometry
		if(!readAndAllocateMesh("cube.mesh",&mesh))
		{
			PRINT_ERROR();
			printf("In adaptCube: readAndAllocateMesh function returned zero instead of one.\n");
			return 0;
		}
		printf("New mesh data loaded: %d Vertices, %d Tetrahedra, %d Triangles.\n",mesh.nver,mesh.ntet,mesh.ntri);
		printf("New geometry loaded: %d Ridges, %d Corners, %d Normals, %d Tangents.\n",mesh.nedg,mesh.ncor,mesh.nnorm,mesh.ntan);
#endif
#ifdef LS_MODE
	#if (LS_MODE)		
		if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,trick,i))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}
	
		// Free the memory allocated for Adjacency table since we are going to rebuild it
		sizeMemory=sizeof(int)+(mesh.nadj)*sizeof(Adjacency);
		printf("\nCleaning allocated memory (%d,%d Mo) for Adjacency table in order to rebuild it.\n",sizeMemory/1000000,sizeMemory%1000000);
		free(mesh.padj);
		mesh.nadj=0;
		mesh.padj=NULL;

		if (!getLevelSetQuadrilaterals(&mesh))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}

		// Saving the mesh in cube.mesh file with the right label on vertices
		if(!writingMeshFile("cube.mesh",&mesh))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}	
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed

	#else
		if (i==0)
		{
			if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,2,i))
			{
				FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
			}
		}

		// Choose to add a random hexahedra k having in its boundary the j-th MPD-boundary quadrilaterals
		j=rand()%(mesh.nadj);
		if (!trick)
		{	
			k=AddOrRemoveHexahedronToMesh(&mesh,j,1);
		}
		else
		{
			k=AddOrRemoveHexahedronToMesh(&mesh,j,-3);
		}
		if(!k)
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}

		// Free the memory allocated for Adjacency table since we are going to rebuild it
		sizeMemory=sizeof(int)+(mesh.nadj)*sizeof(Adjacency);
		printf("\nCleaning allocated memory (%d,%d Mo) for Adjacency table in order to rebuild it.\n",sizeMemory/1000000,sizeMemory%1000000);
		free(mesh.padj);
		mesh.nadj=0;
		mesh.padj=NULL;

		if (!getLevelSetQuadrilaterals(&mesh))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}

		if (!trick)
		{
			if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,2,i))
			{
				FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
			}
		}
		else
		{
			if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,-3,i))
			{
				FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
			}		
		}

		// If probablity to add an element is not increasing the probability, try to delete the corresponding adjacent element sharing j-th quadrilateral
		if (probability>probabilityOld)
		{
			printf("\nIteration %d: adding external hexahedron %d next to MPD-boundary quadrilateral %d increase the probability.\n",i+1,k+1,j);
			mesh.phex[k-1].label=abs(mesh.phex[k-1].label);
		}
		else
		{
			probability=probabilityOld;
			mesh.phex[k-1].label=2;
			if (!trick)
			{	
				k=AddOrRemoveHexahedronToMesh(&mesh,j,-1);
			}
			else
			{
				k=AddOrRemoveHexahedronToMesh(&mesh,j,-2);
			}
			if(!k)
			{
				FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
			}

			// Free the memory allocated for Adjacency table since we are going to rebuild it
			sizeMemory=sizeof(int)+(mesh.nadj)*sizeof(Adjacency);
			printf("\nCleaning allocated memory (%d,%d Mo) for Adjacency table in order to rebuild it.\n",sizeMemory/1000000,sizeMemory%1000000);
			free(mesh.padj);
			mesh.nadj=0;
			mesh.padj=NULL;

			if (!getLevelSetQuadrilaterals(&mesh))
			{
				FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
			}

			if (!trick)
			{
				if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,2,i))
				{
					FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
				}
			}
			else
			{
				if(!shapeDerivative(&mesh,&chemicalSystem,overlapMatrix,eigenvectors,eigenvalues,nuElectrons,&probability,&probabilityOld,-2,i))
				{
					FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
				}		
			}

			if (probability>probabilityOld)
			{
				mesh.phex[k-1].label=abs(mesh.phex[k-1].label);
				printf("\nIteration %d: adding external hexahedron %d next to MPD-boundary quadrilateral %d increase the probability.\n",i+1,k+1,j);
			}
			else
			{
				mesh.phex[k-1].label=3;
				printf("\nSub-iteration %d: adding or removing hexahedra sharing quadrilateral %d does not increase the probability.\n",i+1,j);
			}
		}

		// Saving the mesh in cube.mesh file with the right label on vertices
		if(!writingMeshFile("cube.mesh",&mesh))
		{
			FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_FAILURE);
		}		
		//system("medit cube.mesh");	// Warning: medit software must have been previously installed
	#endif	
#endif

		PRINT_LOCAL_TIME(1+i,STR_PHASE,startLocalTimer,&endLocalTimer);
		printf("ENDING WITH A NEW OPTIMIZED AND ADAPTED MESH.\n%s\n",STR_PHASE);
	}

	system("mv cube.mesh cubeSave.mesh");
	FREE_AND_RETURN(&mesh,&chemicalSystem,startTimer,&endTimer,&overlapMatrix,&eigenvectors,&eigenvalues,EXIT_SUCCESS);
}

/* *************************************************************************************************************************************************************** */
// The function checkAllPreprocessorConstants tests if the constants defined with the preprocessor syntax are valid since they are used throughout the algorithm
// It has all preprocessor constants input arguments and it returns zero if an error is encountered with the different constants, otherwise one is returned
/* *************************************************************************************************************************************************************** */
static int checkAllPreprocessorConstants(int lsmode, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int nx, int ny, int nz, double deltax, double deltay, double deltaz, double lsr, double lsx, double lsy, double lsz, int orb1, int orb2, int orb3, int orb4, int orb5, int orb6, int orb7, int orb8, int orb9, int orb10, int orb11, int orb12, int orb13, int orb14, int orb15, int orb16, int orb17, int orb18, int orb19, int orb20, int orbspin, double metcst, double meterr, double metmin, double metmax, int itermax, double tolmax, double cstA, double cstB, double cstC, double csta, double cstb, double cstc, double cstaa, double cstbb, double cstcc)
{
	int boolean=0;

	printf("\nChecking validity of preprocessor parameters defined in all *.h files.\n");
#ifdef LS_MODE
	// Check the LS_MODE parameter if it is defined (0 generates random perturbations, while 1 generates perturbations according to the shape derivative, other values are forbidden) 
	boolean=(lsmode==0 || lsmode==1);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: LS_MODE=%d can only be set to zero or one. Please modify the parameter accordingly in main.h file.\n",lsmode);
		return 0;
	}
#endif
	// Check the initial parameters of the numerical cube entered in main.h and meshcube.h file
	boolean=(xmin<xmax && ymin<ymax && zmin<zmax && nx>2 && ny>2 && nz>2); 	// nxyz >= 3 in order to have at least one normal/tangent vectors on each faces of the initial cube
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: incorrect description of the cube. Expecting\n");
		printf("(X_MIN=%lf) < (X_MAX=%lf)\n(Y_MIN=%lf) < (Y_MAX=%lf)\n(Z_MIN=%lf) < (Z_MAX=%lf)\n",xmin,xmax,ymin,ymax,zmin,zmax); 
		printf("(N_X=%d) > 2\n(N_Y=%d) > 2\n(N_Z=%d) > 2\nPlease modify the parameters accordingly in main.h and/or meshcube.h files.\n",nx,ny,nz);
		return 0; 
	}
	boolean=(deltax==(xmax-xmin)/(double)(nx-1) && deltay==(ymax-ymin)/(double)(ny-1) && deltaz==(zmax-zmin)/(double)(nz-1));
	if (!boolean) 
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: incorrect discretization of the cube. Expecting\n");
		printf("(DELTA_X=%lf) = %lf\n(DELTA_Y=%lf) = %lf\n(DELTA_Z=%lf) = %lf\n",deltax,(xmax-xmin)/(double)(nx-1),deltay,(ymax-ymin)/(double)(ny-1),deltaz,(zmax-zmin)/(double)(nz-1));
		printf("Please modify the parameters accordingly in main.h and/or meshcube.h files.\n");
		return 0;
	}

	// Check parameters of the initial sphere entered in main.h
	boolean=(lsr>0.);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: the radius LS_R=%lf of the initial sphere is not positive. Please modify the parameter accordingly in main.h file.\n",lsr);
		return 0;
	}
	boolean=(lsx>=xmin && lsx<=xmax && lsy>=ymin && lsy<=ymax && lsz>=zmin && lsz<=zmax);
	if (!boolean)
	{
		printf("\nWarning in checkAllPreprocessorConstants function: the center (%lf,%lf,%lf) of the initial sphere (defined in main.h file) ",lsx,lsy,lsz);
		printf("is not located inside the initial cube [%lf,%lf]x[%lf,%lf]x[%lf,%lf] (defined in meshCube.h file).\n",xmin,xmax,ymin,ymax,zmin,zmax);
	}

	// Check the parameters of main.h file associated with the standard nomenclature of the wfn/wfx format (outputs of Gaussian software)
	boolean=(orb1==1 && orb2==2 && orb3==3 && orb4==4 && orb5==5 && orb6==6 && orb7==7 && orb8==8 && orb9==9 && orb10==10);
	if(!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: expecting\n(ORB_1S=%d) = 1\n(ORB_2PX=%d) = 2\n(ORB_2PY=%d) = 3\n(ORB_2PZ=%d) = 4\n(ORB_3DXX=%d) = 5\n",orb1,orb2,orb3,orb4,orb5);
		printf("(ORB_3DYY=%d) = 6\n(ORB_3DZZ=%d) = 7\n(ORB_3DXY=%d) = 8\n(ORB_3DXZ=%d) = 9\n(ORB_3DYZ=%d) = 10\n",orb6,orb7,orb8,orb9,orb10);
		printf("Please modify the parameters accordingly in main.h file.\n");
		return 0;
	}
	boolean=(orb11==11 && orb12==12 && orb13==13 && orb14==14 && orb15==15 && orb16==16 && orb17==17 && orb18==18 && orb19==19 && orb20==20);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: expecting\n");
		printf("(ORB_4FXXX=%d) = 11\n(ORB_4FYYY=%d) = 12\n(ORB_4FZZZ=%d) = 13\n(ORB_4FXXY=%d) = 14\n(ORB_4FXXZ=%d) = 15\n",orb11,orb12,orb13,orb14,orb15);
		printf("(ORB_4FYYZ=%d) = 16\n(ORB_4FXXY=%d) = 17\n(ORB_4FXZZ=%d) = 18\n(ORB_4FYZZ=%d) = 19\n(ORB_4FXYZ=%d) = 20\n",orb16,orb17,orb18,orb19,orb20);
		printf("Please modify the parameters accordingly in main.h file.\n");
		return 0;	
	}

	// Check the ORB_SPIN parameter defined in readAndSaveFiles.h (it can only be set to 1 (or 0) and it duplicates (or not) the doubly occupied molecular orbitals when loading the chemical structure)
	boolean=(orbspin==0 || orbspin==1);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: ORB_SPIN=%d can only be set to zero or one. Please modify the parameter accordingly in loadChemistry.h file.\n",orbspin);		
		return 0;
	}

	// Check that adaptation parameters of adaptCube.h file are correct
	boolean=(metcst==.28125 && meterr>0. && metmin>0. && metmax>metmin && itermax>0 && tolmax>0.);
	if(!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: expecting\n(MET_CST=%lf) = 0.28125\n(MET_ERR=%lf) > 0\n(MET_MIN=%lf) > 0\n(MET_MAX=%lf) > MET_MIN\n",metcst,meterr,metmin,metmax);
		printf("(ITER_MAX=%d) > 0\n(TOL_MAX=%lf) > 0\nPlease modify the parameters accordingly in adaptCube.h file.\n",itermax,tolmax);
		return 0;
	}

	// Check the integration parameters of shapeDerivative.h file
	boolean=(cstA==0.007091003462846911 && cstB==0.018781320953002642 && cstC==0.012248840519393658);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: expecting\n(CST_A=%.18lf) = 0.007091003462846911\n(CST_B=%.18lf) = 0.018781320953002642\n(CST_C=%.18lf) = 0.012248840519393658\n",cstA,cstB,cstC);
		printf("Please modify the parameters accordingly in shapeDerivative.h file.\n");
		return 0;
	}
	boolean=(csta==0.454496295874350351 && cstb==0.310885919263300610 && cstc==0.092735250310891226);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: expecting\n(CST_a=%.18lf) = 0.454496295874350351\n(CST_b=%.18lf) = 0.310885919263300610\n(CST_c=%.18lf) = 0.092735250310891226\n",csta,cstb,cstc);
		printf("Please modify the parameters accordingly in shapeDerivative.h file.\n");
		return 0;
	}
	boolean=(cstaa==0.045503704125649649 && cstbb==0.067342242210098170 && cstcc==0.721794249067326322);
	if (!boolean)
	{
		PRINT_ERROR();
		printf("In checkAllPreprocessorConstants: expecting\n(CST_aa=%.18lf) = 0.045503704125649649\n(CST_bb=%.18lf) = 0.067342242210098170\n",cstaa,cstbb);
		printf("(CST_cc=%.18lf) = 0.721794249067326322\nPlease modify the parameters accordingly in shapeDerivative.h file.\n",cstcc);
		return 0;
	}

	printf("Preprocessor parameters are ok.\n");

	return 1;
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

