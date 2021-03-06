#include <alignment/algorithms/alignment/AlignmentUtils.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

using namespace blasr;

int ComputeAlignmentScore(std::string &queryStr, std::string &textStr, int matchScores[5][5],
                          int ins, int del)
{
    DistanceMatrixScoreFunction<DNASequence, DNASequence> scoreFn(matchScores, ins, del);
    return ComputeAlignmentScore(queryStr, textStr, scoreFn);
}

int GetNumberWidth(unsigned int value)
{
    // 0 has a width of 1.
    int width = 1;
    while (value / 10 > 0) {
        value = value / 10;
        ++width;
    }
    return width;
}

int ComputeDrift(Block &cur, Block &next)
{

    int tGap = (next.tPos - cur.TEnd());
    int qGap = (next.qPos - cur.QEnd());

    int commonGap = 0;

    if (tGap > 0 and qGap > 0) {
        commonGap = std::abs(tGap - qGap);
    }
    tGap -= commonGap;
    qGap -= commonGap;

    return tGap - qGap;
}
