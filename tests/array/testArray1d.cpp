#include "array/zfparray1.h"
using namespace zfp;

extern "C" {
  #include "constants/1dDouble.h"
  #include "utils/rand64.h"
  #include "utils/hash64.h"
  #include "utils/genSmoothRandNums.h"
};

#include "gtest/gtest.h"
#include "utils/predicates.h"

#define MIN_TOTAL_ELEMENTS 1000000
#define DIMS 1

const double VAL = 4;

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

class Array1dTest : public ::testing::TestWithParam<int> {
protected:
  virtual void SetUp() {
    bitstreamChecksums_[0] = CHECKSUM_FR_8_COMPRESSED_BITSTREAM;
    bitstreamChecksums_[1] = CHECKSUM_FR_16_COMPRESSED_BITSTREAM;
    bitstreamChecksums_[2] = CHECKSUM_FR_32_COMPRESSED_BITSTREAM;

    decompressedChecksums_[0] = CHECKSUM_FR_8_DECOMPRESSED_ARRAY;
    decompressedChecksums_[1] = CHECKSUM_FR_16_DECOMPRESSED_ARRAY;
    decompressedChecksums_[2] = CHECKSUM_FR_32_DECOMPRESSED_ARRAY;
  }

  double getRate() { return 1u << (GetParam() + 3); }

  uint64 getExpectedBitstreamChecksum() { return bitstreamChecksums_[GetParam()]; }

  uint64 getExpectedDecompressedChecksum() { return decompressedChecksums_[GetParam()]; }

  uint64 bitstreamChecksums_[3], decompressedChecksums_[3];
};

TEST_F(Array1dTest, when_constructorCalled_then_rateSetWithWriteRandomAccess)
{
  double rate = ZFP_RATE_PARAM_BITS;
  array1d arr(inputDataTotalLen, rate);
  EXPECT_LT(rate, arr.rate());
}

TEST_F(Array1dTest, when_generateRandomData_then_checksumMatches)
{
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, CHECKSUM_ORIGINAL_DATA_ARRAY, hashArray((uint64*)inputDataArr, inputDataTotalLen, 1));
}

// with write random access in 1d, fixed-rate params rounded up to multiples of 16
INSTANTIATE_TEST_CASE_P(instatiationName, Array1dTest, ::testing::Values(1, 2));

TEST_P(Array1dTest, given_dataset_when_set_then_underlyingBitstreamChecksumMatches)
{
  array1d arr(inputDataTotalLen, getRate());

  uint64 expectedChecksum = getExpectedBitstreamChecksum();
  uint64 checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectNeqPrintHexPred, expectedChecksum, checksum);

  arr.set(inputDataArr);

  checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, expectedChecksum, checksum);
}

TEST_P(Array1dTest, given_setArray1d_when_get_then_decompressedValsReturned)
{
  array1d arr(inputDataTotalLen, getRate());
  arr.set(inputDataArr);

  double* decompressedArr = new double[inputDataTotalLen];
  arr.get(decompressedArr);

  uint64 expectedChecksum = getExpectedDecompressedChecksum();
  uint64 checksum = hashArray((uint64*)decompressedArr, inputDataTotalLen, 1);
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, expectedChecksum, checksum);

  delete[] decompressedArr;
}

TEST_F(Array1dTest, when_setEntryWithEquals_then_entrySetInCacheOnly)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);

  // compressed_data() automatically flushes cache, so call it before setting entries
  uchar* bitstreamPtr = arr.compressed_data();
  size_t bitstreamLen = arr.compressed_size();
  uint64 initializedChecksum = hashBitstream((uint64*)bitstreamPtr, bitstreamLen);

  arr(0) = 4;
  uint64 checksum = hashBitstream((uint64*)bitstreamPtr, bitstreamLen);

  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, initializedChecksum, checksum);
}

TEST_F(Array1dTest, given_setEntryWithEquals_when_flushCache_then_entryWrittenToBitstream)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);

  uint64 initializedChecksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());

  arr(0) = 4;
  uint64 checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());

  EXPECT_PRED_FORMAT2(ExpectNeqPrintHexPred, initializedChecksum, checksum);
}

TEST_F(Array1dTest, when_getIndex_then_refReturned)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);

  arr(0) = VAL;

  EXPECT_EQ(VAL, arr(0));
}

TEST_F(Array1dTest, when_setEntryWithAnotherEntryValue_then_valueSet)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  arr(1) = arr(0);

  EXPECT_EQ(VAL, arr(1));
}

