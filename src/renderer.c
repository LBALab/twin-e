/** @file renderer.c
	@brief 
	This file contains 3d models render routines
	
	Prequengine: a Little Big Adventure engine
	
	Copyright (C) 2008 Prequengine team \n
	Copyright (C) 2002-2007 The TwinEngine team \n

	This program is free software; you can redistribute it and/or \n
	modify it under the terms of the GNU General Public License \n
	as published by the Free Software Foundation; either version 2 \n
	of the License, or (at your option) any later version. \n

	This program is distributed in the hope that it will be useful, \n
	but WITHOUT ANY WARRANTY; without even the implied warranty of \n
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n
	GNU General Public License for more details. \n

	You should have received a copy of the GNU General Public License \n
	along with this program; if not, write to the Free Software \n
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	$URL$
	$Id$
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "renderer.h"
#include "main.h"
#include "sdlengine.h"
#include "shadeangletab.h"
#include "lbaengine.h"
#include "redraw.h"
#include "interface.h"

#define RENDERTYPE_DRAWLINE		0
#define RENDERTYPE_DRAWPOLYGON	1
#define RENDERTYPE_DRAWSPHERE	2

#define POLYGONTYPE_FLAT		0
#define POLYGONTYPE_COPPER		1
#define POLYGONTYPE_BOPPER		2
#define POLYGONTYPE_MARBLE		3
#define POLYGONTYPE_TELE		4
#define POLYGONTYPE_TRAS		5
#define POLYGONTYPE_TRAME		6
#define POLYGONTYPE_GOURAUD		7
#define POLYGONTYPE_DITHER		8

#define ERROR_OUT_OF_SCREEN		2



// --- structures ----

typedef struct renderTabEntry
{
  short int depth;
  short int renderType;
  unsigned char *dataPtr;
} renderTabEntry;

typedef struct pointTab
{
	short int X;
	short int Y;
	short int Z;
} pointTab;

typedef struct elementEntry
{
	short int firstPoint;		// data1
	short int numOfPoints;		// data2
	short int basePoint;		// data3
	short int baseElement;		// param
	short int flag;
	short int rotateZ;
	short int rotateY;
	short int rotateX;
	int numOfShades;			// field_10
	int field_14;
	int field_18;
	int Y;
	int field_20;
	short int field_24;
} elementEntry;

typedef struct lineCoordinates
{
	int data;
	short int x1;
	short int y1;
	short int x2;
	short int y2;
} lineCoordinates;


typedef struct lineData
{
	int data;
	short int p1;
	short int p2;
} lineData;

typedef struct polyHeader
{
	unsigned char renderType; //FillVertic_AType
	unsigned char numOfVertex;
	short int colorIndex;
} polyHeader;

typedef struct polyVertexHeader
{
	short int shadeEntry;
	short int dataOffset;
} polyVertexHeader;


typedef struct computedVertex
{
	short int shadeValue;
	short int x;
	short int y;
} computedVertex;

typedef struct bodyHeaderStruct
{
  short int bodyFlag;     
  short int unk0;         
  short int unk1;         
  short int unk2;         
  short int unk3;         
  short int unk4;         
  short int unk5;         
  short int offsetToData; 
  char *ptrToKeyFrame;
  int keyFrameTime;
} bodyHeaderStruct;

// ---- variables ----

int isUsingOrhoProjection;

int baseMatrixRotationX;
int baseMatrixRotationY;
int baseMatrixRotationZ;

int orthoProjX; // setSomethingVar1
int orthoProjY; // setSomethingVar2
int orthoProjZ; // setSomethingVar2

int baseTransPosX; // setSomething2Var1
int baseTransPosY; // setSomething2Var2
int baseTransPosZ; // setSomething2Var3

int baseRotPosX; // setSomething3Var12
int baseRotPosY; // setSomething3Var14
int baseRotPosZ; // setSomething3Var16

int cameraPosX; // cameraVar1
int cameraPosY; // cameraVar2
int cameraPosZ; // cameraVar3

// ---

short int *shadeAngleTab1; // tab1
short int *shadeAngleTab2; // tab2
short int *shadeAngleTab3; // tab3

int renderAngleX; // _angleX
int renderAngleY; // _angleY
int renderAngleZ; // _angleZ

int renderX; // _X
int renderY; // _Y
int renderZ; // _Z

int destX;
int destY;
int destZ;

// --- 

int baseMatrix[3*3];

int numOfPrimitives;

int numOfPoints;
int numOfElements;
unsigned char *pointsPtr;
unsigned char *elementsPtr;
unsigned char *elementsPtr2;

unsigned char *pri2Ptr2;

int matricesTable[271];
unsigned char *currentMatrixTableEntry;

int *shadePtr;
int shadeMatrix[9];
int lightX;
int lightY;
int lightZ;

pointTab computedPoints[800];		// _projectedPointTable
pointTab flattenPoints[800];	// _flattenPointTable
short int shadeTable[500];

short int primitiveCounter;
renderTabEntry *renderTabEntryPtr;
renderTabEntry *renderTabEntryPtr2;
renderTabEntry *renderTabSortedPtr;

renderTabEntry renderTab[1000];
renderTabEntry renderTabSorted[1000];
unsigned char renderTab7[10000];

short int polyRenderType; //FillVertic_AType; 
unsigned char *renderV19;   // RECHECK THIS
int numOfVertex;
short int vertexCoordinates[193];

// render polygon vars
short int pRenderV3[96];
short int *pRenderV1;
short int *pRenderV2;
short int numOfVertexRemaining;
short int polyCropped;

short int vleft;
short int vtop;
short int vright;
short int vbottom;

unsigned char oldVertexParam;
unsigned char vertexParam1;
unsigned char vertexParam2;

short int polyTab[960];
short int polyTab2[960];
int renderLoop;
// end render polygon vars




int project_position_on_screen(int cX, int cY, int cZ)
{
	if(!isUsingOrhoProjection)
	{
		cX -= baseRotPosX;
		cY -= baseRotPosY;
		cZ -= baseRotPosZ;
		
		if(cZ >= 0)
		{
			int posZ = cZ + cameraPosX;

			if(posZ < 0)
				posZ = 0x7FFF;

			projPosX = (cX * cameraPosY)/posZ + orthoProjX;
			projPosY = (cY * cameraPosZ)/posZ + orthoProjY;
			projPosZ = posZ;
			return -1;
		}
		else
		{
			projPosX = 0;
			projPosY = 0;
			projPosZ = 0;
			return 0;
		}
	}
	else
	{
		projPosX = ((cX - cZ) * 24) / 512 + orthoProjX;
		projPosY = (((cX + cZ) * 12) - cY * 30) / 512 + orthoProjY;
		projPosZ = cY - cX - cZ;
	}
	
	return 1;
}

void set_camera_position( int X, int Y, int cX, int cY, int cZ )
{
	orthoProjX = X;
	orthoProjY = Y;

	cameraPosX = cX;
	cameraPosY = cY;
	cameraPosZ = cZ;

	isUsingOrhoProjection = 0;
}

void set_base_translation(int X, int Y, int Z)
{
	baseTransPosX = X;
	baseTransPosY = Y;
	baseTransPosZ = Z;
}

void set_ortho_projection(int X, int Y, int Z)
{
	orthoProjX = X;
	orthoProjY = Y;
	orthoProjZ = Z;

	isUsingOrhoProjection = 1;
}

void get_base_rotation_position(int X, int Y, int Z)
{
	destX = (baseMatrix[0] * X + baseMatrix[1] * Y + baseMatrix[2] * Z)>>14;
	destY = (baseMatrix[3] * X + baseMatrix[4] * Y + baseMatrix[5] * Z)>>14;
	destZ = (baseMatrix[6] * X + baseMatrix[7] * Y + baseMatrix[8] * Z)>>14;
}

void set_base_rotation(int X, int Y, int Z)
{ 
	int angleXCos;
	int angleXSin;

	int angleYCos;
	int angleYSin;

	int angleZCos;
	int angleZSin;
  
	int matrixElem;

	shadeAngleTab1 = &shadeAngleTable[0];
	shadeAngleTab2 = &shadeAngleTable[256];
	shadeAngleTab3 = &shadeAngleTable[384];

	baseMatrixRotationX = X & 0x3FF;
	baseMatrixRotationY = Y & 0x3FF;
	baseMatrixRotationZ = Z & 0x3FF;

	angleXCos = shadeAngleTab1[baseMatrixRotationX];
	angleXSin = shadeAngleTab1[(baseMatrixRotationX + 256) & 0x3FF];

	angleYCos = shadeAngleTab1[baseMatrixRotationY];
	angleYSin = shadeAngleTab1[(baseMatrixRotationY + 256) & 0x3FF];

	angleZCos = shadeAngleTab1[baseMatrixRotationZ];
	angleZSin = shadeAngleTab1[(baseMatrixRotationZ + 256) & 0x3FF];

	baseMatrix[0] = angleZSin;
	baseMatrix[1] = -angleZCos;
	baseMatrix[3] = (angleZCos * angleXSin)>>14;
	baseMatrix[4] = (angleZSin * angleXSin)>>14;
	baseMatrix[6] = (angleZCos * angleXCos)>>14;
	baseMatrix[7] = (angleZSin * angleXCos)>>14;

	baseMatrix[0] = (angleZSin * angleYSin)>>14;
	baseMatrix[2] = (angleZSin * angleYCos)>>14;

	matrixElem = baseMatrix[3];

	baseMatrix[3] = ((angleYSin * matrixElem) + (angleYCos * angleXCos))>>14;
	baseMatrix[5] = ((angleYCos * matrixElem) - (angleYSin * angleXCos))>>14;

	matrixElem = baseMatrix[6];

	baseMatrix[6] = ((angleYSin * matrixElem) - (angleXSin * angleYCos))>>14;
	baseMatrix[8] = ((angleYCos * matrixElem) + (angleXSin * angleYSin))>>14;

	get_base_rotation_position(baseTransPosX, baseTransPosY, baseTransPosZ);

	baseRotPosX = destX;
	baseRotPosY = destY; // TODO: recheck Y coordinate
	baseRotPosZ = destZ;
}

void get_camera_angle_positions(int X, int Y, int Z)
{
	destX = (baseMatrix[0] * X + baseMatrix[3] * Y + baseMatrix[6] * Z)>>14;
	destY = (baseMatrix[1] * X + baseMatrix[4] * Y + baseMatrix[7] * Z)>>14;
	destZ = (baseMatrix[2] * X + baseMatrix[5] * Y + baseMatrix[8] * Z)>>14;
}

void set_camera_angle(int transPosX, int transPosY, int transPosZ, int rotPosX, int rotPosY, int rotPosZ, int param6)
{
	baseTransPosX = transPosX;
	baseTransPosY = transPosY;
	baseTransPosZ = transPosZ;

	get_base_rotation_position(rotPosX, rotPosY, rotPosZ);

	baseRotPosY += param6; // RECHECK THIS

	get_camera_angle_positions(baseRotPosX, baseRotPosY, baseRotPosZ);

	baseTransPosX = destX;
	baseTransPosY = destY;
	baseTransPosZ = destZ;
}

// ------------------------------------------------------------------------------------------------------

void apply_rotation(int *tempMatrix, int *currentMatrix)
{
	int angleVar1;    // esi
	int angleVar2;    // ecx

	int matrix1[9];
	int matrix2[9];

	if (renderAngleX)
	{
		angleVar2 = shadeAngleTab1[renderAngleX & 0x3FF];
		angleVar1 = shadeAngleTab1[(renderAngleX+0x100) & 0x3FF];

		matrix1[0] = currentMatrix[0];
		matrix1[3] = currentMatrix[3];
		matrix1[6] = currentMatrix[6];

		matrix1[1] = (currentMatrix[2] * angleVar2 + currentMatrix[1] * angleVar1) >> 14;
		matrix1[2] = (currentMatrix[2] * angleVar1 - currentMatrix[1] * angleVar2) >> 14;
		matrix1[4] = (currentMatrix[5] * angleVar2 + currentMatrix[4] * angleVar1) >> 14;
		matrix1[5] = (currentMatrix[5] * angleVar1 - currentMatrix[4] * angleVar2) >> 14;
		matrix1[7] = (currentMatrix[8] * angleVar2 + currentMatrix[7] * angleVar1) >> 14;
		matrix1[8] = (currentMatrix[8] * angleVar1 - currentMatrix[7] * angleVar2) >> 14;
	}
	else
	{
		int i;

		for(i=0;i<9;i++)
		    matrix1[i]=currentMatrix[i];
	}

	if (renderAngleZ)
	{
		angleVar2 = shadeAngleTab1[renderAngleZ & 0x3FF];
		angleVar1 = shadeAngleTab1[(renderAngleZ+0x100) & 0x3FF];

		matrix2[2] = matrix1[2];
		matrix2[5] = matrix1[5];
		matrix2[8] = matrix1[8];

		matrix2[0] = (matrix1[1] * angleVar2 + matrix1[0] * angleVar1) >> 14;
		matrix2[1] = (matrix1[1] * angleVar1 - matrix1[0] * angleVar2) >> 14;
		matrix2[3] = (matrix1[4] * angleVar2 + matrix1[3] * angleVar1) >> 14;
		matrix2[4] = (matrix1[4] * angleVar1 - matrix1[3] * angleVar2) >> 14;
		matrix2[6] = (matrix1[7] * angleVar2 + matrix1[6] * angleVar1) >> 14;
		matrix2[7] = (matrix1[7] * angleVar1 - matrix1[6] * angleVar2) >> 14;
	}
	else
	{
		int i;

		for(i=0;i<9;i++)
		    matrix2[i]=matrix1[i];
	}

	if (renderAngleY)
	{
		angleVar2 = shadeAngleTab1[renderAngleY & 0x3FF];			// esi
		angleVar1 = shadeAngleTab1[(renderAngleY+0x100) & 0x3FF];	// ecx

		tempMatrix[1] = matrix2[1];
		tempMatrix[4] = matrix2[4];
		tempMatrix[7] = matrix2[7];

		tempMatrix[0] = (matrix2[0] * angleVar1 - matrix2[2] * angleVar2) >> 14;
		tempMatrix[2] = (matrix2[0] * angleVar2 + matrix2[2] * angleVar1) >> 14;
		tempMatrix[3] = (matrix2[3] * angleVar1 - matrix2[5] * angleVar2) >> 14;
		tempMatrix[5] = (matrix2[3] * angleVar2 + matrix2[5] * angleVar1) >> 14;

		tempMatrix[6] = (matrix2[6] * angleVar1 - matrix2[8] * angleVar2) >> 14;
		tempMatrix[8] = (matrix2[6] * angleVar2 + matrix2[8] * angleVar1) >> 14;
	}
	else
	{
		int i;

		for(i=0;i<9;i++)
		    tempMatrix[i]=matrix2[i];
	}
}

void apply_points_rotation(unsigned char *firstPointsPtr, int numPoints, pointTab * destPoints, int *rotationMatrix)
{
	short int tmpX;
	short int tmpY;
	short int tmpZ;

	short int *tempPtr;

	int numOfPoints = numPoints;
	unsigned char *pointsPtr;

	do
	{
		pointsPtr = firstPointsPtr;
		tempPtr = (short int *) (firstPointsPtr);

		tmpX = tempPtr[0];
		tmpY = tempPtr[1];
		tmpZ = tempPtr[2];

		destPoints->X = ((rotationMatrix[0] * tmpX + rotationMatrix[1] * tmpY + rotationMatrix[2] * tmpZ) >> 14) + destX;
		destPoints->Y = ((rotationMatrix[3] * tmpX + rotationMatrix[4] * tmpY + rotationMatrix[5] * tmpZ) >> 14) + destY;
		destPoints->Z = ((rotationMatrix[6] * tmpX + rotationMatrix[7] * tmpY + rotationMatrix[8] * tmpZ) >> 14) + destZ;

		destPoints++;
		firstPointsPtr = pointsPtr + 6;
	}while (--numOfPoints);
}

void process_rotated_element(int rotZ, int rotY, int rotX, elementEntry *elemPtr) // unsigned char * elemPtr) // loadPart
{
	int *currentMatrix;
	short int baseElement;

	int firstPoint = elemPtr->firstPoint;
	int numOfPoints = elemPtr->numOfPoints;

	renderAngleX = rotX;
	renderAngleY = rotY;
	renderAngleZ = rotZ;

	if (firstPoint % 6)
	{
		printf("RENDER ERROR: invalid firstPoint in process_rotated_element func\n");
		exit(1);
	}

	//baseElement = *((unsigned short int*)elemPtr+6);
	baseElement = elemPtr->baseElement;

	// if its the first point
	if (baseElement == -1)
	{
		currentMatrix = baseMatrix;

		destX = 0;
		destY = 0;
		destZ = 0;
	}
	else
	{
		int pointIdx = (elemPtr->basePoint)/6;
		currentMatrix = (int *) ((unsigned char *)matricesTable + baseElement);

		destX = computedPoints[pointIdx].X;
		destY = computedPoints[pointIdx].Y;
		destZ = computedPoints[pointIdx].Z;
	}

	apply_rotation((int *) currentMatrixTableEntry, currentMatrix);

	if(!numOfPoints)
	{
		printf("RENDER WARNING: No points in this model!\n");
	}

	apply_points_rotation(pointsPtr + firstPoint, numOfPoints, &computedPoints[firstPoint / 6], (int *) currentMatrixTableEntry);
}

void apply_points_translation(unsigned char *firstPointsPtr, int numPoints, pointTab * destPoints, int *translationMatrix)
{
	short int tmpX;
	short int tmpY;
	short int tmpZ;

	short int *tempPtr;

	int numOfPoints = numPoints;
	unsigned char *pointsPtr;

	do
	{
		pointsPtr = firstPointsPtr;
		tempPtr = (short int *) (firstPointsPtr);

		tmpX = tempPtr[0] + renderAngleZ;
		tmpY = tempPtr[1] + renderAngleY;
		tmpZ = tempPtr[2] + renderAngleX;

		destPoints->X = ((translationMatrix[0] * tmpX + translationMatrix[1] * tmpY + translationMatrix[2] * tmpZ) >> 14) + destX;
		destPoints->Y = ((translationMatrix[3] * tmpX + translationMatrix[4] * tmpY + translationMatrix[5] * tmpZ) >> 14) + destY;
		destPoints->Z = ((translationMatrix[6] * tmpX + translationMatrix[7] * tmpY + translationMatrix[8] * tmpZ) >> 14) + destZ;

		destPoints++;
		firstPointsPtr = pointsPtr + 6;
	}while (--numOfPoints);
}

void process_translated_element(int rotX, int rotY, int rotZ, elementEntry *elemPtr)
{
	int *dest;
	int *source;

	renderAngleX = rotX;
	renderAngleY = rotY;
	renderAngleZ = rotZ;

	if (elemPtr->baseElement == -1) // base point
	{
		int i;
		destX = 0;
		destY = 0;
		destZ = 0;

		dest = (int *) currentMatrixTableEntry;

		for(i=0;i<9;i++)
		    dest[i]=baseMatrix[i];
	}
	else      // dependent
	{
		int i;
		destX = computedPoints[(elemPtr->basePoint) / 6].X;
		destY = computedPoints[(elemPtr->basePoint) / 6].Y;
		destZ = computedPoints[(elemPtr->basePoint) / 6].Z;

		source = (int *) ((unsigned char *)matricesTable + elemPtr->baseElement);
		dest = (int *) currentMatrixTableEntry;

		for(i=0;i<9;i++)
		    dest[i]=source[i];
	}

	apply_points_translation(pointsPtr + elemPtr->firstPoint, elemPtr->numOfPoints, &computedPoints[elemPtr->firstPoint / 6], (int *) currentMatrixTableEntry);
}

void translate_group(short int ax, short int bx, short int cx)
{
	int ebp;
	int ebx;
	int ecx;
	int eax;
	int edi;
	
	ebp = ax;
	ebx = bx;
	ecx = cx;

	edi = shadeMatrix[0];
	eax = shadeMatrix[1];
	edi *= ebp;
	eax *= ebx;
	edi += eax;
	eax = shadeMatrix[2];
	eax *= ecx;
	eax += edi;
	eax >>= 14;

	destX = eax;

	edi = shadeMatrix[3];
	eax = shadeMatrix[4];
	edi *= ebp;
	eax *= ebx;
	edi += eax;
	eax = shadeMatrix[5];
	eax *= ecx;
	eax += edi;
	eax >>= 14;
	destY = eax; // TODO: recheck Z

	ebp *= shadeMatrix[6];
	ebx *= shadeMatrix[7];
	ecx *= shadeMatrix[8];
	ebx += ebp;
	ebx += ecx;
	ebx >>= 14;
	destZ = eax; // TODO: recheck Y
}

void set_light_vector(int angleX, int angleY, int angleZ)
{
	shadeAngleTab1 = &shadeAngleTable[0];
	shadeAngleTab2 = &shadeAngleTable[256];
	shadeAngleTab3 = &shadeAngleTable[384];

	// TODO: RECHECK THIS
	/*_cameraAngleX = angleX;
	_cameraAngleY = angleY;
	_cameraAngleZ = angleZ;*/

	renderAngleX = angleX;
	renderAngleY = angleY;
	renderAngleZ = angleZ;

	apply_rotation(shadeMatrix, baseMatrix);
	translate_group(0, 0, 59);
	
	lightX = destX;
	lightY = destY;
	lightZ = destZ;
}

