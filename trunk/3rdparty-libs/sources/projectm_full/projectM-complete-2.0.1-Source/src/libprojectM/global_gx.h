/*
    Name:       global_gx.h
    Purpose:    header for Wii's 3D graphics wrapper for ProjectM
    Author:     Scanff
    Copyright:  (c) 2010 Scanff@yahoo.com
    Licence:    GPL
*/

#ifndef GLOBAL_GX_H_INCLUDED
#define GLOBAL_GX_H_INCLUDED

// Wii specific functions and wrappers

#ifdef _WII_

#include <gctypes.h>
#include <gccore.h>
#include <ogc/gx.h>
#include <ogc/gu.h>

extern Mtx      modelview;
extern Mtx      textureview;
extern Mtx44    projection;

enum
{
    VERT_CT = 0, //color texture
    VERT_C
};


void WII_SetColors(const u8 r, const u8 g, const u8 b,const u8 a);
void WII_SetTextureAttrib(const int attrib);
void WII_SetCurrentTexture(const int tex_id);
void WII_Init_GX(const int w, const int h);
void WII_Rectd(const float _x1, const float _y1, const float _x2, const float _y2);
void WII_Renderer_Reset();
void WII_Delete_Texture(const int id);
void WII_FBO_Texture();
void WII_ReadFramebuffer();
void WII_SetLineWidth(const float size, const u8 format);
void WII_SetPointSize(const float size);
void WII_VertArrays(const int type, const float* points, const float* tx, const float* colors, const int offset, const int size, const int bytewidth);
void WII_CreateTexture(int id, void* data, int w, int h);

#endif // _WII_
#endif // GLOBAL_GX_H_INCLUDED
