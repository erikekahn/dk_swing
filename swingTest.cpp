/*
 * Copyright 2018 Erik E. Kahn
 * 
 * 
 */
#include "swing.h"
#include "gtest/gtest.h"

#include <cstdio>
using namespace dk_eek;
using namespace std;

/*
 * loads file into swingData
*/
static int loadFromFile(swingData& data, const char *file)
{
	int sampleCount = 0;
	FILE *fp = nullptr;
	unsigned long long ts;
	float ax,ay,az;
	float wx,wy,wz;

	fp = fopen(file, "r");
	if (!fp)
		return -1;

	while (EOF != fscanf(fp, "%llu,%f,%f,%f,%f,%f,%f",
		&ts, &ax, &ay, &az, &wx, &wy, &wz)) {
		swingData::Sample current(ts, ax, ay, az, wx, wy, wz);

		data.addSample(current);
		sampleCount++;
	}

	fclose(fp);
	
	return sampleCount;
}

/*
 * All test cases use the data given to me by Mike. I made up some quick and
 * dirty 'sanity' tests. 
 * 
 * Much room for improvement, needs to test more edge conditions and less 'happy path'
 * cases.
 */

TEST(swing_test, searchContinuityAboveValue)
{
	swingData test;

	ASSERT_GT(loadFromFile(test, "./latestSwing.csv"), 0);

	ASSERT_EQ((unsigned)2,
		test.searchContinuityAboveValue<unsigned long long>(
			swingData::dataCol::TIMESTAMP, 0, 80, 800, 2));

	ASSERT_EQ((unsigned)22,
		test.searchContinuityAboveValue(swingData::dataCol::Ax, 0, 80, 0.10f, 2));
}

TEST(swing_test, backSearchContinuityWithinRange)
{
	swingData test;

	ASSERT_GT(loadFromFile(test, "./latestSwing.csv"), 0);

	ASSERT_EQ((unsigned)59,
		test.backSearchContinuityWithinRange<unsigned long long>(
			swingData::dataCol::TIMESTAMP, 80, 0, 68000, 75000, 2));

	ASSERT_EQ((unsigned)74,
		test.backSearchContinuityWithinRange(swingData::dataCol::Ax, 80, 0, 0.95f, 1.00f, 2));
}


TEST(swing_test, searchContinuityAboveValueTwoSignals)
{
	swingData test;

	ASSERT_GT(loadFromFile(test, "./latestSwing.csv"), 0);

	ASSERT_EQ((unsigned)27,
		test.searchContinuityAboveValueTwoSignals(swingData::dataCol::Ax, swingData::dataCol::Wy,
			0, 80, 0.00f, -11.0f, 2));
}


TEST(swing_test, searchMultiContinuityWithinRange)
{
	swingData test;
	std::tuple<unsigned int, unsigned int> res;

	ASSERT_GT(loadFromFile(test, "./latestSwing.csv"), 0);

	res = test.searchMultiContinuityWithinRange<unsigned long long>(
			swingData::dataCol::TIMESTAMP, 0, 80, 16000, 50000, 5);

	ASSERT_EQ((unsigned)12, std::get<0>(res));
	ASSERT_EQ((unsigned)17, std::get<1>(res));

	res = test.searchMultiContinuityWithinRange<>(
			swingData::dataCol::Wz, 0, 80, -5.0f, -3.0f, 5);

	ASSERT_EQ((unsigned)10, std::get<0>(res));
	ASSERT_EQ((unsigned)15, std::get<1>(res));
}
