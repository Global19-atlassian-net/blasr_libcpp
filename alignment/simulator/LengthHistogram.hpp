#ifndef _SIMULATOR_LENGTH_HISTOGRAM_HPP_
#define _SIMULATOR_LENGTH_HISTOGRAM_HPP_

#include <iostream>
#include <string>
#include <vector>
#include "../../pbdata/alignment/CmpAlignment.hpp"  // pbdata
#include "../../pbdata/utils.hpp"
#include "CDFMap.hpp"

class LengthHistogram
{
public:
    CDFMap<int> lengthHistogram;

    int Read(std::string &inName);

    int Read(std::ifstream &in);

    void GetRandomLength(int &length);

    void BuildFromAlignmentLengths(std::vector<int> &lengths);
};

#endif
