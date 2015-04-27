#ifndef _DP_H_
#define _DP_H_

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <list>

using std::list;

// Risk of underflow if true int_min
#define NEG_INFINITY -100000000
#define MB 1024*1024

typedef struct DPCell {
	//If this cell got its score from substitution (s), insertion(i), or
	//deletion(d)
	char prev;
	//Overall score
	int score;
	//Max substitution score
	int S;
	//Max insertion score
	int I;
	//Max deletion score
	int D;
	//True if i and j characters match
	bool match;
	//True if this cell has already been calculated. Useful for removing
	//sections to calculate next-best local alignments.
	bool scored;

} DPCell;

typedef struct DPGrid {
	DPCell *cells;
	// Number of rows / length of S1 + 1
	int y;
	// Number of columns / length of S2 + 1
	int x;
} DPGrid;

// Contains scores for each possibility for matches, mismatches, and gaps using
// the affine gap method.
typedef struct ScoreParams {
	int match;
	int mismatch;
	int h;
	int g;
} ScoreParams;

typedef struct Alignment {
	bool local;

	DPGrid grid;
	ScoreParams params;

	char *s1_name;
	const char *s1;
	char *s2_name;
	const char *s2;
	int m;
	int n;

	// Offset from 0,0 (useful for local alignments)
	int offseti;
	int offsetj;

	// D for deletion, S for substitution, I for insertion
	char *alignpath;
	int nmatch;
	int nmismatch;
	int ngap;
	int nopengaps;
	int score;

	// For local optimal alignment only. Represents starting coordinates and
	// ending coordinates of optimal local alignment.
	int maxi;
	int maxj;
	int mini;
	int minj;
} Alignment;


DPCell *
getCell(
	DPGrid *grid,
	int x,
	int y
	);

// Gets score for a cell if it were to be chosen in the event of a substitution
int
getSubstitutionScore(
	Alignment *align,
	int i,
	int j
	);

// Gets score for a cell if it were to be chosen in the event of an insertion
int
getInsertionScore(
	Alignment *align,
	int i,
	int j
	);

// Gets score for a cell if it were to be chosen in the event of an deletion
int
getDeletionScore(
	Alignment *align,
	int i,
	int j
	);

void
chooseWhichPath(
	DPCell *cell
	);

void
initializeGrid(
	Alignment *align
	);

void
deleteAlignment(
	Alignment *align
	);

// Sort elements in list by largest to smallest
bool
compare_alignments(
	const Alignment *first,
	const Alignment *second
	);

Alignment *
calculateGlobalAlignment(
	const char *s1,
	int s1_len,
	char *s1_name,
	const char *s2,
	int s2_len,
	char *s2_name,
	ScoreParams *params
	);

Alignment *
calculateLocalAlignment(
	const char *s1,
	int s1_len,
	char *s1_name,
	const char *s2,
	int s2_len,
	char *s2_name,
	ScoreParams *params
	);

void
resetGrid(
	DPGrid *grid
	);

void
calculateAlignment(
	Alignment *align
	);

void
retrace(
	Alignment *align,
	int i,
	int j);

#endif
