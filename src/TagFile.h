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

#ifndef TAGFILE_H
#define TAGFILE_H

#include <stdio.h>

//! Read-only
class TagFile
{
public:
        enum Flags
        {
                FlagCompressed = 0x1
        };

        TagFile(const char *name);
        ~TagFile();
        
        //! \returns -1 on EOF
        int                             readTag();
        
        //! data must point to an area at least as big as getDataSize() bytes.
        unsigned int    readData(unsigned char *data, unsigned int size);
        unsigned int    getDataSize();
        
private:
        struct
        {
                unsigned char   id;
                unsigned char   flags;
                unsigned int    size, uncompressedSize;
        } tag;

        FILE    *file;
};
/*
        def getHeader(self, id, data, flags):
                return (flags << 28) + (id << 24) + len(data)

        def writeTag(self, id, data, flags=0):
                origsize = len(data)
                cdata = zlib.compress(data, 9)
                
#               if flags & FLAG_COMPRESSED:
                if len(cdata) < origsize:
                        data = cdata
                        flags |= FLAG_COMPRESSED
                        
                header = self.getHeader(id, data, flags)
                self.file.write(struct.pack("l", header))
                
                if flags & FLAG_COMPRESSED:
                        self.file.write(struct.pack("l", origsize))
                        print "Wrote tag %2d: %d bytes, %d bytes compressed." % (id, origsize,len(data))
                else:
                        print "Wrote tag %2d: %d bytes." % (id, len(data))
                
                self.file.write(data)
*/
class WriteTagFile
{
public:
        WriteTagFile(const char *name);
        ~WriteTagFile();
        
        bool    writeTag(int id, const char *data, unsigned int dataSize, int flags = 0);
        
private:
        unsigned int    getHeader(int id, unsigned int dataSize, int flags);
        
        FILE    *file;
};

#endif
