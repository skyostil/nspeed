# GL.py

import os
if os.environ.has_key("DICE3DS_DEBUG_OPENGL"):
	#
	# Note: gltrace is a python module I wrote to help me debug
	# OpenGL programs.  It serves as a drop in replacement of
	# OpenGL.GL.  Just import gltrace instead of OpenGL.GL and
	# BAM! all OpenGL calls are output to stderr.  It does NOT,
	# however, come with Dice3DS.
	#
	# If you want it, you can download it from
	# http://www.aerojockey.com/software/misc/gltrace.py
	#
	from gltrace import *

else:
	from OpenGL.GL import *
