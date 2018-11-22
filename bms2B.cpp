/* 
 * Project 2
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    16. 12. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms2B.cpp
 */

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include "lib/ecc.h"


// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;
    outputFile.open(filename.append(".ok"), std::ofstream::binary);

    return outputFile;
}


// determine file length
unsigned GetFileLength(std::ifstream& inputFile)
{
    inputFile.seekg(0, inputFile.end);
    unsigned length = inputFile.tellg();
    inputFile.seekg(0, inputFile.beg);

    return length;
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
    unsigned inputFileLength = GetFileLength(inputFile);

    // allocate memory
    unsigned char* buffer = new unsigned char [inputFileLength];

    // read data as a block
    inputFile.read(reinterpret_cast<char*>(buffer), inputFileLength);

    // create and open output file
    std::ofstream outputFile = CreateOutputFile(filename);

    // initialize error checking and correcting
	initialize_ecc();

	// TODO vvvvvv
	const size_t paritySize = NPAR;
	// 255 is limit of library
	const size_t dataSize = 255 - paritySize;
	//const size_t dataSize = 145;
	const size_t blockSize = dataSize + paritySize;

	unsigned char *input = nullptr;

	// deinterleave
	std::vector<unsigned char> deInterleaved;
	deInterleaved.resize(inputFileLength);

	size_t blocks = std::ceil(inputFileLength / (double) blockSize);
	size_t lastBlockSize = inputFileLength % blockSize;
	size_t dataLength = dataSize;

	for (unsigned j = 0; j < blocks; ++j)
	{
		if (j == blocks - 1)
			dataLength = lastBlockSize - paritySize;
	
		for (unsigned i = 0; i < dataLength + paritySize; ++i)
		{
			int map = j + i * blocks;

			if (i > lastBlockSize)
				map -= i - lastBlockSize;

			deInterleaved[i + j * blockSize] = input[map];
		}
	}

	// decode input
	size_t decoded = 0;
	size_t blockLength = blockSize;
	size_t writeSize = dataSize;

	while (decoded < inputFileLength)
	{
		// If it is last block, adjust size
		if (blockLength + decoded >= inputFileLength)
		{
			blockLength = inputFileLength - decoded;
			writeSize = blockLength - paritySize;
		}

		// Decode date
		decode_data(deInterleaved.data() + decoded, blockLength);
		
		// Corrector errors
		if (check_syndrome() != 0)
			correct_errors_erasures(deInterleaved.data() + decoded, blockLength, 0, nullptr);

		outputFile.write(reinterpret_cast<char*> (deInterleaved.data() + decoded), writeSize);
		decoded += blockLength;
	}
	// TODO ^^^^^^

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
