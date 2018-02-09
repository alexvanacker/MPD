#include "shapeDerivative.h"

/*
// Prototypes of non-static functions in shapeDerivative.c
void freeMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues);
int allocateMatrixMemory(double** pOverlapMatrix, double** pEigenvectors, double** pEigenvalues, int size);
int shapeDerivative(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, double* eigenvectors, double* eigenvalues, int nuElectrons, double* pProbability, double* pProbabilityOld, int trick, int i);

#ifdef LS_MODE
	int AddOrRemoveHexahedronToMesh(Mesh* pMesh, int j, int addOrRemove);
#endif

#ifndef LS_MODE
	int writingShapeSolFile(char* fileLocation, Mesh* pMesh);
#endif
*/

// Prototypes of static functions in shapeDerivative.c
#ifdef LS_MODE
	static int convertingType(int type, int oriention);
	static double evaluateGaussianIntegral(int exp0, double t0, double t1);
	static double evaluateOneIntegralIJ(int expI, int expJ, double cI, double cJ, double t0, double t1);
	static double evaluateTripleIntegralIJ(Mesh *pMesh, ChemicalSystem *pChemicalSystem, MolecularOrbital *pMolecularOrbitalI, MolecularOrbital *pMolecularOrbitalJ, int primitiveI, int primitiveJ, int kHexahedron);
	static int computeOverlapMatrixOnGrid(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, int label);
#endif

#ifndef LS_MODE
	static double computeVolumeTetrahedron(Point* pp1, Point* pp2, Point* pp3, Point* pp4);	
	static int computeOverlapMatrix(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, int label);
#endif

	static int diagonalizeOverlapMatrix(double* overlapMatrix, double* eigenvectors, double* eigenvalues, int sizeMatrix);
	static double computeProbability(double* eigenvalues, int numberElectrons, int nuElectrons);
	static double evaluatingPrimitiveAtVertices(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbital, int primitive);
	static double evaluateOrbitalsIJ(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbitalI, MolecularOrbital* pMolecularOrbitalJ);
	static double evaluateOrbitalsII(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbitalI);
	static int computeShapeDerivative(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* eigenvalues, double* eigenvectors, int nuElectrons, int trick);

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

#ifdef LS_MODE
// Convert the type into another label defined by type=x+10y+100z with x, y , z the exponents in the sous-couche, orientation reverse the labelling
static int convertingType(int type, int orientation)
{
	int returnValue=-1;
	
	if (orientation==1)
	{
		switch(type)
		{
			default: 
				printf("Warning: type should be between 1 and 20 instead of %d. Treated as zero (s-type orbital).\n",type);
			case ORB_1S:
				returnValue=0;
				break;
			case ORB_2PX:
				returnValue=1;
				break;
			case ORB_2PY:
				returnValue=10;
				break;
			case ORB_2PZ:
				returnValue=100;
				break;
			case ORB_3DXX:
				returnValue=2;
				break;
			case ORB_3DYY:
				returnValue=20;
				break;
			case ORB_3DZZ:
				returnValue=200;
				break;			
			case ORB_3DXY:
				returnValue=11;
				break;
			case ORB_3DXZ:
				returnValue=101;
				break;
			case ORB_3DYZ:
				returnValue=110;
				break;
			case ORB_4FXXX:
				returnValue=3;
				break;
			case ORB_4FYYY:
				returnValue=30;
				break;
			case ORB_4FZZZ:
				returnValue=300;
				break;
			case ORB_4FXXY:
				returnValue=12;
				break;
			case ORB_4FXXZ:
				returnValue=102;
				break;
			case ORB_4FXYY:
				returnValue=21;
				break;
			case ORB_4FYYZ:
				returnValue=120;
				break;
			case ORB_4FXZZ:
				returnValue=201;
				break;
			case ORB_4FYZZ:
				returnValue=210;
				break;
			case ORB_4FXYZ:
				returnValue=111;
				break;
		}
	}
	else if (orientation==-1)
	{
		switch(type)
		{
			default: 
				printf("Warning: type %d does not correspond to the known type. Treated as zero (s-type orbital).\n",type);
			case 0:
				returnValue=ORB_1S;
				break;
			case 1:
				returnValue=ORB_2PX;
				break;
			case 10:
				returnValue=ORB_2PY;
				break;
			case 100:
				returnValue=ORB_2PZ;
				break;
			case 2:
				returnValue=ORB_3DXX;
				break;
			case 20:
				returnValue=ORB_3DYY;
				break;
			case 200:
				returnValue=ORB_3DZZ;
				break;			
			case 11:
				returnValue=ORB_3DXY;
				break;
			case 101:
				returnValue=ORB_3DXZ;
				break;
			case 110:
				returnValue=ORB_3DYZ;
				break;
			case 3:
				returnValue=ORB_4FXXX;
				break;
			case 30:
				returnValue=ORB_4FYYY;
				break;
			case 300:
				returnValue=ORB_4FZZZ;
				break;
			case 12:
				returnValue=ORB_4FXXY;
				break;
			case 102:
				returnValue=ORB_4FXXZ;
				break;
			case 21:
				returnValue=ORB_4FXYY;
				break;
			case 120:
				returnValue=ORB_4FYYZ;
				break;
			case 201:
				returnValue=ORB_4FXZZ;
				break;
			case 210:
				returnValue=ORB_4FYZZ;
				break;
			case 111:
				returnValue=ORB_4FXYZ;
				break;
		}
	}
	else
	{
		printf("Warning: orientation should be (+/-) 1 instead of %d. Nothing has been done.\n",orientation);
		returnValue=type;
	}

	return returnValue;
}


static double evaluateGaussianIntegral(int exp0, double t0, double t1)
{
	double integral=0.;

	switch(exp0)
	{
		default: 
			printf("Warning: exp should be 0,1,2,3,4,5 or 6 instead of %d. Treated as zero.\n",exp0);
		case 0:
			integral=sqrt(M_PI)*(erf(t1)-erf(t0));
			break;

		case 1:
			integral=exp(-t0*t0)-exp(-t1*t1);
			break;

		case 2:
			integral=.5*sqrt(M_PI)*(erf(t1)-erf(t0));
			integral+=t0*exp(-t0*t0)-t1*exp(-t1*t1);
			break;

		case 3:
			integral=(1.+t0*t0)*exp(-t0*t0)-(1.+t1*t1)*exp(-t1*t1);
			break;

		case 4:
			integral=.75*sqrt(M_PI)*(erf(t1)-erf(t0));
			integral+=t0*(1.5+t0*t0)*exp(-t0*t0)-t1*(1.5+t1*t1)*exp(-t1*t1);
			break;

		case 5:
			integral=(2.+t0*t0*(2.+t0*t0))*exp(-t0*t0)-(2.+t1*t1*(2.+t1*t1))*exp(-t1*t1);
			break;

		case 6:
			integral=1.875*sqrt(M_PI)*(erf(t1)-erf(t0));
			integral+=t0*(3.75+t0*t0*(2.5+t0*t0))*exp(-t0*t0)-t1*(3.75+t1*t1*(2.5+t1*t1))*exp(-t1*t1);
			break;
	}

	return integral;
}

