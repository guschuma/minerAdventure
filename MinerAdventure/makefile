COMPILER = clang
SOURCE_LIBS = -Ilib/ -Iinclude
OSX_OPT = -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OUTPUT = bin/build_osx
CFILES = src/*.c

build_osx:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) -o $(OUTPUT) $(OSX_OPT)

run: build_osx
	./$(OUTPUT)