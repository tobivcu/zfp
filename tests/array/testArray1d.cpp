#include "array/zfparray1.h"
using namespace zfp;

extern "C" {
  #include "constants/1dDouble.h"
  #include "utils/rand64.h"
  #include "utils/hash64.h"
  #include "utils/genSmoothRandNums.h"
};

#include "gtest/gtest.h"

#define MIN_TOTAL_ELEMENTS 1000000
#define DIMS 1

#define RATE (1u << 5)

size_t inputDataSideLen, inputDataTotalLen;
double* inputDataArr;

class Array1dTestEnv : public ::testing::Environment {
public:
  virtual void SetUp() {
    generateSmoothRandDoubles(MIN_TOTAL_ELEMENTS, DIMS, (double**)&inputDataArr, &inputDataSideLen, &inputDataTotalLen);
  }

  virtual void TearDown() {
    free(inputDataArr);
  }
};

class Array1dTest : public ::testing::Test {};

TEST_F(Array1dTest, when_generateRandomData_then_checksumMatches)
{
  EXPECT_EQ(CHECKSUM_ORIGINAL_DATA_ARRAY, hashArray((uint64*)inputDataArr, inputDataTotalLen, 1));
}

TEST_F(Array1dTest, given_dataset_when_set_then_underlyingBitstreamChecksumMatches)
{
  array1d arr(inputDataTotalLen, RATE);

  uint64 expectedChecksum = CHECKSUM_FR_COMPRESSED_BITSTREAM_2;
  uint64 checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_NE(expectedChecksum, checksum);

  arr.set(inputDataArr);

  checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_EQ(expectedChecksum, checksum);
}

TEST_F(Array1dTest, given_setArray1d_when_get_then_decompressedValsReturned)
{
  array1d arr(inputDataTotalLen, RATE);
  arr.set(inputDataArr);

  double* decompressedArr = new double[inputDataTotalLen];
  arr.get(decompressedArr);

  uint64 expectedChecksum = CHECKSUM_FR_DECOMPRESSED_ARRAY_2;
  uint64 checksum = hashArray((uint64*)decompressedArr, inputDataTotalLen, 1);
  EXPECT_EQ(expectedChecksum, checksum);

  delete[] decompressedArr;
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::Environment* const foo_env = ::testing::AddGlobalTestEnvironment(new Array1dTestEnv);
  return RUN_ALL_TESTS();
}
