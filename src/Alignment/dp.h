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
	char *s1;
	char *s2_name;
	char *s2;
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
initializeGrid(
	Alignment *align
	);

// Sort elements in list by largest to smallest
bool
compare_alignments(
	const Alignment *first,
	const Alignment *second
	);

void
calculateGlobalAlignment(
	char *s1,
	int s1_len,
	char *s1_name,
	char *s2,
	int s2_len,
	char *s2_name,
	ScoreParams *params
	);

void
calculateLocalAlignments(
	char *s1,
	int s1_len,
	char *s1_name,
	char *s2,
	int s2_len,
	char *s2_name,
	ScoreParams *params,
	int nalignments
	);

// Need to re-run the algorithm on all four corners to determine the next best
// optimal local alignment at each recursive step.
//
//                     maxj
//               minj  |
//                  |  |
// --------------------------------------------------------n
// |                xxxx
// |                xxxx
// | corner 1       xxxx     corner 2
// |                xxxx
// |                xxxx
// |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx - mini
// |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx - maxi
// |                xxxx
// |                xxxx
// |                xxxx
// | corner 3       xxxx     corner 4
// |                xxxx
// |                xxxx
// m
void
calculateLocalAlignmentsRecursive(
	Alignment *align,
	list<Alignment*> &alignlist,
	int recursions
	);

void
copyStrings(
	Alignment *source,
	Alignment *dest,
	int i1,
	int i2,
	int j1,
	int j2
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
