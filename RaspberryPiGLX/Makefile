CC=g++
BUILD_PATH=../build
SOURCE_PATH=../src
SOURCE_FILES=$(SOURCE_PATH)/MeshEngine.cpp $(SOURCE_PATH)/RaspberryPi/raspberrypi_platform_layer_glx.cpp
INCLUDE_DIRS=-I./$(SOURCE_PATH)/
DEFINED_MACROS=-DRASPBERRY_PI -DGLX

CFLAGS=$(INCLUDE_DIRS) $(DEFINED_MACROS) 
CFLAGS_DEBUG=$(CFLAGS) -DDEBUG
CFLAGS_RELEASE=$(CFLAGS) -DRELEASE -O3

LFLAGS=-lGL -lGLEW -lX11 -lXext


MeshPi_RELEASE_GLX: $(BUILD_PATH) MeshPi_DEBUG_GLX 
	@echo building $@
	$(CC) $(CFLAGS_RELEASE) $(SOURCE_FILES) -o $(BUILD_PATH)/$@ $(LFLAGS)
	@echo $@ build complete!
	
MeshPi_DEBUG_GLX: $(BUILD_PATH)
	@echo building $@
	$(CC) $(CFLAGS_DEBUG) -g $(SOURCE_FILES) -o  $(BUILD_PATH)/$@ $(LFLAGS)
	@echo $@ build complete!

$(BUILD_PATH):
	mkdir -p $@