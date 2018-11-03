/* 
 * Project 2
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    TODO
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms2A.cpp
 */

// #include <cstdlib>
// #include <math.h>
#include <iostream>
#include <fstream>
// #include <iomanip>

// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;
    outputFile.open((filename + ".out").c_str());

    return outputFile;
}


// MAIN
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr <<  "Invalid argument count! Usage: ./bms2A filename" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    std::ifstream inputFile;
    inputFile.open(filename);

    if (!inputFile.is_open())
    {
        std::cerr <<  "Unable to open input file: " << filename << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile = CreateOutputFile(filename);

    outputFile.close();
    inputFile.close();
    return EXIT_SUCCESS;
}
