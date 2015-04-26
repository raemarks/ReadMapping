#include "dp.h"
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include <list>

using std::list;

DPCell *
getCell(
	DPGrid *grid,
	int x,
	int y
	)
{
	// X*(# columns) + Y, since this is 1D array
	return &grid->cells[x*(grid->y) + y];
}

inline int
max3(
	int x,
	int y,
	int z
	)
{
	return ((x >= y && x >= z) ? x : (y > z ? y : z));
}

inline int
maxzero(
	int x
	)
{
	return ((x > 0) ? x : 0);
}


int
getSubstitutionScore(
	Alignment *align,
	int i,
	int j
	)
{
	int t, sub;
	DPCell *pcell = NULL;

	pcell = getCell(&align->grid, i, j);
	if (align->s1[i-1] == align->s2[j-1])
		pcell->match = true;
	else
		pcell->match = false;

	if (i == 0 && j == 0) {
		// Base case, "init" state where S(0,0) = 0
		sub = 0;
	}
	else if (i == 0 || j == 0) {
		// Base case, "init" state where S(i, 0) = S(0, j) = -infinity
		sub = NEG_INFINITY;
	}
	else {
		// Get max of three values in cell at i-1, j-1 (just its score)
		pcell = getCell(&align->grid, i-1, j-1);
		t = pcell->score;

		if (align->s1[i-1] == align->s2[j-1])
			sub = t + align->params.match;
		else
			sub = t + align->params.mismatch;
	}

	if (align->local == true)
		return maxzero(sub);
	else
		return sub;
}

int
getInsertionScore(
	Alignment *align,
	int i,
	int j
	)
{
	int ins;
	DPCell *pcell;

	if (i == 0 && j == 0) {
		// Base case, "init" state where I(0,0) = -infinity
		ins = NEG_INFINITY;
	}
	else if (j == 0) {
		// Base case, "init" state where I(i, 0) = -infinity
		ins =  NEG_INFINITY;
	}
	else if (i == 0) {
		// Base case, "init" state where I(0, j) = h + j*g
		ins = align->params.h + j*(align->params.g);
	}
	else {
		// Get max of three values in cell at i, j-1
		pcell = getCell(&align->grid, i, j-1);

		ins = max3(pcell->S + align->params.h + align->params.g, // S(i, j-1) + h + g
			pcell->D + align->params.h + align->params.g, // D(i, j-1) + h + g
			pcell->I + align->params.g); // I(i, j-1) + g
	}

	if (align->local == true)
		return maxzero(ins);
	else
		return ins;
}

int
getDeletionScore(
	Alignment *align,
	int i,
	int j
	)
{
	int del;
	DPCell *pcell;

	if (i == 0 && j == 0) {
		// Base case, "init" state where D(0,0) = -infinity
		del = NEG_INFINITY;
	}
	else if (j == 0) {
		// Base case, "init" state where D(i, 0) = h + i*g
		del = align->params.h + i*(align->params.g);
	}
	else if (i == 0) {
		// Base case, "init" state where D(0, j) = -infinity
		del = NEG_INFINITY;
	}
	else {
		// Get max of three values in cell at i-1, j
		pcell = getCell(&align->grid, i-1, j);

		del =max3(pcell->S + align->params.h + align->params.g, // S(i-1, j) + h + g
			pcell->D + align->params.g, // D(i-1, j) + g
			pcell->I + align->params.h + align->params.g); // I(i-1, j) + h + g
	}

	if (align->local == true)
		return maxzero(del);
	else
		return del;
}

void
deleteAlignment(
	Alignment *align
	)
{
	free(align->alignpath);
	free(align->grid.cells);
}

void
initializeGrid(
	Alignment *align
	)
{
	align->grid.x = align->m+1;
	align->grid.y = align->n+1;
	align->grid.cells =
	  (DPCell *) calloc((align->m + 1)*(align->n + 1), sizeof(DPCell));
	align->alignpath = (char *) calloc((align->m+1)*(align->n+1), sizeof(char));
}

Alignment *
calculateGlobalAlignment(
	const char *s1,
	int s1_len,
	char *s1_name,
	const char *s2,
	int s2_len,
	char *s2_name,
	ScoreParams *params
	)
{
	Alignment *align = NULL;

	align = (Alignment*) calloc(1, sizeof(Alignment));
	align->s1 = s1;
	align->s2 = s2;
	align->s1_name = s1_name;
	align->s2_name = s2_name;
	align->m = s1_len;
	align->n = s2_len;
	align->params = *params;
	align->local = false;

	initializeGrid(align);
	calculateAlignment(align);
	retrace(align, align->maxi, align->maxj);

	return align;
}

