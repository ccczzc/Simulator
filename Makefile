all:
	cmake -B ./build -S . && cd build && make
	# ./build/Simulator ./tests/CODE1.S