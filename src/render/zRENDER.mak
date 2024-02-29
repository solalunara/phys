include ../entities/zENTITIES.mak
include ../../glad/glad.mak

GlobalTexture.o : GlobalTexture.h GlobalTexture.cpp window.o texture.o
		g++ $(args) -o $(obj)/GlobalTexture.o GlobalTexture.cpp window.o texture.o

mesh.o : mesh.h mesh.cpp transform.o element.o shader.o GlobalTexture.o cube.o glad.o
		g++ $(args) -o $(obj)/mesh.o mesh.cpp transform.o element.o shader.o GlobalTexture.o cube.o glad.o

shader.o : shader.h shader.cpp glad.o
		g++ $(args) -o $(obj)/shader.o shader.cpp glad.o
	
texture.o : texture.h texture.cpp glad.o window.o
		g++ $(args) -o $(obj)/texture.o texture.cpp glad.o window.o
	
