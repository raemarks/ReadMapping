
INCLUDES=-Isrc/SuffixTree/src -Isrc/Alignment

STDIR=src/SuffixTree/src
CFLAGS=-march=native -O3
DEPS=obj/suffix.o obj/alignment.o
all: $(DEPS)
	g++ -std=c++11 $(CFLAGS) $(INCLUDES) $(DEPS) src/io.cpp src/main.cpp src/ReadMapper.cpp

obj/suffix.o: $(STDIR)/SuffixTree.cpp
	cd src/SuffixTree && make
	cp src/SuffixTree/suffix.o obj/suffix.o

obj/alignment.o: src/Alignment/dp.cpp
	cd src/Alignment && make
	cp src/Alignment/alignment.o obj/alignment.o

clean:
	rm -rf obj/*
