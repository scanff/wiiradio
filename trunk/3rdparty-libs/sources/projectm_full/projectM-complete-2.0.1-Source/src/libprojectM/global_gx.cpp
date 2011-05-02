/*
    Name:       global_gx.cpp
    Purpose:    Wii's 3D graphics wrapper for ProjectM
    Author:     Scanff
    Copyright:  (c) 2010 Scanff@yahoo.com
    Licence:    GPL
*/

// Wii specific functions and wrappers

#ifdef _WII_
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "global_gx.h"

#define MAX_TEXTURES	 (20)
#define FBO_TEX          (0)
#define BYTES_PER_PIXEL  (4)
#define CLAMP_SIZE(x) x > 255 ? x = 255 : x < 6 ? x = 6 : x;

struct _textures
{
	_textures() : data(0), w(0), h(0), inuse(false) {};

    GXTexObj        texture;
    u8*             data;
    int             w;
    int             h;
    unsigned int    size;
	bool			inuse;
};

static _textures    textures[MAX_TEXTURES];
static int          GX_current_texture  = FBO_TEX;
static GXRModeObj   *vmode;
static int          ih, iw;
static int          texture_attrib      = GX_CLAMP;
//static int          textures_used       = FBO_TEX + 1; // always use texture 0 as framebuffer texture

Mtx     modelview;
Mtx     textureview;
Mtx44   projection;
static  u8 gx_r,gx_g,gx_b,gx_a;

struct txVector
{
    float u, v;
};

struct clrVector
{
    float r, g, b, a;
};

struct vVector
{
    float x,y,z;
};

void WII_SetColors(const u8 r, const u8 g, const u8 b,const u8 a)
{
    gx_r = r;
    gx_g = g;
    gx_b = b;
    gx_a = a;
}

void WII_SetTextureAttrib(const int attrib)
{
    texture_attrib = attrib;
}

void WII_SetCurrentTexture(const int tex_id)
{
	GX_current_texture = tex_id;
}


void WII_Delete_Texture(const int id)
{
    if(textures[id].data) free(textures[id].data);

	textures[id].data = 0;
	textures[id].w = textures[id].h = 0;
	textures[id].inuse = false;
}

inline void  _WII_ConvertRGBA_T0_4x4(const int x, const int y, const _textures *tex, const u32 color)
{
    u8*  pixels = (u8*)tex->data;

    const u32 offset = (((y&(~3))<<2)*tex->w) + ((x&(~3))<<4) + ((((y&3)<<2) + (x&3)) <<1);

    *((u16*)(pixels+offset))    = (u16)((color <<8) | (color >>24));
    *((u16*)(pixels+offset+32)) = (u16)(color >>8);

}

void WII_CreateTexture(int id, void* data, int w, int h)
{
    return; // TODO
/*    printf("Image id %d W %d H %d \n",id,w,h);
    // -- check texture is x4
    div_t div_w, div_h;
    div_w = div (w,4);
    div_h = div (h,4);

    if(div_w.rem != 0 || div_h.rem != 0)
    {
        // resise ?
        printf("Image ! x4 - id = %d\n",id);
        return;
    }

    textures[id].inuse = true;
    textures[id].w = w;
    textures[id].h = h;
    textures[id].size = w*h*BYTES_PER_PIXEL;
    textures[id].data = (u8*)memalign(32,w*h*BYTES_PER_PIXEL);

    const int stride = w*BYTES_PER_PIXEL;
	u32* pdat = (u32*)(data);

    for(int y=0;y<h;y++)
    {
        for(int x=0;x<w;x++)
        {
            u32 color = *pdat++; //0x00ff00ff + (y << 24);
            SetPixelTotexImg(x,y,&textures[id],color);
        }
    }

    DCFlushRange(textures[id].data, textures[id].size);

    GX_InitTexObj(&textures[id].texture, (void*)textures[id].data, textures[id].w, textures[id].h,GX_TF_RGBA8,  GX_CLAMP,  GX_CLAMP, GX_FALSE);
    GX_LoadTexObj(&textures[id].texture, GX_TEXMAP0);



  //  free(data); // kill original

*/
}

// FRAME BUF TEX
void WII_FBO_Texture()
{
    if ( textures[FBO_TEX].inuse )  return;

    GX_InvalidateTexAll();

    textures[FBO_TEX].w = iw;
    textures[FBO_TEX].h = ih;
    textures[FBO_TEX].size = textures[FBO_TEX].w*textures[FBO_TEX].h*BYTES_PER_PIXEL;

//	    printf("alloc tex %d, bytes = %d w=%d h=%d\n",r,textures[0].size,w,h);
    textures[FBO_TEX].data = (u8*)memalign(32,textures[FBO_TEX].size);
    memset(textures[FBO_TEX].data,0, textures[FBO_TEX].size);
    DCFlushRange(textures[FBO_TEX].data, textures[FBO_TEX].size);

    GX_InitTexObj(&textures[FBO_TEX].texture, (void*)textures[FBO_TEX].data, textures[FBO_TEX].w, textures[FBO_TEX].h,GX_TF_RGBA8,  GX_LINEAR,  GX_LINEAR, GX_FALSE);
    GX_LoadTexObj(&textures[FBO_TEX].texture, GX_TEXMAP0);

    textures[FBO_TEX].inuse = true;

}

void WII_Init_GX(const int w, const int h)
{
    vmode = VIDEO_GetPreferredMode(NULL);

    ih = h;
    iw = w;

    GX_SetScissor(0,0,vmode->fbWidth, vmode->efbHeight);
	GX_SetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
	//GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    //GX_SetCopyFilter(vmode->aa,vmode->sample_pattern,GX_TRUE,vmode->vfilter);
    WII_Renderer_Reset();

}

