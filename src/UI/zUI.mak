include ../entities/zENTITIES.mak
include ../render/zRENDER.mak

font.o : font.h font.cpp GlobalTexture.o
	g++ $(args) -o $(obj)/font.o font.cpp GlobalTexture.o

panel.o : panel.h panel.cpp element.o text.o
	g++ $(args) -o $(obj)/panel.o panel.cpp element.o text.o
