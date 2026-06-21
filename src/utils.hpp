#pragma once

#include <godot_cpp/core/type_info.hpp>

// Taken from Godot's core/io/marshalls.h
static inline uint16_t decode_uint16(const uint8_t *p_arr) {
	uint16_t u = 0;

	for (int i = 0; i < 2; i++) {
		uint16_t b = *p_arr;
		b <<= (i * 8);
		u |= b;
		p_arr++;
	}

	return u;
}
