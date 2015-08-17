CC := $(CC)
CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)

CURL_CFLAGS := $(shell pkg-config --cflags libcurl)
PNG_CFLAGS := $(shell pkg-config --cflags libpng)
CURL_LIBS := $(shell pkg-config --libs libcurl)
PNG_LIBS := $(shell pkg-config --libs libpng)
JPEG_CFLAGS = -I/usr/local/Cellar/jpeg/8d/include/
JPEG_LIBS = -L/usr/local/Cellar/jpeg/8d/lib/

simulate: simulate.c
	$(CC) -g -Wall -O3 $(CFLAGS) $(LDFLAGS) -o simulate simulate.c $(CURL_CFLAGS) $(PNG_CFLAGS) $(JPEG_CFLAGS) $(CURL_LIBS) $(PNG_LIBS) $(JPEG_LIBS) -ljpeg -lm

deut: deut.c
	$(CC) -g -Wall -O3 $(CFLAGS) $(LDFLAGS) -o deut deut.c $(CURL_CFLAGS) $(PNG_CFLAGS) $(JPEG_CFLAGS) $(CURL_LIBS) $(PNG_LIBS) $(JPEG_LIBS) -ljpeg -lm

clean:
	rm -f stitch