TEST_F(Array1dTest, when_plusEqualsOnEntry_then_valueSet)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;
  arr(1) = VAL;

  arr(1) += arr(0);

  EXPECT_EQ(2 * VAL, arr(1));
}

TEST_F(Array1dTest, when_minusEqualsOnEntry_then_valueSet)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;
  arr(1) = VAL;

  arr(1) -= arr(0);

  EXPECT_EQ(0, arr(1));
}

TEST_F(Array1dTest, when_timesEqualsOnEntry_then_valueSet)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;
  arr(1) = VAL;

  arr(1) *= arr(0);

  EXPECT_EQ(VAL * VAL, arr(1));
}

TEST_F(Array1dTest, when_divideEqualsOnEntry_then_valueSet)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;
  arr(1) = VAL;

  arr(1) /= arr(0);

  EXPECT_EQ(1, arr(1));
}

TEST_F(Array1dTest, when_swapTwoEntries_then_valuesSwapped)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  swap(arr(0), arr(1));

  EXPECT_EQ(0, arr(0));
  EXPECT_EQ(VAL, arr(1));
}

TEST_F(Array1dTest, given_entryPointer_when_dereference_then_originalValueReturned)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  double d = *(&arr(0));

  EXPECT_EQ(VAL, d);
}

TEST_F(Array1dTest, given_entryPointer_when_postIncrement_then_ptrAdvancedAfterEval)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(1) = VAL;

  array1d::pointer p = &arr(0);
  double d = *p++;

  EXPECT_EQ(0, d);
  EXPECT_EQ(VAL, *p);
}

TEST_F(Array1dTest, given_entryPointer_when_postDecrement_then_ptrAdvancedAfterEval)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  array1d::pointer p = &arr(1);
  double d = *p--;

  EXPECT_EQ(0, d);
  EXPECT_EQ(VAL, *p);
}

TEST_F(Array1dTest, given_entryPointer_when_preIncrement_then_ptrAdvancedBeforeEval)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(1) = VAL;

  array1d::pointer p = &arr(0);

  EXPECT_EQ(VAL, *++p);
}

TEST_F(Array1dTest, given_entryPointer_when_preDecrement_then_ptrAdvancedBeforeEval)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  array1d::pointer p = &arr(1);

  EXPECT_EQ(VAL, *--p);
}

TEST_F(Array1dTest, given_entryPointer_when_addToPointer_then_ptrAdvanced)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(2) = VAL;

  array1d::pointer p = &arr(0);
  p = p + 2;

  EXPECT_EQ(VAL, *p);
}

TEST_F(Array1dTest, given_entryPointer_when_pointerPlusEquals_then_ptrAdvanced)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(2) = VAL;

  array1d::pointer p = &arr(0);
  p += 2;

  EXPECT_EQ(VAL, *p);
}

TEST_F(Array1dTest, given_entryPointer_when_subtractFromPointer_then_ptrMovedBack)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  array1d::pointer p = &arr(2);
  p = p - 2;

  EXPECT_EQ(VAL, *p);
}

TEST_F(Array1dTest, given_entryPointer_when_pointerMinusEquals_then_ptrMovedBack)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);
  arr(0) = VAL;

  array1d::pointer p = &arr(2);
  p -= 2;

  EXPECT_EQ(VAL, *p);
}

TEST_F(Array1dTest, given_entryPointers_when_subtractPointers_then_resultIsEntryDifference)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);

  int i2 = 2;
  int i1 = 0;
  array1d::pointer p2 = &arr(i2);
  array1d::pointer p1 = &arr(i1);

  EXPECT_EQ(i2 - i1, p2 - p1);
}

TEST_F(Array1dTest, given_sameEntryPointers_when_compareForEquality_then_resultTrue)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);

  int i = 0;
  array1d::pointer p1 = &arr(i) + 2;
  array1d::pointer p2 = &arr(i + 2);

  EXPECT_TRUE(p1 == p2);
}

TEST_F(Array1dTest, given_differentEntryPointers_when_compareForInequality_then_resultTrue)
{
  array1d arr(8, ZFP_RATE_PARAM_BITS);

  int i = 0;
  array1d::pointer p1 = &arr(i);
  array1d::pointer p2 = &arr(i + 2);

  EXPECT_TRUE(p1 != p2);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::Environment* const foo_env = ::testing::AddGlobalTestEnvironment(new Array1dTestEnv);
  return RUN_ALL_TESTS();
}
