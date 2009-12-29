SUBDIRS = engine framework
AM_CPPFLAGS = -DVERSION=\"@VERSION@\" @CPPFLAGS@ -DMAEMO_VERSION=5

bindir = $(prefix)/games
bin_PROGRAMS = nspeed
nspeed_SOURCES = FixedPointMath.cpp FixedPointMatrix.cpp FixedPointVector.cpp Rasterizer.cpp View.cpp World.cpp Land.cpp Mixer.cpp ModPlayer.cpp Track.cpp Track.h Car.cpp BitmapFont.cpp GameEngine.cpp Environment.cpp Environment.h GameEngine.h Set.h Mesh.cpp Mesh.h TagFile.cpp Object.cpp MeshSet.cpp Texture.cpp Menu.cpp

# the library search path.
nspeed_LDFLAGS = $(all_libraries) @LIBS_LIBS@
nspeed_CXXFLAGS = $(SDL_CFLAGS) @LIBS_CFLAGS@
nspeed_LDADD = $(top_builddir)/src/framework/sdl/libsdlframework.a $(top_builddir)/src/engine/libengine.a
noinst_HEADERS = World.h FixedPointMath.h FixedPointMatrix.h FixedPointVector.h Rasterizer.h Land.h Mixer.h ModPlayer.h Car.h BitmapFont.h TagFile.h Object.h MeshSet.h Renderable.h Texture.h Menu.h Config.h View.h
