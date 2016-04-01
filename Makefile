OUTPUT_NAME = pipair

SRC_DIR = ./pi/partA
INCLUDES = -I${SRC_DIR}

CXX = g++
CXXFLAGS = -g -std=c++98 -Wall -MMD
OBJECTS = ${SRC_DIR}/pipair.o
DEPENDS = ${OBJECTS:.o=.d}

${OUTPUT_NAME}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${OUTPUT_NAME}

clean:
	rm -rf ${DEPENDS} ${OBJECTS} ${OUTPUT_NAME}

-include ${DEPENDS}
