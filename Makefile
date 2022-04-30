# opencv lib
build: main.cpp ./compound_eyes/compound_eyes.cpp ./tcp_server/tcp_server.cpp ./ommatidia_server/ommatidia_server.cpp ./JPEG/jpeg_data.cpp
	g++ $^ -g `pkg-config --cflags --libs opencv` -pthread -o build/main

run: build/main
	./build/main

debug: build/main
	gdb ./build/main

clean:
	rm -f build/main photo/*.jpg