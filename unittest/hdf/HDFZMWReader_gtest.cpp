/*
 * ==========================================================================
 *
 *       Filename:  HDFZMWReader_gtest.cpp
 *
 *    Description:  Test hdf/HDFZMWReader.hpp
 *
 *        Version:  1.0
 *        Created:  08/21/2013 07:00:49 PM
 *       Revision:  08/20/2014  
 *       Compiler:  gcc
 *
 *         Author:  Yuan Li (yli), yli@pacificbiosciences.com
 *        Company:  Pacific Biosciences
 *
 * ==========================================================================
 */
#include "HDFZMWReader.hpp"
#include "gtest/gtest.h"
#include "pbdata/testdata.h"

using namespace H5;

class HDFZMWReaderTEST : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    void Initialize(H5File &pbihdfFile, std::string fileName, std::string groupName, HDFGroup &callsGroup)
    {
        try {
            FileAccPropList propList;
            pbihdfFile.openFile(fileName.c_str(), H5F_ACC_RDONLY, propList);
        } catch (Exception &e) {
            std::cout << "ERROR, could not open hdf file" << fileName << ", exiting." << std::endl;
            exit(EXIT_FAILURE);
        }

        ASSERT_NE(callsGroup.Initialize(pbihdfFile, groupName), 0);

        /*
        HDFGroup rootGroup, pulseDataGroup;
        if (rootGroup.Initialize(pbihdfFile, "/") == 0) {
            std::cout << "ERROR, could not open /" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pulseDataGroup.Initialize(rootGroup, "PulseData") == 0){
            std::cout << "ERROR, could not open /PulseData" << std::endl;
            exit(EXIT_FAILURE);
        }

        ASSERT_NE(callsGroup.Initialize(pulseDataGroup, "BaseCalls"), 0);
        */
    }

    void Close(H5File &pbihdfFile, HDFGroup &callsGroup)
    {
        pbihdfFile.close();
        callsGroup.Close();
    }

    void TestGetNext(HDFZMWReader &zmwReader)
    {
        int count = 0;
        ZMWGroupEntry entry;
        while (zmwReader.GetNext(entry)) {
            count++;
            ASSERT_EQ(zmwReader.curZMW, count);
        }
        ASSERT_EQ(count, zmwReader.nZMWEntries);
    }
};

TEST_F(HDFZMWReaderTEST, ReadZMWFromBaseCalls)
{
    HDFZMWReader zmwReader;
    std::string fileName = baxFile2;
    std::string groupName = "/PulseData/BaseCalls";
    H5File pbihdfFile;
    HDFGroup baseCallsGroup;

    Initialize(pbihdfFile, fileName, groupName, baseCallsGroup);
    ASSERT_NE(zmwReader.Initialize(&baseCallsGroup), 0);

    TestGetNext(zmwReader);
    zmwReader.Close();
    Close(pbihdfFile, baseCallsGroup);
}

TEST_F(HDFZMWReaderTEST, ReadZMWFromPulseCalls)
{
    std::string fileName = plsFile1;
    H5File pbihdfFile;
    try {
        FileAccPropList propList;
        pbihdfFile.openFile(fileName.c_str(), H5F_ACC_RDONLY, propList);
    } catch (Exception &e) {
        std::cout << "ERROR, could not open hdf file" << fileName << ", exiting." << std::endl;
        exit(EXIT_FAILURE);
    }

    HDFGroup pulseCallsGroup;
    ASSERT_NE(pulseCallsGroup.Initialize(pbihdfFile, "/PulseData/PulseCalls"), 0);

    HDFZMWReader zmwReader;
    ASSERT_NE(zmwReader.Initialize(&pulseCallsGroup), 0);
    TestGetNext(zmwReader);
    zmwReader.Close();

    // Switch to read from CCS ZMW.
    HDFGroup ccsGroup;
    ASSERT_NE(ccsGroup.Initialize(pbihdfFile, "/PulseData/ConsensusBaseCalls"), 0);
    ASSERT_NE(zmwReader.Initialize(&ccsGroup), 0);
    TestGetNext(zmwReader);
    zmwReader.Close();

    // Close
    ccsGroup.Close();
    pulseCallsGroup.Close();
    pbihdfFile.close();
}
