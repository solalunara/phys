all :
	make glad.o

glad.o : glad.c
	cc -c -o $(obj)/glad.o glad.c