# ReadMapping
The goal of this project is to implement an algorithm for the read mapping problem, which is defined as follows: Given a reference genome sequence G of length N, and a set of m sequences (called "reads"), identify the genomic locus to which each read best aligns. 

## Building
Once you have the source, make sure you can build c+11 code with gcc (minimum version of 4.4). Once you meet this requirement, do the following in the project root directory:
```bash
make clean
make
```

## Usage
The program takes two .fasta files as input, one containing a single reference genome and another containing several sequences for reads. To run the program and map the reads to the reference genome, do the following in the project root directory:
```bash
./ReadMapper reference_genome.fasta reads_to_map.fasta
```

The program will output some performance statistics at the end of the read mapping output.

## License
MIT
