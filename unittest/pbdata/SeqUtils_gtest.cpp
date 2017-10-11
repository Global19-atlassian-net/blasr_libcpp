/*
 * =====================================================================================
 *
 *       Filename:  SeqUtils_gtest.cpp
 *
 *    Description:  Test pbdata/SeqUtils.hpp, SeqUtils_Impl.hpp
 *
 *        Version:  1.0
 *        Created:  10/29/2012 05:21:58 PM
 *        Updated:  08/18/2014 05:49:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan Li (yli), yli@pacificbiosciences.com
 *        Company:  Pacific Biosciences
 *
 * =====================================================================================
 */
#include "SeqUtils.hpp"
#include "gtest/gtest.h"

TEST(SeqUtils, OnlyACTG)
{
    DNASequence seq;
    Nucleotide seqnt[] = "ATGC";
    seq.seq = seqnt;
    seq.length = 4;
    EXPECT_EQ(OnlyACTG(seq), 1);

    Nucleotide seqnt1[] = "ATXYZ";
    seq.seq = seqnt1;
    seq.length = 5;
    EXPECT_EQ(OnlyACTG(seq), 0);
}

TEST(SeqUtils, CountMasked)
{
    DNASequence seq;
    Nucleotide seqnt[] = "ATGCNNNNNNATGC";
    seq.seq = seqnt;
    seq.length = 14;
    EXPECT_EQ(CountMasked(seq), 6u);
}

TEST(SeqUtils, CountNotMasked)
{
    DNASequence seq;
    Nucleotide seqnt[] = "ATGCNNNNNNATGC";
    seq.seq = seqnt;
    seq.length = 14;
    EXPECT_EQ(CountNotMasked(seq), 8);
}
