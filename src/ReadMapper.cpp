#include "ReadMapper.h"

ReadMapper::ReadMapper():
	st(nullptr)
{
}

bool 
ReadMapper::ReadInputFiles(
	std::string& genomeFile,
	std::string& readsFile
	)
{
	return true;
}

void
ReadMapper::MapReads()
{
	for (std::string r : reads) {
	}
}

std::vector<int>
ReadMapper::FindLoc(
	std::string r
	)
{
	std::vector<int> vec;

	return vec;
}
