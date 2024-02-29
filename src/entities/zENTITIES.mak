

cube.o: cube.cpp cube.h transform.o window.o element.o mesh.o physics.o function.o collide.o
	g++ -c -o $(obj)/cube.o cube.cpp transform.o window.o element.o mesh.o physics.o function.o collide.o

element.o: element.cpp element.h transform.o window.o element.o physics.o function.o collide.o
	g++ -c -o $(obj)/element.o element.cpp transform.o window.o element.o physics.o function.o collide.o

text.o: text.cpp text.h element.o mesh.o window.o font.o
	g++ -c -o $(obj)/text.o text.cpp element.o mesh.o window.o font.o


