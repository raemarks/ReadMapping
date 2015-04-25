
INCLUDES=-Isrc/SuffixTree/src

DEPS=obj/suffix.o
all: $(DEPS)
	g++ -std=c++11 $(INCLUDES) src/io.cpp src/main.cpp src/ReadMapper.cpp $(DEPS)

obj/suffix.o:
	cd src/SuffixTree && make
	cp src/SuffixTree/suffix.o obj/suffix.o
