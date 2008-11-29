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

#ifndef BITMAPFONT_H
#define BITMAPFONT_H

#include "engine/Engine.h"

class BitmapFont
{
public:
    //! the surface's pixel format must match that of the screen's.
    BitmapFont(Game::Surface *t = 0);
    ~BitmapFont();

    void	renderText(Game::Surface *screen, const char *text, int x, int y, Game::Pixel colorMask = (Game::Pixel)-1);
    int		getTextWidth(const char *text);

    void	load(Game::Framework *framework, const char *name);
    void	unload();

unsigned int getHeight() { return texture?texture->height:0; }

private:
    typedef struct
    {
        Game::Pixel	*pixels;
        int		width, height;
    }
    Glyph;

    //! this is a pretty generic blitter actually
    template<typename Pixel>
    void	renderGlyph(Game::Surface *screen, Glyph *g, int x, int y, Game::Pixel colorMask);

    void	build(Game::Surface *t);
    void	clear();

    //! the first glyph is always space
    Glyph	        *glyph;
    int		        minGlyph, maxGlyph;
    Game::Surface	*texture;
    bool	        autoDelete;
};

#endif
