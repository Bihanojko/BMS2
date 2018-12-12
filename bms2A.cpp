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
    size_t dataSize = 255 - NPAR;
	unsigned int mapIndex;
	// size_t encodedLength = 0;
	size_t dataLength = dataSize;

	size_t encodedSize = (inputFileLength / dataSize) * (dataSize + NPAR);
    unsigned char* output = new unsigned char [256];
    unsigned char* encoded = new unsigned char [encodedSize];
	size_t lastBlockSize = inputFileLength % dataSize + NPAR;

	if (inputFileLength % dataSize != 0)
		encodedSize += lastBlockSize;

	size_t blocks = std::ceil(encodedSize / (double) (dataSize + NPAR));

    size_t encodedLength;
    unsigned int blocksCounter;

	for (encodedLength = 0, blocksCounter = 0; encodedLength < inputFileLength; encodedLength += dataSize, ++blocksCounter)
	{
		// set different size for the last block
			dataLength = inputFileLength - encodedLength;
	
		// encode input sequence
    	encode_data(buffer + encodedLength, dataLength, output);
	
        // TODO
    	// interleave the input sequence TODO returning result in deinterleavedInput variable
    	// Save encoded data to vector + interleave it
		for (unsigned i = 0; i < dataLength + NPAR; ++i)
		{

			// Need to adjust index if last codeword is smaller
			if (i > lastBlockSize)
				mapIndex -= i - lastBlockSize;;
			encoded[mapIndex] = output[i];
		}
	}
    // TODO ^^^^^^^

    // output the encoded sequence to file
    outputFile.write((char*) encoded, encodedSize);

 	// free allocated memory
    delete [] encoded;
    delete [] output;
    delete [] buffer;

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
