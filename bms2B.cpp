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

#include "rscode-1.3/ecc.h"

// length of the block of data that is processed
unsigned int blockLength = 255;

// GLOBAL FILE PARAMETERS
// length of the input file
size_t inputFileLength = 0;
// the length of the last processed block
unsigned int lastBlockSize = 0;
// the number of blocks that will be processed and outputted
unsigned int blockCount = 0;



// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;
    outputFile.open(filename.append(".ok"), std::ofstream::binary);

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


// deinterleave input sequence given in buffer and return it in deinterleavedInput
void DeinterleaveInput(unsigned char* buffer, std::vector<unsigned char>& deinterleavedInput)
{
	unsigned int dataLength = blockLength;
	unsigned int index = 0;

	for (unsigned int i = 0; i < blockCount; ++i)
	{
		if (i + 1 == blockCount)
			dataLength = lastBlockSize;
	
		for (unsigned int j = 0; j < dataLength; ++j)
		{
			index = j <= lastBlockSize
				? i + j * blockCount
				: i + j * blockCount - j + lastBlockSize;

			deinterleavedInput.push_back(buffer[index]);
		}
	}
}


// decode the input sequence in deinterleavedInput and write the output to outputFile
void DecodeInput(std::vector<unsigned char>& deinterleavedInput, std::ofstream& outputFile)
{
	for (unsigned int i = 0; i < inputFileLength; i += blockLength)
	{
		// set different size for the last block
		if (blockLength >= inputFileLength - i)
			blockLength = inputFileLength - i;

		// decode input sequence
		decode_data(deinterleavedInput.data() + i, blockLength);
		
		// check if any errors occurred and if so, try to correct them
		if (check_syndrome())
			correct_errors_erasures(deinterleavedInput.data() + i, blockLength, 0, nullptr);

		// output the decoded and corrected sequence to file
		outputFile.write((char*) (deinterleavedInput.data() + i), blockLength - NPAR);
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

	blockCount = std::ceil(inputFileLength / (double) blockLength);
	lastBlockSize = inputFileLength % blockLength;

    // allocate memory
    unsigned char* buffer = new unsigned char [inputFileLength];

    // read data as a block
    inputFile.read((char*) buffer, inputFileLength);

    // initialize error checking and correcting
	initialize_ecc();

	// to store the input sequence after deinterleaving
	std::vector<unsigned char> deinterleavedInput;

	// deinterleave the input sequence returning result in deinterleavedInput variable
	DeinterleaveInput(buffer, deinterleavedInput);

	// decode the deinterleved input sequence
	DecodeInput(deinterleavedInput, outputFile);

	// free allocated memory
	delete [] buffer;

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
