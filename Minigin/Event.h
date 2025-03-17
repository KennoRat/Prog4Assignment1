#pragma once
#include <cstdint>

namespace dae
{
	struct EventArg {};

	struct Event final
	{
		const unsigned int id;

		static const uint8_t MAX_ARGS = 6;
		uint8_t numArgs;
		EventArg args[MAX_ARGS] = {};

		explicit Event(const unsigned int _id) : id{ _id } {};
	};
}

template <int length> struct sdbm_hash
{
	constexpr static unsigned int _calculate(const char* const text, unsigned int& value) {
		const unsigned int character = sdbm_hash<length - 1>::_calculate(text, value);
		value = character + (value << 6) + (value << 16) - value;
		return text[length - 1];
	}

	constexpr static unsigned int calculate(const char* const text) {
		unsigned int value = 0;
		const auto character = _calculate(text, value);
		return character + (value << 6) + (value << 16) - value;
	}
};

template <> struct sdbm_hash<1>
{
	constexpr static int _calculate(const char* const text, unsigned int&) { return text[0]; }
};

template <size_t N> constexpr unsigned int make_sdbm_hash(const char(&text)[N]) 
{
	return sdbm_hash<N - 1>::calculate(text);
}
