#ifndef KISS_FFT_LOG_H
#define KISS_FFT_LOG_H

#include <stdio.h>
#include <stdlib.h>

// Define error handling macro
#define KISS_FFT_ERROR(msg) do { fprintf(stderr, "Error: %s\n", msg); exit(1); } while(0)

// Disable debug and warning logs in release builds
#define KISS_FFT_WARNING(...)
#define KISS_FFT_DEBUG(...)
#define KISS_FFT_LOG(...)

#endif // KISS_FFT_LOG_H