// Compute from t0 to t1 the integral of 2 *(t-cI)^expI * (t+cJ)^expJ * exp(-t^2) with respect to the t-variable, and according to the different 16 cases (expI=0..3 expJ=0..3)
static double evaluateOneIntegralIJ(int expI, int expJ, double cI, double cJ, double t0, double t1)
{
	double integral=0.;

	switch(expI)
	{	
		default: 
			printf("Warning: expI should be 0,1,2, or 3 instead of %d. Treated as zero (s-type orbital).\n",expI);	
		case 0:
			switch(expJ)
			{
				default: 
					printf("Warning: expJ should be 0,1,2, or 3 instead of %d. Treated as zero (s-type orbital).\n",expJ);
				case 0:
					integral=evaluateGaussianIntegral(0,t0,t1);
					break;

				case 1:
					integral=evaluateGaussianIntegral(1,t0,t1);
					integral+=cJ*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 2:
					integral=evaluateGaussianIntegral(2,t0,t1);
					integral+=2.*cJ*evaluateGaussianIntegral(1,t0,t1);
					integral+=cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 3:	
					integral=evaluateGaussianIntegral(3,t0,t1);
					integral+=3.*cJ*evaluateGaussianIntegral(2,t0,t1);
					integral+=3.*cJ*cJ*evaluateGaussianIntegral(1,t0,t1);
					integral+=cJ*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;
			}
			break;

		case 1:
			switch(expJ)
			{
				default: 
					printf("Warning: expJ should be 0,1,2, or 3 instead of %d. Treated as zero (s-type orbital).\n",expJ);
				case 0:
					integral=evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 1:
					integral=evaluateGaussianIntegral(2,t0,t1);
					integral+=(cJ-cI)*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 2:	
					integral=evaluateGaussianIntegral(3,t0,t1);
					integral+=(2.*cJ-cI)*evaluateGaussianIntegral(2,t0,t1);
					integral+=cJ*(cJ-2.*cI)*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 3:
					integral=evaluateGaussianIntegral(4,t0,t1);
					integral+=(3.*cJ-cI)*evaluateGaussianIntegral(3,t0,t1);
					integral+=3.*cJ*(cJ-cI)*evaluateGaussianIntegral(2,t0,t1);
					integral+=cJ*cJ*(cJ-3.*cI)*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cJ*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;		
			}
			break;

		case 2:
			switch(expJ)
			{
				default: 
					printf("Warning: expJ should be 0,1,2, or 3 instead of %d. Treated as zero (s-type orbital).\n",expJ);

				case 0:
					integral=evaluateGaussianIntegral(2,t0,t1);
					integral-=2.*cI*evaluateGaussianIntegral(1,t0,t1);
					integral+=cI*cI*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 1:	
					integral=evaluateGaussianIntegral(3,t0,t1);
					integral+=(cJ-2.*cI)*evaluateGaussianIntegral(2,t0,t1);
					integral+=cI*(cI-2.*cJ)*evaluateGaussianIntegral(1,t0,t1);
					integral+=cI*cI*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;
				case 2:
					integral=evaluateGaussianIntegral(4,t0,t1);
					integral+=2.*(cJ-cI)*evaluateGaussianIntegral(3,t0,t1);
					integral+=(cJ*cJ-4.*cI*cJ+cI*cI)*evaluateGaussianIntegral(2,t0,t1);
					integral+=2.*cI*cJ*(cI-cJ)*evaluateGaussianIntegral(1,t0,t1);
					integral+=cI*cI*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 3:
					integral=evaluateGaussianIntegral(5,t0,t1);
					integral+=(3.*cJ-2.*cI)*evaluateGaussianIntegral(4,t0,t1);
					integral+=(cI*cI-6.*cI*cJ+3.*cJ*cJ)*evaluateGaussianIntegral(3,t0,t1);
					integral+=cJ*(cJ*cJ-6.*cI*cJ+3.*cI*cI)*evaluateGaussianIntegral(2,t0,t1);
					integral+=cI*cJ*cJ*(3.*cI-2.*cJ)*evaluateGaussianIntegral(1,t0,t1);
					integral+=cI*cI*cJ*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;		
			}
			break;

		case 3:
			switch(expJ)
			{
				default: 
					printf("Warning: expJ should be 0,1,2, or 3 instead of %d. Treated as zero (s-type orbital).\n",expJ);
				case 0:	
					integral=evaluateGaussianIntegral(3,t0,t1);
					integral-=3.*cI*evaluateGaussianIntegral(2,t0,t1);
					integral+=3.*cI*cI*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cI*cI*evaluateGaussianIntegral(0,t0,t1);
					break;
				case 1:
					integral=evaluateGaussianIntegral(4,t0,t1);
					integral+=(cJ-3.*cI)*evaluateGaussianIntegral(3,t0,t1);
					integral+=3.*cI*(cI-cJ)*evaluateGaussianIntegral(2,t0,t1);
					integral+=cI*cI*(3.*cJ-cI)*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cI*cI*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;

				case 2:
					integral=evaluateGaussianIntegral(5,t0,t1);
					integral+=(2.*cJ-3.*cI)*evaluateGaussianIntegral(4,t0,t1);
					integral+=(cJ*cJ-6.*cI*cJ+3.*cI*cI)*evaluateGaussianIntegral(3,t0,t1);
					integral-=cI*(cI*cI-6.*cI*cJ+3.*cJ*cJ)*evaluateGaussianIntegral(2,t0,t1);
					integral+=cI*cI*cJ*(3.*cJ-2.*cI)*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cI*cI*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;	

				case 3:
					integral=evaluateGaussianIntegral(6,t0,t1);
					integral+=3.*(cJ-cI)*evaluateGaussianIntegral(5,t0,t1);
					integral+=3.*(cJ*cJ-3.*cI*cJ+cI*cI)*evaluateGaussianIntegral(4,t0,t1);
					integral+=(cJ-cI)*(cI*cI-8.*cI*cJ+cJ*cJ)*evaluateGaussianIntegral(3,t0,t1);
					integral-=3.*cI*cJ*(cJ*cJ-3.*cI*cJ+cI*cI)*evaluateGaussianIntegral(2,t0,t1);
					integral+=3.*cI*cI*cJ*cJ*(cJ-cI)*evaluateGaussianIntegral(1,t0,t1);
					integral-=cI*cI*cI*cJ*cJ*cJ*evaluateGaussianIntegral(0,t0,t1);
					break;	
			}
			break;
	}

	return integral;
}

static double evaluateTripleIntegralIJ(Mesh *pMesh, ChemicalSystem *pChemicalSystem, MolecularOrbital *pMolecularOrbitalI, MolecularOrbital *pMolecularOrbitalJ, int primitiveI, int primitiveJ, int kHexahedron)
{
	// First, if coefficient is zero, return zero to avoid waist of time
	double coefficientI=pMolecularOrbitalI->coeff[primitiveI], coefficientJ=pMolecularOrbitalJ->coeff[primitiveJ];
	if (coefficientI==0.||coefficientJ==0.)
	{
		return 0.;
	}
	double exponentI=pMolecularOrbitalI->exp[primitiveI], exponentJ=pMolecularOrbitalJ->exp[primitiveJ], exponentIJ=exponentI+exponentJ;
	int typeI=pMolecularOrbitalI->type[primitiveI], typeJ=pMolecularOrbitalJ->type[primitiveJ], layerI=convertingType(typeI,1), layerJ=convertingType(typeJ,1);
	int sublayerIz=layerI/100, sublayerIy=layerI%100, sublayerIx=sublayerIy%10, sublayerJz=layerJ/100, sublayerJy=layerJ%100, sublayerJx=sublayerJy%10;
	sublayerIy/=10;
 	sublayerJy/=10;
	int layerIJ=sublayerIx+sublayerIy+sublayerIz+sublayerJx+sublayerJy+sublayerJz;
	int iNucleus=pMolecularOrbitalI->nucl[primitiveI]; 	
	Nucleus* pCenterI=&pChemicalSystem->pnucl[iNucleus-1]; 	// Warning: we stored Nuclei ref as in chemicalSystem.txt so do not forget to remove one from these int when calling ->pnucl
	iNucleus=pMolecularOrbitalJ->nucl[primitiveJ];
	Nucleus* pCenterJ=&pChemicalSystem->pnucl[iNucleus-1]; 	// Warning: we stored Nuclei ref as in chemicalSystem.txt so do not forget to remove one from these int when calling ->pnucl
	iNucleus=pMesh->phex[kHexahedron].p1;
	Point *pPoint=&pMesh->pver[iNucleus-1];
	double dxI=(pPoint->x)-(pCenterI->x), dxJ=(pPoint->x)-(pCenterJ->x), dyI=(pPoint->y)-(pCenterI->y), dyJ=(pPoint->y)-(pCenterJ->y), dzI=(pPoint->z)-(pCenterI->z), dzJ=(pPoint->z)-(pCenterJ->z);
	double dx=(pCenterI->x)-(pCenterJ->x), dy=(pCenterI->y)-(pCenterJ->y), dz=(pCenterI->z)-(pCenterJ->z), distanceSquared=dx*dx+dy*dy+dz*dz;
	double exponent=exponentI*exponentJ/exponentIJ, coefficient=.125*coefficientI*coefficientJ*exp(-exponent*distanceSquared)/pow(exponentIJ,1.5+.5*layerIJ);
	exponentIJ=sqrt(exponentIJ);
	double integral=coefficient, valuePlusX=(exponentI*(dxI+DELTA_X)+exponentJ*(dxJ+DELTA_X))/exponentIJ, valueMoinsX=(exponentI*dxI+exponentJ*dxJ)/exponentIJ;
	double valuePlusY=(exponentI*(dyI+DELTA_Y)+exponentJ*(dyJ+DELTA_Y))/exponentIJ, valueMoinsY=(exponentI*dyI+exponentJ*dyJ)/exponentIJ;
	double valuePlusZ=(exponentI*(dzI+DELTA_Z)+exponentJ*(dzJ+DELTA_Z))/exponentIJ, valueMoinsZ=(exponentI*dzI+exponentJ*dzJ)/exponentIJ;
	double cIx=exponentJ*dx/exponentIJ, cJx=exponentI*dx/exponentIJ, cIy=exponentJ*dy/exponentIJ, cJy=exponentI*dy/exponentIJ, cIz=exponentJ*dz/exponentIJ, cJz=exponentI*dz/exponentIJ;
 
	// Evaluating integrate coordinate by coordinate
	integral*=evaluateOneIntegralIJ(sublayerIx,sublayerJx,cIx,cJx,valueMoinsX,valuePlusX);
	integral*=evaluateOneIntegralIJ(sublayerIy,sublayerJy,cIy,cJy,valueMoinsY,valuePlusY);
	integral*=evaluateOneIntegralIJ(sublayerIz,sublayerJz,cIz,cJz,valueMoinsZ,valuePlusZ);

	return integral;
}

