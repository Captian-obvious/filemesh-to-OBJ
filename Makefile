# fmtoobj: FileMesh to OBJ converter
CXX = g++
CXXFLAGS = -Os
TARGET = fmtoobj
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
check:
	ls | grep fmtoobj
distcheck:
	./fmtoobj ./test/testv2.mesh
clean:
	rm -f $(TARGET)
