#ifndef DEF_CHECK_ALL_PREPROCESSOR_CONSTANTS
#define DEF_CHECK_ALL_PREPROCESSOR_CONSTANTS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <math.h>	// if it is not in comment add -lm with gcc command
#include <lapacke.h>	// if it is not in comment add -llapacke with gcc command
#include <omp.h> 	// if it is not in comment add -fopenmp with gcc command

// Macro function to initialize diagnostic of an error
#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

/* ************************************************************************************************ */
// Definitions of the different pre-processor constants that are common to the different *.h files
/* ************************************************************************************************ */

// If LS_MODE is set to 0 then cubic algorithm is performed with random perturbations; if LS_MODE is set to 1 then cubic algorithm is performed with level-set perturbations;
#define LS_MODE 0			// if LS_MODE is not defined (put in comment) then the tetrahedral algorithm is performed with level-set perturbations.

// Number of points in the initial mesh
#define N_X 71				// WARNING: we must have (int) N_XYZ > 2 (odd form should be preferred since usually XYZ_MIN=-XYZ_MAX) because we impose at least to have one normal vector
#define N_Y 71
#define N_Z 71

// Space between points in the initial mesh
#define DELTA_X 0.1			// WARNING: we must have (double) DELTA_XYZ==(XYZ_MAX-XYZ_MIN)/(double)(N_XYZ-1)
#define DELTA_Y 0.1
#define DELTA_Z 0.1

// Level-set parameters of the initial domain: signed distance function of a sphere of radius LS_R centered at the point (LS_X,LS_Y,LS_Z)
#define LS_X 0.5			
#define LS_Y 0.77
#define LS_Z 0.3
#define LS_R 0.5			// WARNING: we mus have (double) LS_R > 0

// Used to distinguish the different types in the basis set of molecular orbitals according to the standard nomenclature (wfn/wfx format, outputs of Gaussian software)
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
/*
// Not used in the current program (probably never but we save the values here just in case they might be needed later)
#define ORB_5GXXXX 21
#define ORB_5GYYYY 22
#define ORB_5GZZZZ 23
#define ORB_5GXXXY 24
#define ORB_5GXXXZ 25
#define ORB_5GXYYY 26
#define ORB_5GYYYZ 27
#define ORB_5GXZZZ 28
#define ORB_5GYZZZ 29
#define ORB_5GXXYY 30
#define ORB_5GXXZZ 31
#define ORB_5GYYZZ 32
#define ORB_5GXXYZ 33
#define ORB_5GXYYZ 34
#define ORB_5GXYZZ 35
#define ORB_6HZZZZZ 36
#define ORB_6HYZZZZ 37
#define ORB_6HYYZZZ 38
#define ORB_6HYYYZZ 39
#define ORB_6HYYYYZ 40
#define ORB_6HYYYYY 41
#define ORB_6HXZZZZ 42
#define ORB_6HXYZZZ 43
#define ORB_6HXYYZZ 44
#define ORB_6HXYYYZ 45
#define ORB_6HXYYYY 46
#define ORB_6HXXZZZ 47
#define ORB_6HXXYZZ 48
#define ORB_6HXXYYZ 49
#define ORB_6HXXYYY 50
#define ORB_6HXXXZZ 51
#define ORB_6HXXXYZ 52
#define ORB_6HXXXYY 53
#define ORB_6HXXXXZ 54
#define ORB_6HXXXXY 55
#define ORB_6HXXXXX 56
*/

/* **************************************************************************************************************************************************************************************** */
// Definitions of the different structures used for the mesh according to the LS_MODE parameter (if it is defined, then the mesh is made of hexahedra, otherwise it is made of tetrahedra)
/* **************************************************************************************************************************************************************************************** */
typedef struct {
	double x;			// First coordinate of the point
	double y;			// Second coordinate of the point	
	double z;			// Third coordinate of the point
	int label;			// Integer value associated with the point
	double value;			// Real value associated with the point
} Point;

#ifndef LS_MODE
	typedef struct {
		int p;			// Integer referring to the point at which the vector is expressed (usually its position in the array associated with the vertices of the mesh)		
		double x;		// First coordinate of the vector
		double y;		// Second coordinate of the vector
		double z;		// Third coordinate of the vector
	} Vector;

	typedef struct {
		int p1;			// First point of the edge referred to as its position in the array associated with the vertices of the mesh
		int p2;			// Second point of the edge referred to as its position in the array associated with the vertices of the mesh
		int label;		// Integer value associated with the edge
	} Edge;

	typedef struct {
		int p1;			// First point of the triangle referred to as its position in the array associated with the vertices of the mesh
		int p2;			// Second point of the triangle referred to as its position in the array associated with the vertices of the mesh
		int p3;			// Third point of the triangle referred to as its position in the array associated with the vertices of the mesh
		int label;		// Integer value associated with the triangle
	} Triangle;

	typedef struct {
		int p1;			// First point of the tetrahedron referred to as its position in the array associated with the vertices of the mesh
		int p2;			// Second point of the tetrahedron referred to as its position in the array associated with the vertices of the mesh
		int p3;			// Third point of the tetrahedron referred to as its position in the array associated with the vertices of the mesh
		int p4;			// Fourth point of the tetrahedron referred to as its position in the array associated with the vertices of the mesh
		int label;		// Integer value associated with the tetrahedron
	} Tetrahedron;

	typedef struct {
		int nver;		// Integer used to store the number of vertices in the mesh
		Point* pver;		// Pointer used to define the array containing the vertices of the mesh

		int ncor;		// Integer used to store the number of corners in the mesh

		int nnorm;		// Integer used to store the number of normal vectors defined at some (boundary) vertices of the mesh
		Vector* pnorm;		// Pointer used to define the array containing the normal vectors of the mesh

		int ntan;		// Integer used to store the number of tangent vectors defined at some (boundary) vertices of the mesh
		Vector* ptan;		// Pointer used to define the array containing the tangent vectors of the mesh	

		int nedg;		// Integer used to store the number of (boundary) edges in the mesh
		Edge* pedg;		// Pointer used to define the array containing the edges of the mesh

		int ntri;		// Integer used to store the number of (boundary) triangles in the mesh
		Triangle* ptri;		// Pointer used to define the array containing the triangles of the mesh

		int ntet;		// Integer used to store the number of tetrahedra in the mesh
		Tetrahedron* ptet;	// Pointer used to define the array containing the tetrahedra of the mesh
	} Mesh;