/* ******************************************************************************************************************************************************************************************************* */
// The function computeOverlapMatrixOnGrid calculates the overlap matrix S(Omega)_ij = int_Omega Orb(i)*Orb(j) of pChemicalSystem on pMesh elements that are NOT tagged by the integer label argument 
//  It has the ChemicalSystem*, Mesh* variables (described in the readAndSaveFile.h, included in all *.h files) as input arguments, but also the label not to ignore and the array overlapMatrix to store
// It returns one on success, otherwise zero. Remark: in mmmg3d Exterior=2, Interior=3, Level-set=10 so put label=2 to get S(Omega), label=3 to get S(R^3\Omega), label=-1 to get S(R^3) in main function  
/* ******************************************************************************************************************************************************************************************************* */
static int computeOverlapMatrixOnGrid(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, int label)
{
	int i=0, j=0, k=0, lI=0, lJ=0, numberOfHexahedra=pMesh->nhex, numberOfMolecularOrbitals=pChemicalSystem->nmorb, numberOfPrimitives=pChemicalSystem->ngauss;
	double integral=0., integralK=0.;
	MolecularOrbital *pMolecularOrbitalI=NULL, *pMolecularOrbitalJ=NULL;

	printf("\nEvaluating the overlap matrix S(%d) on mesh.\n",label);
	// Checking chemical parameters
	if (numberOfHexahedra<1)
	{
		PRINT_ERROR();
		printf("In computeOverlapMatrixOnGrid: we have %d Hexahedra while at least one is expected.\n",numberOfHexahedra);
		return 0;
	}
	else if (numberOfMolecularOrbitals<1)
	{
		PRINT_ERROR();
		printf("In computeOverlapMatrixOnGrid: we have %d Orbitals while at least one is expected.\n",numberOfMolecularOrbitals);
		return 0;
	}
	else if (numberOfPrimitives<1)
	{
		PRINT_ERROR();
		printf("In computeOverlapMatrixOnGrid: we have %d Primitives while at least one is expected.\n", numberOfPrimitives);
		return 0;
	}

	// Evaluation of the symmetric overlap matrix
	for (i=0; i<numberOfMolecularOrbitals; i++)
	{
		pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
		for (j=0; j<=i; j++)
		{
			pMolecularOrbitalJ=&pChemicalSystem->pmorb[j];
			if (label>-2)
			{			
				overlapMatrix[i*numberOfMolecularOrbitals+j]=0.;
			}
			if((pMolecularOrbitalI->spin)!=(pMolecularOrbitalJ->spin))
			{
				continue;	// Case where Orbitals i and j have different spin (nothing to do since value initialized at zero) 
			}

			integralK=0.;

#pragma omp parallel for reduction(+:integralK) private(integral,lI,lJ)
			for (k=0; k<numberOfHexahedra; k++)
			{
				integral=0.;
				// Skip the domain label given in the input
				if (label<-1)
				{
					if (pMesh->phex[k].label!=label)
					{
						continue;
					}
				}
				else
				{	
					if (pMesh->phex[k].label==label)
					{
						continue;
					}
				}
				for (lI=0; lI<numberOfPrimitives; lI++)
				{
					for (lJ=0; lJ<numberOfPrimitives; lJ++)
					{
						integral+=evaluateTripleIntegralIJ(pMesh,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ,lI,lJ,k);
					}
				}
				integralK+=integral;
			}

			if (label==-2)
			{
				overlapMatrix[i*numberOfMolecularOrbitals+j]-=integralK;
			}
			else
			{
				overlapMatrix[i*numberOfMolecularOrbitals+j]+=integralK;
			}
			printf("S(%d,%d)=%le\n",i+1,j+1,overlapMatrix[i*numberOfMolecularOrbitals+j]);
		}
		// Do not need to complete the matrix which is symmetric (remark for lapacke: if matrix is stored column-wise, only its upper triangular part contains non-zero coefficients)	
	}

	return 1;
}
#endif

#ifndef LS_MODE
/* ********************************************************************************************************************************************************* */
// The function computeVolumeTetrahedron evaluates the volume (in fact 6 times the volume) of a tetrehedron defined by four points.
// It has the four Point* variables as input arguments, and it returns the volume of the trihedron frame (12,13,14) in terms of determinant (vector product) 
/* ********************************************************************************************************************************************************* */
static double computeVolumeTetrahedron(Point* pp1, Point* pp2, Point* pp3, Point* pp4)
{
	double a11=pp2->x, a12=pp3->x, a13=pp4->x; 
	double a21=pp2->y, a22=pp3->y, a23=pp4->y;
	double a31=pp2->z, a32=pp3->z, a33=pp4->z;
	double determinant=pp1->x;

	a11-=determinant;
	a12-=determinant;
	a13-=determinant;
	determinant=pp1->y;
	a21-=determinant;
	a22-=determinant;
	a23-=determinant;
	determinant=pp1->z;
	a31-=determinant;
	a32-=determinant;
	a33-=determinant;
	
	determinant=a11*(a22*a33-a32*a23)-a21*(a12*a33-a32*a13)+a31*(a12*a23-a22*a13);

	return fabs(determinant); 	// Remark: in fact we have computed 6*volumeOfTetrahedron but it is already included in the sum of the weight coefficients (6*CST_A+4*CST_B+4*CST_C=1/6)
}

