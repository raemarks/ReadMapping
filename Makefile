
INCLUDES=-Isrc/SuffixTree/src -Isrc/Alignment -Isrc/yardstick/src

STDIR=src/SuffixTree/src
CFLAGS=-march=native -O3
DEPS=obj/suffix.o obj/alignment.o obj/yardstick.o
all: $(DEPS)
	g++ -std=c++11 -lrt $(CFLAGS) $(INCLUDES) $(DEPS) src/io.cpp src/main.cpp src/ReadMapper.cpp

obj/suffix.o: $(STDIR)/SuffixTree.cpp
	cd src/SuffixTree && make
	cp src/SuffixTree/suffix.o obj/suffix.o

obj/alignment.o: src/Alignment/dp.cpp
	cd src/Alignment && make
	cp src/Alignment/alignment.o obj/alignment.o

obj/yardstick.o:
	cd src/yardstick && make
	cp src/yardstick/yardstick.o obj/yardstick.o

clean:
	rm -rf obj/*
