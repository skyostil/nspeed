#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <stdlib.h>
#include <string.h>
#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif
#include "TagFile.h"
#include "Config.h"

/*!
\example 3ds2h.c
 
Compile with gcc 3ds2h.c -l3ds -o 3ds2h
 
Converts meshes of a <i>3DS</i> file into a C++ header file.
 
*/


static void
help()
{
  fprintf(stderr,
          "Syntax: 3ds2mesh [options] filename [options]\n"
          "\n"
          "Options:\n"
          "  -h/--help              This help\n"
          "  -o/--output filename   Write output to file instead of stdout\n"
          "  -c/--classname name    Override generated class name\n"
          "  -m/--mesh name         Write only specific mesh\n"
          "\n"
         );
  exit(1);
}


static const char* filename=0;
static const char* output=0;
static const char* mesh=0;
static const char* classname=0;

static void
parse_args(int argc, char **argv)
{
  int i;

  for (i=1; i<argc; ++i)
  {
    if (argv[i][0]=='-')
    {
      if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0))
      {
        help();
      }
      else
        if ((strcmp(argv[i],"-o")==0) || (strcmp(argv[i],"--output")==0))
        {
          ++i;
          if (output || (i>=argc))
          {
            help();
          }
          output=argv[i];
        }
        else
          if ((strcmp(argv[i],"-m")==0) || (strcmp(argv[i],"--mesh")==0))
          {
            ++i;
            if (mesh || (i>=argc))
            {
              help();
            }
            mesh=argv[i];
          }
          else
            if ((strcmp(argv[i],"-c")==0) || (strcmp(argv[i],"--class")==0))
            {
              ++i;
              if (classname || (i>=argc))
              {
                help();
              }
              classname=argv[i];
            }
            else
            {
              help();
            }
    }
    else
    {
      if (filename)
      {
        help();
      }
      filename=argv[i];
    }
  }
  if (!filename)
  {
    help();
  }
}

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
typedef struct
{
  signed long x, y, z;
  signed long u, v;
} PACKED SerializedVertex;

typedef struct
{
  signed short a, b, c;
} PACKED SerializedTriangle;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

static void
dump_m_file(Lib3dsFile *f, const char *o)
{
  Lib3dsMesh *m;
  int points=0;
  int faces=0;
  unsigned i;
  Lib3dsVector pos;
  WriteTagFile output(o);

  for (m=f->meshes; m; m=m->next)
  {
    if (mesh)
    {
      if (strcmp(mesh, m->name)!=0)
      {
        continue;
      }
    }

    if (!classname)
      classname = m->name;
	  
	SerializedVertex *vertex = new SerializedVertex[m->points];
//	const float scale = 1.0/256.0;
	const float scale = 1.0;
	
	printf("%d vertices, %d faces.\n", m->points, m->faces);
	
    for (i=0; i<m->points; ++i)
    {
		lib3ds_vector_transform(pos, m->matrix, m->pointL[i].pos);
		
		vertex[i].x = (int)(pos[0]*65536.0*scale);
		vertex[i].y = (int)(pos[1]*65536.0*scale);
		vertex[i].z = (int)(pos[2]*65536.0*scale);
		vertex[i].u = 
		vertex[i].v = 0;
	  
		if (m->texelL)
		{
			vertex[i].u = (int)((1.0-m->texelL[i][0])*256.0*65536.0);
			vertex[i].v = (int)((1.0-m->texelL[i][1])*256.0*65536.0);
		}
    }
	
	output.writeTag(0, (const char*)vertex, sizeof(SerializedVertex)*m->points);
	delete[] vertex;
	
	SerializedTriangle *face = new SerializedTriangle[m->faces];

    for (i=0; i<m->faces; ++i)
    {
		face[i].a = points+m->faceL[i].points[0];
		face[i].b = points+m->faceL[i].points[1];
		face[i].c = points+m->faceL[i].points[2];
    }
	
	output.writeTag(1, (const char*)face, sizeof(SerializedTriangle)*m->faces);
	delete[] face;
	
    points+=m->points;
    faces+=m->faces;
  }
}


int
main(int argc, char **argv)
{
  Lib3dsFile *f=0;

  parse_args(argc, argv);
  f=lib3ds_file_load(filename);
  if (!f)
  {
    fprintf(stderr, "***ERROR***\nLoading file %s failed\n", filename);
    exit(1);
  }
  if (output)
  {
    dump_m_file(f,output);
  }
  else
  {
    fprintf(stderr, "***ERROR***\nNo output file specified.\n");
    exit(1);
  }

  lib3ds_file_free(f);
  return(0);
}