/* ******************************************************************************************************************************************************************************************************* */
// The function computeOverlapMatrix calculates the overlap matrix S(Omega)_ij = int_Omega Orb(i)*Orb(j) of pChemicalSystem on pMesh elements that are NOT tagged by the integer label argument 
//  It has the ChemicalSystem*, Mesh* variables (described in the readAndSaveFile.h, included in all *.h files) as input arguments, but also the label not to ignore and an array overlapMatrix to store
// It returns one on success, otherwise zero. Remark: in mmmg3d Exterior=2, Interior=3, Level-set=10 so put label=2 to get S(Omega), label=3 to get S(R^3\Omega), label=-1 to get S(R^3) in main function  
/* ******************************************************************************************************************************************************************************************************* */
static int computeOverlapMatrix(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, int label)
{
	int i=0, j=0, k=0, ip1=0, ip2=0, ip3=0, ip4=0, numberOfTetrahedra=pMesh->ntet, numberOfMolecularOrbitals=pChemicalSystem->nmorb, numberOfPrimitives=pChemicalSystem->ngauss;
	double integral=0., integralK=0., volumeTetrahedron=0., value=0., px=0., py=0., pz=0.;
	MolecularOrbital *pMolecularOrbitalI=NULL, *pMolecularOrbitalJ=NULL;
	Point  *pp1=NULL, *pp2=NULL, *pp3=NULL, *pp4=NULL;

	printf("\nEvaluating the overlap matrix S(%d) on mesh.\n",label);
	// Checking chemical parameters
	if (numberOfTetrahedra<1)
	{
		PRINT_ERROR();
		printf("In computeOverlapMatrix: we have %d Tetrahedra while at least one is expected.\n",numberOfTetrahedra);
		return 0;
	}
	else if (numberOfMolecularOrbitals<1)
	{
		PRINT_ERROR();
		printf("In computeOverlapMatrix: we have %d Orbitals while at least one is expected.\n",numberOfMolecularOrbitals);
		return 0;
	}
	else if (numberOfPrimitives<1)
	{
		PRINT_ERROR();
		printf("In computeOverlapMatrix: we have %d Primitives while at least one is expected.\n", numberOfPrimitives);
		return 0;
	}

	// Evaluation of the symmetric overlap matrix
	for (i=0; i<numberOfMolecularOrbitals; i++)
	{
		pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
		for (j=0; j<i; j++)
		{
			pMolecularOrbitalJ=&pChemicalSystem->pmorb[j];			
			overlapMatrix[i*numberOfMolecularOrbitals+j]=0.;
			if((pMolecularOrbitalI->spin)!=(pMolecularOrbitalJ->spin))
			{
				continue;	// Case where Orbitals i and j have different spin (nothing to do since value initialized at zero) 
			}
			integralK=0.;
#pragma omp parallel for reduction(+:integralK) private(integral,volumeTetrahedron,value,ip1,ip2,ip3,ip4,pp1,pp2,pp3,pp4,px,py,pz)
			for (k=0; k<numberOfTetrahedra; k++)
			{
				// Skip the domain label given in the input (if abs(label)!=10 ok else only compute the one given by label)
				if (pMesh->ptet[k].label==label)
				{
					continue;
				}

				// Get the adresses of the points of the k-th tetrahedron
				ip1=pMesh->ptet[k].p1;	// Warning here: we have stored point reference as given in .mesh file so do not forget to remove one from these int when calling the pMesh->pver
				ip2=pMesh->ptet[k].p2;
				ip3=pMesh->ptet[k].p3;
				ip4=pMesh->ptet[k].p4;

				pp1=&pMesh->pver[ip1-1];
				pp2=&pMesh->pver[ip2-1];
				pp3=&pMesh->pver[ip3-1];
				pp4=&pMesh->pver[ip4-1];
				// 1st to 6th point estimation
				px=CST_a*((pp1->x)+(pp2->x))+CST_aa*((pp3->x)+(pp4->x));
				py=CST_a*((pp1->y)+(pp2->y))+CST_aa*((pp3->y)+(pp4->y));
				pz=CST_a*((pp1->z)+(pp2->z))+CST_aa*((pp3->z)+(pp4->z));
				value=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_a*((pp1->x)+(pp3->x))+CST_aa*((pp2->x)+(pp4->x));
				py=CST_a*((pp1->y)+(pp3->y))+CST_aa*((pp2->y)+(pp4->y));
				pz=CST_a*((pp1->z)+(pp3->z))+CST_aa*((pp2->z)+(pp4->z));
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);				

				px=CST_a*((pp1->x)+(pp4->x))+CST_aa*((pp2->x)+(pp3->x));
				py=CST_a*((pp1->y)+(pp4->y))+CST_aa*((pp2->y)+(pp3->y));
				pz=CST_a*((pp1->z)+(pp4->z))+CST_aa*((pp2->z)+(pp3->z));
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_a*((pp2->x)+(pp3->x))+CST_aa*((pp1->x)+(pp4->x));
				py=CST_a*((pp2->y)+(pp3->y))+CST_aa*((pp1->y)+(pp4->y));
				pz=CST_a*((pp2->z)+(pp3->z))+CST_aa*((pp1->z)+(pp4->z));
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_a*((pp2->x)+(pp4->x))+CST_aa*((pp1->x)+(pp3->x));
				py=CST_a*((pp2->y)+(pp4->y))+CST_aa*((pp1->y)+(pp3->y));
				pz=CST_a*((pp2->z)+(pp4->z))+CST_aa*((pp1->z)+(pp3->z));
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_a*((pp3->x)+(pp4->x))+CST_aa*((pp1->x)+(pp2->x));
				py=CST_a*((pp3->y)+(pp4->y))+CST_aa*((pp1->y)+(pp2->y));
				pz=CST_a*((pp3->z)+(pp4->z))+CST_aa*((pp1->z)+(pp2->z));
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);
				value*=CST_A;
				integral=value;
				// 7th to 10th point estimation
				px=CST_b*((pp1->x)+(pp2->x)+(pp3->x))+CST_bb*(pp4->x);
				py=CST_b*((pp1->y)+(pp2->y)+(pp3->y))+CST_bb*(pp4->y);
				pz=CST_b*((pp1->z)+(pp2->z)+(pp3->z))+CST_bb*(pp4->z);
				value=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_b*((pp1->x)+(pp2->x)+(pp4->x))+CST_bb*(pp3->x);
				py=CST_b*((pp1->y)+(pp2->y)+(pp4->y))+CST_bb*(pp3->y);
				pz=CST_b*((pp1->z)+(pp2->z)+(pp4->z))+CST_bb*(pp3->z);
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_b*((pp1->x)+(pp3->x)+(pp4->x))+CST_bb*(pp2->x);
				py=CST_b*((pp1->y)+(pp3->y)+(pp4->y))+CST_bb*(pp2->y);
				pz=CST_b*((pp1->z)+(pp3->z)+(pp4->z))+CST_bb*(pp2->z);
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_b*((pp2->x)+(pp3->x)+(pp4->x))+CST_bb*(pp1->x);
				py=CST_b*((pp2->y)+(pp3->y)+(pp4->y))+CST_bb*(pp1->y);
				pz=CST_b*((pp2->z)+(pp3->z)+(pp4->z))+CST_bb*(pp1->z);
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);
				value*=CST_B;
				integral+=value;
				// 10th to 14th point estimation
				px=CST_c*((pp1->x)+(pp2->x)+(pp3->x))+CST_cc*(pp4->x);
				py=CST_c*((pp1->y)+(pp2->y)+(pp3->y))+CST_cc*(pp4->y);
				pz=CST_c*((pp1->z)+(pp2->z)+(pp3->z))+CST_cc*(pp4->z);
				value=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_c*((pp1->x)+(pp2->x)+(pp4->x))+CST_cc*(pp3->x);
				py=CST_c*((pp1->y)+(pp2->y)+(pp4->y))+CST_cc*(pp3->y);
				pz=CST_c*((pp1->z)+(pp2->z)+(pp4->z))+CST_cc*(pp3->z);
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_c*((pp1->x)+(pp3->x)+(pp4->x))+CST_cc*(pp2->x);
				py=CST_c*((pp1->y)+(pp3->y)+(pp4->y))+CST_cc*(pp2->y);
				pz=CST_c*((pp1->z)+(pp3->z)+(pp4->z))+CST_cc*(pp2->z);
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);

				px=CST_c*((pp2->x)+(pp3->x)+(pp4->x))+CST_cc*(pp1->x);
				py=CST_c*((pp2->y)+(pp3->y)+(pp4->y))+CST_cc*(pp1->y);
				pz=CST_c*((pp2->z)+(pp3->z)+(pp4->z))+CST_cc*(pp1->z);
				value+=evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);
				value*=CST_C;
				integral+=value;
				// Multiply by the volume of the tetrahedron
				volumeTetrahedron=computeVolumeTetrahedron(pp1,pp2,pp3,pp4);
				integral*=volumeTetrahedron;
				integralK=integralK+integral;
			}
			overlapMatrix[i*numberOfMolecularOrbitals+j]=integralK;
			printf("S(%d,%d)=%le\n",i+1,j+1,overlapMatrix[i*numberOfMolecularOrbitals+j]);
		}
		// Treat the particular case i=j
		overlapMatrix[i*numberOfMolecularOrbitals+i]=0.;
		integralK=0.;
#pragma omp parallel for reduction(+:integralK) private(integral,volumeTetrahedron,value,ip1,ip2,ip3,ip4,pp1,pp2,pp3,pp4,px,py,pz)
		for (k=0; k<numberOfTetrahedra; k++)
		{		
			// Skip the domain label given in the input
			if (pMesh->ptet[k].label==label)
			{
				continue;	
			}
			// Get the adresses of the points of the k-th tetrahedron
			ip1=pMesh->ptet[k].p1;	// Warning here: we have stored point reference as given in .mesh file so do not forget to remove one from these int when calling the pMesh->pver
			ip2=pMesh->ptet[k].p2;
			ip3=pMesh->ptet[k].p3;
			ip4=pMesh->ptet[k].p4;

			pp1=&pMesh->pver[ip1-1];
			pp2=&pMesh->pver[ip2-1];
			pp3=&pMesh->pver[ip3-1];
			pp4=&pMesh->pver[ip4-1];
			// 1st to 6th point estimation
			px=CST_a*((pp1->x)+(pp2->x))+CST_aa*((pp3->x)+(pp4->x));
			py=CST_a*((pp1->y)+(pp2->y))+CST_aa*((pp3->y)+(pp4->y));
			pz=CST_a*((pp1->z)+(pp2->z))+CST_aa*((pp3->z)+(pp4->z));
			value=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_a*((pp1->x)+(pp3->x))+CST_aa*((pp2->x)+(pp4->x));
			py=CST_a*((pp1->y)+(pp3->y))+CST_aa*((pp2->y)+(pp4->y));
			pz=CST_a*((pp1->z)+(pp3->z))+CST_aa*((pp2->z)+(pp4->z));
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);				

			px=CST_a*((pp1->x)+(pp4->x))+CST_aa*((pp2->x)+(pp3->x));
			py=CST_a*((pp1->y)+(pp4->y))+CST_aa*((pp2->y)+(pp3->y));
			pz=CST_a*((pp1->z)+(pp4->z))+CST_aa*((pp2->z)+(pp3->z));
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_a*((pp2->x)+(pp3->x))+CST_aa*((pp1->x)+(pp4->x));
			py=CST_a*((pp2->y)+(pp3->y))+CST_aa*((pp1->y)+(pp4->y));
			pz=CST_a*((pp2->z)+(pp3->z))+CST_aa*((pp1->z)+(pp4->z));
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_a*((pp2->x)+(pp4->x))+CST_aa*((pp1->x)+(pp3->x));
			py=CST_a*((pp2->y)+(pp4->y))+CST_aa*((pp1->y)+(pp3->y));
			pz=CST_a*((pp2->z)+(pp4->z))+CST_aa*((pp1->z)+(pp3->z));
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_a*((pp3->x)+(pp4->x))+CST_aa*((pp1->x)+(pp2->x));
			py=CST_a*((pp3->y)+(pp4->y))+CST_aa*((pp1->y)+(pp2->y));
			pz=CST_a*((pp3->z)+(pp4->z))+CST_aa*((pp1->z)+(pp2->z));
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);
			value*=CST_A;
			integral=value;
			// 7th to 10th point estimation
			px=CST_b*((pp1->x)+(pp2->x)+(pp3->x))+CST_bb*(pp4->x);
			py=CST_b*((pp1->y)+(pp2->y)+(pp3->y))+CST_bb*(pp4->y);
			pz=CST_b*((pp1->z)+(pp2->z)+(pp3->z))+CST_bb*(pp4->z);
			value=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_b*((pp1->x)+(pp2->x)+(pp4->x))+CST_bb*(pp3->x);
			py=CST_b*((pp1->y)+(pp2->y)+(pp4->y))+CST_bb*(pp3->y);
			pz=CST_b*((pp1->z)+(pp2->z)+(pp4->z))+CST_bb*(pp3->z);
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_b*((pp1->x)+(pp3->x)+(pp4->x))+CST_bb*(pp2->x);
			py=CST_b*((pp1->y)+(pp3->y)+(pp4->y))+CST_bb*(pp2->y);
			pz=CST_b*((pp1->z)+(pp3->z)+(pp4->z))+CST_bb*(pp2->z);
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_b*((pp2->x)+(pp3->x)+(pp4->x))+CST_bb*(pp1->x);
			py=CST_b*((pp2->y)+(pp3->y)+(pp4->y))+CST_bb*(pp1->y);
			pz=CST_b*((pp2->z)+(pp3->z)+(pp4->z))+CST_bb*(pp1->z);
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);
			value*=CST_B;
			integral+=value;
			// 10th to 14th point estimation
			px=CST_c*((pp1->x)+(pp2->x)+(pp3->x))+CST_cc*(pp4->x);
			py=CST_c*((pp1->y)+(pp2->y)+(pp3->y))+CST_cc*(pp4->y);
			pz=CST_c*((pp1->z)+(pp2->z)+(pp3->z))+CST_cc*(pp4->z);
			value=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_c*((pp1->x)+(pp2->x)+(pp4->x))+CST_cc*(pp3->x);
			py=CST_c*((pp1->y)+(pp2->y)+(pp4->y))+CST_cc*(pp3->y);
			pz=CST_c*((pp1->z)+(pp2->z)+(pp4->z))+CST_cc*(pp3->z);
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_c*((pp1->x)+(pp3->x)+(pp4->x))+CST_cc*(pp2->x);
			py=CST_c*((pp1->y)+(pp3->y)+(pp4->y))+CST_cc*(pp2->y);
			pz=CST_c*((pp1->z)+(pp3->z)+(pp4->z))+CST_cc*(pp2->z);
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);

			px=CST_c*((pp2->x)+(pp3->x)+(pp4->x))+CST_cc*(pp1->x);
			py=CST_c*((pp2->y)+(pp3->y)+(pp4->y))+CST_cc*(pp1->y);
			pz=CST_c*((pp2->z)+(pp3->z)+(pp4->z))+CST_cc*(pp1->z);
			value+=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);
			value*=CST_C;
			integral+=value;
			// Multiply by the volume of the tetrahedron
			volumeTetrahedron=computeVolumeTetrahedron(pp1,pp2,pp3,pp4);
			integral*=volumeTetrahedron;
			integralK=integralK+integral;			
		}
		overlapMatrix[i*numberOfMolecularOrbitals+i]=integralK;
		printf("S(%d,%d)=%le\n",i+1,i+1,overlapMatrix[i*numberOfMolecularOrbitals+i]);
		// Do not need to complete the matrix which is symmetric (remark for lapacke: if matrix is stored column-wise, only its upper triangular part contains non-zero coefficients)	
	}

	return 1;
}

