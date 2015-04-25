#ifndef IO_STUFF_H_CPP_SUCKS
#define IO_STUFF_H_CPP_SUCKS
#include <string>
#include <fstream>
#include <vector>

using std::string;
using std::ifstream;
using std::getline;
using std::vector;

typedef struct Sequence {
	string title;
	string content;
} Sequence;

int readInput(string finame, vector<Sequence> &out);
#endif