#endif

#ifdef LS_MODE
	typedef struct {
		int p1;			// First point of the quadrilateral referred to as its position in the array associated with the vertices of the mesh
		int p2;			// Second point of the quadrilateral referred to as its position in the array associated with the vertices of the mesh
		int p3;			// Third point of the quadrilateral referred to as its position in the array associated with the vertices of the mesh
		int p4;			// Fourth point of the quadrilateral referred to as its position in the array associated with the vertices of the mesh
		int label;		// Integer value associated with the quadrilateral
	} Quadrilateral;

	typedef struct {
		int p1;			// First point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p2;			// Second point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p3;			// Third point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p4;			// Fourth point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p5;			// Fifth point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p6;			// Sixth point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p7;			// Seventh point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int p8; 		// Eighth point of the hexahedron referred to as its position in the array associated with the vertices of the mesh
		int label;		// Integer value associated with the hexahedron
	} Hexahedron;

	typedef struct {
		int quad;		// Integer referring to the position of a (boundary) quadrilateral in the array associated with the quadrilaterals of the mesh
		int hexin;		// Integer referring to the position of the hexahedron having quad among its faces and located inside a subdomain of the mesh
		int hexout;		// Integer referring to the position of the hexahedron having quad among its faces and located outside a subdomain of the mesh
	} Adjacency;			// The Adjacency structure is used to locate fastly the two hexahedra sharing a common quadrilateral (usually used for dealing with the boundary of a subdomain) 

	typedef struct {
		int nver;		// Integer used to store the number of vertices in the mesh
		Point* pver;		// Pointer used to define the array containing the vertices of the mesh	

		int nqua;		// Integer used to store the number of (boundary) quadrilaterals in the mesh
		Quadrilateral* pqua;	// Pointer used to define the array containing the quadrilaterals of the mesh 

		int nhex;		// Integer used to store the number of hexahedra in the mesh
		Hexahedron* phex;	// Pointer used to define the array containing the hexahedra of the mesh

		int nadj;		// Integer used to store the number of quadrilaterals concerned by the Adjacency structure of the mesh
		Adjacency* padj;	// Pointer used to define the array containing the Adjacency structure of the mesh
	} Mesh;
#endif

/* ********************************************************************** */
// Definitions of the different chemical structures used in the program
/* ********************************************************************** */
typedef struct {
	double x;			// First coordinate of the nucleus center
	double y;			// Second coordinate of the nucleus center
	double z;			// Third coordinate of the nucleus center
} Nucleus;

typedef struct {
	int spin;			// Integer used to define the spin associated with the molecular orbital (+1=up, -1=down)
	double* coeff;			// Pointer used to define the array containing the coefficients in the expansion of the molecular orbital into a sum of (Gaussian-type) primitives
	double* exp;			// Pointer used to define the array containing the exponents in the expansion of the molecular orbital into a sum of (Gaussian-type) primitives
	int* nucl;			// Pointer used to define the array containing the center reference of the nuclei in the expansion of the molecular orbital into a sum of (Gaussian-type) primitives
	int* type;			// Pointer used to define the array containing the type of Gaussian in the expansion of the molecular orbital into a sum of primitives
} MolecularOrbital;

typedef struct {
	int nmorb;			// Integer used to store the number of molecular orbitals in the chemical system considered
	MolecularOrbital* pmorb;	// Pointer used to define the array containing the molecular orbitals of the chemical system considered

	int nnucl;			// Integer used to store the number of nuclei in the chemical system considered
	Nucleus* pnucl;			// Pointer used to define the array containing the nuclei centers of the chemical system considered
	
	int ngauss;			// Integer used to store the number of (Gaussian-type) primitives used in the expansion of the molecular orbitals
} ChemicalSystem;

// Prototypes of non-static functions in main.c
int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);

/*
// Prototypes of static functions in main.c
static int checkAllPreprocessorConstants(int lsmode, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int nx, int ny, int nz, double deltax, double deltay, double deltaz, double lsr, double lsx, double lsy, double lsz, int orb1, int orb2, int orb3, int orb4, int orb5, int orb6, int orb7, int orb8, int orb9, int orb10, int orb11, int orb12, int orb13, int orb14, int orb15, int orb16, int orb17, int orb18, int orb19, int orb20, int orbspin, double metcst, double meterr, double metmin, double metmax, int itermax, double tolmax, double cstA, double cstB, double cstC, double csta, double cstb, double cstc, double cstaa, double cstbb, double cstcc);
*/

#endif