#endif

/* ************************************************************************************************************************************************************************ */
// The function diagonalizeOverlapMatrix diagonalize the (square symmetric) overlapMatrix of sizeMatrix and computes its pEigenvalues and associated pEigenvectors
// It has three arrays (eigenvalues,eigenvectors,overlapMatrix), the int sizeMatrix as input arguments, and it returns one on sucess, otherwise zero (error encountered)
/* ************************************************************************************************************************************************************************ */
static int diagonalizeOverlapMatrix(double* overlapMatrix, double* eigenvectors, double* eigenvalues, int sizeMatrix)
{
	int i=0, j=0, matrixLayout=LAPACK_COL_MAJOR;	// Since Lapack is written in Fortran (column-wise storage), better to use LAPACK_COL_MAJOR instead of LAPACK_ROW_MAJOR to avoid transposition step
	char jobz='V';	// If jobz='N' only computes the eigenvalues and if jobz='V' computes also the (orthonormal) eigenvectors on exit in A (each column is an eigenvector if LAPACK_COL_MAJOR is used) 
	char uplo='U';	// if uplo='U' (respectively uplo='L') consider the upper (resp. lower) part of the matrix as input for the symmetric matrix to diagonalize
	lapack_int n=sizeMatrix, lda=sizeMatrix;	// n is used to interpret the array with matrixLayout and lda is the leading dimension of the array
	lapack_int info=0;	//info is the function output (=0 successfull exit, =-i<0 if i-th input variable is illegal, =i>0 if an error occurs in the diagonalization process)

	// Check that sizeMatrix is positive
	if (sizeMatrix<1)
	{
		PRINT_ERROR();
		printf("In diagonalizeOverlapMatrix: the size of the overlapMatrix is %d and should be positive.\n",sizeMatrix);
		return 0;
	}

	printf("\nEvaluating eigenvalues of the overlap matrix.\n");

	// Filling the eigenvector matrix with the coefficients of the overlapMatrix and initializing the eigenvalues to zero
	for (i=0; i<n; i++)
	{
		for (j=0; j<n; j++)
		{
			eigenvectors[i*n+j]=overlapMatrix[i*n+j];
		}
		eigenvalues[i]=0.;
	}
	// Diagonalization using lapacke library (lapack interface for c)
	info=LAPACKE_dsyev(matrixLayout,jobz,uplo,n,eigenvectors,lda,eigenvalues);
	if(info)
	{
		PRINT_ERROR();
		printf("In diagonalizeOverlapMatrix: when diagonalizing overlapMatrix, Lapack dsyev routine failed and returns %d.\n",info);
		return 0;
	}
	// Printing the eigenvalues and eigenvectors
	for (i=0; i<n; i++)
	{
		printf("%le \n",eigenvalues[i]);
	}
	printf("\nEigenvectors: \n");
	for (i=0; i<n; i++)
	{
		for (j=0; j<n; j++)
		{
			printf("%le ",eigenvectors[i*n+j]);
		}
		printf("\n");
	}

	return 1;
}

/* ****************************************************************************************************************************************************************************************************** */
// The function computeProbability calculates the probability to find nuElectrons in Omega and numberOfElectrons-nuElectrons in the complement of Omega by using the eigenvalues (Cancès et al. formula)
// It has two integers (numberOfElectrons, nuElectrons), a double array (eigenvalues) as input arguments, and it returns the probability, otherwise zero (error encountered) is returned as double value
/* ****************************************************************************************************************************************************************************************************** */
static double computeProbability(double* eigenvalues, int numberOfElectrons, int nuElectrons)
{
	double *coefficients=NULL, value=0., alpha=0., beta=0.;
	int i=0, j=0, n=numberOfElectrons+1;

	if (numberOfElectrons<1||nuElectrons<0||nuElectrons>numberOfElectrons)
	{
		PRINT_ERROR();
		printf("In computeProbability: search for nu=%d electrons among %d electrons.\n",nuElectrons,numberOfElectrons);
		return 0.;
	}
	printf("\nComputing the probability to find %d electrons among %d in the domain Omega:\n",nuElectrons,numberOfElectrons);
	// Allocate memory for the coefficients
	coefficients=(double*)calloc(n*n,sizeof(double));
	if (coefficients==NULL)
	{
		PRINT_ERROR();
		printf("In computeProbability: could not allocate memory for the coefficients in the evaluation of the probability.\n");
		return 0.;
	}
	// Computing the recursive formula (5)--(7) of Cancès, Keriven, Lodier and Savin (2004, p.374) allowing to compute the probability in O(numberOfElectrons^2) operations
	coefficients[0]=1.;
	for (i=1; i<n; i++)
	{
		beta=eigenvalues[i-1];
		alpha=1.-beta;
		value=alpha*coefficients[(i-1)*n];
		coefficients[i*n]=value;
		for (j=1; j<i; j++)
		{
			value=beta*coefficients[(i-1)*n+(j-1)]+alpha*coefficients[(i-1)*n+j];
			coefficients[i*n+j]=value;
		}
		value=beta*coefficients[(i-1)*n+(i-1)];
		coefficients[i*n+i]=value;
	}
	// Printing the different probabilities
	for (i=0; i<n; i++)
	{
		value=coefficients[numberOfElectrons*n+i];
		if (value<0.||value>1.)
		{
			printf("\nWarning in computeProbability function: probability(%d electrons in Omega) = %lf which is not between zero and one.\n",i,value);
		} 
		if (i==nuElectrons)
		{
			printf("\n");
		}
		printf("Probability(%d electrons in Omega) = %lf\n",i,value);
		if (i==nuElectrons)
		{
			printf("\n");
		}
	}
	// Printing the total population of electrons in Omega using the formula sum_nu nu*P_nu(Omega)
	value=0.;
	for (i=1; i<n; i++)
	{
		value+=i*coefficients[numberOfElectrons*n+i];
	}
	printf("\nTotal population in Omega is equal to %lf\n",value);

	// Free the memory
	value=coefficients[numberOfElectrons*n+nuElectrons];
	if (coefficients!=NULL)
	{
		free(coefficients);
		coefficients=NULL;
	}

	return value;
}

