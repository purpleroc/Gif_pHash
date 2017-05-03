#############################################################
#
#        Makefile for PDS project at BUT FIT 2014
#                   Fridolin Pokorny
#                fridex.devel@gmail.com
#
#############################################################

CXX=g++
#LDFLAGS=-lm `pkg-config opencv --cflags --libs opencv` -g
LDFLAGS=-lfreeimage -g -lm
CXXFLAGS=-DNDEBUG -I /usr/local/include/opencv -I /usr/lib -L /usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_gpu -lopencv_ts -lopencv_video -lopencv_objdetect -lopencv_ml 

SRCS=main.cpp gif2ipl.cpp phash.cpp
HDRS=gif2ipl.h common.h phash.h
AUX=Makefile

PACKNAME=getphash.zip

all: clean getphash

.PHONY: clean pack

getphash: ${SRCS}
	$(CXX) $(LDFLAGS) $^ $(CXXFLAGS) -o $@

pack:
	#make -C DOC/
	#mv DOC/Documentation.pdf .
	zip -R $(PACKNAME) $(SRCS) $(HDRS) ./$(AUX) Documentation.pdf

clean:
	@rm -f *.o getphash $(PACKNAME) Documentation.pdf

