# Configure installation prefix and application name
AM_CPPFLAGS = -DPREFIX=\"$(prefix)\" -DAPPNAME=\"@PACKAGE@\" @LIBS_CFLAGS@ @CPPFLAGS@ -DHAVE_LIBXSP -DHAVE_LIBOSSO
AM_LDFLAGS = @LIBS_LIBS@ @LDFLAGS@

INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO
noinst_LIBRARIES =  libsdlframework.a
noinst_HEADERS = SDLFramework.h
libsdlframework_a_SOURCES = SDLFramework.cpp
libsdlframework_a_CXXFLAGS = -I../../../unix