/* ******************************************************************************************************************************************************************************************************* */
// The function evaluatingPrimitiveAtVertices evaluates a (gaussian function) primitive of pMolecularOrbital described in pChemicalSystem at the point defined by (px,py,pz) 
// It has the ChemicalSystem*, MolecularOrbital* variables (described in the readAndSaveFile.h, included in all *.h files), the point coordinates and the int characterizing the gaussian as input arguments 
// It returns the value of the gaussian primitive
/* ******************************************************************************************************************************************************************************************************* */
static double evaluatingPrimitiveAtVertices(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbital, int primitive)
{
	// First, if coefficient is zero, return zero to avoid waist of time
	double coefficient=pMolecularOrbital->coeff[primitive];
	if (coefficient==0.)
	{
		return 0.;
	}
	double exponent=pMolecularOrbital->exp[primitive];
	int type=pMolecularOrbital->type[primitive], iNucleus=pMolecularOrbital->nucl[primitive]; 
	Nucleus* pCenter=&pChemicalSystem->pnucl[iNucleus-1]; 	// Warning: we stored Nuclei ref as in chemicalSystem.txt so do not forget to remove one from these int when calling ->pnucl
	double dx=px-(pCenter->x), dy=py-(pCenter->y), dz=pz-(pCenter->z), distanceSquared=dx*dx+dy*dy+dz*dz;
	double function=coefficient*exp(-exponent*distanceSquared);
	switch(type)
	{
		case ORB_1S:	// These parameters are defined in readAndSaveFiles.h (common file in all *.h files) bacause it is also used in adaptCube.c functions
			break;
		case ORB_2PX:	
			function*=dx;
			break;
		case ORB_2PY:	
			function*=dy;
			break;
		case ORB_2PZ:	
			function*=dz;
		case ORB_3DXX:	
			function*=dx*dx;
			break;
		case ORB_3DYY:	
			function*=dy*dy;
			break;
		case ORB_3DZZ:	
			function*=dz*dz;
			break;
		case ORB_3DXY:	
			function*=dx*dy;
			break;
		case ORB_3DXZ:	
			function*=dx*dz;
			break;
		case ORB_3DYZ:	
			function*=dy*dz;
			break;
		case ORB_4FXXX:	
			function*=dx*dx*dx;
			break;
		case ORB_4FYYY:	
			function*=dy*dy*dy;
			break;
		case ORB_4FZZZ:	
			function*=dz*dz*dz;
			break;
		case ORB_4FXXY:	
			function*=dx*dx*dy;
			break;
		case ORB_4FXXZ:	
			function*=dx*dx*dz;
			break;
		case ORB_4FYYZ:	
			function*=dy*dy*dz;
			break;
		case ORB_4FXYY:	
			function*=dx*dy*dy;
			break;
		case ORB_4FXZZ:	
			function*=dx*dz*dz;
			break;
		case ORB_4FYZZ:	
			function*=dy*dz*dz;
			break;
		case ORB_4FXYZ:	
			function*=dx*dy*dz;
			break;
		default:
			printf("\nWarning in evaluatingPrimitiveAtVertices function: type %d of %d-th primitive should be positive and less than 20 here. Treated as one (s-type orbital).\n",type,primitive+1);	
			break;
	}

	return function;
}

/* ******************************************************************************************************************************************************************************************************* */
// The function evaluateOrbitalsIJ computes the value of Orb(i)*Orb(j) at the point defined by (px,py,pz), where the orbitals are stored in pMolecularOrbitalI and pMolecularOrbitalJ of pChemicalSystem
// It has the ChemicalSystem*, MolecularOrbital* variables (described in the readAndSaveFile.h, included in all *.h files), the point coordinates as input variables, and it returns (Orb_i*Orb_j)(px,py,pz)
/* ******************************************************************************************************************************************************************************************************* */
static double evaluateOrbitalsIJ(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbitalI, MolecularOrbital* pMolecularOrbitalJ)
{
	int k=0, kMax=pChemicalSystem->ngauss;
	double functionI=0., functionJ=0.;

	if(pMolecularOrbitalI->spin!=pMolecularOrbitalJ->spin)
	{
		return 0.;	// Case where Orbitals i and j have different spin
	}
	for(k=0; k<kMax; k++)
	{
		// Evaluating function for Orbital i and j
		functionI+=evaluatingPrimitiveAtVertices(px,py,pz,pChemicalSystem,pMolecularOrbitalI,k);
		functionJ+=evaluatingPrimitiveAtVertices(px,py,pz,pChemicalSystem,pMolecularOrbitalJ,k);			
	}
	
	return functionI*functionJ;
}

/* ****************************************************************************************************************************************************************************************************** */
// The function evaluateOrbitalsII computes the value of Orb(i)*Orb(i) at the point defined by (px,py,pz), where the orbital i is stored in pMolecularOrbitalI of pChemicalSystem structure
// It has the ChemicalSystem*, MolecularOrbital* variables (described in the readAndSaveFile.h, included in all *.h files), the point coordinates as input variables, and it returns (Orb_i)(px,py,pz)^2
/* ****************************************************************************************************************************************************************************************************** */
static double evaluateOrbitalsII(double px, double py, double pz, ChemicalSystem* pChemicalSystem, MolecularOrbital* pMolecularOrbitalI)
{
	int k=0, kMax=pChemicalSystem->ngauss;
	double function=0.;

	for(k=0; k<kMax; k++)
	{
		// Evaluating function for Orbital i
		function+=evaluatingPrimitiveAtVertices(px,py,pz,pChemicalSystem,pMolecularOrbitalI,k);
	}
	
	return function*function;
}

/* ******************************************************************************************************************************************************************************************************* */
// The function computeShapeDerivative calculates the shape derivative of probability to find nuElectrons at pMesh vertices with the eigenvalues and eigenvectors of the overlap matrix of pChemicalSystem
// It has Mesh*, ChemicalSystem* variable (described in the readAndSaveFile.h file, included in all *.h files), two double arrays and an int as input argument, and it returns one on sucess, otherwise zero
/* ******************************************************************************************************************************************************************************************************* */
static int computeShapeDerivative(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* eigenvalues, double* eigenvectors, int nuElectrons, int trick)
{
	double *coefficients=NULL, value=0., function=0., alpha=0., beta=0., px=0., py=0., pz=0.;
	int i=0, j=0, k=0, l=0, p=0, q=0, numberOfElectrons=pChemicalSystem->nmorb, n=numberOfElectrons+1, numberOfVertices=pMesh->nver;
	MolecularOrbital *pMolecularOrbitalI=NULL, *pMolecularOrbitalJ=NULL;

	if (numberOfElectrons<1||nuElectrons<0||nuElectrons>numberOfElectrons)
	{
		PRINT_ERROR();
		printf("In computeShapeDerivative: search for nu=%d electrons among %d electrons.\n",nuElectrons,numberOfElectrons);
		return 0;
	}
	printf("\nComputing the shape derivative of the probability to find %d electrons.\n",nuElectrons);
	// Allocate memory for the coefficients
	coefficients=(double*)calloc(n*n,sizeof(double));
	if (coefficients==NULL)
	{
		PRINT_ERROR();
		printf("In computeShapeDerivative:: could not allocate memory for the coefficients in the evaluation of the shape derivative.\n");
		return 0;
	}

#ifdef LS_MODE
	#if (LS_MODE==1)

		double shapeGradientAtThePoint=0.;

		numberOfVertices=pMesh->nadj; 	// Here we use the variable numberOfVertices but it refers to the number of Quadrilateral of internal domains
		for (k=0; k<numberOfVertices; k++)
		{
			// Loading the corrodinate of middle point in the quadrilateral k 
			shapeGradientAtThePoint=0.;
			px=0.;
			py=0.;
			pz=0.;
			l=pMesh->padj[k].quad;
			p=pMesh->pqua[l-1].p1;
			px+=pMesh->pver[p-1].x;
			py+=pMesh->pver[p-1].y;
			pz+=pMesh->pver[p-1].z;

			p=pMesh->pqua[l-1].p2;
			px+=pMesh->pver[p-1].x;
			py+=pMesh->pver[p-1].y;
			pz+=pMesh->pver[p-1].z;

			p=pMesh->pqua[l-1].p3;
			px+=pMesh->pver[p-1].x;
			py+=pMesh->pver[p-1].y;
			pz+=pMesh->pver[p-1].z;

			p=pMesh->pqua[l-1].p4;
			px+=pMesh->pver[p-1].x;
			py+=pMesh->pver[p-1].y;
			pz+=pMesh->pver[p-1].z;

			px*=.25;
			py*=.25;
			pz*=.25;
			
			// Computing the shape gradient of the probability to find nuElectrons at this point 
			for (i=0; i<numberOfElectrons; i++)
			{
				pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
				for (j=0; j<i; j++)
				{
					pMolecularOrbitalJ=&pChemicalSystem->pmorb[j];
					if (pMolecularOrbitalI->spin!=pMolecularOrbitalJ->spin)
					{
						continue;
					}
					// Evaluate the orbitals Orb(i)*Orb(j) at the point
					function=2.*evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);	// Here we multiply by two to consider case j<i but also i<j by symmetry
					// Computing the recursive formula above (15) of Cancès, Keriven, Lodier and Savin (2004, p.376) allowing to get the shape derivative in O(numberOfElectrons^2) operations 
					value=0.;
					for (l=0; l<numberOfElectrons; l++)
					{
						coefficients[0]=1.;
						for (p=1; p<n; p++)
						{
							if (l==p-1)
							{
								alpha=-1.;
								beta=1.;
							}
							else
							{
								beta=eigenvalues[p-1];
								alpha=1.-beta;
							}
							coefficients[p*n]=alpha*coefficients[(p-1)*n];
							for (q=1; q<p; q++)
							{
								coefficients[p*n+q]=beta*coefficients[(p-1)*n+(q-1)]+alpha*coefficients[(p-1)*n+q];
							}
							coefficients[p*n+p]=beta*coefficients[(p-1)*n+(p-1)];
						}
						value+=coefficients[numberOfElectrons*n+nuElectrons]*eigenvectors[l*numberOfElectrons+i]*eigenvectors[l*numberOfElectrons+j]; // A checker ici ce qu'a sorti lapacke
					}
					shapeGradientAtThePoint+=value*function;
				}
				// We now treat the case i=j
				function=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);
				// Computing the recursive formula above (15) of Cancès, Keriven, Lodier and Savin (2004, p.376) allowing to compute the shape derivative in O(numberOfElectrons^2) operations 
				value=0.;
				for (l=0; l<numberOfElectrons; l++)
				{
					coefficients[0]=1.;
					for (p=1; p<n; p++)
					{
						if (l==p-1)
						{
							alpha=-1.;
							beta=1.;
						}
						else
						{
							beta=eigenvalues[p-1];
							alpha=1.-beta;
						}
						coefficients[p*n]=alpha*coefficients[(p-1)*n];
						for (q=1; q<p; q++)
						{
							coefficients[p*n+q]=beta*coefficients[(p-1)*n+(q-1)]+alpha*coefficients[(p-1)*n+q];
						}
						coefficients[p*n+p]=beta*coefficients[(p-1)*n+(p-1)];
					}
					value+=coefficients[numberOfElectrons*n+nuElectrons]*eigenvectors[l*numberOfElectrons+i]*eigenvectors[l*numberOfElectrons+j]; // A checker ici ce qu'a sorti lapacke
				}
				shapeGradientAtThePoint+=value*function;
			}
			if (trick)
			{
				if (shapeGradientAtThePoint>1.e-8)
				{
					l=AddOrRemoveHexahedronToMesh(pMesh,k,-3);	
				}
				else if (shapeGradientAtThePoint<-1.e-8)
				{
					l=AddOrRemoveHexahedronToMesh(pMesh,k,-2);
				}
				else
				{
					continue;
				}
				if(!l)
				{
					PRINT_ERROR();
					printf("In computeShapeDerivative: addOrRemove = %d is not a correct output of AddOrRemoveHexahedronToMesh function.\n",l);
					if (coefficients!=NULL)
					{
						free(coefficients);
						coefficients=NULL;
					}
					return 0;
				}
			}
			else
			{
				if (shapeGradientAtThePoint>1.e-8)
				{
					l=AddOrRemoveHexahedronToMesh(pMesh,k,1);	
				}
				else if (shapeGradientAtThePoint<-1.e-8)
				{
					l=AddOrRemoveHexahedronToMesh(pMesh,k,-1);
				}
				else
				{
					continue;
				}
				if(!l)
				{
					PRINT_ERROR();
					printf("In computeShapeDerivative: addOrRemove = %d is not a correct output of AddOrRemoveHexahedronToMesh function.\n",l);
					if (coefficients!=NULL)
					{
						free(coefficients);
						coefficients=NULL;
					}
					return 0;
				}
			}

		}
	#endif
