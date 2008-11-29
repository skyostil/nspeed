# Configure installation prefix and application name
AM_CPPFLAGS = -DPREFIX=\"$(prefix)\" -DAPPNAME=\"@PACKAGE@\" @CPPFLAGS@

INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO
noinst_LIBRARIES =  libsdlframework.a
libsdlframework_a_SOURCES = SDLFramework.cpp
libsdlframework_a_CXXFLAGS = -I../../../unix
