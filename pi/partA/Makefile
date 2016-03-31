CXX = g++
CXXFLAGS = -g -Wall -MMD -std=c++0x
OBJECTS = main.o Pizza.o CheesePizza.o Decorator.o ExtraCheeseDecorator.o PepperoniDecorator.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = exec


${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

clean:
	rm -rf ${DEPENDS} ${OBJECTS} ${EXEC}

main.o: main.cpp Pizza.h Pizza.cpp CheesePizza.h
Pizza.o: Pizza.h Pizza.cpp
CheesePizza.o: Pizza.h CheesePizza.h CheesePizza.cpp
ExtraCheeseDecorator.o: Pizza.h ExtraCheeseDecorator.h ExtraCheeseDecorator.cpp
Decorator.o: Decorator.h Decorator.cpp Pizza.h
PepperoniDecorator.o: Pizza.h PepperoniDecorator.h PepperoniDecorator.cpp

-include ${DEPENDS}