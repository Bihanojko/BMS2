/* 
 * Project 2
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    16. 12. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms2A.cpp
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
    outputFile.open(filename.append(".out"), std::ofstream::binary);

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
    unsigned inputFileLength = GetFileLength(inputFile);

    // allocate memory
    unsigned char* buffer = new unsigned char [inputFileLength];

    // read data as a block
    inputFile.read(reinterpret_cast<char*>(buffer), inputFileLength);

    // initialize error checking and correcting
    initialize_ecc();

    // TODO vvvvvvv
    const size_t paritySize = NPAR;
    // 255 is limit of library
    const size_t dataSize = 255 - paritySize;
    //const size_t dataSize = 145;
    const size_t blockSize = dataSize + paritySize;

    unsigned char *input = nullptr;
    unsigned char output[256];
    std::vector<unsigned char> encoded;

    // encode input
	unsigned mapIndex;
	size_t encodedLength = 0;
	size_t dataLength = dataSize;

	size_t encodedSize = (inputFileLength / dataSize) * blockSize;
	size_t lastBlockSize = inputFileLength % dataSize + paritySize;
	if (inputFileLength % dataSize != 0)
		encodedSize += lastBlockSize;

	size_t blocks = std::ceil(encodedSize / (double)blockSize);
	encoded.clear();
	encoded.resize(encodedSize);
	unsigned blocksCounter = 0;

	while (encodedLength < inputFileLength)
	{
		// For last block
		if (dataLength + encodedLength > inputFileLength)
			dataLength = inputFileLength - encodedLength;
	
    	encode_data(input + encodedLength, dataLength, output);
	
    	// Save encoded data to vector + interleave it
		for (unsigned i = 0; i < dataLength + paritySize; ++i)
		{
			mapIndex = blocksCounter + i * blocks;
			// Need to adjust index if last codeword is smaller
			if (i > lastBlockSize)
				mapIndex -= i - lastBlockSize;;
			encoded[mapIndex] = output[i];
		}
    
		encodedLength += dataSize;
		// Move to next codeword
		++blocksCounter;
		// Without interleaving
		//encoded.insert(encoded.end(), output, output + dataLength + paritySize);
	}
    // TODO ^^^^^^^

    // create and open output file
    std::ofstream outputFile = CreateOutputFile(filename);

	// TODO 
    outputFile.write((const char *) &encoded.front(), encoded.size());

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
