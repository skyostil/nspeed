bin_PROGRAMS = fixedpoint3d
fixedpoint3d_SOURCES = TestGame.cpp FixedPointMath.cpp FixedPointMatrix.cpp FixedPointVector.cpp Rasterizer.cpp View.cpp World.cpp Land.cpp Mixer.cpp ModPlayer.cpp Track.cpp Track.h Car.cpp BitmapFont.cpp

# set the include path found by configure
INCLUDES = $(all_includes) -I../../unix

# the library search path.
fixedpoint3d_LDFLAGS = $(all_libraries) $(LIBSDL_RPATH)
fixedpoint3d_CXXFLAGS = $(LIBSDL_CFLAGS)
fixedpoint3d_LDADD = $(top_builddir)/src/framework/sdl/libsdlframework.a $(top_builddir)/src/engine/libengine.a $(LIBSDL_LIBS) -lSDL_image
SUBDIRS = engine framework 3ds2h
noinst_HEADERS = World.h FixedPointMath.h FixedPointMatrix.h FixedPointVector.h Rasterizer.h Land.h Mixer.h ModPlayer.h Car.h Car.cpp.h BitmapFont.h
