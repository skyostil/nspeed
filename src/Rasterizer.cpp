/***************************************************************************
 *   Copyright (C) 2004 by Sami Kyöstilä                                   *
 *   skyostil@kempele.fi                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Rasterizer.h"
#include <stdio.h>
#include <string.h>

Rasterizer::Rasterizer(Game::Surface *_screen):
        screen(_screen),
        currentVertex(0),
        texture(0),
        color(0xffff),
        initialEdgeFlag(false),
        textureTileList(0),
        flags(FlagPerspectiveCorrection),
        minY(0), maxY(_screen->height)
{
        int y;
        
        edge = new Scanline[screen->height];
        
        for(y=0; y<screen->height; y++)
        {
                edge[y].offset = &screen->pixels[y*screen->width];
        }
}

Rasterizer::~Rasterizer()
{
        delete[] edge;
}

void Rasterizer::beginPolygon()
{
        int y;
        
        for(y=minY; y<maxY; y++)
        {
                edge[y].x2 = -1;
                edge[y].x1 = FPInt(screen->width+1);
        }
        
        minY = screen->height;
        maxY = 0;
        
        currentVertex = 0;
        initialEdgeFlag = false;
}

void Rasterizer::setInvZ(scalar invz)
{
        vertex[currentVertex].invz = invz;
}

void Rasterizer::setTexCoord(scalar u, scalar v)
{
        vertex[currentVertex].uz = u;
        vertex[currentVertex].vz = v;
}

void Rasterizer::addVertex(scalar sx, scalar sy)
{
        vertex[currentVertex].sx = sx;
        vertex[currentVertex].sy = sy;
        finalizeVertex();
}

void Rasterizer::addVertex(ScreenVertex *v)
{
        memcpy(&vertex[currentVertex], v, sizeof(ScreenVertex));
        finalizeVertex();
}

void Rasterizer::finalizeVertex()
{
        
        switch(currentVertex)
        {
        case 0:
                currentVertex = 1;
        break;
        case 1:
                if (!initialEdgeFlag)
                {
                        addEdge(&vertex[0], &vertex[1]);
                        initialEdgeFlag = true;
                }
                else
                        addEdge(&vertex[2], &vertex[1]);
                        
                currentVertex = 2;
        break;
        case 2:
                addEdge(&vertex[1], &vertex[2]);
                currentVertex = 1;
        break;
        }
}

void Rasterizer::addEdge(ScreenVertex *v1, ScreenVertex *v2)
{
        if (v2->sy < v1->sy)
        {
                ScreenVertex *tmp = v1;
                v1 = v2;
                v2 = tmp;
        }

//      scalar h = (v2->sy - v1->sy) + (1<<16);
        scalar h = v2->sy - v1->sy;
        
        if (h < (1<<FP))
                return;
                
        // setup interpolation
        scalar dx = FPDiv(v2->sx - v1->sx, h);
        scalar dz = FPDiv(v2->invz - v1->invz, h);
        scalar du = FPDiv(v2->uz - v1->uz, h);
        scalar dv = FPDiv(v2->vz - v1->vz, h);

//      printf("height: %3d\n", h>>FP);
                
        scalar x = v1->sx;
        scalar z = v1->invz;
        scalar u = v1->uz;
        scalar v = v1->vz;
//      scalar y1 = (v1->sy+(FP_ONE>>1))>>FP;
        scalar y1 = v1->sy>>FP;
        scalar y2 = v2->sy>>FP;

//      printf("scanline: %3d - %3d\n", v1->sx>>FP, v2->sx>>FP);
        
/*      
        // add sub-pixel correction
        scalar subpixFix = ((1<<FP) - ((v1->sy) & ((1<<FP)-1))) >> (FP/2);
        
        x += (subpixFix * (dx >> (FP/4))) >> (FP/4);
        z += (subpixFix * (dz >> (FP/4))) >> (FP/4);
        u += (subpixFix * (du >> (FP/4))) >> (FP/4);
        v += (subpixFix * (dv >> (FP/4))) >> (FP/4);
*/      
        // add sub-pixel correction
        scalar subpixFix = ((v1->sy + FP_ONE) & FP_INTMASK) - v1->sy;
//      scalar subpixFix = ((1<<FP) - ((v1->sy) & ((1<<FP)-1))) >> (FP/2);
        
        x += FPMul(subpixFix, dx);
        z += FPMul(subpixFix, dz);
        u += FPMul(subpixFix, du);
        v += FPMul(subpixFix, dv);
        
        h = (v2->sy>>FP) - (v1->sy>>FP);
//      h = (v2->sy | 0xffff) - (v1->sy);
//	  h >>= FP;

//		h = (((v2->sy | 0xffff)) - (v1->sy & 0xffff0000));
//		if (h == 0)
//			h = 1;

#ifdef RASTERIZER_2D_CLIPPING
        if (y1 < 0)
        {
                x += FPMul(dx,-v1->sy);
                z += FPMul(dz,-v1->sy);
                u += FPMul(du,-v1->sy);
                v += FPMul(dv,-v1->sy);
                h += y1;
                y1 = 0;
        }

        if (y2 >= screen->height-1)
        {
                h -= y2 - (screen->height-2);
                y2 = screen->height-1;
        }
