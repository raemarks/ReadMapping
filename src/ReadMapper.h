#ifndef READ_MAPPER_H
#define READ_MAPPER_H

#include <string>
#include <vector>

#include "SuffixTree/SuffixTree.h"

class ReadMapper {
	private:
		static const int X = 25;
		std::vector<std::string> reads;
		std::string genome;
		//For the reference genome
		suffixtree::Tree *st;

		//Returns a vector of starting locations of all instances of the longest
		//common substring shared by the read r and the genome.
		std::vector<int> FindLoc(
			std::string r
			);

	public:
		ReadMapper();

		bool ReadInputFiles(
			std::string& genomeFile,
			std::string& readsFile
			);

		void MapReads();

};

#endif