#endif

#ifndef LS_MODE
	for (k=0; k<numberOfVertices; k++)
	{
		pMesh->pver[k].value=0.;
		// Save the point coordinates	
		px=pMesh->pver[k].x;
		py=pMesh->pver[k].y;
		pz=pMesh->pver[k].z;	
		for (i=0; i<numberOfElectrons; i++)
		{
			pMolecularOrbitalI=&pChemicalSystem->pmorb[i];
			for (j=0; j<i; j++)
			{
				pMolecularOrbitalJ=&pChemicalSystem->pmorb[j];
				if (pMolecularOrbitalI->spin!=pMolecularOrbitalJ->spin)
				{
					continue;
				}
				// Evaluate the orbitals Orb(i)*Orb(j) at the point
				function=2.*evaluateOrbitalsIJ(px,py,pz,pChemicalSystem,pMolecularOrbitalI,pMolecularOrbitalJ);	// Here we multiply by two to consider case j<i but also i<j by symmetry
				// Computing the recursive formula above (15) of Cancès, Keriven, Lodier and Savin (2004, p.376) allowing to get the shape derivative in O(numberOfElectrons^2) operations 
				value=0.;
				for (l=0; l<numberOfElectrons; l++)
				{
					coefficients[0]=1.;
					for (p=1; p<n; p++)
					{
						if (l==p-1)
						{
							alpha=-1.;
							beta=1.;
						}
						else
						{
							beta=eigenvalues[p-1];
							alpha=1.-beta;
						}
						coefficients[p*n]=alpha*coefficients[(p-1)*n];
						for (q=1; q<p; q++)
						{
							coefficients[p*n+q]=beta*coefficients[(p-1)*n+(q-1)]+alpha*coefficients[(p-1)*n+q];
						}
						coefficients[p*n+p]=beta*coefficients[(p-1)*n+(p-1)];
					}
					value+=coefficients[numberOfElectrons*n+nuElectrons]*eigenvectors[l*numberOfElectrons+i]*eigenvectors[l*numberOfElectrons+j]; // A checker ici ce qu'a sorti lapacke
				}
				pMesh->pver[k].value+=value*function;
			}
			// We now treat the case i=j
			function=evaluateOrbitalsII(px,py,pz,pChemicalSystem,pMolecularOrbitalI);
			// Computing the recursive formula above (15) of Cancès, Keriven, Lodier and Savin (2004, p.376) allowing to compute the shape derivative in O(numberOfElectrons^2) operations 
			value=0.;
			for (l=0; l<numberOfElectrons; l++)
			{
				coefficients[0]=1.;
				for (p=1; p<n; p++)
				{
					if (l==p-1)
					{
						alpha=-1.;
						beta=1.;
					}
					else
					{
						beta=eigenvalues[p-1];
						alpha=1.-beta;
					}
					coefficients[p*n]=alpha*coefficients[(p-1)*n];
					for (q=1; q<p; q++)
					{
						coefficients[p*n+q]=beta*coefficients[(p-1)*n+(q-1)]+alpha*coefficients[(p-1)*n+q];
					}
					coefficients[p*n+p]=beta*coefficients[(p-1)*n+(p-1)];
				}
				value+=coefficients[numberOfElectrons*n+nuElectrons]*eigenvectors[l*numberOfElectrons+i]*eigenvectors[l*numberOfElectrons+j]; // A checker ici ce qu'a sorti lapacke
			}
			pMesh->pver[k].value+=value*function;
		}
	}
#endif

	// Free the memory
	if (coefficients!=NULL)
	{
		free(coefficients);
		coefficients=NULL;
	}

	return 1;
}

#ifndef LS_MODE
/* ********************************************************************************************************************************************************************************************** */
// The function writingShapeSolFile writes surface vertices values stored in pMesh->pver[i].value, multiplied by normal vector in *.sol file at FileLocation (warning: reset/overwrite if exists)
// It has the char* and Mesh* variable (described in main.h) as input arguments and it returns zero if an error occurred, otherwise one
/* ********************************************************************************************************************************************************************************************** */
int writingShapeSolFile(char* fileLocation, Mesh* pMesh)
{
	int i=0, iMax=pMesh->nnorm, j=0, jMax=0, jMin=0;
	double nx=0., ny=0., nz=0., value=0.;
	FILE *shapeFile=NULL;

	// Testing if file is well opened  (warning: reset and overwrite file at fileLocation if already exists)
	printf("\nOpening %s file. ",fileLocation);
	shapeFile=fopen(fileLocation,"w+");	// fopen function returns a FILE pointer on success, otherwise a NULL pointer is returned	
	if (shapeFile==NULL)
	{
		PRINT_ERROR();
		printf("In writingShapeSolFile: could not write in %s file.\n",fileLocation);
		return 0;
	}
	printf("Start writing vectorial data. ");	

	// First check that normals were stored in increasing order otherwise code must be modified (rebuilt normal adjacency)
	for (i=1; i<iMax; i++)
	{
		if (pMesh->pnorm[i-1].p>=pMesh->pnorm[i].p)
		{
		PRINT_ERROR();
		printf("In writingShapeSolFile: the normals %d and %d were not stored in increasing order.\n",pMesh->pnorm[i-1].p,pMesh->pnorm[i].p);
			return 0;
		}
	}
	// Writing cubeOut.sol file: MeshVersionFormated (1=single 2=double precision), Dimension (2 or 3), SolAtVertices numberOfSolution typeOfSolution (1=scalar, 2=vector, 3=symmetric tensor) 
	fprintf(shapeFile,"MeshVersionFormatted 2\n\nDimension 3\n\nSolAtVertices\n%d\n1 2 \n\n",pMesh->nver);
	jMin=0;
	for (i=0;i<iMax;i++)
	{
		jMax=(pMesh->pnorm[i].p)-1;
		for (j=jMin; j<jMax; j++)
		{
			fprintf(shapeFile,"%le %le %le \n",0.,0.,0.);
			switch(pMesh->pver[j].label)	// Label zero the point with no normal except those on ridges (12,14,15,16,23,25,26,34,35,36,45,46) and corners (125,126,145,146,235,236,345,346)
			{
				case 12:
				case 14:
				case 15:
				case 16:
				case 23:
				case 25:
				case 26:
				case 34:
				case 35:
				case 36:
				case 45:
				case 46:
				case 125:
				case 126:
				case 145:
				case 146:
				case 235:
				case 236:
				case 345:
				case 346:
					break;
				default:
					pMesh->pver[j].label=0;
			}
		}	
		value=pMesh->pver[jMax].value;		
		nx=value*(pMesh->pnorm[i].x);
		ny=value*(pMesh->pnorm[i].y);
		nz=value*(pMesh->pnorm[i].z);
		fprintf(shapeFile,"%le %le %le \n",nx,ny,nz);
		if (pMesh->pver[jMax].label<1||pMesh->pver[jMax].label>6)
		{
			pMesh->pver[jMax].label=10;  // Label ten the point with normal (on level-set) except those on faces (1,2,3,4,5,6) 
		}
		jMin=jMax+1;
	}
	if (jMin<pMesh->nver)
	{
		jMax=pMesh->nver;
		for (j=jMin; j<jMax; j++)
		{
			fprintf(shapeFile,"%le %le %le \n",0.,0.,0.);
			switch(pMesh->pver[j].label)	// Label zero the point with no normal except those on ridges (12,14,15,16,23,25,26,34,35,36,45,46) and corners (125,126,145,146,235,236,345,346)
			{
				case 12:
				case 14:
				case 15:
				case 16:
				case 23:
				case 25:
				case 26:
				case 34:
				case 35:
				case 36:
				case 45:
				case 46:
				case 125:
				case 126:
				case 145:
				case 146:
				case 235:
				case 236:
				case 345:
				case 346:
					break;
				default:
					pMesh->pver[j].label=0;
			}
		}
	}
	fprintf(shapeFile,"\nEnd");

	// Testing if file is well closed
	if(fclose(shapeFile))
	{
		shapeFile=NULL;
		PRINT_ERROR();
		printf("In writingShapeSolFile: could not close %s file properly.\n",fileLocation);
		return 0;
	}
	shapeFile=NULL;
	printf("Closing file.\n");

	return 1;
}
#endif

