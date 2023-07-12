
g++ -o test test.cpp $(pkg-config --cflags --libs gstreamer-1.0)

gcc -o your_program your_program.c $(pkg-config --cflags --libs gstreamer-1.0)
g++ -o test test.cpp $(pkg-config --cflags --libs gstreamer-1.0)

Mac VS code
/usr/bin/clang++ -fcolor-diagnostics -fansi-escape-codes -std=c++17 -stdlib=libc++ -g -I/Library/Frameworks/Python.framework/Versions/3.11/include/python3.11 /Volumes/Projects/Projects/cpp/SymReg/symreg.cpp -o /Volumes/Projects/Projects/cpp/SymReg/symreg
