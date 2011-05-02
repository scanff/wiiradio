/*
 * Filters.cpp
 *
 *  Created on: Jun 18, 2008
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

#include "Filters.hpp"

#ifdef _WII_
#include "global_gx.h"
#endif
void Brighten::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};
#ifdef _WII_

    GX_SetAlphaUpdate(GX_TRUE);
    WII_SetColors(255,255,255,255);

    GX_SetBlendMode(GX_BM_LOGIC, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_AND);
	WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

	GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_DSTCLR, GX_LO_AND);
	WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

	GX_SetBlendMode(GX_BM_LOGIC, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_AND);
	WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

   // GX_SetAlphaUpdate(GX_FALSE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,points);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_ZERO, GL_DST_COLOR);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void Darken::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};

#ifdef _WII_

    GX_SetAlphaUpdate(GX_TRUE);

    WII_SetColors(255,255,255,255);
    GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_DSTCLR, GX_LO_AND);
	WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

   // GX_SetAlphaUpdate(GX_FALSE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

#else
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2,GL_FLOAT,0,points);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBlendFunc(GL_ZERO, GL_DST_COLOR);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void Invert::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};

#ifdef _WII_
    GX_SetAlphaUpdate(GX_TRUE);
	WII_SetColors(255,255,255,255);

    GX_SetBlendMode(GX_BM_LOGIC, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_AND);
	WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

	//GX_SetAlphaUpdate(GX_FALSE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

#else
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2,GL_FLOAT,0,points);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void Solarize::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
					      {-0.5,  0.5},
					      { 0.5,  0.5},
					      { 0.5,  -0.5}};

#ifdef _WII_
    GX_SetAlphaUpdate(GX_TRUE);
    WII_SetColors(255,255,255,255);

    GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_INVDSTCLR, GX_LO_AND );
	WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

	GX_SetBlendMode(GX_BM_LOGIC, GX_BL_DSTCLR, GX_BL_ONE, GX_LO_AND );
    WII_VertArrays(GX_TRIANGLEFAN,&points[0][0],0,0,0,4,2);

    //GX_SetAlphaUpdate(GX_FALSE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

#else

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,points);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_COLOR);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_DST_COLOR, GL_ONE);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

