/*=========================================================
INCLUDES
=========================================================*/

#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspkernel.h>

/*=========================================================
VARIABLES
=========================================================*/

// terain patch

struct Vertex
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

struct Vertex __attribute__((aligned(16))) vertices[2*3] =
{
    { 0, 0, 0xff7f0000, -1.0f, 0.0f, 1.0f },
    { 0, 0, 0xff7f0000, 1.0f, 0.0f, 1.0f },
    { 0, 0, 0xff7f0000, 1.0f, 0.0f, -1.0f },

    { 0, 0, 0xff7f0000, 1.0f, 0.0f, -1.0f },
    { 0, 0, 0xff7f0000, -1.0f, 0.0f, -1.0f },
    { 0, 0, 0xff7f0000, -1.0f, 0.0f, 1.0f },
};

/*=========================================================
FUNCTIONS
=========================================================*/

void TER_init()
{
    
}

void TER_render()
{
		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		sceGumPerspective(75.0f,16.0f/9.0f,0.5f,1000.0f);

		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();

        sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		{
			ScePspFVector3 pos = { 0, 0.0f, -5.5f };
			//ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
			sceGumTranslate(&pos);
			//sceGumRotateXYZ(&rot);
		}




        		sceGumDrawArray(GU_TRIANGLES, /* GU_TEXTURE_32BITF |*/ GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2*3, 0, vertices);

}