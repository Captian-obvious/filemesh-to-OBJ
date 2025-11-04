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
	echo "Testing conversion..."
	./fmtoobj ./test/testv2.mesh -o ./test/testv2.obj
	./fmtoobj ./test/testv3.mesh -o ./test/testv3.obj
	./fmtoobj ./test/testv4.mesh -o ./test/testv4.obj
	./fmtoobj ./test/testv5.mesh -o ./test/testv5.obj
	ls ./test | grep .obj
	echo "All tests passed."
	echo "Moving to BUILD directory..."
	mkdir -p BUILD
	mv fmtoobj BUILD/
	mv ./test/*.obj BUILD/
	echo "Distribution check complete."
clean:
	rm -f $(TARGET)
