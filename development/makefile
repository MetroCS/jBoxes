OFILES = Lesson.o \
		MessageQueue.o \
		box.o \
		boxes.o \
		chargridtype.o \
		jbtoken.o \
		mystring.o \
		noteinfo.o \
		portstype.o \
		translator.o 

jboxes : $(OFILES)
	gcc -o jboxes $(OFILES) -Wall -framework GLUT -framework OpenGL -lobjc -lstdc++

#	gcc -o jboxes $(OFILES) -framework GLUT -framework OpenGL -L/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries -lGLU -lGL -lm -lglut 

MessageQueue.o:  MessageQueue.cpp
	gcc -c MessageQueue.cpp

Lesson.o: Lesson.cpp
	gcc -c Lesson.cpp

box.o: box.cpp
	gcc -c box.cpp

boxes.o: boxes.cpp
	gcc -c boxes.cpp

chargridtype.o: chargridtype.cpp
	gcc -c chargridtype.cpp

jbtoken.o: jbtoken.cpp
	gcc -c jbtoken.cpp

mystring.o: mystring.cpp
	gcc -c mystring.cpp

noteinfo.o: noteinfo.cpp
	gcc -c noteinfo.cpp

portstype.o:  portstype.cpp
	gcc -c portstype.cpp

translator.o: translator.cpp
	gcc -c translator.cpp

.PHONY: clean

binaries=jboxes

clean:
	rm -f *.o $(binaries)