#endif
//      h = y2 - y1;

        // update vertical limits
        if (y1 < minY && y1 >= 0)
                minY = y1;
        if (y2 > maxY && y2 < screen->height)
                maxY = y2;
                
        Scanline *scanline = &edge[y1];

//      h = ((v2->sy+FP_ONE) & FP_INTMASK) - ((v1->sy + FP_ONE) & FP_INTMASK);
//      h = (v2->sy & FP_INTMASK) - (v1->sy & FP_INTMASK);
//      h>>=FP;
/*      
        if (((v2->sx - v1->sx)>0) != (dx>0))
        {
                printf(":  %3d, %3d, %3d, %3d\n", v1->sx>>FP, v2->sx>>FP, (v2->sx - v1->sx)>>FP, dx>>FP);
        }
*/      
//      while((h-=(1<<16)) > -1)
        while(h-->0)
        {
                if (x < scanline->x1)
                {
//                      printf("left:  %3d, %3d\n", x>>FP, dx>>FP);
                        scanline->x1 = x;
                        scanline->u1z = u;
                        scanline->v1z = v;
                        scanline->invz1 = z;
                }
                if (x > scanline->x2)
                {
//                      printf("right: %d\n", x>>FP);
                        scanline->x2 = x;
                        scanline->u2z = u;
                        scanline->v2z = v;
                        scanline->invz2 = z;
                }
                
                x+=dx;
                z+=dz;
                u+=du;
                v+=dv;
                scanline++;
        }
}

void Rasterizer::endPolygon()
{
        switch(currentVertex)
        {
        case 1:
                addEdge(&vertex[2], &vertex[0]);
        break;
        case 2:
                addEdge(&vertex[1], &vertex[0]);
        break;
        default:
                return;
        }
        
//      printf("min: %d, max: %d\n", minY, maxY);

        if (texture)
        {
                if (flags & FlagPerspectiveCorrection)
                {
                        if (textureTileList && flags & FlagTileTexture)
                        {
                                tileTextureRasterizer();
                        }
                        else
                        {
                                perspectiveTextureRasterizer();
                        }
                } else // no perspective correction
                {
                        textureRasterizer();
                }
        }
        else
        {
                flatRasterizer();
        }
}

void Rasterizer::flatRasterizer()
{
        int x, y;
        
        for(y=minY; y<maxY; y++)
        {
                if (edge[y].x2 > edge[y].x1)
                {
                        scalar len = (edge[y].x2 - edge[y].x1);
                        
                        len>>=FP;
                        
#ifdef RASTERIZER_2D_CLIPPING
                        if (edge[y].x1 < 0)
                        {
                                len += (edge[y].x1>>FP);
                                edge[y].x1 = 0;
                        }

                        if ((edge[y].x2>>FP) >= screen->width)
                        {
                                len -= (edge[y].x2>>FP) - (screen->width-1);
                        }
#endif

                        Game::Pixel *p = edge[y].offset + (edge[y].x1>>FP);
                        while(len-->=0)
                        {
                                *p++ = color;
                        }
                }
//              edge[y].offset[edge[y].x1>>FP] = 0xf000;
//              edge[y].offset[edge[y].x2>>FP] = 0x0f00;
        }
}

void Rasterizer::textureRasterizer()
{
        int x, y;
        
        for(y=minY; y<maxY; y++)
        {
                if (edge[y].x2 > edge[y].x1)
                {
                        Game::Pixel *t = texture->pixels;
                        scalar len = (edge[y].x2 - edge[y].x1);
                        
                        if (len < (1<<FP))
                                continue;
                        
                        scalar u1 = edge[y].u1z;
                        scalar v1 = edge[y].v1z;
                        scalar u2 = edge[y].u2z;
                        scalar v2 = edge[y].v2z;
                        
                        scalar du = FPDiv(u2 - u1, len);
                        scalar dv = FPDiv(v2 - v1, len);
                        
                        scalar subpixFix = ((edge[y].x1 + FP_ONE) & FP_INTMASK) - edge[y].x1;
                        u1 += FPMul(subpixFix, du);
                        v1 += FPMul(subpixFix, dv);
                        
                        len>>=FP;

#ifdef RASTERIZER_2D_CLIPPING
                        if (edge[y].x1 < 0)
                        {
                                u1 += FPMul(du, -edge[y].x1);
                                v1 += FPMul(dv, -edge[y].x1);
                                len += (edge[y].x1>>FP);
                                edge[y].x1 = 0;
                        }

                        if ((edge[y].x2>>FP) >= screen->width)
                        {
                                len -= (edge[y].x2>>FP) - (screen->width-1);
                        }
#endif

                        Game::Pixel *p = edge[y].offset + (edge[y].x1>>FP);
                        while(len-->=0)
                        {
                                *p++ = t[((u1>>FP)&0xff) + (((v1>>FP)&0xff)<<8)];
                                
                                u1+=du;
                                v1+=dv;
                        }
                }
        }
}

