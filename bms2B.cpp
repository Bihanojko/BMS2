/* 
 * Project 2
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    16. 12. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms2B.cpp
 */

// #include <cstdlib>
// #include <math.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include "lib/ecc.h"


// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;
    outputFile.open(filename.append(".ok"), std::ofstream::binary);

    return outputFile;
}


// determine file length
int GetFileLength(std::ifstream& inputFile)
{
    inputFile.seekg(0, inputFile.end);
    int length = inputFile.tellg();
    inputFile.seekg(0, inputFile.beg);

    return length;
}


// TODO
void deInterleave()
{
    std::vector<unsigned char> deInterleaved;
	deInterleaved.resize(inputFileLength);

	size_t blocks = std::ceil(size/(double)blockSize);
	size_t lastBlockSize = size % blockSize;
	size_t dataLength = dataSize;
	for(int j=0; j< blocks; j++) {
		if(j == blocks - 1) {
			dataLength = lastBlockSize - paritySize;
		}
		for(int i=0; i<dataLength + paritySize; ++i)
		{
			int map = j + i * blocks;
			if(i > lastBlockSize)
				map -= i - lastBlockSize;
			deInterleaved[i + j * blockSize] = input[map];
		}
	}
}


// TODO
void decodeInput(std::ofstream& outputFile)
{
	deInterleave();
	size_t decoded = 0;
	size_t blockLength = blockSize;
	size_t writeSize = dataSize;

	while(decoded < size)
	{
		// If it is last block, adjust size
		if (blockLength + decoded >= size)
		{
			blockLength = size - decoded;
			writeSize = blockLength - paritySize;
		}
		// Decode date
		decode_data(deInterleaved.data() + decoded, blockLength);
		// Corrector errors
		if (check_syndrome() != 0)
			correct_errors_erasures(deInterleaved.data() + decoded, blockLength, 0, nullptr);

		outputFile.write(reinterpret_cast<char*>(deInterleaved.data() + decoded), writeSize);
		decoded += blockLength;
	}
}


// MAIN
int main(int argc, char** argv)
{    
    // check argument count
    if (argc != 2)
    {
		std::cerr <<  "Invalid argument count! Usage: ./bms2B filename" << std::endl;
		return EXIT_FAILURE;
    }

    // open input file
    std::string filename = argv[1];
    std::ifstream inputFile;
    inputFile.open(filename, std::ifstream::binary);

    // check if input file was opened successfully
    if (!inputFile.is_open())
    {
        std::cerr <<  "Unable to open input file: " << filename << std::endl;
        return EXIT_FAILURE;
    }

    // determine input file length
    int inputFileLength = GetFileLength(inputFile);

    // allocate memory
    unsigned char* buffer = new unsigned char [inputFileLength];

    // read data as a block
    inputFile.read(reinterpret_cast<char*>(buffer), inputFileLength);

    // create and open output file
    std::ofstream outputFile = CreateOutputFile(filename);

    // initialize error checking and correcting
	initialize_ecc();

    // TODO
	decodeInput(outputFile);

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
