#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <stdlib.h>
#include <string.h>
#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

/*!
\example 3ds2h.c

Compile with gcc 3ds2h.c -l3ds -o 3ds2h

Converts meshes of a <i>3DS</i> file into a C++ header file.

*/


static void
help()
{
  fprintf(stderr,
"Syntax: 3ds2h [options] filename [options]\n"
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
  
  for (i=1; i<argc; ++i) {
    if (argv[i][0]=='-') {
      if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0)) {
        help();
      }
      else
      if ((strcmp(argv[i],"-o")==0) || (strcmp(argv[i],"--output")==0)) {
        ++i;
        if (output || (i>=argc)) {
          help();
        }
        output=argv[i];
      }
      else
      if ((strcmp(argv[i],"-m")==0) || (strcmp(argv[i],"--mesh")==0)) {
        ++i;
        if (mesh || (i>=argc)) {
          help();
        }
        mesh=argv[i];
      }
      else
      if ((strcmp(argv[i],"-c")==0) || (strcmp(argv[i],"--class")==0)) {
        ++i;
        if (classname || (i>=argc)) {
          help();
        }
        classname=argv[i];
      }
      else {
        help();
      }
    }
    else {
      if (filename) {
        help();
      }
      filename=argv[i];
    }
  }
  if (!filename) {
    help();
  }
}


static void
dump_m_file(Lib3dsFile *f, FILE *o)
{
  Lib3dsMesh *m;
  int points=0;
  int faces=0;
  unsigned i;
  Lib3dsVector pos;

  for (m=f->meshes; m; m=m->next) {
    if (mesh) {
      if (strcmp(mesh, m->name)!=0) {
        continue;
      }
    }
    
    if (!classname)
    	classname = m->name;
    
    fprintf(o, "// Generated with 3ds2h.\n");
    fprintf(o, "#ifndef %s_H\n", classname);
    fprintf(o, "#define %s_H\n", classname);
    fprintf(o, "#include \"engine/Engine.h\"\n");
    fprintf(o, "#include \"World.h\"\n");
    fprintf(o, "#include \"FixedPointMath.h\"\n");
    fprintf(o, "\n");
    fprintf(o, "class %s: public Object\n", classname);
    fprintf(o, "{\n");
    fprintf(o, "public:\n");
    fprintf(o, "	%s(scalar s = FP_ONE, Game::Surface *t = 0): Object(%d, %d)\n", classname, m->points, m->faces);
    fprintf(o, "	{\n");
    fprintf(o, "		Vector v;\n");
    fprintf(o, "		beginMesh();\n");
    
    for (i=0; i<m->points; ++i) {
      lib3ds_vector_transform(pos, m->matrix, m->pointL[i].pos);
      if (m->texelL) {
        fprintf(o, "		setTexCoord(%d, %d);\n",
          (int)((1.0-m->texelL[i][0])*256.0*65536.0),
          (int)((1.0-m->texelL[i][1])*256.0*65536.0));
      }
      
      fprintf(o, "		v.set(FPMul(%d,s), FPMul(%d,s), FPMul(%d,s)); addVertex(v);\n",
        (int)(pos[0]*65536.0), (int)(pos[1]*65536.0), (int)(pos[2]*65536.0));
/*        (int)(m->pointL[i].pos[0]*65536.0), (int)(m->pointL[i].pos[1]*65536.0), (int)(m->pointL[i].pos[2]*65536.0)); */
    }

    for (i=0; i<m->faces; ++i) {
      fprintf(o, "		beginFace(3);\n");
      fprintf(o, "		setTexture(t);\n");
      fprintf(o, "		addFaceVertex(%d);\n", points+m->faceL[i].points[0]);
      fprintf(o, "		addFaceVertex(%d);\n", points+m->faceL[i].points[1]);
      fprintf(o, "		addFaceVertex(%d);\n", points+m->faceL[i].points[2]);
      fprintf(o, "		endFace();\n");
    }

    fprintf(o, "	}\n");
    fprintf(o, "};\n");
    fprintf(o, "#endif\n");
        
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
  if (!f) {
    fprintf(stderr, "***ERROR***\nLoading file %s failed\n", filename);
    exit(1);
  }
  if (output) {
    FILE *o=fopen(output, "w+");
    if (!o) {
      fprintf(stderr, "***ERROR***\nCan't open %s for writing\n", output);
      exit(1);
    }
    dump_m_file(f,o);
    fclose(o);
  }
  else {
    dump_m_file(f,stdout);
  }

  lib3ds_file_free(f);
  return(0);
}







