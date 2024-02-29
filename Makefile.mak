libs = -lglfw -lpng -lwebp -lfreetype
includes = -I/usr/include/freetype2 -Isrc
export args = $(libs) $(includes)
export obj = ./obj

all:
	+$(MAKE) -C src -f zSRC.mak