void Rasterizer::perspectiveTextureRasterizer()
{
        int x, y;
        
        for(y=minY; y<maxY; y++)
        {
                if (edge[y].x2 > edge[y].x1)
                {
                        Game::Pixel *t = texture->pixels;
                        scalar len = (edge[y].x2 - edge[y].x1);
                        
                        if (len < (1<<FP))
                                continue;
                                
                        scalar u1 = FPHighPrecDiv(edge[y].u1z,edge[y].invz1>>INVZ_SCALE);
                        scalar v1 = FPHighPrecDiv(edge[y].v1z,edge[y].invz1>>INVZ_SCALE);
                        scalar u2 = FPHighPrecDiv(edge[y].u2z,edge[y].invz2>>INVZ_SCALE);
                        scalar v2 = FPHighPrecDiv(edge[y].v2z,edge[y].invz2>>INVZ_SCALE);
                                                                
                        scalar du = FPDiv(u2 - u1, len);
                        scalar dv = FPDiv(v2 - v1, len);
                        
                        scalar subpixFix = ((edge[y].x1 + FP_ONE) & FP_INTMASK) - edge[y].x1;
                        u1 += FPMul(subpixFix, du);
                        v1 += FPMul(subpixFix, dv);
                        
                        len>>=FP;
                        
#ifdef RASTERIZER_2D_CLIPPING
                        if (edge[y].x1 < 0)
                        {
                                u1 += FPMul(du, -edge[y].x1);
                                v1 += FPMul(dv, -edge[y].x1);
                                len += (edge[y].x1>>FP);
                                edge[y].x1 = 0;
                        }

                        if ((edge[y].x2>>FP) >= screen->width)
                        {
                                len -= (edge[y].x2>>FP) - (screen->width-1);
                        }
#endif

                        Game::Pixel *p = edge[y].offset + (edge[y].x1>>FP);
                        while(len-->=0)
                        {
                                *p++ = t[((u1>>FP)&0xff) + (((v1>>FP)&0xff)<<8)];
                                
                                u1+=du;
                                v1+=dv;
                        }
                }
        }
}

void Rasterizer::tileTextureRasterizer()
{
        int x, y;
        
        for(y=minY; y<maxY; y++)
        {
                if (edge[y].x2 > edge[y].x1)
                {
                        unsigned char *t = (unsigned char*)texture->pixels;
                        scalar len = (edge[y].x2 - edge[y].x1);
                        
                        if (len < (1<<FP))
                                continue;
                                
                        scalar u1 = FPHighPrecDiv(edge[y].u1z,edge[y].invz1>>INVZ_SCALE);
                        scalar v1 = FPHighPrecDiv(edge[y].v1z,edge[y].invz1>>INVZ_SCALE);
                        scalar u2 = FPHighPrecDiv(edge[y].u2z,edge[y].invz2>>INVZ_SCALE);
                        scalar v2 = FPHighPrecDiv(edge[y].v2z,edge[y].invz2>>INVZ_SCALE);
                        
                        scalar du = FPDiv(u2 - u1, len);
                        scalar dv = FPDiv(v2 - v1, len);
                        
                        scalar subpixFix = ((edge[y].x1 + FP_ONE) & FP_INTMASK) - edge[y].x1;
                        u1 += FPMul(subpixFix, du);
                        v1 += FPMul(subpixFix, dv);
                        
                        len>>=FP;
                        
#ifdef RASTERIZER_2D_CLIPPING
                        if (edge[y].x1 < 0)
                        {
                                u1 += FPMul(du, -edge[y].x1);
                                v1 += FPMul(dv, -edge[y].x1);
                                len += (edge[y].x1>>FP);
                                edge[y].x1 = 0;
                        }

                        if ((edge[y].x2>>FP) >= screen->width)
                        {
                                len -= (edge[y].x2>>FP) - (screen->width-1);
                        }
#endif

                        Game::Pixel *p = edge[y].offset + (edge[y].x1>>FP);
                        while(len-->=0)
                        {
                                unsigned char tile = t[((u1>>FP)&0xff) + (((v1>>FP)&0xff)<<8)];
                                
                                if (tile)
                                        *p++ = textureTileList[tile]->pixels[
                                                ((u1>>(FP-TILE_TEXTURE_BITS))&TILE_TEXTURE_MASK)
                                                +
                                                (((v1>>(FP-TILE_TEXTURE_BITS))&TILE_TEXTURE_MASK)<<TILE_TEXTURE_BITS)
                                        ];
                                else
                                        p++;
                                
                                u1+=du;
                                v1+=dv;
                        }
                }
        }
}


void Rasterizer::setTexture(Game::Surface *_texture)
{
        if (_texture && _texture->width == 256)
                texture = _texture;
        else
                texture = 0;
}

void Rasterizer::setColor(unsigned short _color)
{
        color = _color;
}

void Rasterizer::setTextureTileList(Game::Surface **tiles)
{
        textureTileList = tiles;
}

