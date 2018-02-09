#ifndef DEF_CHECK_ALL_PREPROCESSOR_CONSTANTS
#define DEF_CHECK_ALL_PREPROCESSOR_CONSTANTS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ************************************************************************************************ */
// Definitions of the different pre-processor constants that are common to the different *.h files
/* ************************************************************************************************ */

#define LS_MODE 0
#define X_MIN -3.5
#define X_MAX 3.5
#define Y_MIN -3.5
#define Y_MAX 3.5
#define Z_MIN -3.5
#define Z_MAX 3.5
#define N_X 71
#define N_Y 71
#define N_Z 71
#define DELTA_X 0.1
#define DELTA_Y 0.1
#define DELTA_Z 0.1
#define LS_R 0.5
#define LS_X 0.5
#define LS_Y 0.77
#define LS_Z 0.3
#define ORB_1S 1
#define ORB_2PX 2
#define ORB_2PY 3
#define ORB_2PZ 4
#define ORB_3DXX 5
#define ORB_3DYY 6
#define ORB_3DZZ 7
#define ORB_3DXY 8
#define ORB_3DXZ 9
#define ORB_3DYZ 10
#define ORB_4FXXX 11
#define ORB_4FYYY 12
#define ORB_4FZZZ 13
#define ORB_4FXXY 14
#define ORB_4FXXZ 15
#define ORB_4FYYZ 16
#define ORB_4FXYY 17
#define ORB_4FXZZ 18
#define ORB_4FYZZ 19
#define ORB_4FXYZ 20
#define ORB_SPIN 0
#define MET_CST 0.28125
#define MET_ERR 0.01
#define MET_MIN 0.01
#define MET_MAX 1.0
#define ITER_MAX 100000
#define TOL_MAX 1.e-10
#define CST_A 0.007091003462846911
#define CST_B 0.018781320953002642
#define CST_C 0.012248840519393658
#define CST_a 0.454496295874350351
#define CST_b 0.310885919263300610
#define CST_c 0.092735250310891226
#define CST_aa 0.045503704125649649
#define CST_bb 0.067342242210098170
#define CST_cc 0.721794249067326322

// Macro function to initialize diagnostic of an error
#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

/*
static int checkAllPreprocessorConstants(int lsmode, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int nx, int ny, int nz, double deltax, double deltay, double deltaz, double lsr, double lsx, double lsy, double lsz, int orb1, int orb2, int orb3, int orb4, int orb5, int orb6, int orb7, int orb8, int orb9, int orb10, int orb11, int orb12, int orb13, int orb14, int orb15, int orb16, int orb17, int orb18, int orb19, int orb20, int orbspin, double metcst, double meterr, double metmin, double metmax, int itermax, double tolmax, double cstA, double cstB, double cstC, double csta, double cstb, double cstc, double cstaa, double cstbb, double cstcc);
*/

#endif

