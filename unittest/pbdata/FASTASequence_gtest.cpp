/*
 * =====================================================================================
 *
 *       Filename:  FASTASequence_gtest.cpp
 *
 *    Description:  Test pbdata/FASTASequence.hpp
 *
 *        Version:  1.0
 *        Created:  10/29/2012 05:19:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan Li (yli), yli@pacificbiosciences.com
 *        Company:  Pacific Biosciences
 *
 * =====================================================================================
 */

#include "FASTASequence.hpp"
#include "gtest/gtest.h"
#include <climits>
#include <iostream>
#include <fstream>


using namespace std;

class FASTASequenceTest : public ::testing::Test {
public:
    virtual void SetUp() {
    }

    virtual void TearDown() {
        fastaOne.Free();
        fastaTwo.Free();
        fastaThree.Free();
    }

    FASTASequence fastaOne;
    FASTASequence fastaTwo;
    FASTASequence fastaThree;

    std::streambuf * sbuf;
    ofstream ofs;
};


//Test FASTASequence 
TEST_F(FASTASequenceTest, ALLFUNC) {
    // Test constructor 
    EXPECT_TRUE(fastaOne.title == NULL);
    EXPECT_TRUE(fastaOne.titleLength == 0);
    EXPECT_TRUE(fastaOne.seq == NULL);
    EXPECT_TRUE(fastaOne.length == 0);
    EXPECT_FALSE(fastaOne.deleteOnExit);
    EXPECT_EQ(fastaOne.GetStorageSize(), 0);

    DNASequence dna;
    Nucleotide thisNuc[] = "ATGCATGCTC";
    dna.seq = thisNuc;
    dna.length = 10;

    int titleLength = 22;
    string title("fasta_seq_one comments");
    fastaOne.title = new char [titleLength];
    memcpy(fastaOne.title, title.c_str(), titleLength);

    fastaOne.titleLength = titleLength;

    EXPECT_EQ(fastaOne.GetName(), string("fasta_seq_one"));
    fastaOne.seq = thisNuc;
    fastaOne.length = 10;

    // use ShallowCopy carefully, since title may double free
    // fastaTwo.ShallowCopy(fastaOne);
    // EXPECT_EQ(fastaTwo.seq, fastaOne.seq);
    // EXPECT_EQ(fastaTwo.title, fastaOne.title);


    // Test AppendToTitle
    fastaOne.AppendToTitle(string("XXX"));
    EXPECT_EQ(fastaOne.titleLength, 26);

    string newTitle = "fasta_seq_one commentsXXX";
    EXPECT_STREQ(fastaOne.title, newTitle.c_str());


    // Test ReverseComplementSelf()
    fastaOne.ReverseComplementSelf();
    string rcSeq = "GAGCATGCAT";
    for (int i = 0; i < rcSeq.size(); i++) {
        EXPECT_EQ(fastaOne.seq[i], rcSeq[i]);
    }

    // Test operator =
    fastaTwo=fastaOne;
    EXPECT_NE(fastaOne.title, fastaTwo.title);
    EXPECT_EQ(fastaOne.titleLength, fastaTwo.titleLength);
    EXPECT_STREQ(fastaOne.title, fastaTwo.title);
    EXPECT_NE(fastaOne.seq, fastaTwo.seq);
    for (int i = 0; i < fastaOne.length; i++) {
        EXPECT_EQ(fastaOne.seq[i], fastaTwo.seq[i]);
    }

    // Test MakeRC(rhs&)
    fastaOne.MakeRC(fastaThree);

    // Test PrintSeq
    stringstream ss;
    fastaThree.PrintSeq(ss);
    string thisTitle, thisComment, thisSeq;
    ss >> thisTitle;
    ss >> thisComment;
    ss >> thisSeq;

    EXPECT_EQ(thisTitle, ">fasta_seq_one");
    EXPECT_EQ(thisComment, "commentsXXX");
    EXPECT_EQ(thisSeq, "ATGCATGCTC");
}


TEST_F(FASTASequenceTest, CopyFromString) {
    string name = "read_name";
    string seq = "ATGGGCGC";
    fastaOne.Copy(name, seq);
    EXPECT_EQ(fastaOne.title, name);
    EXPECT_EQ(fastaOne.length, seq.size());
    EXPECT_EQ(fastaOne.deleteOnExit, true);
    for(int i = 0; i < fastaOne.length; i++) {
        EXPECT_EQ(fastaOne.seq[i], seq[i]);
    }

    // Copy sequence from another string. 
    string seq2 = "GGTTGTG";
    fastaOne.Copy(seq2);
    // Name not changed.
    EXPECT_EQ(fastaOne.title, name);
    EXPECT_EQ(fastaOne.length, seq2.size());
    EXPECT_EQ(fastaOne.deleteOnExit, true);
    for(int i = 0; i < fastaOne.length; i++) {
        EXPECT_EQ(fastaOne.seq[i], seq2[i]);
    }
}

TEST_F(FASTASequenceTest, ReverseComplementSelf) {
    const std::string name = "fasta-seq-name";
    const std::string s = "TTAAGG";
    const std::string r = "CCTTAA";
    fastaOne.Copy(name, s);

    EXPECT_EQ(fastaOne.title, name);
    EXPECT_EQ(fastaOne.ReverseComplementSelf().ToString(), r);
    EXPECT_EQ(fastaOne.title, name);
    EXPECT_EQ(fastaOne.ReverseComplementSelf().ToString(), s);

    FASTASequence fastaTwo;
    fastaTwo.ReferenceSubstring(fastaOne, 1, 3);
    fastaTwo.CopyTitle(fastaOne.title);

    EXPECT_EQ(fastaTwo.deleteOnExit, false);
    EXPECT_EQ(fastaTwo.title, name);
    EXPECT_EQ(fastaTwo.ToString(), "TAA");
    EXPECT_EQ(fastaTwo.ReverseComplementSelf().ToString(), "TTA");
    EXPECT_EQ(fastaTwo.title, name);
    EXPECT_EQ(fastaTwo.deleteOnExit, true);
}