Alignment *
calculateLocalAlignment(
	const char *s1,
	int s1_len,
	char *s1_name,
	const char *s2,
	int s2_len,
	char *s2_name,
	ScoreParams *params
	)
{
	Alignment *align = (Alignment*) calloc(1, sizeof(Alignment));

	align->s1 = s1;
	align->s2 = s2;
	align->s1_name = s1_name;
	align->s2_name = s2_name;
	align->m = s1_len;
	align->n = s2_len;
	align->params = *params;
	align->local = true;

	initializeGrid(align);
	calculateAlignment(align);
	retrace(align, align->maxi, align->maxj);

	return align;
}

void
copyStrings(
	Alignment *source,
	Alignment *dest,
	int i1,
	int i2,
	int j1,
	int j2
	)
{
	dest->s1_name = strdup(source->s1_name);
	dest->s2_name = strdup(source->s2_name);
	dest->s1 = strndup(source->s1 + i1, i2 - i1 + 1);
	dest->s2 = strndup(source->s2 + j1, j2 - j1 + 1);
	dest->m = strlen(dest->s1);
	dest->n = strlen(dest->s2);
}

bool
compare_alignments(
	const Alignment *first,
	const Alignment *second
	)
{
	// Sort in order of highest to lowest score
	if (first->score > second->score)
		return true;
	else
		return false;
}

void
resetGrid(
	DPGrid *grid
	)
{
	int i, j;
	DPCell *pcell;

	for (i = 0; i < grid->x; i++) {
		for (j = 0; j < grid->y; j++) {
			pcell = getCell(grid, i, j);
			pcell->match = false;
		}
	}
}

void
calculateAlignment(
	Alignment *align
	)
{
	int i = 0, j = 0;
	DPCell *pcell = NULL;
	align->score = 0;
	align->maxi = 0;
	align->maxj = 0;
	align->mini = 0;
	align->minj = 0;

	for (i = 0; i < align->m + 1; i++) {
		for (j = 0; j < align->n + 1; j++) {
			pcell = getCell(&align->grid, i, j);
			pcell->S = getSubstitutionScore(align, i, j);
			pcell->I = getInsertionScore(align, i, j);
			pcell->D = getDeletionScore(align, i, j);

			pcell->score = max3(pcell->S, pcell->I, pcell->D);
			if (pcell->score == pcell->S) {
				pcell->prev = 's';
			}
			else if (pcell->score == pcell->I) {
				pcell->prev = 'i';
			}
			else {
				pcell->prev = 'd';
			}

			// Keep overall max
			if (align->local == true && pcell->score > align->score) {
				align->score = pcell->score;
				align->maxi = i;
				align->maxj = j;
			}
		}
	}
	if (align->local == false) {
		align->score = pcell->score;
		align->maxi = align->m;
		align->maxj = align->n;
	}
}

void
retrace(
	Alignment *align,
	int i,
	int j)
{
	DPCell *pcell, *prev;
	int k = 0;
	char *start, *end, c;

	align->nmatch = 0;
	align->nmismatch = 0;
	align->ngap = 0;
	align->nopengaps = 0;
	align->mini = 0;
	align->minj = 0;

	while(i > 0 || j > 0) {
		pcell = getCell(&align->grid, i, j);

		if (pcell->score <= 0 && align->local == true)
			break;

		align->mini = i;
		align->minj = j;

		switch(pcell->prev) {
		case 's':
			// Previous cell must have been from result of substitution. Add
			// substitution to path and backtrack to upper left diagonal cell.

			if (pcell->match) {
				align->nmatch++;
				align->alignpath[k] = 'S';
			}
			else {
				align->nmismatch++;
				align->alignpath[k] = 's';
			}

			i--;
			j--;
			break;
		case 'i':
			// Previous cell must have been from result of insertion. Add
			// insertion to path and backtrack to left cell.
			prev = getCell(&align->grid, i, j-1);
			//Check if previous score was an opening gap
			if (pcell->score - align->params.h - align->params.g == prev->score)
				align->nopengaps++;

			align->ngap++;
			align->alignpath[k] = 'I';
			j--;
			break;
		case 'd':
			// Previous cell must have been from result of deletion. Add
			// deletion to path and backtrack to above cell.
			prev = getCell(&align->grid, i-1, j);
			//Check if previous score was an opening gap
			if (pcell->score - align->params.h - align->params.g == prev->score)
				align->nopengaps++;

			align->ngap++;
			align->alignpath[k] = 'D';
			i--;
			break;
		default:
			printf("Something went wrong! i: %d, j: %d\n", i, j);
			exit(1);
		}

		k++;
		if (k >= (align->n+1)*(align->m+1)) {
			// For debug, shouldn't reach this
			printf("K IS TOO LARGE\n");
		}
	}

	// Reverse string since path was built backwards
	// O(n) operation
	end = &(align->alignpath[k-1]);
	start = &(align->alignpath[0]);
	while (end > start) {
		c = *end;
		*end = *start;
		*start = c;
		start++;
		end--;
	}
}
