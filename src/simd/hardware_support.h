#ifndef HARDWARE_SUPPORT_H
#define HARDWARE_SUPPORT_H

void supported_SIMD()
{
   if (__builtin_cpu_supports("sse"))
    {
        // Allow sse features
    }
    if (__builtin_cpu_supports("sse2"))
    {
        // Allow sse2 features
    }
    if (__builtin_cpu_supports("sse3"))
    {
        // Allow sse3 features
    }
    if (__builtin_cpu_supports("sse4.1"))
    {
        // Allow sse4.1 features
    }
    if (__builtin_cpu_supports("sse4.2"))
    {
        // Allow sse4.2 features
    }
    if (__builtin_cpu_supports("fma4"))
    {
        // Allow fma4 features
    }
    if (__builtin_cpu_supports("avx"))
    {
        // Allow avx features
    }
    if (__builtin_cpu_supports("avx2"))
    {
        // Allow avx2 features
    }
    if (__builtin_cpu_supports("avx512f"))
    {
        // Allow avx512f features
    }
}

#endif
