#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>
#include "io.h"

using std::ifstream;

inline bool
isDNA(
	char c
	 )
{
	return c == 'A' || c == 'C' || c == 'G' || c == 'T';
}

bool
readSequences(
	std::ifstream &in,
	char **s1,
	char **s2,
	char **s1_name,
	char **s2_name
	)
{
	char buf[MB], name[MB/2], c = '\0';
	int i = 0;
	memset(buf, 0, MB);

	// Skip over '>'
	while (c != '>') {
		in >> c;
	}

	in.getline(buf, MB);
	// Get unique identifier of sequence, up to first whitespace
	sscanf(buf, "%s", name);
	// Allocate and copy
	asprintf(s1_name, "%s", name);
	memset(buf, 0, MB);
	i = 0;
	c = '\0';
	while (c != '>') {
		if (isDNA(c)) {
			buf[i++] = c;
		}
		in >> c;
	}
	asprintf(s1, "%s", buf);
	memset(buf, 0, MB);

	// Repeat for sequence 2, but > already consumed
	in.getline(buf, MB);
	sscanf(buf, "%s", name);
	asprintf(s2_name, "%s", name);
	memset(buf, 0, MB);
	i = 0;
	c = '\0';
	while (c != '>' && !in.eof()) {
		if (isDNA(c)) {
			buf[i++] = c;
		}
		in >> c;
	}
	asprintf(s2, "%s", buf);
	memset(buf, 0, MB);

	if (c == '>') {
		// Not done reading sequences, put > back for next time
		in.putback(c);
		return false;
	}
	else
		return true;
}

void
readConfig(
	ifstream &in,
	ScoreParams *params
	)
{
	char buf[25];

	in >> buf;
	in >> params->match;
	in >> buf;
	in >> params->mismatch;
	in >> buf;
	in >> params->h;
	in >> buf;
	in >> params->g;
}

void
outputResult(
	Alignment *align
	)
{
	int len, i = align->mini-1, j = align->minj-1, k, l;
	len = strlen(align->alignpath);

	for (l = 0; l < ceil(len/60.0); l++) {
		// Print s1
		printf("%-25s:  %5d  ", align->s1_name, i+1+align->offseti);
		for (k = 0 + l*60; k < 60+(l*60) && k < len; k++) {
			switch(align->alignpath[k]) {
			case 'S':
				putchar(align->s1[i++]);
				break;
			case 's':
				putchar(align->s1[i++]);
				break;
			case 'I':
				putchar('-');
				break;
			case 'D':
				putchar(align->s1[i++]);
				break;
			}
		}
		printf(" %d\n", i+align->offseti);

		// Print middle part showing "|" for matches only
		printf("                                   ");
		for (k = 0 + l*60; k < 60+(l*60) && k < len; k++) {
			switch(align->alignpath[k]) {
			case 'S':
				putchar('|');
				break;
			case 's':
				putchar(' ');
				break;
			case 'I':
				putchar(' ');
				break;
			case 'D':
				putchar(' ');
				break;
			}
		}
		putchar('\n');

		// Print s2
		printf("%-25s:  %5d  ", align->s2_name, j+1+align->offsetj);
		for (k = 0 + l*60; k < 60+(l*60) && k < len; k++) {
			switch(align->alignpath[k]) {
			case 'S':
				putchar(align->s2[j++]);
				break;
			case 's':
				putchar(align->s2[j++]);
				break;
			case 'I':
				putchar(align->s2[j++]);
				break;
			case 'D':
				putchar('-');
				break;
			}
		}
		printf(" %d\n", j+align->offsetj);
		putchar('\n');
	}
}

