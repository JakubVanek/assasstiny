#ifndef HEX_H
#define HEX_H

// convert lower nibble of byte to hex char (unsanitized)
inline static char nibbleToHex(uint8_t nibble) {
	return (nibble < 10) ? (nibble + '0') : (nibble - 10 + 'a'); // add right chars
}
// convert upper nibble of byte to hex char
inline static char upperToHex (uint8_t byte) {
	return nibbleToHex(byte >> 4); // shift upper nibble to lower half
}
// convert lower nibble of byte to hex char
inline static char lowerToHex (uint8_t byte) {
	return nibbleToHex(byte & 0xF); // mask upper nibble
}

#endif