void WII_Renderer_Reset()
{
    GX_SetZMode (GX_FALSE, GX_GEQUAL, GX_FALSE);
    GX_SetCullMode(GX_CULL_NONE);

    GX_SetAlphaUpdate(GX_FALSE);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetDstAlpha(GX_DISABLE, 0xFF);

    //GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);

    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GX_SetNumChans(1);

    GX_SetDither(GX_TRUE);
}

void inline GX_Settings(const int draw_mode)
{

   // GX_InvVtxCache();
	GX_ClearVtxDesc();


    switch(draw_mode)
    {
        case VERT_CT:

            GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
            GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
            GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

            GX_SetNumTexGens(1);


            GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
            GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);// : GX_IDENTITY);
            GX_LoadTexMtxImm(textureview,GX_TEXMTX0,GX_MTX2x4);

            DCFlushRange(textures[FBO_TEX].data, textures[FBO_TEX].size);

            //GX_InitTexObjLOD(&textures[FBO_TEX].texture, GX_NEAR, GX_NEAR,0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
            GX_InitTexObjWrapMode(&textures[FBO_TEX].texture,texture_attrib,texture_attrib);
            GX_LoadTexObj(&textures[FBO_TEX].texture, GX_TEXMAP0);

        break;

        case VERT_C:

            GX_SetNumTexGens(0);

            GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
            GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

            GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
            GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        break;
    };

    GX_SetCurrentMtx(GX_PNMTX0);


}


void WII_ReadFramebuffer()
{

    if (!textures[FBO_TEX].data)
        return;

	GX_DrawDone();
    //DCFlushRange(textures[FBO_TEX].data, textures[FBO_TEX].size);

	GX_SetTexCopySrc(0, 0, textures[FBO_TEX].w,textures[FBO_TEX].h);
	GX_SetTexCopyDst(textures[FBO_TEX].w,textures[FBO_TEX].h, GX_TF_RGBA8, GX_FALSE);

    //GX_SetCopyFilter(GX_FALSE, NULL, GX_FALSE, NULL);

	GX_CopyTex((void*)textures[FBO_TEX].data, GX_TRUE);
	GX_PixModeSync();


    //GX_SetCopyFilter(vmode->aa, vmode->sample_pattern, GX_TRUE, vmode->vfilter);

}

void WII_SetLineWidth(const float size, const u8 format)
{
	int width = (int)(6 * ( size + 1.0f));
	CLAMP_SIZE(width);

	GX_SetLineWidth((u8)width, format);
}

void WII_SetPointSize(const float size)
{
	int width = (int)(6 * ( size + 1.0f));

    CLAMP_SIZE(width);

	GX_SetPointSize((u8)width, GX_TO_ZERO);
}

void inline *OffsetByte(const void *p, const int n)
{
	u8 * ptr = (u8 *) p;
	return ptr+n;
}

void WII_Rectd(const float _x1, const float _y1, const float _x2, const float _y2)
{
    GX_Settings(VERT_C);

    GX_SetCurrentMtx(GX_PNMTX0);

    if (_x2 > _x1 && _y2 > _y1) {
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

            GX_Position3f32(_x1, _y2,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

            GX_Position3f32(_x2, _y2,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

            GX_Position3f32(_x2, _y1,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

            GX_Position3f32(_x1, _y1,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

        GX_End();
    } else {
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

            GX_Position3f32(_x1, _y1,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

            GX_Position3f32(_x2, _y1,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

            GX_Position3f32(_x2, _y2,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

            GX_Position3f32(_x1, _y2,0.0f);
            GX_Color4u8(gx_r,gx_g,gx_b,gx_a);

        GX_End();
    }
}

void WII_VertArrays(const int type, const float* points, const float* tx, const float* colors, const int offset, const int size, const int bytewidth)
{
    const int draw_mode = (tx ? VERT_CT : VERT_C); // 0 = Vert+Clr+Tex, 1 = Vert + Clr

	GX_Settings(draw_mode);

    const int colorPointerStride      = 4 * sizeof(float);
    const int vertexPointerStride     = bytewidth * sizeof(float);
    const int texturePointerStride    = 2 *  sizeof(float);

    switch(draw_mode)
    {
        case VERT_CT:

            GX_Begin(type, GX_VTXFMT0, size);

            for (int index = offset; index != offset + size; ++index)
            {
                const vVector* v = (vVector*)OffsetByte(points, index*vertexPointerStride);
                const txVector* t = (txVector*)OffsetByte(tx, index*texturePointerStride);
                GX_Position3f32(v->x, v->y, bytewidth == 2 ? 0.0f : v->z);
                GX_Color4u8(gx_r,gx_g,gx_b,gx_a);
                GX_TexCoord2f32(t->u, t->v);

            }
            GX_End();

        break;

        case VERT_C:

            GX_Begin(type, GX_VTXFMT0, size);

                for (int index = offset; index != offset + size; ++index)
                {
                    const vVector* v    = (vVector*)OffsetByte(points,index*vertexPointerStride);
                    const clrVector* c  = (clrVector*)OffsetByte(colors,index*colorPointerStride);

                    GX_Position3f32(v->x, v->y, bytewidth == 2 ? 0.0f : v->z);

                    if (colors)
                    {
                        // -- Colors are created as float (GL style) so rescale to u8
                        GX_Color4u8(c->r*0xff,c->g*0xff,c->b*0xff,c->a*0xff);
                    }else{
                        // -- Should have alread * 255 in ProjectM code
                        GX_Color4u8(gx_r,gx_g,gx_b,gx_a);
                    }
                }

            GX_End();
        break;
    }

}

#endif
