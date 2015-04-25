#include "ReadMapper.h"
#include "io.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

int main (int argc, char *argv[]) {
	if (argc < 2) {
		printf("Fuck off and give me some more arguments, kay?\n");
		return 1;
	}

	vector<Sequence> sequences;
	readInput(argv[1], sequences);

	for (Sequence s : sequences) {
		cout << s.content << endl;
	}
	return 0;
}
