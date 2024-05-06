all:
	mkdir -p build && cd build && cmake .. && make
	./build/Simulator ./tests/CODE1.S
	# ./build/Simulator ./tests/CODE1.S
	# ./build/Simulator ./tests/CODE2.S
	