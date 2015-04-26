#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <fstream>
#include "dp.h"


// f is already expected to be open.
bool
readSequences(
	std::ifstream &in,
	char **s1,
	char **s2,
	char **s1_name,
	char **s2_name
	);

void
readConfig(
	std::ifstream &in,
	ScoreParams *params
	);

void
outputResult(
	Alignment *align
	);

void
outputGlobalResult(
	Alignment *align
	);

void
outputLocalResult(
	Alignment *align
	);

void
printGrid(
	Alignment *align
	);

#endif
