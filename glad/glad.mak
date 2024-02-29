all :
	make glad.o

glad.o : glad.c
	g++ -o $(obj)/glad.o glad.c