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

unsigned int BLOCK_LENGTH = 255;


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


// TODO
void DeinterleaveInput(unsigned char* buffer, std::vector<unsigned char>& deinterleavedInput, size_t inputFileLength)
{
	size_t blocks = std::ceil(inputFileLength / (float) BLOCK_LENGTH);
	size_t lastBlockSize = inputFileLength % BLOCK_LENGTH;
	size_t dataLength = BLOCK_LENGTH - NPAR;

	// deinterleavedInput.resize(inputFileLength);

	for (unsigned int i = 0; i < blocks; ++i)
	{
		if (i == blocks - 1)
			dataLength = lastBlockSize - NPAR;
	
		for (unsigned int j = 0; j < BLOCK_LENGTH; ++j)
		{
			int map = i + j * blocks;

			if (j > lastBlockSize)
				map -= j - lastBlockSize;

			deinterleavedInput.append();
			// deinterleavedInput[j + i * BLOCK_LENGTH] = buffer[map];
		}
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
    size_t inputFileLength = GetFileLength(inputFile);

    // allocate memory
    unsigned char* buffer = new unsigned char [inputFileLength];

    // read data as a block
    inputFile.read((char*) buffer, inputFileLength);

    // initialize error checking and correcting
	initialize_ecc();

	// to store the input sequence after deinterleaving
	std::vector<unsigned char> deinterleavedInput;

	// deinterleave the input sequence returning result in deinterleavedInput variable
	DeinterleaveInput(buffer, deinterleavedInput, inputFileLength);

	// TODO vvvvvv
	// decode input
	for (size_t decoded = 0; decoded < inputFileLength; decoded += BLOCK_LENGTH)
	{
		// If it is last block, adjust size
		if (BLOCK_LENGTH + decoded >= inputFileLength)
			BLOCK_LENGTH = inputFileLength - decoded;

		// Decode date
		decode_data(deinterleavedInput.data() + decoded, BLOCK_LENGTH);
		
		// Corrector errors
		if (check_syndrome() != 0)
			correct_errors_erasures(deinterleavedInput.data() + decoded, BLOCK_LENGTH, 0, nullptr);

		outputFile.write((char*) (deinterleavedInput.data() + decoded), BLOCK_LENGTH - NPAR);
	}
	// TODO ^^^^^^

	delete [] buffer;

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