#ifdef LS_MODE
/* ******************************************************************************************************************************************************************************************************* */
// The function AddOrRemoveHexahedronToMesh modify the pMesh structure according to the addOrRemove variable (+/-1) so that the hexahedron (.hexin/.hexout) saved in pMesh->padj[j] is added/removed 
// It has the ChemicalSystem*, MolecularOrbital* variables (described in the readAndSaveFile.h, included in all *.h files), the int j and addOrRemove, and it returns one on sucess, otherwise zero
/* ******************************************************************************************************************************************************************************************************* */
int AddOrRemoveHexahedronToMesh(Mesh* pMesh, int j, int addOrRemove)
{
	int iHexahedron=0;

	switch (addOrRemove)
	{
		case 1:
			iHexahedron=pMesh->padj[j].hexout;	//Warning: hexahedron references saved as in .mesh file so do not forget to remove one from these int when calling them 
			pMesh->phex[iHexahedron-1].label=3;
			break;

		case -1:
			iHexahedron=pMesh->padj[j].hexin;	//Warning: hexahedron references saved as in .mesh file so do not forget to remove one from these int when calling them 
			pMesh->phex[iHexahedron-1].label=2;
			break;

		case -3:
			iHexahedron=pMesh->padj[j].hexout;	//Warning: hexahedron references saved as in .mesh file so do not forget to remove one from these int when calling them 
			pMesh->phex[iHexahedron-1].label=-3;
			break;

		case -2:
			iHexahedron=pMesh->padj[j].hexin;	//Warning: hexahedron references saved as in .mesh file so do not forget to remove one from these int when calling them 
			pMesh->phex[iHexahedron-1].label=-2;
			break;

		default:
			printf("\nWarning: addOrRemove should be (+/-)1 instead of %d.\n",addOrRemove);
			break;
	}

	return iHexahedron;
}
#endif

/* **************************************************************************************************************************************************************************************** */
// The function shapeDerivative computes the overlapMatrix, the probability to find nuElectrons in the domain, and the shape derivative of this probability on the boundary of the domain
// It has the Mesh*, ChemicalSystem* (described in main.h), three arrays of double, two int and two double variables as input arguments and it returns one on success, otherwise zero 
/* **************************************************************************************************************************************************************************************** */
int shapeDerivative(Mesh* pMesh, ChemicalSystem* pChemicalSystem, double* overlapMatrix, double* eigenvectors, double* eigenvalues, int nuElectrons, double* pProbability, double* pProbabilityOld, int trick, int i)
{
#ifdef LS_MODE
	#if (LS_MODE)
		int j=0, jMax=0;
	#endif
#endif 

#ifndef LS_MODE
	// Computing the overlap matix
	printf("\nWe recall that here -1=R^3, 2=Omega, and 3=R^3/Omega.");	// Warning: we may have inverted sign of distance function to get outernormal from Omega
	if(!computeOverlapMatrix(pMesh,pChemicalSystem,overlapMatrix,trick))	// Triangles are labbelled 10 but not necessarily the associated vertices (corrected in writeShapeSolFile)
	{
		PRINT_ERROR();
		printf("In shapeDerivative: computeOverlapMatrix function returns zero instead of one.\n");
		return 0;
	}

	// diagonalize the overlapMatrix (warning: using lapacke.h, lapacke package must have been previously installed) 
	if(!diagonalizeOverlapMatrix(overlapMatrix,eigenvectors,eigenvalues,pChemicalSystem->nmorb))
	{
		PRINT_ERROR();
		printf("In shapeDerivative: diagonalizeOverlapMatrix function returns zero instead of one.\n");
		return 0;
	}

	// Compute the probability
	*pProbabilityOld=*pProbability;
	*pProbability=computeProbability(eigenvalues,pChemicalSystem->nmorb,nuElectrons);
	if (*pProbability<=0.||*pProbability>1.)
	{
		printf("\nWarning in shapeDerivative function: probability p(%d)=%lf is not between zero and one.\n",nuElectrons,*pProbability);
	}

	// Compute the shape derivative
	if(!computeShapeDerivative(pMesh,pChemicalSystem,eigenvalues,eigenvectors,nuElectrons,0))	
	{
		PRINT_ERROR();
		printf("In shapeDerivative: computeShapeDerivative function returns zero instead of one.\n");
		return 0;
	}
#endif

#ifdef LS_MODE
	#if (LS_MODE)
		// Computing the overlap matrix
		if (i==0 || trick==0)
		{	
			printf("\nWe recall that here -1=R^3, 2=Omega, and 3=R^3/Omega.");
			if(!computeOverlapMatrixOnGrid(pMesh,pChemicalSystem,overlapMatrix,2)) 
			{
				PRINT_ERROR();
				printf("In shapeDerivative: computeOverlapMatrixOnGrid function returns zero instead of one.\n");
				return 0;
			}
		}
		else
		{
			printf("\nWe recall that here -2=(Omega)-(HexaInOmega_With_NegShapeDer) and -3=(Omega)+(HexaOutOmega_With_PosShapeDer).");
			if(!computeOverlapMatrixOnGrid(pMesh,pChemicalSystem,overlapMatrix,-2)) 
			{
				PRINT_ERROR();
				printf("In shapeDerivative: computeOverlapMatrixOnGrid function returns zero instead of one.\n");
				return 0;
			}
			if(!computeOverlapMatrixOnGrid(pMesh,pChemicalSystem,overlapMatrix,-3)) 
			{
				PRINT_ERROR();
				printf("In shapeDerivative: computeOverlapMatrixOnGrid function returns zero instead of one.\n");
				return 0;
			}
			jMax=pMesh->nhex;
			for (j=0; j<jMax; j++)
			{
				pMesh->phex[j].label=abs(pMesh->phex[j].label);
			}		
		}

		// diagonalize the overlapMatrix (warning: using lapacke.h, lapacke package must have been previously installed) 
		if(!diagonalizeOverlapMatrix(overlapMatrix,eigenvectors,eigenvalues,pChemicalSystem->nmorb))
		{
			PRINT_ERROR();
			printf("In shapeDerivative: diagonalizeOverlapMatrix function returns zero instead of one.\n");
			return 0;
		}

		// Compute the probability
		*pProbabilityOld=*pProbability;
		*pProbability=computeProbability(eigenvalues,pChemicalSystem->nmorb,nuElectrons);
		if (*pProbability<=0.||*pProbability>1.)
		{
			printf("\nWarning in shapeDerivative function: probability p(%d)=%lf is not between zero and one.\n",nuElectrons,*pProbability);
		}

		// Compute the shape derivative
		if(!computeShapeDerivative(pMesh,pChemicalSystem,eigenvalues,eigenvectors,nuElectrons,trick))	
		{
			PRINT_ERROR();
			printf("In shapeDerivative: computeShapeDerivative function returns zero instead of one.\n");
			return 0;
		}
	#else
		// Computing the overlap matrix
		printf("\nWe recall that here -2=(Omega)-(HexaInOmega_With_NegShapeDer) and -3=(Omega)+(HexaOutOmega_With_PosShapeDer).");
		if(!computeOverlapMatrixOnGrid(pMesh,pChemicalSystem,overlapMatrix,trick)) 
		{
			PRINT_ERROR();
			printf("In shapeDerivative: computeOverlapMatrixOnGrid function returns zero instead of one.\n");
			return 0;
		}		

		// diagonalize the overlapMatrix (warning: using lapacke.h, lapacke package must have been previously installed) 
		if(!diagonalizeOverlapMatrix(overlapMatrix,eigenvectors,eigenvalues,pChemicalSystem->nmorb))
		{
			PRINT_ERROR();
			printf("In shapeDerivative: diagonalizeOverlapMatrix function returns zero instead of one.\n");
			return 0;
		}

		// Compute the probability
		*pProbabilityOld=*pProbability;
		*pProbability=computeProbability(eigenvalues,pChemicalSystem->nmorb,nuElectrons);
		if (*pProbability<=0.||*pProbability>1.)
		{
			printf("\nWarning in shapeDerivative function: probability p(%d)=%lf is not between zero and one.\n",nuElectrons,*pProbability);
		}
	#endif
#endif

	return 1;
}

