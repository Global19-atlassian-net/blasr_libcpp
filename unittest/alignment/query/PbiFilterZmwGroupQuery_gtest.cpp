#include <string>

#include <gtest/gtest.h>

#include <alignment/query/PbiFilterZmwGroupQuery.h>
#include <pbdata/testdata.h>

using namespace PacBio;
using namespace PacBio::BAM;

static const std::string testChunking = xmlFile1;
static const std::string testNoFilter = xmlFile2;

static void TestPbiFilterZmwGroupQuery(const std::string& fn, const std::vector<size_t>& expected,
                                       const int32_t min_zmw, const int32_t max_zmw)
{
    EXPECT_NO_THROW({
        PbiFilterZmwGroupQuery qQuery(fn);

        std::vector<size_t> counts;
        for (const std::vector<BamRecord>& records : qQuery) {
            counts.push_back(records.size());
            EXPECT_GT(records.size(), 0u);
            std::string movieName = records[0].MovieName();
            int32_t holeNumber = records[0].HoleNumber();
            EXPECT_TRUE(holeNumber >= min_zmw);
            EXPECT_TRUE(holeNumber <= max_zmw);
            for (const BamRecord& record : records) {
                EXPECT_EQ(holeNumber, record.HoleNumber());
                EXPECT_EQ(movieName, record.MovieName());
            }
        }
        EXPECT_EQ(expected, counts);
    });
}

static void TestNoneConstPbiFilterZmwGroupQuery(const std::string& fn,
                                                const std::vector<size_t>& expected,
                                                const int32_t min_zmw, const int32_t max_zmw)
{
    EXPECT_NO_THROW({
        PbiFilterZmwGroupQuery qQuery(fn);

        std::vector<size_t> counts;
        for (std::vector<BamRecord>& records : qQuery) {
            counts.push_back(records.size());
            EXPECT_GT(records.size(), 0u);
            std::string movieName = records[0].MovieName();
            int32_t holeNumber = records[0].HoleNumber();
            EXPECT_TRUE(holeNumber >= min_zmw);
            EXPECT_TRUE(holeNumber <= max_zmw);
            for (BamRecord& record : records) {
                EXPECT_EQ(holeNumber, record.HoleNumber());
                EXPECT_EQ(movieName, record.MovieName());
            }
        }
        EXPECT_EQ(expected, counts);
    });
}

TEST(PbiFilterZmwGroupQueryTest, GetNext)
{
    std::string fn = testChunking;
    std::vector<size_t> expected({2, 21, 13, 1, 5, 13, 1, 34, 12, 2, 20, 5, 3, 7, 11});
    const int32_t min_zmw = 55;
    const int32_t max_zmw = 1816;
    TestPbiFilterZmwGroupQuery(fn, expected, min_zmw, max_zmw);
    TestNoneConstPbiFilterZmwGroupQuery(fn, expected, min_zmw, max_zmw);
}

TEST(PbiFilterZmwGroupQueryTest, NoFilter)
{
    std::string fn = testNoFilter;
    std::vector<size_t> expected(
        {2,  21, 13, 1,  5,  13, 1,  34, 12, 2,  20, 5,  3,  7,  11, 14, 6,  8,  23, 53, 17, 21, 7,
         5,  35, 3,  26, 6,  21, 37, 26, 59, 2,  6,  30, 34, 32, 2,  14, 3,  24, 1,  15, 1,  12, 26,
         6,  3,  1,  9,  3,  21, 12, 10, 24, 3,  6,  1,  6,  17, 34, 11, 24, 4,  11, 1,  10, 8,  10,
         20, 3,  4,  6,  27, 5,  2,  21, 3,  14, 1,  9,  5,  30, 37, 6,  1,  26, 7,  7,  32});
    const int32_t min_zmw = 0;
    const int32_t max_zmw = 1000000;
    TestPbiFilterZmwGroupQuery(fn, expected, min_zmw, max_zmw);
    TestNoneConstPbiFilterZmwGroupQuery(fn, expected, min_zmw, max_zmw);
}
