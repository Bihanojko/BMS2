/* 
 * Project 2
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    16. 12. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms2A.cpp
 */

// #include <cstdlib>
// #include <math.h>
#include <iostream>
#include <fstream>
#include <cmath>
// #include <iomanip>

#include "lib/ecc.h"


// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;
    outputFile.open(filename.append(".out"), std::ofstream::binary);

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
void encodeInput()
{
	unsigned mapIndex;
	size_t encodedLength = 0;
	size_t dataLength = dataSize;

	size_t encodedSize = (size / dataSize) * blockSize;
	size_t lastBlockSize = size % dataSize + paritySize;
	if (size % dataSize != 0)
		encodedSize += lastBlockSize;

	size_t blocks = std::ceil(encodedSize / (double)blockSize);
	encoded.clear();
	encoded.resize(encodedSize);
	unsigned blocksCounter = 0;

	while(encodedLength < size)
	{
		// For last block
		if(dataLength + encodedLength > size)
			dataLength = size - encodedLength;
		encode_data(input + encodedLength, dataLength, output);
		// Save encoded data to vector + interleave it
		for(int i=0; i<dataLength + paritySize; ++i)
		{
			mapIndex = blocksCounter + i * blocks;
			// Need to adjust index if last codeword is smaller
			if(i > lastBlockSize)
				mapIndex -= i - lastBlockSize;;
			encoded[mapIndex] = output[i];
		}
		encodedLength += dataSize;
		// Move to next codeword
		++blocksCounter;
		// Without interleaving
		//encoded.insert(encoded.end(), output, output + dataLength + paritySize);
	}
}


// MAIN
int main(int argc, char** argv)
{
    // check argument count
    if (argc != 2)
    {
        std::cerr <<  "Invalid argument count! Usage: ./bms2A filename" << std::endl;
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

    // initialize error checking and correcting
    initialize_ecc();

	// TODO 
    encodeInput();

    // create and open output file
    std::ofstream outputFile = CreateOutputFile(filename);

	// TODO 
    outputFile.write((const char *)&encoded.front(), encoded.size());

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
