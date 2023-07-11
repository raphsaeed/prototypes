
g++ -o test test.cpp $(pkg-config --cflags --libs gstreamer-1.0)

gcc -o your_program your_program.c $(pkg-config --cflags --libs gstreamer-1.0)
g++ -o test test.cpp $(pkg-config --cflags --libs gstreamer-1.0)
