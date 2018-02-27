#include "src/encode3f.c"

#include "constants/3dFloat.h"
#include "utils/hash32.h"
#include "zfpEndtoendBase.c"

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(when_seededRandomSmoothDataGenerated_expect_ChecksumMatches),

    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedPrecision_expect_BitstreamChecksumMatches, setupFixedPrec0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedPrecision_expect_ArrayChecksumMatches, setupFixedPrec0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedPrecision_expect_BitstreamChecksumMatches, setupFixedPrec1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedPrecision_expect_ArrayChecksumMatches, setupFixedPrec1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedPrecision_expect_BitstreamChecksumMatches, setupFixedPrec2, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedPrecision_expect_ArrayChecksumMatches, setupFixedPrec2, teardown),

    cmocka_unit_test(given_3dFloatZfpStream_when_SetRateWithWriteRandomAccess_expect_RateRoundedUpProperly),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedRate_expect_CompressedBitrateComparableToChosenRate, setupFixedRate0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedRate_expect_CompressedBitrateComparableToChosenRate, setupFixedRate1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedRate_expect_BitstreamChecksumMatches, setupFixedRate2, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedRate_expect_ArrayChecksumMatches, setupFixedRate2, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedRate_expect_CompressedBitrateComparableToChosenRate, setupFixedRate2, teardown),

    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedAccuracy_expect_BitstreamChecksumMatches, setupFixedAccuracy0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedAccuracy_expect_ArrayChecksumMatches, setupFixedAccuracy0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedAccuracy_expect_CompressedValuesWithinAccuracy, setupFixedAccuracy0, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedAccuracy_expect_BitstreamChecksumMatches, setupFixedAccuracy1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedAccuracy_expect_ArrayChecksumMatches, setupFixedAccuracy1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedAccuracy_expect_CompressedValuesWithinAccuracy, setupFixedAccuracy1, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedAccuracy_expect_BitstreamChecksumMatches, setupFixedAccuracy2, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpDecompressFixedAccuracy_expect_ArrayChecksumMatches, setupFixedAccuracy2, teardown),
    cmocka_unit_test_setup_teardown(given_3dFloatArray_when_ZfpCompressFixedAccuracy_expect_CompressedValuesWithinAccuracy, setupFixedAccuracy2, teardown),
  };

  return cmocka_run_group_tests(tests, setupRandomData, teardownRandomData);
}
