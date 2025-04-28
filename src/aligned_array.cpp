/************************************************************
 * Author: Kyle Bueche
 * 
 * Implementation for the aligned array class.
 * 
 * Typical SIMD load calls requires aligned memory addresses.
 * For example, 128-bit loads must pass a pointer that is
 * 16-byte aligned, and 16 bytes after that pointer must
 * be owned memory
 *
 * This class is implemented in a way that the true
 * length is a multiple of the passed alignment of
 * bytes, and is greater than or equal to the passed
 * length.
 *
 * Trailing values are initialized to 0 for SIMD math safety
 * 
 * Subclasses are defined that align the internal array to
 * 128 bits, 256 bits, and 512 bits respectively.
 **********************************************************/

#include "aligned_array.h"

/**********************************************************************************
 * Allocates an aligned array of at least the specified length of elements.
 *
 * Params:
 * byte_alignment - The number of bytes to align the memory to, MUST be a power of 2
 *         length - The minimum number of array elements
 ***********************************************************************************/
AlignedArray::AlignedArray(int byte_alignment, int length)
{
    // Calculate the minimum multiple of byte_alignment needed to accomodate length
    size_t bytes_per_element = sizeof(T);
    size_t bytes_requested = sizeof(T) * length;
    size_t leftover_bytes = bytes_requested % byte_alignment;
    size_t bytes_to_allocate = bytes_requested - leftover_bytes + byte_alignment;

    // Allocate memory and set internal values
    this->data = aligned_alloc(byte_alignment, bytes_to_allocate);
    this->length = bytes_to_allocate / sizeof(T);
    this->alignment = alignment;

    // Initialize unrequested values to 0, for SIMD math safety
    memset(&this->data[length], 0, this->length - length);
}

// Standard allignment for 128 bit, 256 bit, and 512 bit SIMD registers:
void AllignedArray128::AllignedArray128(int length)
{
    AllignedArray::AllignedArray(16, length);
}

void AllignedArray256::AllignedArray256(int length)
{
    AllignedArray::AllignedArray(32, length);
}

void AllignedArray512::AllignedArray256(int length)
{
    AllignedArray::AllignedArray(64, length);
}

// Getters:
AlignedArray::~AlignedArray()
{
    free(this->data);
}

int AlignedArray::length()
{
    return this->length;
}

int AllignedArray::alignment()
{
    return this->alignment;
}

T* AllignedArray::data()
{
    return this->data;
}

