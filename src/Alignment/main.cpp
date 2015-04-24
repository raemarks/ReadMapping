#include "io.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

using std::ifstream;

int main(int argc, char *argv[]) {
	char *s1, *s2, *s1_name, *s2_name;

	if (argc < 3) {
		printf("Error, not enough arguments.\n");
		printf("Usage: '%s <input file containing both s1 and s2> <0: global, "
			"1: local> <optional: path to parameters config file>'\n", argv[0]);
		return 1;
	}

	ifstream in(argv[1], ifstream::in);
	if (!in.good()) {
		printf("Error: file '%s' not valid.\n", argv[1]);
		return 2;
	}
	readSequences(in, &s1, &s2, &s1_name, &s2_name);

	ifstream *inc;
	if (argc == 4) {
		inc = new ifstream(argv[3], ifstream::in);
		if (!inc->good()) {
			printf("Error: file '%s' not valid.\n", argv[3]);
			return 3;
		}
	}
	else {
		inc = new ifstream("parameters.config", ifstream::in);
	}
	ScoreParams params;
	readConfig(*inc, &params);
	inc->close();
	delete inc;

	int global = atoi(argv[2]);
	if (global == 0)
		calculateGlobalAlignment(s1, s2, s1_name, s2_name, &params);
	else if (global == 1)
		calculateLocalAlignments(s1, s2, s1_name, s2_name, &params, 4);
	else {
		printf("Error: input 0 for global alignment and 1 for local alignment\n");
		return 4;
	}

	free(s1);
	free(s2);
	free(s1_name);
	free(s2_name);

	return 0;
}
