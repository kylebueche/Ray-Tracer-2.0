
// Experimental: Unsure how the intrinsic type __m128 & __m128i handle being "stored" in a class
//
// These types might be able to be stored, but ideally they live in the cache, the only constraint
// being the number of registers available.
//
// In the case that this is slower:
// It is likely messing with caching, writing and reading the state to and from real memory too often
// Then the boilerplate of state and expFactor being stored explicitely and externally will be worth it
//
// In the case that this is faster:
// It could, by storing the random state, free up the cache to work on other parts
class Rand128
{
    public:
        Rand128()
        {
            state = _mm_set_epi32(0x12345678, 0x87654321, 0xabcdef12, 0x12fedcba);
            mantissaShift = 32 - FLT_MANT_DIG;
            expFactor = _mm_set_ps1(1.0f / (UINT32_C(1) << FLT_MANT_DIG));
        }
        
        __m128i nextInt()
        {
            xorshift();
            return state;
        }

        __m128 nextFloat()
        {
            xorshift();
            return _mm_mul_ps(_mm_castsi128_ps(_mm_srli_epi32(state, mantissaShift)), expFactor);
        }
        
    private:
        __m128i state;
        __m128 expFactor;
        int mantissaShift;

        void xorshift()
        {
            state = _mm_xor_si128(state, _mm_slli_epi32(state, 13));
            state = _mm_xor_si128(state, _mm_srli_epi32(state, 17));
            state = _mm_xor_si128(state, _mm_slli_epi32(state, 5));
        }
}
