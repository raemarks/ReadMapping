#include "io.h"

int readInput(string finame, vector<Sequence> &out) {
	ifstream file(finame.c_str());

	string title;
	string content;
	while(file.good()) {
		string line;
		getline(file, line);

		if (line.length() == 0) {
			continue;
		}
		if (line[0] == '>') {
			if (content.length() > 0) {
				Sequence nseq;
				nseq.title = title;
				nseq.content = content;
				out.push_back(nseq);
			}
			content = "";
			int length = line.length() - 1;
			if (length > 10) {
				length = 10;
			}
			title = line.substr(1, length);

		} else {
			if (line[line.length() - 1] == '\n') {
				line = line.substr(0, line.length() - 1);
			}
			content += line;
		}
	}
	Sequence nseq;
	nseq.title = title;
	nseq.content = content;
	out.push_back(nseq);

	return 0;
}


