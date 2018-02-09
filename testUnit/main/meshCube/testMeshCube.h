#ifndef DEF_TEST_MESH_CUBE
#define DEF_TEST_MESH_CUBE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ************************************************************************************************ */
// Definitions of the different pre-processor constants that are common to the different *.h files
/* ************************************************************************************************ */

// If LS_MODE is set to 0 then cubic algorithm is performed with random perturbations; if LS_MODE is set to 1 then cubic algorithm is performed with level-set perturbations;
//#define LS_MODE 1			// if LS_MODE is not defined (put in comment) then the tetrahedral algorithm is performed with level-set perturbations.

// Number of points in the initial mesh
#define N_X 11				// WARNING: we must have (int) N_XYZ > 2 (odd form should be preferred since usually XYZ_MIN=-XYZ_MAX)
#define N_Y 5
#define N_Z 22

// Space between points in the initial mesh
#define DELTA_X 0.75 			// WARNING: we must have (double) DELTA_XYZ==(XYZ_MAX-XYZ_MIN)/(double)(N_XYZ-1)
#define DELTA_Y 0.15
#define DELTA_Z 0.01

// Macro function to initialize diagnostic of an error
#define PRINT_ERROR() do {														\
	printf("\n%s %s: error encountered at line %d in %s file. Leaving program.\n",(__DATE__),(__TIME__),(__LINE__),(__FILE__));	\
} while(0)

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

int initialFileExists(char* fileLocation);
void closeTheFile(FILE** pFileToClose);

#endif

