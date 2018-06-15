#CPPFLAGS+=-O3 -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi
CPPFLAGS+=-DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

LDFLAGS+=-L/opt/vc/lib/ -lbrcmEGL -lbrcmGLESv2 -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt

INCLUDES+=-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

all: rasperf3d.exe librasperf3d.so

%.o: %.c
	@rm -f $@ 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

%.o: %.cpp
	@rm -f $@ 
	$(CXX) $(CPPFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

rasperf3d.exe: main.o font.o renderStats.o shaderLoader.o util/bitmap.o util/ktx.o util/native.o
	$(CXX) main.o font.o renderStats.o shaderLoader.o util/bitmap.o util/ktx.o util/native.o -o $@  $(LDFLAGS) 

librasperf3d.so: cinterface.o main.o font.o renderStats.o shaderLoader.o util/bitmap.o util/ktx.o util/native.o
	$(CXX) -shared -fPIC -o librasperf3d cinterface.o main.o font.o renderStats.o shaderLoader.o util/bitmap.o util/ktx.o util/native.o -o $@  $(LDFLAGS) 

clean:
	for i in main.o font.o renderStats.o shaderLoader.o util/bitmap.o util/ktx.o util/native.o; do (if test -e "$$i"; then ( rm $$i ); fi ); done