// ------------------------------------------------------------------------------------------------------

int compute_polygons()
{
	short int vertexX, vertexY;
	short int *ptr1, *ptr3;
	int i;
	short int push1, push2;
	char direction = 1;
	int echange;
	short int oldVertexX, oldVertexY;
	short int currentVertexX, currentVertexY;
	short int size;
	int temp2, temp4, temp5, temp6;
	int step;
	float vfloat, vfloat2;

	pRenderV1 = vertexCoordinates;
	pRenderV2 = pRenderV3;
	numOfVertexRemaining = numOfVertex;
	polyCropped = 0;

	vleft = vtop = 32767;
	vright = vbottom = -32768;

	ptr1 = vertexCoordinates;

	for (i = 0; i < numOfVertex; i++)
	{
		ptr1++;   // discarding the 1st parameter

		vertexX = *(ptr1++);

		if (vertexX < vleft)
			vleft = vertexX;
		if (vertexX > vright)
			vright = vertexX;

		vertexY = *(ptr1++);

		if (vertexY < vtop)
			vtop = vertexY;
		if (vertexY > vbottom)
		vbottom = vertexY;
	}

	ptr1[0] = pRenderV1[0];
	ptr1[1] = pRenderV1[1];
	ptr1[2] = pRenderV1[2];
	
	if(vleft<0)
		return ERROR_OUT_OF_SCREEN;
	if(vright>=640)
		return ERROR_OUT_OF_SCREEN;
	if(vtop<0)
		return ERROR_OUT_OF_SCREEN;
	if(vbottom>=480)
		return ERROR_OUT_OF_SCREEN;
	
	ptr1 = pRenderV1;

	vertexParam1 = vertexParam2 = (*(ptr1++)) & 0xFF;
	oldVertexX = *(ptr1++);
	oldVertexY = *(ptr1++);

	do
	{
		oldVertexParam = vertexParam1;

		vertexParam1 = vertexParam2 = (*(ptr1++)) & 0xFF;
		currentVertexX = *(ptr1++);
		currentVertexY = *(ptr1++);

		// drawLine(oldVertexX,oldVertexY,currentVertexX,currentVertexY,255);

		if (currentVertexY == oldVertexY) // since it's scanline based, we don't care when we are only moving along X
		{
			oldVertexX = size = currentVertexX;
		}
		else
		{
			push1 = currentVertexX; // let's save the current coordinates since we are going to modify the values
			push2 = currentVertexY;

			if (currentVertexY < oldVertexY)  // if we are going up
			{
				size = oldVertexY - currentVertexY;
				direction = -1;

				if (oldVertexX < currentVertexX)  // if we are going up right
				{
					echange = oldVertexX; // we invert the vertex to draw from new to old
					oldVertexX = currentVertexX;
					currentVertexX = echange;

					echange = currentVertexY;
					currentVertexY = oldVertexY;

					oldVertexY = echange;

					echange = oldVertexParam;
					oldVertexParam = vertexParam2;
					vertexParam2 = echange;

					direction = 1;  // we will draw by going down the tab
				}

				temp2 = oldVertexY; // temp2 is the starting Y position
				oldVertexY = size;  // oldVertexY now become the number of pixel
				size = temp2 * 2;

				ptr3 = &polyTab[temp2 + 480]; // ptr3 is the output ptr in the renderTab

				temp4 = ((oldVertexX - currentVertexX) << 16);  // temp4 = size in X << 16

				temp5 = temp4 / oldVertexY; // temp5 is the size of a step << 16
				temp6 = temp4 % oldVertexY; // temp6 is the remaining << 16

				vfloat = ((float) (oldVertexX - currentVertexX)) / ((float) oldVertexY);

				temp6 >>= 1;
				temp6 += 0x7FFF;

				step = (unsigned short) temp5;  // retrieve the size of a step

				// temp7 = (((unsigned short)temp6) | ((oldVertexX & 0xFFFF)<<16));
				vfloat2 = oldVertexX;

				oldVertexX = oldVertexY;  // oldVertexX is now the number of vertical pixels

				oldVertexY += 2;

				for (i = 0; i < oldVertexY; i++)
				{
					// *(ptr3)=((temp7&0xFFFF0000)>>16);
					if((ptr3-polyTab) < 960)
						*(ptr3) = (short int) vfloat2;
					ptr3 += direction;
					// temp7+=step;
					vfloat2 -= vfloat;
				}

				if (polyRenderType >= 7) // we must compute the color progression
				{
					short int* ptr3 = &polyTab2[temp2 + 480];

					temp4 = (vertexParam2 - oldVertexParam); // compute the color difference

					if (temp4 >= 0)
					{
						union
						{
							struct
							{
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union
						{
							struct
							{
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}reste;
  
						test.bit.al = oldVertexParam;
						test.bit.ah = vertexParam2;

						test.bit.ah -= test.bit.al;

						test.bit.al = 0;

						reste.temp = test.temp % oldVertexX;

						test.temp /= oldVertexX;

						reste.bit.al >>=1;
						reste.bit.al += 0x7F;

						reste.bit.ah = oldVertexParam;

						oldVertexX+=2;

						for (i = 0; i < oldVertexX; i++)
						{
							if((ptr3-polyTab2) < 960)
								*(ptr3) = reste.temp;
							ptr3 += direction;
							reste.temp += test.temp;
						}
					}
					else
					{
						union
						{
							struct
							{
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union
						{
							struct
							{
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}reste;

						test.bit.al = oldVertexParam;
						test.bit.ah = vertexParam2;

						test.bit.ah -= test.bit.al;
						test.bit.ah = -test.bit.ah;

						test.bit.al = 0;

						reste.temp = test.temp % (oldVertexX);

						test.temp /= oldVertexX;

						reste.bit.al >>=1;
						reste.bit.al = -reste.bit.al;
						reste.bit.al += 0x7F;

						reste.bit.ah = oldVertexParam;

						for (i = 0; i <= oldVertexX; i++)
						{
							if((ptr3-polyTab2) < 960)
								*(ptr3) = reste.temp;
							ptr3 += direction;
							reste.temp -= test.temp;
						}
					}
				}
				direction = 1;
				oldVertexY = push2;
				oldVertexX = push1;
			}
			else  // if we are going down
			{
				size = currentVertexY - oldVertexY; // size is the number of pixel we must go
				// verticaly

				if (oldVertexX > currentVertexX)  // if we are going down and to the left
				{
					echange = oldVertexX; // in that case, we will draw the line the other
					// side (from new point to old point)
					oldVertexX = currentVertexX;
					currentVertexX = echange;

					echange = currentVertexY;
					currentVertexY = oldVertexY;
					oldVertexY = echange;

					echange = oldVertexParam;
					oldVertexParam = vertexParam2;
					vertexParam2 = echange;

					direction = -1; // since we are going backward in the screen
				}

				temp2 = oldVertexY; // temp2 is the starting Y position
				oldVertexY = size;  // oldVertexY now become the number of pixel
				size = temp2 * 2;

				ptr3 = &polyTab[temp2]; // ptr3 is the output ptr in the renderTab

				temp4 = ((currentVertexX - oldVertexX) << 16);  // temp4 = size in X << 16

				temp5 = temp4 / oldVertexY; // temp5 is the size of a step << 16
				temp6 = temp4 % oldVertexY; // temp6 is the remaining << 16

				vfloat = ((float) (currentVertexX - oldVertexX)) / ((float) oldVertexY);

				temp6 >>= 1;
				temp6 += 0x7FFF;

				step = (unsigned short) temp5;  // retrieve the size of a step

				// temp7 = (((unsigned short)temp6) | ((oldVertexX & 0xFFFF)<<16));
				vfloat2 = oldVertexX;

				oldVertexX = oldVertexY;  // oldVertexX is now the number of vertical pixels

				oldVertexY += 2;

				for (i = 0; i < oldVertexY; i++)
				{
					// *(ptr3)=((temp7&0xFFFF0000)>>16);
					if((ptr3-polyTab) < 960)
						*(ptr3) = (short int) vfloat2;
					ptr3 += direction;
					// temp7+=step;
					vfloat2 += vfloat;
				}

				if (polyRenderType >= 7)
				{
					short int* ptr3 = &polyTab2[temp2];

					temp4 = ((vertexParam2 - oldVertexParam)); // compute the color difference

					if (temp4 >= 0)
					{
						union
						{
							struct
							{
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union
						{
						struct
						{
							unsigned char al;
							unsigned char ah;
						} bit;
						unsigned short int temp;
					}reste;

					test.bit.al = oldVertexParam;
					test.bit.ah = vertexParam2;

					test.bit.ah -= test.bit.al;

					test.bit.al = 0;

					reste.temp = test.temp % oldVertexX;

					test.temp /= oldVertexX;

					reste.bit.al >>=1;
					reste.bit.al += 0x7F;

					reste.bit.ah = oldVertexParam;

					oldVertexX+=2;

					for (i = 0; i < oldVertexX; i++)
					{
						if((ptr3-polyTab2) < 960)
							*(ptr3) = reste.temp;
						ptr3 += direction;
						reste.temp += test.temp;
					}
				}
				else
				{
					union
					{
						struct
						{
							unsigned char al;
							unsigned char ah;
						} bit;
						unsigned short int temp;
					}test;

					union
					{
						struct
						{
							unsigned char al;
							unsigned char ah;
						}	bit;
						unsigned short int temp;
					}reste;

					test.bit.al = oldVertexParam;
					test.bit.ah = vertexParam2;

					test.bit.ah -= test.bit.al;
					test.bit.ah = -test.bit.ah;

					test.bit.al = 0;

					reste.temp = test.temp % (oldVertexX);

					test.temp /= oldVertexX;

					reste.bit.al >>=1;
					reste.bit.al = -reste.bit.al;
					reste.bit.al += 0x7F;

					reste.bit.ah = oldVertexParam;

					for (i = 0; i <= oldVertexX; i++)
					{
						if((ptr3-polyTab2) < 960)
							*(ptr3) = reste.temp;
						ptr3 += direction;
						reste.temp -= test.temp;
					}
				}
			}
			direction = 1;
			oldVertexY = push2;
			oldVertexX = push1;
		}
	}
	}while (--numOfVertexRemaining);

	return (1);
}

void render_polygons(int ecx, int edi)
{
	unsigned char *out, *out2;
	short int *ptr1;
	short int *ptr2;
	int vsize, hsize;
	int color;
	int j;
	int currentLine;

	short int start, stop;

	float varf2;
	float varf3;
	float varf4;

 /* if (vtop <= 0 || vbottom <= 0)
    return;
  if (vleft <= 0 || vright <= 0)
    return;
  if (vleft >= 640)
    return;
  // if(vright>=640)
    // return;
  if (vtop >= 480 || vbottom >= 480)
    return;*/

	if(vtop<0)
	{
		return;
	}

	if(vbottom >= 479)
	{
		return;
	}

	out = frontVideoBuffer + 640*vtop;

	ptr1 = &polyTab[vtop];
	ptr2 = &polyTab2[vtop];

	vsize = vbottom - vtop;
	vsize++;

	color = edi;

	switch (ecx)
	{
		case POLYGONTYPE_FLAT:
		{
			currentLine = vtop;
			do
			{
				if(currentLine >=0 && currentLine <480)
				{
					stop = ptr1[480];
					start = ptr1[0];

					ptr1++;
					hsize = stop - start;

					if (hsize >= 0)
					{
						hsize++;
						out2 = start + out;

						for(j = start; j < hsize+start; j++)
						{
							if(j>=0 && j<640)
								out[j] = color;
						}
					}
				}
				out += 640;
				currentLine++;
			}while(--vsize);
			break;

		}
		case POLYGONTYPE_COPPER:
		{
			currentLine = vtop;
			do
			{
				if(currentLine >=0 && currentLine <480)
				{
					start = ptr1[0];
					stop = ptr1[480];

					ptr1++;
					hsize = stop - start;

					if(hsize>=0)
					{
						unsigned short int mask = 0x43DB;
						unsigned short int dx;
						int startCopy;
          
						dx = (unsigned char)color;
						dx |= 0x300;

						hsize++;
						out2 = start + out;
						startCopy = start;

						for(j = startCopy; j< hsize+startCopy; j++)
						{
							start += mask;
							start = (start &0xFF00) | ((start&0xFF) & (unsigned char)(dx>>8)) ;
							start = (start &0xFF00) | ((start&0xFF) + (dx&0xFF));
							if(j>=0 && j<640)
							{
								out[j] = start&0xFF;
							}
							mask = (mask << 2) | (mask >> 14);
							mask++;
						}
					}
          
				}
				out += 640;
				currentLine++;
			}while(--vsize);
			break;
		}
		case POLYGONTYPE_BOPPER: // TODO: fix this, it have bugs - (1 pixel for 2)
		{
			currentLine = vtop;
			do
			{
				if(currentLine >=0 && currentLine <480)
				{
					start = ptr1[0];
					stop = ptr1[480];
					ptr1++;
					hsize = stop - start;

					if(hsize >= 0)
					{
						hsize++;
						out2 = start + out;
						for (j = start; j < hsize+start; j++)
						{
							if((start+(vtop%1))&1)
							{
								if(j>=0&&j<640)
								{
									out[j] = color;
								}
							}
							out2++;
						}
					}
          
				}
				out += 640;
				currentLine++;
			}while(--vsize);
			break;
		}
		case POLYGONTYPE_MARBLE: // TODO: implement this
		{
			break;
		}
		case POLYGONTYPE_TELE: // TODO: implement this
		{
			break;
		}
		case POLYGONTYPE_TRAS: // TODO: implement this
		{
			break;
		}
		case POLYGONTYPE_TRAME: // TODO: fix this, it have some bugs
		{
			unsigned char bh=0;

			currentLine = vtop;
			do
			{
				if(currentLine >=0 && currentLine <480)
				{
					start = ptr1[0];
					stop = ptr1[480];
					ptr1++;
					hsize = stop - start;

					if(hsize >= 0)
					{
						hsize++;
						out2 = start + out;
          
						hsize/=2;
						if(hsize>1)
						{
							unsigned short int ax;
							bh ^= 1;
							ax = *((unsigned short int*)out2);
							ax &= 1;
							if(ax ^ bh)
							{
								out2++;
							}

							for (j = 0; j < hsize; j++)
							{
								*(out2) = (unsigned char)color;
								out2+=2;
							}
						}
					}
          
				}
				out+=640;
				currentLine++;
			}while(--vsize);
			break;
		}
		case POLYGONTYPE_GOURAUD:
		{
			renderLoop = vsize;
			currentLine = vtop;
			do
			{
				if(currentLine >=0 && currentLine <480)
				{
					unsigned short int startColor = ptr2[0];
					unsigned short int stopColor = ptr2[480];

					short int colorSize = stopColor - startColor;

					stop = ptr1[480];  // stop
					start = ptr1[0]; // start

					ptr1++;
					out2 = start + out;
					hsize = stop - start;

					varf2 = ptr2[480];
					varf3 = ptr2[0];

					ptr2++;

					varf4 = (float)((int)varf2 - (int)varf3);

					if (hsize == 0)
					{
						if(start>=0 && start <640)
							*out2 = ((startColor + stopColor) / 2)>>8; // moyenne des 2 couleurs
					}
					else if (hsize > 0)
					{
						if (hsize == 1)
						{
							if(start>=-1 && start <640-1)
								*(out2 + 1) = stopColor>>8;

							if(start>=0 && start <640)
								*(out2) = startColor>>8;
						}
						else if (hsize == 2)
						{
							if(start>=-2 && start<640-2)
								*(out2 + 2) = stopColor>>8;

							if(start>=-1 && start<640-1)
								*(out2 + 1) = ((startColor + stopColor) / 2)>>8;

							if(start>=0 && start<640)
								*(out2) = startColor>>8;
						}
						else
						{
							int currentXPos = start;
							colorSize /= hsize;
							hsize++;

							if(hsize%2)
							{
								hsize/=2;
								if(currentXPos>=0 && currentXPos<640)
								*(out2)=startColor>>8;
								out2++;
								currentXPos++;
								startColor+=colorSize;
							}
							else
							{
								hsize/=2;
							}

							do
							{
								if(currentXPos>=0 && currentXPos<640)
									*(out2)=startColor>>8;

								currentXPos++;
								startColor+=colorSize;

								if(currentXPos>=0 && currentXPos<640)
									*(out2+1)=startColor>>8;

								currentXPos++;
								out2+=2;
								startColor+=colorSize;
							}while(--hsize);
						}
					}
				}
				out += 640;
				currentLine++;
			}while (--renderLoop);
			break;
		}
		case POLYGONTYPE_DITHER: // dithering
		{
			renderLoop = vsize;

			currentLine = vtop;
			do
			{
				if(currentLine >=0 && currentLine <480)
				{
					stop = ptr1[480]; // stop
					start = ptr1[0];  // start
					ptr1++;
					hsize = stop - start;

					if(hsize>=0)
					{
						unsigned short int startColor = ptr2[0];
						unsigned short int stopColor = ptr2[480];
						int currentXPos = start;

						out2 = start + out;
						ptr2++;

						if(hsize==0)
						{
							if(currentXPos>=0 && currentXPos<640)
								*(out2)=(unsigned char)(((startColor + stopColor)/2)>>8);
						}
						else
						{
							short int colorSize = stopColor - startColor;
							if(hsize==1)
							{
								unsigned short int currentColor = startColor;
								hsize++;
								hsize/=2;

								currentColor&=0xFF;
								currentColor+=startColor;
								if(currentXPos>=0 && currentXPos<640)
									*(out2) = currentColor>>8;

								currentColor&=0xFF;
								startColor+=colorSize;
								currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
								currentColor +=startColor;

								currentXPos++;
								if(currentXPos>=0 && currentXPos<640)
									*(out2+1) = currentColor>>8;
							}
							else if(hsize==2)
							{
								unsigned short int currentColor = startColor;
								hsize++;
								hsize/=2;
                              
								currentColor&=0xFF;
								colorSize/=2;
								currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
								currentColor +=startColor;
								if(currentXPos>=0 && currentXPos<640)
									*(out2) = currentColor>>8;

								out2++;
								currentXPos++;
								startColor+=colorSize;

								currentColor&=0xFF;
								currentColor+=startColor;

								if(currentXPos>=0 && currentXPos<640)
									*(out2) = currentColor>>8;

								currentColor&=0xFF;
								startColor+=colorSize;
								currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
								currentColor +=startColor;

								currentXPos++;
								if(currentXPos>=0 && currentXPos<640)
									*(out2+1) = currentColor>>8;
							}
							else
							{
								unsigned short int currentColor = startColor;
								colorSize/=hsize;
								hsize++;
                

								if(hsize%2)
								{
									hsize/=2;
									currentColor&=0xFF;
									currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
									currentColor +=startColor;
									if(currentXPos>=0 && currentXPos<640)
										*(out2) = currentColor>>8;
									out2++;
									currentXPos++;
								}
								else
								{
									hsize/=2;
								}

								do
								{
									currentColor&=0xFF;
									currentColor+=startColor;
									if(currentXPos>=0 && currentXPos<640)
										*(out2) = currentColor>>8;
									currentXPos++;
									currentColor&=0xFF;
									startColor+=colorSize;
									currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
									currentColor +=startColor;
									if(currentXPos>=0 && currentXPos<640)
										*(out2+1) = currentColor>>8;
									currentXPos++;
									out2+=2;
									startColor+=colorSize;
								}while(--hsize);
							}
						}
					}
				}
				out += 640;
				currentLine++;
			}while(--renderLoop);
			break;
		}
		default:
		{
			#ifdef GAMEMOD
			printf("RENDER WARNING: Unsuported render type %d\n",ecx);
			#endif
			break;
		}
	};
}

void circle_fill(int x,int y, int radius, char color)
{
	int currentLine;

	radius+=1;

	for(currentLine = -radius; currentLine <= radius; currentLine++)
	{
		double width;

		if(abs(currentLine) != radius)
		{
			width = sin(acos((float)currentLine/(float)radius));
		}
		else
		{
			width = 0;
		}

		width *= radius;

		if(width < 0)
			width = - width;

		draw_line((int)(x-width),currentLine + y, (int)(x+width), currentLine+y, color);
	}
}

int render_model_elements(unsigned char *esi)
{
	unsigned char *edi;
	short int temp;
	int eax, ecx;

	short int counter;
	short int type;
	short int color;

	lineData *lineDataPtr;
	lineCoordinates *lineCoordinatesPtr;

	int point1;

	int point2;

	int depth;
	int bestDepth;
	int currentDepth;
	short int bestZ;
	int j;
	int bestPoly = 0;
	short int shadeEntry;
	short int shadeValue;

	short int ax, bx, cx;

	unsigned char *destPtr;
	int i;

	unsigned char *render23;
	unsigned char *render24;
	int render25;

	polyVertexHeader *currentPolyVertex;
	polyHeader *currentPolyHeader;
	polyHeader *destinationHeader;
	computedVertex *currentComputedVertex;
	pointTab *currentVertex;
	pointTab *destinationVertex;

	// prepare polygons

	edi = renderTab7;			// renderTab7 coordinates buffer
	temp = *((short int*)esi);  // we read the number of polygones
	esi += 2;

	if (temp)
	{
		primitiveCounter = temp;  // the number of primitives = the number of polygones

		do      // loop that load all the polygones
		{
			render23 = edi;
			currentPolyHeader = (polyHeader *) esi;
			ecx = *((int*)esi);
			esi += 2;
			polyRenderType = currentPolyHeader->renderType;

			// TODO: RECHECK coordinates axis
			if (polyRenderType >= 9)
			{
				destinationHeader = (polyHeader *) edi;

				destinationHeader->renderType = currentPolyHeader->renderType - 2;
				destinationHeader->numOfVertex = currentPolyHeader->numOfVertex;
				destinationHeader->colorIndex = currentPolyHeader->colorIndex;

				esi += 2;
				edi += 4;

				counter = destinationHeader->numOfVertex;

				bestDepth = -32000;
				renderV19 = edi;

				do
				{
					currentPolyVertex = (polyVertexHeader *) esi;

					shadeValue = currentPolyHeader->colorIndex + shadeTable[currentPolyVertex->shadeEntry];

					currentComputedVertex = (computedVertex *) edi;

					currentComputedVertex->shadeValue = shadeValue;

					currentVertex = &flattenPoints[currentPolyVertex->dataOffset / 6];
					destinationVertex = (pointTab *) (edi + 2);

					destinationVertex->X = currentVertex->X;
					destinationVertex->Y = currentVertex->Y;

					edi += 6;
					esi += 4;

					currentDepth = currentVertex->Z;

					if (currentDepth > bestDepth)
						bestDepth = currentDepth;
				}while (--counter);
			}
			else if (polyRenderType >= 7) // only 1 shade value is used
			{
				destinationHeader = (polyHeader *) edi;

				destinationHeader->renderType = currentPolyHeader->renderType - 7;
				destinationHeader->numOfVertex = currentPolyHeader->numOfVertex;

				color = currentPolyHeader->colorIndex;

				shadeEntry = *((short int*)(esi+2));
			
				esi += 4;

				*((short int*)(edi+2)) = color + shadeTable[shadeEntry];

				edi += 4;
				renderV19 = edi;
				bestDepth = -32000;
				counter = destinationHeader->numOfVertex;

				do
				{
					eax = *((short int*)esi);
					esi += 2;

					currentVertex = &flattenPoints[eax / 6];

					destinationVertex = (pointTab *) (edi + 2);

					destinationVertex->X = currentVertex->X;
					destinationVertex->Y = currentVertex->Y;

					edi += 6;

					currentDepth = currentVertex->Z;

					if (currentDepth > bestDepth)
						bestDepth = currentDepth;
				}while (--counter);
			}
			else  // no shade is used
			{
				destinationHeader = (polyHeader *) edi;

				destinationHeader->renderType = currentPolyHeader->renderType;
				destinationHeader->numOfVertex = currentPolyHeader->numOfVertex;
				destinationHeader->colorIndex = currentPolyHeader->colorIndex;

				esi += 2;
				edi += 4;

				bestDepth = -32000;
				renderV19 = edi;
				eax = 0;
				counter = currentPolyHeader->numOfVertex;

				do
				{
					eax = *((short int*)esi);
					esi += 2;

					currentVertex = &flattenPoints[eax / 6];

					destinationVertex = (pointTab *) (edi + 2);

					destinationVertex->X = currentVertex->X;
					destinationVertex->Y = currentVertex->Y;

					edi += 6;

					currentDepth = currentVertex->Z;

					if (currentDepth > bestDepth)
						bestDepth = currentDepth;
				}while (--(counter));
			}

			render24 = edi;
			edi = renderV19;

			render25 = bestDepth;

			ax = *((short int*)(edi + 4));
			bx = *((short int*)(edi + 8));

			ax -= *((short int*)(edi + 16));
			bx -= *((short int*)(edi + 2));

			ax *= bx;

			bestDepth = ax;
			bx = currentDepth;

			ax = *((short int*)(edi + 2));
			cx = *((short int*)(edi + 10));

			ax -= *((short int*)(edi + 14));
			cx -= *((short int*)(edi + 4));

			ax *= cx;

			ax -= bestDepth;
			currentDepth -= (bx) - 1; // peut-etre une erreur la

			if (currentDepth < 0)
			{
				edi = render23;
			}
			else
			{
				numOfPrimitives++;

				renderTabEntryPtr->depth = render25;
				renderTabEntryPtr->renderType = 1;
				renderTabEntryPtr->dataPtr = render23;
				renderTabEntryPtr++;

				edi = render24;
			}
		}while (--primitiveCounter);
	}

	// prepare lines

	temp = *((short int*)esi);
	esi += 2;
	if (temp)
	{
		numOfPrimitives += temp;
		do
		{
			int param;
			lineDataPtr = (lineData *) esi;
			lineCoordinatesPtr = (lineCoordinates *) edi;

			if (*((short int*)&lineDataPtr->p1) % 6 != 0 || *((short int*)&lineDataPtr->p2) % 6 != 0)
			{
				printf("RENDER ERROR: lineDataPtr reference is malformed !\n");
				exit(1);
			}

			point1 = *((short int*)&lineDataPtr->p1) / 6;
			point2 = *((short int*)&lineDataPtr->p2) / 6;
			param = *((int*)&lineDataPtr->data);
			*((int*)&lineCoordinatesPtr->data) = param;
			*((short int*)&lineCoordinatesPtr->x1) = flattenPoints[point1].X;
			*((short int*)&lineCoordinatesPtr->y1) = flattenPoints[point1].Y;
			*((short int*)&lineCoordinatesPtr->x2) = flattenPoints[point2].X;
			*((short int*)&lineCoordinatesPtr->y2) = flattenPoints[point2].Y;
			bestDepth = flattenPoints[point1].Z;
			depth = flattenPoints[point2].Z;

			if (depth >= bestDepth)
				bestDepth = depth;

			renderTabEntryPtr->depth = bestDepth;
			renderTabEntryPtr->renderType = 0;
			renderTabEntryPtr->dataPtr = edi;
			renderTabEntryPtr++;

			esi += 8;
			edi += 12;
		}while (--temp);
	}

	// prepare spheres

	temp = *((short int*)esi);
	esi += 2;
	if (temp)     
	{
		numOfPrimitives+=temp;
		do
		{
			unsigned char color = *(esi+1);
			short int center = *((unsigned short int*)(esi+6));
			short int size = *((unsigned short int*)(esi+4));

			*(unsigned char*)edi = color;
			*((short int*)(edi+1)) = flattenPoints[center/6].X;
			*((short int*)(edi+3)) = flattenPoints[center/6].Y;
			*((short int*)(edi+5)) = size;

			renderTabEntryPtr->depth = flattenPoints[center/6].Z;
			renderTabEntryPtr->renderType = 2;
			renderTabEntryPtr->dataPtr = edi;
			renderTabEntryPtr++;

			esi += 8;
			edi += 7;
		}while (--temp);
	}

	renderTabEntryPtr2 = renderTab;

	renderTabSortedPtr = renderTabSorted;
	for (i = 0; i < numOfPrimitives; i++)  // then we sort the polygones | WARNING: very slow | TODO: improve this
	{
		renderTabEntryPtr2 = renderTab;
		bestZ = -0x7FFF;
		for (j = 0; j < numOfPrimitives; j++)
		{
			if (renderTabEntryPtr2->depth > bestZ)
			{
				bestZ = renderTabEntryPtr2->depth;
				bestPoly = j;
			}
			renderTabEntryPtr2++;
		}
		renderTabSortedPtr->depth = renderTab[bestPoly].depth;
		renderTabSortedPtr->renderType = renderTab[bestPoly].renderType;
		renderTabSortedPtr->dataPtr = renderTab[bestPoly].dataPtr;
		renderTabSortedPtr++;
		renderTab[bestPoly].depth = -0x7FFF;
	}
	renderTabEntryPtr2 = renderTabSorted;

	// prepare to render elements

	if (numOfPrimitives)
	{
		primitiveCounter = numOfPrimitives;
		renderV19 = esi;

		do
		{
			type = renderTabEntryPtr2->renderType;
			esi = renderTabEntryPtr2->dataPtr;
			renderV19 += 8;

			switch (type)
			{
				case RENDERTYPE_DRAWLINE: // draw a line
				{
					unsigned int x1;
					unsigned int y1;
					unsigned int x2;
					unsigned int y2;

					lineCoordinatesPtr = (lineCoordinates *) esi;
					color = (*((int*)&lineCoordinatesPtr->data) & 0xFF00) >> 8;
			  
					x1 = *((short int*)(unsigned short int*)&lineCoordinatesPtr->x1);
					y1 = *((short int*)(unsigned short int*)&lineCoordinatesPtr->y1);
					x2 = *((short int*)(unsigned short int*)&lineCoordinatesPtr->x2);
					y2 = *((short int*)(unsigned short int*)&lineCoordinatesPtr->y2);
			  
					draw_line(x1,y1,x2,y2,color);
					break;
				}
				case RENDERTYPE_DRAWPOLYGON: // draw a polygon
				{
					eax = *((int*)esi);
					esi += 4;

					polyRenderType = eax & 0xFF;
					numOfVertex = (eax & 0xFF00) >> 8;
					color = (eax & 0xFF0000) >> 16;

					destPtr = (unsigned char *) vertexCoordinates;

					for (i = 0; i < (numOfVertex * 3); i++)
					{
						*((short int*)destPtr) = *((short int*)esi);
						destPtr += 2;
						esi += 2;
					}

					if (compute_polygons() != ERROR_OUT_OF_SCREEN)
					{
						render_polygons(polyRenderType, color);
					}

					break;
				}
				case RENDERTYPE_DRAWSPHERE: // draw a sphere
				{
					int circleParam1;
					//int circleParam2;
					int circleParam3;
					int circleParam4;
					int circleParam5;

					eax = *(int*) esi;

					circleParam1 = *(unsigned char*)esi;
					circleParam4 = *((short int*)(esi+1));
					circleParam5 = *((short int*)(esi+3));
					circleParam3 = *((short int*)(esi+5));

					if(!isUsingOrhoProjection)
					{
						circleParam3 = (circleParam3 * cameraPosY) / (cameraPosX + *(short int*)esi);
					}
					else
					{
						circleParam3 = (circleParam3 * 34) >> 9;
					}

					circleParam3+=3;

					if(circleParam4 + circleParam3 > renderRight)
						renderRight = circleParam4 + circleParam3;

					if(circleParam4 - circleParam3 < renderLeft)
						renderLeft = circleParam4 - circleParam3;

					if(circleParam5 + circleParam3 > renderBottom)
						renderBottom = circleParam5 + circleParam3;

					if(circleParam5 - circleParam3 < renderTop)
						renderTop = circleParam5 - circleParam3;

					circleParam3-=3;

					circle_fill(circleParam4, circleParam5, circleParam3, circleParam1);
				}
				default:
				{
					break;
				}
			}

			esi = renderV19;
			renderTabEntryPtr2++;
		}while (--primitiveCounter);
	}
	else
	{
		renderRight = -1;
		renderBottom = -1;
		renderLeft = -1;
		renderTop = -1;
		return (-1);
	}

	return (0);
}

int render_animated_model(unsigned char *bodyPtr)
{
	elementEntry *elemEntryPtr;
	pointTab *pointPtr;
	pointTab *pointPtrDest;
	int coX;
	int coY;
	int coZ;
	unsigned char *tmpElemPtr;
	int *tmpLightMatrix;
	unsigned char *tmpShadePtr;
	int numOfShades;

	numOfPoints = *((unsigned short int*)bodyPtr);
	bodyPtr += 2;
	pointsPtr = bodyPtr;

	bodyPtr += numOfPoints * 6;

	numOfElements = *((unsigned short int*)bodyPtr);
	bodyPtr += 2;
	elementsPtr = elementsPtr2 = bodyPtr;

	currentMatrixTableEntry = (unsigned char *) matricesTable;

	process_rotated_element(renderAngleX, renderAngleY, renderAngleZ, (elementEntry *) elementsPtr);

	elementsPtr += 38;

	elemEntryPtr = (elementEntry *) elementsPtr;

	if (numOfElements - 1 != 0)
	{
		numOfPrimitives = numOfElements - 1;
		currentMatrixTableEntry = (unsigned char *) &matricesTable[9];

		do
		{
			int boneType = elemEntryPtr->flag;

			if (boneType == 0)
			{
				process_rotated_element(elemEntryPtr->rotateX, elemEntryPtr->rotateY, elemEntryPtr->rotateZ, elemEntryPtr);  // rotation
			}
			else if (boneType == 1)
			{
				process_translated_element(elemEntryPtr->rotateX, elemEntryPtr->rotateY, elemEntryPtr->rotateZ, elemEntryPtr); // translation
			}

			currentMatrixTableEntry += 36;
			elementsPtr += 38;
			elemEntryPtr = (elementEntry *) elementsPtr;

		}while (--numOfPrimitives);
	}

	numOfPrimitives = numOfPoints;

	pointPtr = (pointTab *) computedPoints;
	pointPtrDest = (pointTab *) flattenPoints;

	if (isUsingOrhoProjection != 0) // use standard projection
	{
		do
		{
			coX = pointPtr->X + renderX;
			coY = pointPtr->Y + renderY;
			coZ = -(pointPtr->Z + renderZ);

			pointPtrDest->X = (coX + coZ)* 24 /512 + orthoProjX;
			pointPtrDest->Y = (((coX - coZ) *12) - coY*30) /512 + orthoProjY;
			pointPtrDest->Z = coZ - coX - coY;

			if (pointPtrDest->X < renderLeft)
				renderLeft = pointPtrDest->X;
			if (pointPtrDest->X > renderRight)
				renderRight = pointPtrDest->X;

			if (pointPtrDest->Y < renderTop)
				renderTop = pointPtrDest->Y;
			if (pointPtrDest->Y > renderBottom)
				renderBottom = pointPtrDest->Y;

			pointPtr++;
			pointPtrDest++;
		}while (--numOfPrimitives);
	}
	else
	{
		do
		{
			coX = pointPtr->X + renderX;
			coY = pointPtr->Y + renderY;
			coZ = -(pointPtr->Z + renderZ);

			coZ += cameraPosX;

			if(coZ<=0)
				coZ = 0x7FFFFFFF;

			// X projection
			{
				coX= (coX * cameraPosY) / coZ + orthoProjX;

				if(coX > 0xFFFF)
					coX = 0x7FFF;

				pointPtrDest->X = coX;

				if (pointPtrDest->X < renderLeft)
					renderLeft = pointPtrDest->X;

				if (pointPtrDest->X > renderRight)
					renderRight = pointPtrDest->X;
			}

			// Y projection
			{
				coY= (-coY * cameraPosZ) / coZ + orthoProjY;

				if(coY > 0xFFFF)
					coY = 0x7FFF;

				pointPtrDest->Y = coY;

				if (pointPtrDest->Y < renderTop)
					renderTop = pointPtrDest->Y;
				if (pointPtrDest->Y > renderBottom)
					renderBottom = pointPtrDest->Y;
			}

			// Z projection
			{
				if(coZ > 0xFFFF)
					coZ = 0x7FFF;

				pointPtrDest->Z = coZ;
			}

			pointPtr++;
			pointPtrDest++;

		}while (--numOfPrimitives);
	}

	shadePtr = (int *) elementsPtr;

	numOfShades = *((unsigned short int*)shadePtr);

	shadePtr = (int *) (((unsigned char *) shadePtr) + 2);

	if (numOfShades)     // process normal data
	{
		int color;
		int shade;

		unsigned char *currentShadeDestination = (unsigned char *) shadeTable;
		int *lightMatrix = matricesTable;
		unsigned char *pri2Ptr2;

		numOfPrimitives = numOfElements;

		tmpElemPtr = pri2Ptr2 = elementsPtr2 + 18;

		//assert(frontVideoBufferbis == frontVideoBuffer);

		do // for each element
		{
			numOfShades = *((unsigned short int*)tmpElemPtr);
			
			if (numOfShades)
			{
				int numShades = numOfShades;

				shadeMatrix[0] = (*lightMatrix) * lightX;
				shadeMatrix[1] = (*(lightMatrix + 1)) * lightX;
				shadeMatrix[2] = (*(lightMatrix + 2)) * lightX;

				shadeMatrix[3] = (*(lightMatrix + 3)) * lightY;
				shadeMatrix[4] = (*(lightMatrix + 4)) * lightY;
				shadeMatrix[5] = (*(lightMatrix + 5)) * lightY;

				shadeMatrix[6] = (*(lightMatrix + 6)) * lightZ;
				shadeMatrix[7] = (*(lightMatrix + 7)) * lightZ;
				shadeMatrix[8] = (*(lightMatrix + 8)) * lightZ;

				do  // for each normal
				{
					short int col1;
					short int col2;
					short int col3;

					short int *colPtr;

					colPtr = (short int *) shadePtr;
					
					col1 = *((short int*)colPtr++);
					col2 = *((short int*)colPtr++);
					col3 = *((short int*)colPtr++);

					color =  shadeMatrix[0] * col1 + shadeMatrix[1] * col2 + shadeMatrix[2] * col3;
					color += shadeMatrix[3] * col1 + shadeMatrix[4] * col2 + shadeMatrix[5] * col3;
					color += shadeMatrix[6] * col1 + shadeMatrix[7] * col2 + shadeMatrix[8] * col3;

					shade = 0;

					if (color > 0)
					{
						color >>= 14;
						tmpShadePtr = (unsigned char *) shadePtr;
						color /= *((unsigned short int*)(tmpShadePtr+6));
						shade = (unsigned short int) color;
					}

					*((unsigned short int*)currentShadeDestination) = shade;				
					currentShadeDestination += 2;
					shadePtr += 2;

				}while (--numShades);
			}
			
			tmpElemPtr = pri2Ptr2 = pri2Ptr2 + 38; // next element

			tmpLightMatrix = lightMatrix = lightMatrix + 9;
		}while (--numOfPrimitives);
	}

	return render_model_elements((unsigned char *) shadePtr);
}

void prepare_iso_model(unsigned char *bodyPtr) // loadGfxSub
{
	bodyHeaderStruct *bodyHeader;
	short int offsetToData;
	unsigned char *bodyDataPtr;
	short int numOfElement1;
	short int numOfPoint;
	unsigned char *ptrToKeyData;
	int i;
	int bp = 36;
	int bx = 38;
	unsigned char *ptr2;

	bodyHeader = (bodyHeaderStruct *)bodyPtr;

	if (!(bodyHeader->bodyFlag & 2))	// no animation applicable
	{
		return;
	}

	offsetToData = bodyHeader->offsetToData;

	bodyDataPtr = bodyPtr + offsetToData + 16;

	numOfElement1 = *((short int*)bodyDataPtr);
	ptr2 = bodyDataPtr + 2 + numOfElement1 * 6;

	numOfPoint = *((short int*)ptr2);

	ptrToKeyData = ptr2 + 2;

	for (i = 0; i < numOfPoint; i++)
	{
		ptrToKeyData += 38;
		*((short int*)(ptrToKeyData + 6)) = (*((short int*)(ptrToKeyData + 6)) * bp) / bx;
	}
}

int render_iso_model(int X, int Y, int Z, int angleX, int angleY, int angleZ, unsigned char *bodyPtr) // AffObjetIso
{
	unsigned char *ptr;
	short int bodyHeader;

	shadeAngleTab1 = &shadeAngleTable[0];
	shadeAngleTab2 = &shadeAngleTable[256];
	shadeAngleTab3 = &shadeAngleTable[384];

	renderAngleX = angleX;
	renderAngleY = angleY;
	renderAngleZ = angleZ;

	// model render size reset
	renderLeft = 32767;
	renderTop = 32767;
	renderRight = -32767;
	renderBottom = -32767;

	if (isUsingOrhoProjection == 0)
	{       
		set_base_rotation(X,Y,Z);

		renderX = destX - baseRotPosX;
		renderY = destY - baseRotPosY; // RECHECK
		renderZ = destZ - baseRotPosZ;
	}
	else
	{
		renderX = X;
		renderY = Y;
		renderZ = Z;
	}

	// reset the number of primitives in the model
	numOfPrimitives = 0; 

	// restart at the beginning of the renderTable
	renderTabEntryPtr = renderTab;  

	bodyHeader = *((unsigned short int*)bodyPtr);

	// jump after the header
	ptr = bodyPtr + 16 + *((unsigned short*)(bodyPtr + 14)); 

	if (bodyHeader & 2) // if animated
	{
		// the mostly used renderer code
		return (render_animated_model(ptr));
	}
	else
	{
		printf("Unsupported unanimated model render!\n");
		exit(1);
	}

	return (0);
}

void copy_actor_intern_anim(unsigned char *bodyPtrSrc, unsigned char *bodyPtrDest)
{
	short int cx;
	short int ax;
	int i;

	// check if both characters allow animation
	if(!(*((short int*)bodyPtrSrc)&2))
		return;

	if(!(*((short int*)bodyPtrDest)&2))
		return;

	// skip header
	bodyPtrSrc+=16; 
	bodyPtrDest+=16;

	*((unsigned int*)bodyPtrDest) = *((unsigned int*)bodyPtrSrc);
	*((unsigned int*)(bodyPtrDest+4)) = *((unsigned int*)(bodyPtrSrc+4));

	bodyPtrSrc=bodyPtrSrc+*((short int*)(bodyPtrSrc-2));
	bodyPtrSrc=bodyPtrSrc+(*((short int*)bodyPtrSrc))*6+2;
	cx=*((short int*)bodyPtrSrc);

	bodyPtrDest=bodyPtrDest+*((short int*)(bodyPtrDest-2));
	bodyPtrDest=bodyPtrDest+(*((short int*)bodyPtrDest))*6+2;
	ax=*((short int*)bodyPtrDest);

	if(cx>ax)
	cx=ax;

	bodyPtrSrc+=10;
	bodyPtrDest+=10;

	for(i=0;i<cx;i++)
	{
		*((unsigned int*)bodyPtrDest) = *((unsigned int*)bodyPtrSrc);
		*((unsigned int*)(bodyPtrDest+4)) = *((unsigned int*)(bodyPtrSrc+4));

		bodyPtrDest+=30;
		bodyPtrSrc+=30;
	}
}
