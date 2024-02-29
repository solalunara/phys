collide.o: collide.h collide.cpp element.o physics.o mesh.o
	g++ -c -o $(obj)/collide.o collide.cpp element.o physics.o mesh.o

physics.o: physics.h physics.cpp element.o function.o collide.o
	g++ -c -o $(obj)/physics.o physics.cpp element.o function.o collide.o