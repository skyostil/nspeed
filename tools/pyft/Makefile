all: freetypec.so

freetypec.so: freetype_wrap.c
	gcc --shared freetype_wrap.c  -I/usr/include/python2.3 -I/usr/include/freetype -lttf -lpython2.3 -o freetypec.so

clean:
	rm freetypec.so

