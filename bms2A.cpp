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

#include "rscode-1.3/ecc.h"

// length of the block of data that is processed
unsigned int blockLength = 255;
// length of the input file
size_t inputFileLength = 0;


// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;
    outputFile.open(filename.append(".out"), std::ofstream::binary);

    return outputFile;
}


// determine file length
size_t GetFileLength(std::ifstream& inputFile)
{
    inputFile.seekg(0, inputFile.end);
    size_t length = inputFile.tellg();
    inputFile.seekg(0, inputFile.beg);

    return length;
}


// TODO
// interleave input sequence given in encodedInput and return it in interleavedInput
void InterleaveInput(unsigned char* encodedInput, unsigned char* interleavedInput, unsigned int blocksCounter, unsigned int blockCount, unsigned int dataLength, size_t lastBlockSize)
{
	unsigned int index;

    // Save encoded data to vector + interleave it
    for (unsigned int i = 0; i < dataLength + NPAR; ++i)
    {
        index = i <= lastBlockSize
            ? blocksCounter + i * blockCount
            : blocksCounter + i * blockCount - i + lastBlockSize;

        interleavedInput[index] = encodedInput[i];
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

    // create and open output file
    std::ofstream outputFile = CreateOutputFile(filename);

    // check if both files were opened successfully
    if (!inputFile.is_open() || !outputFile.is_open())
    {
        std::cerr <<  "Unable to open input or output file!" << std::endl;

        if (inputFile.is_open())
            inputFile.close();
        if (outputFile.is_open())
            outputFile.close();

        return EXIT_FAILURE;
    }

    // determine input file length
    inputFileLength = GetFileLength(inputFile);

    // allocate memory
    unsigned char* buffer = new unsigned char [inputFileLength];

    // read data as a block
    inputFile.read((char*) buffer, inputFileLength);

    // initialize error checking and correcting
    initialize_ecc();

    // TODO vvvvvvv
	unsigned int dataLength = blockLength - NPAR;

	size_t lastBlockSize = inputFileLength % (blockLength - NPAR) + NPAR;
	size_t encodedSize = inputFileLength % (blockLength - NPAR)
        ? (inputFileLength / (blockLength - NPAR)) * blockLength + lastBlockSize
        : (inputFileLength / (blockLength - NPAR)) * blockLength;

    unsigned char* encodedInput = new unsigned char [blockLength + 1];
	unsigned int blockCount = std::ceil(encodedSize / (double) blockLength);
    unsigned char* interleavedInput = new unsigned char [encodedSize];

    size_t encodedLength;
    unsigned int blocksCounter;
    // TODO ^^^^^^^

	for (encodedLength = 0, blocksCounter = 0; encodedLength < inputFileLength; encodedLength += blockLength - NPAR, ++blocksCounter)
	{
		// set different size for the last block
		if (dataLength > inputFileLength - encodedLength)
			dataLength = inputFileLength - encodedLength;
	
		// encode input sequence
    	encode_data(buffer + encodedLength, dataLength, encodedInput);

        // TODO vyjmout interleave az pred output
    	// interleave the input sequence given in encodedInput returning result in interleavedInput variable
        InterleaveInput(encodedInput, interleavedInput, blocksCounter, blockCount, dataLength, lastBlockSize);
	}

    // output the encoded sequence to file
    outputFile.write((char*) interleavedInput, encodedSize);

 	// free allocated memory
    delete [] interleavedInput;
    delete [] encodedInput;
    delete [] buffer;

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
