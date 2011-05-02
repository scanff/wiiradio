/*
 * VideoEcho.cpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */
#ifndef _WII_
#ifdef USE_GLES1
#include <GLES/gl.h>
#else
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif
#endif
#include "VideoEcho.hpp"
#ifdef _WII_
#include "global_gx.h"
#endif
VideoEcho::VideoEcho(): a(0), zoom(1), orientation(Normal)
{
	// TODO Auto-generated constructor stub

}

VideoEcho::~VideoEcho()
{
	// TODO Auto-generated destructor stub
}

void VideoEcho::Draw(RenderContext &context)
{

		float tex[4][2] = {{0, 1},
				   {0, 0},
				   {1, 0},
				   {1, 1}};
/*
		float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};
*/
//Now Blend the Video Echo
#ifndef _WII_

		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//	glVertexPointer(2,GL_FLOAT,0,points);
		glTexCoordPointer(2,GL_FLOAT,0,tex);

		//Now Blend the Video Echo
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_TEXTURE);

		//draw video echo
		glColor4f(1.0, 1.0, 1.0, a * masterAlpha);
		glTranslatef(.5, .5, 0);
		glScalef(1.0/zoom, 1.0/zoom, 1);
		glTranslatef(-.5, -.5, 0);
#endif
		int flipx=1, flipy=1;
		switch (orientation)
		{
			case Normal: flipx=1;flipy=1;break;
			case FlipX: flipx=-1;flipy=1;break;
			case FlipY: flipx=1;flipy=-1;break;
			case FlipXY: flipx=-1;flipy=-1;break;
			default: flipx=1;flipy=1; break;
		}

		float pointsFlip[4][2] = {{-0.5*flipx, -0.5*flipy},
					  {-0.5*flipx,  0.5*flipy},
					  { 0.5*flipx,  0.5*flipy},
					  { 0.5*flipx, -0.5*flipy}};
#ifdef _WII_



		WII_SetColors(255,255,255, (a * masterAlpha) * 0xFF);

        guMtxIdentity(textureview);
        //guMtxCopy(modelview,textureview);
        GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        guMtxTransApply(modelview,modelview, 0.5f, 0.5f, 0.0f);
		guMtxScaleApply(textureview,textureview,1.0f/zoom, 1.0f/zoom, 1.0f);
		guMtxTransApply(modelview, modelview, -0.5f, -0.5f, 0.0f);

        GX_LoadPosMtxImm(modelview, GX_PNMTX0);

        WII_VertArrays(GX_TRIANGLEFAN,&pointsFlip[0][0],&tex[0][0],0,0,4,2);


        //GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
#else
		glVertexPointer(2,GL_FLOAT,0,pointsFlip);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);

		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}
