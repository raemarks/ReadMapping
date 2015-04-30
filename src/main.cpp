#include "ReadMapper.h"
#include "io.h"
#include <SuffixTree.h>
#include <yardstick.h>
#include <dp.h>
#include <iostream>
#include <vector>

#define IdX ((double)0.90)
#define CoverY ((double)0.80)

using std::cout;
using std::endl;
using std::vector;
using namespace suffixtree;

ScoreParams sp;

typedef struct result {
	int start;
	int end;
} result;

typedef struct stats {
	int hits;
	int withreads;
	int aligns;
} stats;


void mapReadsRange(Tree *st, Sequence &genome, vector<Sequence> &seqs, int beg, int end, stats *out, vector<result*> &res);
result *mapReads(Tree *st, Sequence &genome, stats *readstats, Sequence &s);

int main (int argc, char *argv[]) {
	if (argc < 3) {
		printf("must pass at least two paramters\n");
		return 1;
	}

	stats readstats = {0};
	Tree *st = nullptr;
	vector<Sequence> genome;
	readInput(argv[1], genome);
	cout << "read genome, length = " << genome[0].content.length() << endl;

	vector<Sequence> sequences;
	readInput(argv[2], sequences);
	cout << "read " << sequences.size() <<  " sequences" << endl;

	sp.match = 1;
	sp.mismatch = -2;
	sp.h = -5;
	sp.g = -1;

	auto output = new Yardstick();
	auto mapreads = new Yardstick();
	auto construct = new Yardstick();
	auto prepare = new Yardstick();
	auto totalt = new Yardstick();

	totalt->start();

	construct->start();
	{
		st = new Tree(genome[0].content, "");
		st->Build();
	}
	construct->end();

	prepare->start();
	{
		st->PrepareIndexArray();
	}
	prepare->end();


	vector<result*> results(sequences.size());

	mapreads->start();
	{
		mapReadsRange(st, genome[0], sequences, 0, sequences.size(), &readstats, results);
	}
	mapreads->end();

	output->start();
	for (int i = 0; i < results.size(); i++) {
		Sequence &s = sequences[i];
		if (results[i] == nullptr) {
			cout << s.title << " No hit found" << endl;
		} else {
			cout << s.title << " " << results[i]->start << " " << results[i]->end << endl;
			readstats.hits++;
		}
	}
	output->end();
	totalt->end();

	int aligns = readstats.aligns;
	int withreads = readstats.withreads;
	int hits = readstats.hits;

	printf("hit rate = %lf ( %d / %d )\n", 100 * (double)hits / (double)sequences.size(), hits, sequences.size());
	printf("average alignments per read (total): %lf\n", (double)aligns/ sequences.size());
	printf("average alignments per read: %lf (%d / %d)\n", (double)aligns/ withreads, aligns, withreads);
	printf("mapreads = %lf seconds\n", mapreads->total());
	printf("output = %lf seconds\n", output->total());
	printf("build st = %lf seconds\n", construct->total());
	printf("prepare = %lf seconds\n", prepare->total());
	printf("total = %lf seconds\n", totalt->total());

	return 0;
}

void mapReadsRange(Tree *st, Sequence &genome, vector<Sequence> &seqs, int beg, int end, stats *out, vector<result*> &res) {
	for (int i = beg; i < end; i++) {
		res[i] = mapReads(st, genome, out, seqs[i]);
	}
}

result *mapReads(Tree *st, Sequence &genome, stats *readstats, Sequence &s) {
	vector<int> L = st->FindLoc(s.content);
	readstats->aligns += L.size();
	if (L.size() > 0) {
		readstats->withreads++;
	}
	int best_i = -1;
	double best_coverage = 0;
	int start = 0;
	int end = 0;
	for (int j = 0; j < L.size(); j++) {
		int offset = L[j];
		if (offset < s.content.length()) {
			offset = 0;
		} else {
			offset -= s.content.length();
		}
		int length = 3 * s.content.length();
		if (length + offset > genome.content.length()) {
			length -= ((length + offset) - genome.content.length());
		}

		Alignment *a = calculateLocalAlignment(
				genome.content.c_str() + offset,
				length,
				genome.title.c_str(),
				s.content.c_str(),
				s.content.length(),
				s.title.c_str(),
				&sp
				);
		int alignlen = a->nmatch + a->nmismatch + a->ngap;
		double identity = (double)a->nmatch / ((double)alignlen);
		double coverage = ((double)s.content.length()) / (double)alignlen;
		if (identity > IdX && coverage > CoverY && coverage > best_coverage) {
			best_i = j;
			best_coverage = coverage;
			start = offset + a->mini;
			end = offset + a->maxi;
		}
		deleteAlignment(a);
	}
	if (best_i != -1) {
		result *r = new result();
		r->start = start;
		r->end = end;
		return r;
	}
	return nullptr;
}
