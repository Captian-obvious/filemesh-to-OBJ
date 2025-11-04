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
	echo "Testing reading..."
	./fmtoobj ./test/testv2.mesh --no-output
	./fmtoobj ./test/testv3.mesh --no-output
	./fmtoobj ./test/testv4.mesh --no-output
	./fmtoobj ./test/testv5.mesh --no-output
clean:
	rm -f $(TARGET)
