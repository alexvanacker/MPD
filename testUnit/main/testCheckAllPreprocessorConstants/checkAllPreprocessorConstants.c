#include "checkAllPreprocessorConstants.h"

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
static int checkAllPreprocessorConstants(int lsmode, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int nx, int ny, int nz, double deltax, double deltay, double deltaz, double lsr, double lsx, double lsy, double lsz, int orb1, int orb2, int orb3, int orb4, int orb5, int orb6, int orb7, int orb8, int orb9, int orb10, int orb11, int orb12, int orb13, int orb14, int orb15, int orb16, int orb17, int orb18, int orb19, int orb20, int orbspin, double metcst, double meterr, double metmin, double metmax, int itermax, double tolmax, double cstA, double cstB, double cstC, double csta, double cstb, double cstc, double cstaa, double cstbb, double cstcc);

int main(void)
{
	time_t startTimer=0, endTimer=0;
	int returnValue=0, expectedValue=0, counter=0, counterSuccess=0, counterFail=0, readChar=0;

	int lsmode=0, nx=N_X, ny=N_Y, nz=N_Z, orb1=ORB_1S, orb2=ORB_2PX, orb3=ORB_2PY, orb4=ORB_2PZ, orb5=ORB_3DXX, orb6=ORB_3DYY, orb7=ORB_3DZZ, orb8=ORB_3DXY, orb9=ORB_3DXZ, orb10=ORB_3DYZ;
	int orb11=ORB_4FXXX, orb12=ORB_4FYYY, orb13=ORB_4FZZZ, orb14=ORB_4FXXY, orb15=ORB_4FXXZ, orb16=ORB_4FYYZ, orb17=ORB_4FXYY, orb18=ORB_4FXZZ, orb19=ORB_4FYZZ, orb20=ORB_4FXYZ, orbspin=ORB_SPIN;
	double xmin=X_MIN, xmax=X_MAX, ymin=Y_MIN, ymax=Y_MAX, zmin=Z_MIN, zmax=Z_MAX, deltax=DELTA_X, deltay=DELTA_Y, deltaz=DELTA_Z, lsr=LS_R, lsx=LS_X, lsy=LS_Y, lsz=LS_Z;
	double metcst=MET_CST, meterr=MET_ERR, metmin=MET_MIN, metmax=MET_MAX, itermax=ITER_MAX, tolmax=TOL_MAX, cstA=CST_A, cstB=CST_B, cstC=CST_C;
	double csta=CST_a, cstb=CST_b, cstc=CST_c, cstaa=CST_aa, cstbb=CST_bb, cstcc=CST_cc;

	// Test starts
	time(&startTimer);
	printf("Testing checkAllPreprocessorConstants function.\n");

#ifdef LS_MODE	
	for (lsmode=-1; lsmode<4; lsmode++)
	{
		expectedValue=1;
		if (lsmode<0 || lsmode>1)
		{
			expectedValue=0;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	lsmode=LS_MODE;
#endif

	for (xmin=-1.*X_MIN; xmin>=X_MIN; xmin+=X_MIN) 
	{
		for (xmax=-X_MAX; xmax<=X_MAX; xmax+=X_MAX)	
		{
			for (nx=-N_X; nx<=N_X; nx+=N_X)
			{
				for (deltax=-DELTA_X; deltax<=DELTA_X; deltax+=DELTA_X)
				{
					expectedValue=0;
					if (xmin==X_MIN && xmax==X_MAX && nx==N_X && deltax==DELTA_X)
					{
						expectedValue=1;
					}
					PRINT_TEST_START(counter,expectedValue);
					returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5, orb6,orb7,orb8,orb9,orb10,orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
				deltax=DELTA_X;
			}
			nx=N_X;
		}
		xmax=X_MAX;
	}
	xmin=X_MIN;

	for (ymin=-1.*Y_MIN; ymin>=Y_MIN; ymin+=Y_MIN)
	{
		for (ymax=-Y_MAX; ymax<=Y_MAX; ymax+=Y_MAX)
		{
			for (ny=-N_Y; ny<=N_Y; ny+=N_Y)
			{
				for (deltay=-DELTA_Y; deltay<=DELTA_Y; deltay+=DELTA_Y)
				{
					expectedValue=0;
					if (ymin==Y_MIN && ymax==Y_MAX && ny==N_Y && deltay==DELTA_Y)
					{
						expectedValue=1;
					}
					PRINT_TEST_START(counter,expectedValue);
					returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5, orb6,orb7,orb8,orb9,orb10,orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
				deltay=DELTA_Y;
			}
			ny=N_Y;
		}
		ymax=Y_MAX;
	}
	ymin=Y_MIN;
	
	for (zmin=-1.*Z_MIN; zmin>=Z_MIN; zmin+=Z_MIN)
	{
		for (zmax=-Z_MAX; zmax<=Z_MAX; zmax+=Z_MAX)
		{
			for (nz=-N_Z; nz<=N_Z; nz+=N_Z)
			{
				for (deltaz=-DELTA_Z; deltaz<=DELTA_Z; deltaz+=DELTA_Z)
				{
					expectedValue=0;
					if (zmin==Z_MIN && zmax==Z_MAX && nz==N_Z && deltaz==DELTA_Z)
					{
						expectedValue=1;
					}
					PRINT_TEST_START(counter,expectedValue);
					returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5, orb6,orb7,orb8,orb9,orb10,orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
				deltaz=DELTA_Z;
			}
			nz=N_Z;
		}
		zmax=Z_MAX;
	}
	zmin=Z_MIN;
	
	for (lsr=-LS_R; lsr<=LS_R; lsr+=LS_R)
	{
		for (lsx=2.*X_MIN; lsx<=2.*X_MAX; lsx+=LS_X)
		{
			for (lsy=-2.*Y_MIN; lsy<=2.*Y_MAX; lsy+=LS_Y)
			{
				for (lsz=-2.*Z_MIN; lsz<=2.*Z_MAX; lsz+=LS_Z)
				{
					expectedValue=0;
					if (lsr==LS_R)
					{
						expectedValue=1;
					}
					PRINT_TEST_START(counter,expectedValue);
					returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5, orb6,orb7,orb8,orb9,orb10,orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
				lsz=LS_Z;
			}
			lsy=LS_Y;
		}
		lsx=LS_X;
	}
	lsr=LS_R;
	
	for (orb1=-ORB_1S; orb1<=ORB_1S; orb1+=ORB_1S)
	{
		expectedValue=0;
		if (orb1>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);

	}
	orb1=ORB_1S;

	for (orb2=-ORB_2PX; orb2<=ORB_2PX; orb2+=ORB_2PX)
	{
		expectedValue=0;
		if (orb2>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb2=ORB_2PX;

	for (orb3=-ORB_2PY; orb3<=ORB_2PY; orb3+=ORB_2PY)
	{
		expectedValue=0;
		if (orb3>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb3=ORB_2PY;

	for (orb4=-ORB_2PZ; orb4<=ORB_2PZ; orb4+=ORB_2PZ)
	{
		expectedValue=0;
		if (orb4>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb4=ORB_2PZ;

	for (orb5=-ORB_3DXX; orb5<=ORB_3DXX; orb5+=ORB_3DXX)
	{
		expectedValue=0;
		if (orb5>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb5=ORB_3DXX;

	for (orb6=-ORB_3DYY; orb6<=ORB_3DYY; orb6+=ORB_3DYY)
	{
		expectedValue=0;
		if (orb6>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb6=ORB_3DYY;

	for (orb7=-ORB_3DZZ; orb7<=ORB_3DZZ; orb7+=ORB_3DZZ)
	{
		expectedValue=0;
		if (orb7>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb7=ORB_3DZZ;

	for (orb8=-ORB_3DXY; orb8<=ORB_3DXY; orb8+=ORB_3DXY)
	{
		expectedValue=0;
		if (orb8>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb8=ORB_3DXY;

	for (orb9=-ORB_3DXZ; orb9<=ORB_3DXZ; orb9+=ORB_3DXZ)
	{
		expectedValue=0;
		if (orb9>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb9=ORB_3DXZ;

	for (orb10=-ORB_3DYZ; orb10<=ORB_3DYZ; orb10+=ORB_3DYZ)
	{
		expectedValue=0;
		if (orb10>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb10=ORB_3DYZ;

	for (orb11=-ORB_4FXXX; orb11<=ORB_4FXXX; orb11+=ORB_4FXXX)
	{
		expectedValue=0;
		if (orb11>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb11=ORB_4FXXX;

	for (orb12=-ORB_4FYYY; orb12<=ORB_4FYYY; orb12+=ORB_4FYYY)
	{
		expectedValue=0;
		if (orb12>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb12=ORB_4FYYY;

	for (orb13=-ORB_4FZZZ; orb13<=ORB_4FZZZ; orb13+=ORB_4FZZZ)
	{
		expectedValue=0;
		if (orb13>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb13=ORB_4FZZZ;

	for (orb14=-ORB_4FXXY; orb14<=ORB_4FXXY; orb14+=ORB_4FXXY)
	{
		expectedValue=0;
		if (orb14>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb14=ORB_4FXXY;

	for (orb15=-ORB_4FXXZ; orb15<=ORB_4FXXZ; orb15+=ORB_4FXXZ)
	{
		expectedValue=0;
		if (orb15>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb15=ORB_4FXXZ;

	for (orb16=-ORB_4FYYZ; orb16<=ORB_4FYYZ; orb16+=ORB_4FYYZ)
	{
		expectedValue=0;
		if (orb16>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb16=ORB_4FYYZ;

	for (orb17=-ORB_4FXYY; orb17<=ORB_4FXYY; orb17+=ORB_4FXYY)
	{
		expectedValue=0;
		if (orb17>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb17=ORB_4FXYY;

	for (orb18=-ORB_4FXZZ; orb18<=ORB_4FXZZ; orb18+=ORB_4FXZZ)
	{
		expectedValue=0;
		if (orb18>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb18=ORB_4FXZZ;

	for (orb19=-ORB_4FYZZ; orb19<=ORB_4FYZZ; orb19+=ORB_4FYZZ)
	{
		expectedValue=0;
		if (orb19>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb19=ORB_4FYZZ;

	for (orb20=-ORB_4FXYZ; orb20<=ORB_4FXYZ; orb20+=ORB_4FXYZ)
	{
		expectedValue=0;
		if (orb20>0)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orb20=ORB_4FXYZ;

	for (orbspin=-2; orbspin<4; orbspin++)
	{
		expectedValue=1;
		if (orbspin<0 || orbspin>1)
		{
			expectedValue=0;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	orbspin=ORB_SPIN;




	for (metcst=-MET_CST; metcst<=MET_CST; metcst+=MET_CST)
	{
		for (meterr=-MET_ERR; meterr<=MET_ERR; meterr+=MET_ERR)
		{
			for (metmin=-MET_MIN; metmin<=MET_MIN; metmin+=MET_MIN)
			{
				for (metmax=-MET_MAX; metmax<=MET_MAX; metmax+=MET_MAX)
				{
					expectedValue=0;
					if (metcst==MET_CST && meterr==MET_ERR && metmax==MET_MAX && metmin==MET_MIN)
					{
						expectedValue=1;
					}
					PRINT_TEST_START(counter,expectedValue);
					returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5, orb6,orb7,orb8,orb9,orb10,orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
					PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
				}
				metmax=MET_MAX;
			}
			metmin=MET_MIN;
		}
		meterr=MET_ERR;
	}
	metcst=MET_CST;

	for (itermax=-ITER_MAX; itermax<=ITER_MAX; itermax+=ITER_MAX)
	{
		for (tolmax=-TOL_MAX; tolmax<=TOL_MAX; tolmax+=TOL_MAX)
		{
			expectedValue=0;
			if (itermax==ITER_MAX && tolmax==TOL_MAX)
			{
				expectedValue=1;
			}
			PRINT_TEST_START(counter,expectedValue);
			returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5, orb6,orb7,orb8,orb9,orb10,orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
			PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
		}
		tolmax=TOL_MAX;
	}
	itermax=ITER_MAX;

	for (cstA=CST_A-1.e-18; cstA<=CST_A+1.e-18; cstA+=1.e-18)
	{
		expectedValue=0;
		if (cstA==CST_A)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstA=CST_A;


	for (cstB=CST_B-1.e-17; cstB<=CST_B+1.e-17; cstB+=1.e-17)
	{
		expectedValue=0;
		if (cstB==CST_B)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstB=CST_B;

	for (cstC=CST_C-1.e-18; cstC<=CST_C+1.e-18; cstC+=1.e-18)
	{
		expectedValue=0;
		if (cstC==CST_C)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstC=CST_C;

	for (csta=CST_a-1.e-16; csta<=CST_a+1.e-16; csta+=1.e-16)
	{
		expectedValue=0;
		if (csta==CST_a)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	csta=CST_a;

	for (cstb=CST_b-1.e-16; cstb<=CST_b+1.e-16; cstb+=1.e-16)
	{
		expectedValue=0;
		if (cstb==CST_b)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstb=CST_b;

	for (cstc=CST_c-1.e-17; cstc<=CST_c+1.e-17; cstc+=1.e-17)
	{
		expectedValue=0;
		if (cstc==CST_c)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstc=CST_c;

	for (cstaa=CST_aa-1.e-17; cstaa<=CST_aa+1.e-17; cstaa+=1.e-17)
	{
		expectedValue=0;
		if (cstaa==CST_aa)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstaa=CST_aa;

	for (cstbb=CST_bb-1.e-17; cstbb<=CST_bb+1.e-17; cstbb+=1.e-17)
	{
		expectedValue=0;
		if (cstbb==CST_bb)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstbb=CST_bb;

	for (cstcc=CST_cc-1.e-16; cstcc<=CST_cc+1.e-16; cstcc+=1.e-16)
	{
		expectedValue=0;
		if (cstcc==CST_cc)
		{
			expectedValue=1;
		}
		PRINT_TEST_START(counter,expectedValue);
		returnValue=checkAllPreprocessorConstants(lsmode,xmin,xmax,ymin,ymax,zmin,zmax,nx,ny,nz,deltax,deltay,deltaz,lsr,lsx,lsy,lsz,orb1,orb2,orb3,orb4,orb5,orb6,orb7,orb8,orb9,orb10, orb11,orb12,orb13,orb14,orb15,orb16,orb17,orb18,orb19,orb20,orbspin,metcst,meterr,metmin,metmax,itermax,tolmax,cstA,cstB,cstC,csta,cstb,cstc,cstaa,cstbb,cstcc);
		PRINT_TEST_RESULT(counter,counterSuccess,counterFail,returnValue,expectedValue,readChar);
	}
	cstcc=CST_cc;

	expectedValue=1;
	PRINT_TEST_START(counter,expectedValue);
	returnValue=checkAllPreprocessorConstants(lsmode,X_MIN,X_MAX,Y_MIN,Y_MAX,Z_MIN,Z_MAX,N_X,N_Y,N_Z,DELTA_X,DELTA_Y,DELTA_Z,LS_R,LS_X,LS_Y,LS_Z,ORB_1S,ORB_2PX,ORB_2PY,ORB_2PZ, ORB_3DXX,ORB_3DYY,ORB_3DZZ,ORB_3DXY,ORB_3DXZ,ORB_3DYZ,ORB_4FXXX,ORB_4FYYY,ORB_4FZZZ,ORB_4FXXY,ORB_4FXXZ,ORB_4FYYZ,ORB_4FXYY,ORB_4FXZZ,ORB_4FYZZ,ORB_4FXYZ,ORB_SPIN,MET_CST,MET_ERR,MET_MIN,MET_MAX, ITER_MAX,TOL_MAX,CST_A,CST_B,CST_C,CST_a,CST_b,CST_c,CST_aa,CST_bb,CST_cc);
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

