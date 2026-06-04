COMPILER = gcc
SOURCE_LIBS = -Ilib/ -Iinclude
CFILES = src/*.c

# macOS
OSX_OPT = -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OUTPUT = bin/build_osx

build_osx:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) -o $(OUTPUT) $(OSX_OPT)

run: build_osx
	./$(OUTPUT)

# Linux
build_linux:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) -o bin/build_linux \
	-lraylib -lGL -lm -lX11 -lpthread -ldl -lrt

run_linux: build_linux
	./bin/build_linux