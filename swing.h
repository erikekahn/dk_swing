/*
 * Copyright 2018 Erik E. Kahn
 * 
 */
#pragma once

#include <vector>
#include <cstddef>
#include <tuple>

namespace dk_eek {

class swingData {
public:
	struct Vec3 {
		Vec3() {}
		Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

		float x;
		float y;
		float z;
	};

	struct Sample {
		Sample() {}
		Sample(unsigned long long _ts, float _ax, float _ay, float _az,
			float _wx, float _wy, float _wz) :
			ts(_ts),
			a(_ax, _ay, _az),
			w(_wx, _wy, _wz) {}

		unsigned long long ts;
		Vec3 a;
		Vec3 w;
	};

	enum dataCol {
		TIMESTAMP = offsetof(Sample, ts),
		Wx = offsetof(Sample, w.x),
		Wy = offsetof(Sample, w.y),
		Wz = offsetof(Sample, w.z),
		Ax = offsetof(Sample, a.x),
		Ay = offsetof(Sample, a.y),
		Az = offsetof(Sample, a.z)
	};

	
	swingData() {}
	swingData(size_t n) { data.reserve(n); }

	void addSample(Sample& s) { data.push_back(s); }

	/*
	 * searchContinuityAboveValue()
	 *
	 * Description (verbatim Mike's email): 
	 * Return the first index where data has values that meet this criteria
	 * for at least 'winLength' samples.
	 *
	 * Parameters:
	 *
	 * dataCol: Data Column we are searching in swingData (see enum above)
	 * indexBegin: Inclusive index search starts at
	 * indexEnd: Exclusive index search ends at
	 * threshold: Threshold that items must exceed.
	 * winLength: How many items must meet 'threshold' before 
	 *
	 * return: index of winLength'th match, or indexEnd on no match.
	 *
	 * Assumptions:
	 * - indexBegin < indexEnd, and not exceeding the size of the dataset.
	 * - winLength is less than the size of the dataset
	 */
	template<typename T>
	unsigned int searchContinuityAboveValue(
			dataCol col, unsigned int indexBegin,
			unsigned int indexEnd, T threshold,
			unsigned int winLength) const
	{
		unsigned int i;
		T val;
		Sample *current;

		current = (Sample *)&(data[indexBegin]);
		for (i = indexBegin; i < indexEnd; ++i) {
			val = *(T *)( ((char *)current) + col);
			
			if (val > threshold) {
				winLength--;
				if (!winLength)
					break;
			}
			current++;
		}

		return i;
	}
	

	/*
	 * backSearchContinuityWithinRange()
	 *
	 * Description (verbatim Mike's email): 
	 * from 'indexBegin' to 'indexEnd' (where 'indexBegin is larger than
	 * 'indexEnd'), search data for values that are higher than 'thresholdLo'
	 * and lower than 'thresholdHi'. Return the first index where data has values
	 * that meet this criteria for at least 'winLength' samples.
	 *
	 * Parameters:
	 *
	 * dataCol: Data Column we are searching in swingData (see enum above)
	 * indexBegin: EXCLUSIVE index search starts at (higher than indexEnd)
	 * indexEnd: INCLUSIVE index search ends at (lower than indexBegin)
	 * thresholdLo: Threshold that items must exceed.
	 * thresholdHi: Threshold that items must NOT exceed..
	 * winLength: How many items must meet 'threshold' before 
	 *
	 * return: index of winLength'th match, or indexBegin on no match.
	 *
	 * Assumptions:
	 * - indexBegin > indexEnd, and not exceeding the size of the dataset.
	 * - winLength is less than the size of the dataset
	 * - thresholdLo < thresholdHi
	 */
	template<typename T>
	unsigned int backSearchContinuityWithinRange(
			dataCol col, unsigned int indexBegin,
			unsigned int indexEnd, T thresholdLo,
			T thresholdHi, unsigned int winLength) const
	{
		unsigned int i;
		T val;
		Sample *current;

		current = (Sample *)&(data[indexBegin]);
		for (i = indexBegin; i > indexEnd; --i) {
			val = *(T *)( ((char *)current) + col);
			
			if (val > thresholdLo && val < thresholdHi) {
				winLength--;
				if (!winLength)
					break;
			}
			current--;
		}

		return i;
	}

