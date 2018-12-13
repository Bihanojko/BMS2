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

unsigned int chunkSize = blockLength - NPAR;
size_t lastBlockSize;
size_t encodedSize;
unsigned int blockCount;


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
void InterleaveInput(unsigned char* encodedInput, unsigned char* interleavedInput, unsigned int blocksCounter, unsigned int blockCount, unsigned int chunkSize, size_t lastBlockSize)
{
	unsigned int index;

    // Save encoded data to vector + interleave it
    for (unsigned int i = 0; i < chunkSize + NPAR; ++i)
    {
        index = i <= lastBlockSize
            ? blocksCounter + i * blockCount
            : blocksCounter + i * blockCount - i + lastBlockSize;

        interleavedInput[index] = encodedInput[i];
    }
}


// TODO
// encode and interleave the input sequence in buffer and return it in interleavedInput 
void EncodeInput(unsigned char* buffer, unsigned char* interleavedInput)
{
    unsigned char* encodedOutput = new unsigned char [blockLength + 1];

	for (unsigned int inputOffset = 0, outputOffset = 0, blocksCounter = 0; inputOffset < inputFileLength; inputOffset += chunkSize, outputOffset += blockLength, ++blocksCounter)
	{
		// set different size for the last block
		if (chunkSize > inputFileLength - inputOffset)
			chunkSize = inputFileLength - inputOffset;
	
		// encode input sequence
    	encode_data(buffer + inputOffset, chunkSize, encodedOutput);
    	// encode_data(buffer + inputOffset, chunkSize, encodedOutput + outputOffset);

        // // TODO vyjmout interleave az pred output
    	// // interleave the input sequence given in encodedOutput returning result in interleavedInput variable
        InterleaveInput(encodedOutput, interleavedInput, blocksCounter, blockCount, chunkSize, lastBlockSize);
	}

    delete [] encodedOutput;
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
	lastBlockSize = inputFileLength % (blockLength - NPAR) + NPAR;
	encodedSize = inputFileLength % (blockLength - NPAR)
        ? (inputFileLength / (blockLength - NPAR)) * blockLength + lastBlockSize
        : (inputFileLength / (blockLength - NPAR)) * blockLength;

	blockCount = std::ceil(encodedSize / (double) blockLength);
    unsigned char* interleavedInput = new unsigned char [encodedSize];

	// encode the input sequence
    EncodeInput(buffer, interleavedInput);

    // output the encoded sequence to file
    outputFile.write((char*) interleavedInput, encodedSize);

 	// free allocated memory
    delete [] interleavedInput;
    delete [] buffer;

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
