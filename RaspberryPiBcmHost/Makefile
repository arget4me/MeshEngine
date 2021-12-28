CC=g++
BUILD_PATH=../build
SOURCE_PATH=../src
SOURCE_FILES=$(SOURCE_PATH)/MeshEngine.cpp $(SOURCE_PATH)/RaspberryPi/raspberrypi_platform_layer.cpp
INCLUDE_DIRS=-I./$(SOURCE_PATH)/ -I/opt/vc/include/ -I./libs/ilclient -I./libs/vgfont -I./libs/revision
DEFINED_MACROS=-DRASPBERRY_PI -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

CFLAGS=$(INCLUDE_DIRS) $(DEFINED_MACROS) 
CFLAGS_DEBUG=$(CFLAGS) -DDEBUG
CFLAGS_RELEASE=$(CFLAGS) -DRELEASE


LFLAGS=-L/opt/vc/lib/  -lbrcmGLESv2 -lbrcmEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm

MeshPi_RELEASE_MAKE: $(BUILD_PATH) MeshPi_DEBUG_MAKE 
	@echo building $@
	g++ $(CFLAGS_DEBUG) $(SOURCE_FILES) -O3 -o $(BUILD_PATH)/$@ $(LFLAGS)
	@echo $@ build complete!
	
MeshPi_DEBUG_MAKE: $(BUILD_PATH)
	@echo building $@
	g++ $(CFLAGS_DEBUG) -g $(SOURCE_FILES) -o $(BUILD_PATH)/$@ $(LFLAGS)
	@echo $@ build complete!

$(BUILD_PATH):
	mkdir -p $@