	/*
	 * searchContinuityAboveValueTwoSignals()
	 *
	 * Description (verbatim Mike's email): 
	 * from 'indexBegin' to 'indexEnd' search 'col1' for values that are
	 * higher than 'threshold1' and also search 'col2' for values that are
	 * higher than 'threshold2'. Return the first index where both 'col1'
	 * and 'col2' have values that meet these critera for at least
	 * 'winLength' samples.
	 *
	 * Parameters:
	 *
	 * col1: First Data Column we are searching in swingData (see enum above)
	 * col2: Second Data Column we are searching in swingData (see enum above)
	 * indexBegin: Inclusive index search starts at
	 * indexEnd: Exclusive index search ends at
	 * threshold1: Threshold that items in col1 must exceed.
	 * threshold2: Threshold that items in col2 must exceed.
	 * winLength: How many items must meet 'threshold' before 
	 *
	 * return: index of winLength'th match, or indexEnd on no match.
	 *
	 * Assumptions:
	 * - indexBegin < indexEnd, and not exceeding the size of the dataset.
	 * - winLength is less than the size of the dataset
	 * - col1 != col2
	 */
	template<typename T>
	unsigned int searchContinuityAboveValueTwoSignals(
			dataCol col1, dataCol col2,
			unsigned int indexBegin, unsigned int indexEnd,
			T threshold1, T threshold2,
			unsigned int winLength) const
	{
		unsigned int i;
		T val1, val2;
		Sample *current;

		current = (Sample *)&(data[indexBegin]);
		for (i = indexBegin; i < indexEnd; ++i) {
			val1 = *(T *)( ((char *)current) + col1);
			val2 = *(T *)( ((char *)current) + col2);

			if (val1 > threshold1 && val2 > threshold2) {
				winLength--;
				if (!winLength)
					break;
			}
			current++;
		}
		
		return i;
	}

	/*
	 * searchContinuityAboveValueTwoSignals()
	 *
	 * Description (verbatim Mike's email): 
	 * From 'indexBegin' to 'indexEnd', search data for values that are
	 * higher than 'thresholdLo' and lower than 'thresholdHi'. Return the
	 * starting index and ending index of all continious samples that meet
	 * this criteria for at least 'winLength' data points.
	 *
	 * Parameters:
	 *
	 * col: Data Column we are searching in swingData (see enum above)
	 * indexBegin: Inclusive index search starts at
	 * indexEnd: Exclusive index search ends at
	 * thresholdLo: Threshold that items in col must exceed.
	 * thresholdHi: Threshold that items in col must NOT exceed.
	 * winLength: How many items must meet 'threshold' before 
	 *
	 * return: tuple of indicies showing range of elements containing 
	 * samples that continously meet threshold criteria.
	 * Returns tuple of (indexEnd,indexEnd) if no such window is found.
	 *
	 * Assumptions:
	 * - indexBegin < indexEnd, and not exceeding the size of the dataset.
	 * - winLength is less than the size of the dataset
	 * - tresholdLo < thresholdHi
	 */
	template<typename T>
	std::tuple<unsigned int,unsigned int> searchMultiContinuityWithinRange(
			dataCol col, unsigned int indexBegin,
			unsigned int indexEnd, T thresholdLo,
			T thresholdHi, unsigned int winLength) const
	{
		unsigned int i;
		T val;
		Sample *current;
		unsigned int curWindow;

		curWindow = winLength;
		current = (Sample *)&(data[indexBegin]);
		for (i = indexBegin; i < indexEnd; ++i) {
			val = *(T *)( ((char *)current) + col);
			
			if (val > thresholdLo && val < thresholdHi) {
				curWindow--;
				if (!curWindow)
					break;
			} else if (curWindow != winLength) {
				// Broke continuity - reset window
				curWindow = winLength;
			}
			current++;
		}

		if (curWindow)
			return std::make_tuple(i,i);

		return std::make_tuple(i-winLength,i);
	}

	const std::vector<Sample>& getData() const { return data; }

private:
	std::vector<Sample> data;
};

}
