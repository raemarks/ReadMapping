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

int main (int argc, char *argv[]) {
	if (argc < 3) {
		printf("must pass at least two paramters\n");
		return 1;
	}

	vector<Sequence> genome;
	readInput(argv[1], genome);
	cout << "read genome, length = " << genome[0].content.length() << endl;

	vector<Sequence> sequences;
	readInput(argv[2], sequences);
	cout << "read " << sequences.size() <<  " sequences" << endl;

	ScoreParams sp = {0};
	sp.match = 1;
	sp.mismatch = -2;
	sp.h = -5;
	sp.g = -1;

	auto mapreads = new Yardstick();
	auto output = new Yardstick();
	auto construct = new Yardstick();
	auto prepare = new Yardstick();
	auto totalt = new Yardstick();
	totalt->start();

	construct->start();
	Tree *st = new Tree(genome[0].content, "");
	st->Build();
	auto build_time = construct->end();

	prepare->start();
	st->PrepareIndexArray();
	auto prepare_time = prepare->end();



	int hits = 0;
	int withreads = 0;
	int aligns = 0;
	mapreads->start();
	for (int i = 0; i < sequences.size(); i++) {
		Sequence& s = sequences[i];
		vector<int> L = st->FindLoc(s.content);
		aligns += L.size();
		if (L.size() > 0) {
			withreads++;
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
			int length = 2 * s.content.length();
			if (length + offset > genome[0].content.length()) {
				length -= ((length + offset) - genome[0].content.length());
			}

			Alignment *a = calculateLocalAlignment(
					genome[0].content.c_str() + offset,
					length,
					genome[0].title.c_str(),
					s.content.c_str(),
					s.content.length(),
					s.title.c_str(),
					&sp
					);
			int alignlen = a->nmatch + a->ngap + a->nmismatch;
			double identity = (double)a->nmatch / ((double)alignlen);
			double coverage = (double)alignlen / ((double)s.content.length());
			if (identity > IdX && coverage > CoverY &&coverage > best_coverage) {
				best_i = j;
				best_coverage = coverage;
				start = offset;
				end = offset + length;
			}
			deleteAlignment(a);
		}
		output->start();
		if (best_i == -1) {
			cout << s.title << " No hit found" << endl;
		} else {
			cout << s.title << " " << start << " " << end << endl;
			hits++;
		}
		output->end();
	}
	auto reads_time = mapreads->end();

	totalt->end();

	printf("hit rate = %lf ( %d / %d )\n", 100 * (double)hits / (double)sequences.size(), hits, sequences.size());
	printf("average alignments per read (total): %lf\n", (double)aligns/ sequences.size());
	printf("average alignments per read: %lf\n", (double)aligns/ withreads);
	printf("mapreads = %lf seconds\n", mapreads->total() - output->total());
	printf("output = %lf seconds\n", output->total());
	printf("build st = %lf seconds\n", construct->total());
	printf("prepare = %lf seconds\n", prepare->total());
	printf("total = %lf seconds\n", totalt->total());

	return 0;
}
