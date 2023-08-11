all:
    mkdir -p build
    cd build && cmake ..
    cd build && make

clean:
    rm -rf build

.PHONY: all clean