#ifndef ALIGNED_ARRAY
#define ALIGNED_ARRAY

/**********************************************************************
 * File: aligned_array.h
 *
 * Author: Kyle Bueche
 *
 * Stores an array aligned to a certain number of bytes.
 *
 * Intended to be used with throughput-oriented SIMD functions in
 * SOA data structures.
 *********************************************************************/

template <typename T>
class AlignedArray
{
    private:
        T *data;
        int length;

    public:
        AlignedArray(int length);
        ~AlignedArray();
        int length();
        int alignment();
        T* data();
};

class AlignedArray128 : public AlignedArray {};
class AlignedArray256 : public AlignedArray {};
class AlignedArray512 : public AlignedArray {};
