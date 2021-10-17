#pragma once

#include <cstdint>

// Repack bytes to 10-bits words, returns word count
int repackBytesTo10bits(uint8_t *bytes, int byte_length, uint16_t *words);

// Repack bytes to 12-bits words, returns word count
int repackBytesTo12bits(uint8_t *bytes, int byte_length, uint16_t *words);

// Repack bytes to 16-bits words, returns word count
int repackBytesTo16bits(uint8_t *bytes, int byte_length, uint16_t *words);
