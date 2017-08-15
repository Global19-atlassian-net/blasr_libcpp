/*
 * =====================================================================================
 *
 *       Filename:  FASTQReader_gtest.cpp
 *
 *    Description:  Test pbdata/FASTQReader.hpp
 *
 *        Version:  1.0
 *        Created:  10/29/2012 05:19:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan Li (yli), yli@pacificbiosciences.com
 *        Company:  Pacific Biosciences
 *
 * =====================================================================================
 */

#include "FASTQReader.hpp"
#include "gtest/gtest.h"
#include "pbdata/testdata.h"

const std::string movie = "m130328_211423_ethan_c100499512550000001823070408081371_s1_p0";
const int numSeqs = 208;

class FASTQReaderTest : public ::testing::Test
{
public:
    void SetUp()
    {
        std::string filename(fastqFile1);
        reader.Initialize(filename);
    }

    void TearDown()
    {
        reader.Close();
        seq.Free();
    }

    FASTQReader reader;
    FASTQSequence seq;
};

TEST_F(FASTQReaderTest, GetNext)
{
    reader.GetNext(seq);
    EXPECT_EQ(std::strcmp(seq.title, std::string(movie + "/8").c_str()), 0);
    EXPECT_EQ(seq.length, 752u);
    std::string expected_seq(
        "AATAAAAAAAAAAGAAAGCTTCGAAGTGAGCGAATTACTCTCAGGCAACT"
        "GCGGGTGAAGCCAGAGCAGGCATGATGACACTGGGGAATTTACGCAAATT"
        "TTACCATTGAATTTACACATGCGATGTGCTGGAATGCGGAAGACGGAAAC"
        "GAAACCAGCAATACATCAAACGCCGCACCAGAGAAGAGATATTTGCGCCC"
        "TAAACTAGGTAAGGCGGTTGACTTGAACAGCAAATCAAACGTCAACGAGC"
        "AGCGTGAGTATATACAAGTTATCTCGGATGGAGAACGTATTCTAAATGTA"
        "AGCACGAATCCCGGAAGAGGAAACCAGTTTCTTGGTTTTTCGCCATCCTC"
        "GAAGACCTGTTACAAACCGCACTGGACCTGGAAAGTTTCTGCGCGTAATC"
        "GACAAGACTAGTAACTATCGACATCAACCATCGATTACGGGTTGGGTCAA"
        "TGGGTTCAGATGCAGGTGAGTATCCTTCATATGATAGTCTGACGCTGGCA"
        "TTCGCTCAAAGGAAGTAGACGGTTTTGTAAATAGAAACGCTTGTGAAAAG"
        "CTGAATTTCGCGTCGTCTTCCAGCGATGCAGAGCTGTAGTAGTTCAGATG"
        "ATGACCGTTACTCAAAGTGCCTGCAACGGCTCGGGGCGTGCGCGTCCTGT"
        "GGTGGCTGCTTTTGTTGCGCTGTTTGCAGTGTATGGTTGTCGGGTGATGT"
        "TGCCTGCAAACCCACAAAACCCCACACACACAACAGTTGGGTTGTTGATT"
        "GG");
    std::string expected_qual("(,)'(')''++),.$\"+*$'--.-/+&.$-./$',-.&#'/,.,)-,--,");

    for (size_t i = 0; i < seq.length; i++) {
        EXPECT_EQ(seq.seq[i], expected_seq[i]);
    }

    for (size_t i = 0; i < expected_qual.size(); i++) {
        EXPECT_EQ(seq.qual[i] + FASTQSequence::charToQuality, expected_qual[i]);
    }

    reader.GetNext(seq);
    EXPECT_EQ(strcmp(seq.title, std::string(movie + "/9").c_str()), 0);

    // Continue to read
    for (int i = 2; i < numSeqs; i++) {
        EXPECT_TRUE(reader.GetNext(seq));
    }
    EXPECT_EQ(std::strcmp(seq.title, std::string(movie + "/249").c_str()), 0);

    // Can not proceed.
    EXPECT_FALSE(reader.GetNext(seq));
}