void
outputGlobalResult(
	Alignment *align
	)
{
	int len;
	len = strlen(align->alignpath);

	printf("Scores:  match = %d, mismatch = %d, h = %d, g = %d\n\n",
		align->params.match, align->params.mismatch, align->params.h,
		align->params.g);
	printf("Sequence 1=\"%s\", length = %d characters\n", align->s1_name,
		align->m);
	printf("Sequence 2=\"%s\", length = %d characters\n\n", align->s2_name,
		align->n);

	outputResult(align);

	printf("Report: \n\n");
	printf("%-40s%d\n", "Global optimal score = ", align->score);
	printf("%-40s%d\n", "Score according to numbers generated = ",
		align->nmatch*align->params.match +
		align->nmismatch*align->params.mismatch +
		align->ngap*align->params.g +
		align->nopengaps*align->params.h);
	printf("Number of matches = %d, mismatches = %d, gaps = %d, opening gaps = %d\n",
		align->nmatch, align->nmismatch, align->ngap, align->nopengaps);
	printf("Identities = %d/%d (%d%%), Gaps = %d/%d (%d%%)\n", align->nmatch,
		len, ((int) ((float) 100*align->nmatch/len)), align->ngap, len,
		((int) ((float) 100*align->ngap/len)));
}

void
outputLocalResult(
	Alignment *align
	)
{
	int len;
	len = strlen(align->alignpath);

	printf("Scores:  match = %d, mismatch = %d, h = %d, g = %d\n",
		align->params.match, align->params.mismatch, align->params.h,
		align->params.g);
	printf("Sequence 1=\"%s\", length = %d characters\n", align->s1_name,
		align->m);
	printf("Sequence 2=\"%s\", length = %d characters\n\n", align->s2_name,
		align->n);

	outputResult(align);

	printf("Report: \n");
	printf("%-40s%d\n", "Local alignment score = ", align->score);
	printf("%-40s%d\n", "Score according to numbers generated = ",
		align->nmatch*align->params.match +
		align->nmismatch*align->params.mismatch +
		align->ngap*align->params.g +
		align->nopengaps*align->params.h);
	printf("Local alignment score = %d at cell [%d,%d]\n", align->score,
		align->maxi+align->offseti, align->maxj+align->offsetj);
	printf("Number of matches = %d, mismatches = %d, gaps = %d, opening gaps = %d\n",
		align->nmatch, align->nmismatch, align->ngap, align->nopengaps);
	printf("Identities = %d/%d (%d%%), Gaps = %d/%d (%d%%)\n", align->nmatch,
		len, ((int) ((float) 100*align->nmatch/len)), align->ngap, len,
		((int) ((float) 100*align->ngap/len)));
}

void
printGrid(
	Alignment *align
	)
{
	DPCell *cell = NULL;

	printf("Grid: \n");
	for (int i = 0; i < align->m + 1; i++) {
		for (int j = 0; j < align->n + 1; j++)
			printf("----------");
		printf("\n| ");
		for (int j = 0; j < align->n + 1; j++) {
			cell = getCell(&align->grid, i, j);
			if (cell->S < -1000)
				printf("S: -1000 |");
			else
				printf("S: %5d |", cell->S);
		}
		printf("\n| ");
		for (int j = 0; j < align->n + 1; j++) {
			cell = getCell(&align->grid, i, j);
			if (cell->I < -1000)
				printf("I: -1000 |");
			else
				printf("I: %5d |", cell->I);
		}
		printf("\n| ");
		for (int j = 0; j < align->n + 1; j++) {
			cell = getCell(&align->grid, i, j);
			if (cell->D < -1000)
				printf("D: -1000 |");
			else
				printf("D: %5d |", cell->D);
		}
		printf("\n| ");
		for (int j = 0; j < align->n + 1; j++) {
			cell = getCell(&align->grid, i, j);
			printf("prev:  %c |", cell->prev);
		}
		printf("\n");
	}
	for (int i = 0; i < align->n + 1; i++)
		printf("----------");
	printf("\n\n");
	printf("m: %d n: %d\n", align->m, align->n);
	printf("Alignment path: %s\n", align->alignpath);
	printf("%-40s%d\n", "Local alignment score = ", align->score);
	printf("%-40s%d\n", "Score according to numbers generated = ",
		align->nmatch*align->params.match +
		align->nmismatch*align->params.mismatch +
		align->ngap*align->params.g +
		align->nopengaps*align->params.h);
	printf("Number of matches = %d, mismatches = %d, gaps = %d, opening gaps = %d\n",
		align->nmatch, align->nmismatch, align->ngap, align->nopengaps);
	printf("Scores:  match = %d, mismatch = %d, h = %d, g = %d\n",
		align->params.match, align->params.mismatch, align->params.h,
		align->params.g);


}
