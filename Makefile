INC_DIR:= ./include
SRC_DIR:= ./src/*.cc
DES_DIR:= ./bin
SRCS:=$(wildcard *.cc $(SRC_DIR))
OBJS:=$(patsubst %.cc, %.o, $(SRCS))
LIBS:= -llog4cpp -lpthread -ljson

CXX:=g++

CXXFLAGS:= -w -g $(LIBS)

EXE:=./bin/server

$(EXE):$(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS)
clean:
	rm $(OBJS)
	rm $(EXE)
