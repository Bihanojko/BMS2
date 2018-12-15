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

// length of the block of data that is outputted
const unsigned int blockLength = 255;
// length of the input data that are being processed per each block
const unsigned int chunkSize = blockLength - NPAR;

// GLOBAL FILE PARAMETERS
// length of the input file
size_t inputFileLength = 0;
// length of the output sequence (encoded and interlaved)
unsigned int outputSize = 0;
// the length of the last processed block
unsigned int lastBlockSize = 0;
// the number of blocks that will be processed and outputted
unsigned int blockCount = 0;


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


// set global parameters regarding the file being processed
void SetFileParameters()
{
    // set the length of the last processed block
    lastBlockSize = inputFileLength % (blockLength - NPAR) + NPAR;

    // set length of the output sequence (encoded and interlaved)
    outputSize = inputFileLength % (blockLength - NPAR)
        ? (inputFileLength / (blockLength - NPAR)) * blockLength + lastBlockSize
        : (inputFileLength / (blockLength - NPAR)) * blockLength;

    // set the number of blocks that will be processed and outputted
    blockCount = std::ceil(outputSize / (double) blockLength);
}


// encode the input sequence in buffer and return it in encodedInput 
void EncodeInput(unsigned char* buffer, std::vector<unsigned char>& encodedInput)
{
    unsigned char* encodedChunk = new unsigned char [blockLength + 1];
    unsigned int inputCharCount = chunkSize;
    unsigned int encodedCharCount = blockLength;

    for (unsigned int inputOffset = 0; inputOffset < inputFileLength; inputOffset += chunkSize)
    {
        // set different size for the last block
        if (inputCharCount > inputFileLength - inputOffset)
        {
            inputCharCount = inputFileLength - inputOffset;
            encodedCharCount = lastBlockSize;
        }
    
        // encode input sequence
        encode_data(buffer + inputOffset, inputCharCount, encodedChunk);

        encodedInput.insert(encodedInput.end(), encodedChunk, encodedChunk + encodedCharCount);
    }

    delete [] encodedChunk;
}


// interleave input block given in encodedInput and return it in interleavedInput
void InterleaveBlock(std::vector<unsigned char>& encodedInput, unsigned char* interleavedInput, unsigned int blocksCounter, unsigned int encodedCharCount)
{
    unsigned int blockOffset = blocksCounter * blockLength;
    unsigned int index;

    // assign encoded data cells to diferent indeces in interleavedInput  
    for (unsigned int i = 0; i < encodedCharCount + NPAR; ++i)
    {
        index = i <= lastBlockSize
            ? blocksCounter + i * blockCount
            : blocksCounter + i * blockCount - i + lastBlockSize;

        interleavedInput[index] = encodedInput.at(blockOffset + i);
    }
}


// interleave input sequence given in encodedInput and return it in interleavedInput
void InterleaveInput(std::vector<unsigned char>& encodedInput, unsigned char* interleavedInput)
{
    unsigned int encodedCharCount = chunkSize;

     for (unsigned int inputOffset = 0, blocksCounter = 0; inputOffset < inputFileLength; inputOffset += encodedCharCount, ++blocksCounter)
    {
        // set different size for the last block
        if (encodedCharCount > inputFileLength - inputOffset)
            encodedCharCount = inputFileLength - inputOffset;
    
        // interleave the encoded current block
        InterleaveBlock(encodedInput, interleavedInput, blocksCounter, encodedCharCount);
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

    // set global file parameters
    SetFileParameters();

    // store encoded input before interleaving
    std::vector<unsigned char> encodedInput;

    // encode the input sequence returning result in encodedInput variable
    EncodeInput(buffer, encodedInput);

    // store encoded and interleaved input
    unsigned char* interleavedInput = new unsigned char [outputSize];

    // interleave the encoded sequence returning result in interleavedInput variable
    InterleaveInput(encodedInput, interleavedInput);

    // output the encoded sequence to file
    outputFile.write((char*) interleavedInput, outputSize);

     // free allocated memory
    delete [] interleavedInput;
    delete [] buffer;

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
