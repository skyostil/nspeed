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
#include "TagFile.h"
#include "Config.h"

#ifdef EPOC
#include <ezlib.h>
#else
#include <zlib.h>
#endif

#define MAGIC "TAG1"

TagFile::TagFile(const char *name)
{
        char magic[4];
        tag.id = 0;
        tag.flags = 0;
        tag.size = 0;
        tag.uncompressedSize = 0;
        file = fopen(name, "rb");
        
        if (file)
        {
                fread(magic, sizeof(magic), 1, file);
                if (
                        magic[0] != MAGIC[0] ||
                        magic[1] != MAGIC[1] ||
                        magic[2] != MAGIC[2] ||
                        magic[3] != MAGIC[3])
                {
                        fprintf(stderr, "TagFile: Bad magic: '%s'\n", name);
                        fclose(file);
                        file = 0;
                }
        }
}

TagFile::~TagFile()
{
        if (file)
        {
                fclose(file);
                file = 0;
        }
}

int TagFile::readTag()
{
        if (file)
        {
                unsigned int header;
                if (fread(&header, sizeof(header), 1, file) != 1)
                {
                        tag.size = tag.uncompressedSize = 0;
                        return -1;
                }
                
                tag.flags = (header>>28);
                tag.id = (header>>24) & 0xf;
                tag.size = header & 0x00ffffff;
                
                if (tag.flags & FlagCompressed)
                {
                        fread(&tag.uncompressedSize, sizeof(tag.uncompressedSize), 1, file);
                }
                
                return tag.id;
        }
        return -1;
}

unsigned int TagFile::getDataSize()
{
        if (tag.flags & FlagCompressed)
        {
                return tag.uncompressedSize;
        }
        return tag.size;
}

unsigned int TagFile::readData(unsigned char *data, unsigned int size)
{
        if (!file || tag.size == 0)
                return 0;
        
        if (size < getDataSize())
                return 0;

        if (tag.flags & FlagCompressed)
        {
                unsigned char *compressed = new unsigned char[tag.size];
                unsigned long len = tag.uncompressedSize;
                fread(compressed, tag.size, 1, file);
                
                bool success = (uncompress((Bytef*)data, &len, compressed, tag.size) == Z_OK);
                
                delete[] compressed;
                return len;
        }
        
        return fread(data, tag.size, 1, file);
}


WriteTagFile::WriteTagFile(const char *name)
{
        file = fopen(name, "wb");
        if (file)
        {
                fwrite(MAGIC, 4, 1, file);
        }
}

WriteTagFile::~WriteTagFile()
{
        if (file)
        {
                fclose(file);
                file = 0;
        }
}

unsigned int WriteTagFile::getHeader(int id, unsigned int dataSize, int flags)
{
        return (flags << 28) + (id << 24) + dataSize;
}
/*
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
                
*/

bool WriteTagFile::writeTag(int id, const unsigned char *data, unsigned int dataSize, int flags)
{
        if (!file)
            return false;

        unsigned long origSize = dataSize, compressedSize = dataSize*2;
        unsigned char *compressed = new unsigned char[dataSize*2];
        unsigned int header;

        if (dataSize > 16)
            compress((Bytef*)compressed, &compressedSize, (const Bytef*)data, dataSize);
        else
            compressedSize = origSize + 1;
        
        if (compressedSize < origSize)
        {
                flags |= TagFile::FlagCompressed;
        }
        
        if (flags & TagFile::FlagCompressed)
        {
                header = getHeader(id, compressedSize, flags);
                fwrite(&header, sizeof(header), 1, file);
                fwrite(&origSize, sizeof(origSize), 1, file);
                fwrite(compressed, compressedSize, 1, file);
        } else
        {
                header = getHeader(id, dataSize, flags);
                fwrite(&header, sizeof(header), 1, file);
                fwrite(data, dataSize, 1, file);
        }
        
        delete[] compressed;
        return true;
}


