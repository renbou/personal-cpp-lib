// Artem Mikheev 2019
// GNU GPLv3 License

#include "vector.hpp"
#include "vartypes.hpp"
#include <stdexcept>
#include <utility>

#ifndef STRING_HPP
#define STRING_HPP

// String class based on vector which stores UTF32 characters

class String {
	Vector<char32_t> _charData;
public:
	const sizeT *size = _charData.size;

	String ():
	_charData() {}

	String(const String &other)
	: _charData(other._charData) {}

	String(std::initializer_list<char32_t> arr) {
		sizeT n = arr.size();
		if (*(arr.end()-1) == U'\0') {
			_charData = Vector<char32_t>(n - 1);
			n--;
		}
		auto it = arr.begin();
		for (sizeT i = 0; i < n; i++, it++)
			_charData[i] = *it;
	}

	String(std::initializer_list<char16_t> arr) {
		sizeT n = arr.size();
		if (*(arr.end()-1) == static_cast<char16_t>('\0')) {
			_charData = Vector<char32_t>(n - 1);
			n--;
		}
		auto it = arr.begin();
		for (sizeT i = 0; i < n; i++, it++)
			_charData[i] = static_cast<char32_t>(*it);
	}

	String(std::initializer_list<char> arr) {
		sizeT n = arr.size();
		if (*(arr.end()-1) == '\0') {
			_charData = Vector<char32_t>(n - 1);
			n--;
		}
		auto it = arr.begin();
		for (sizeT i = 0; i < n; i++, it++)
			_charData[i] = static_cast<char32_t>(*it);
	}

	template<sizeT n> String(const char32_t (&arr)[n])
	{
		sizeT size = n;
		if (arr[n-1] == U'\0') {
			_charData = Vector<char32_t>(n - 1);
			size--;
		}
		for (sizeT i = 0; i < size; i++)
			_charData[i] = arr[i];
	}

	template<sizeT n> String(const char16_t (&arr)[n])
	{
		sizeT size = n;
		if (arr[n-1] == static_cast<char16_t>('\0')) {
			_charData = Vector<char32_t>(n - 1);
			size--;
		}
		for (sizeT i = 0; i < size; i++)
			_charData[i] = static_cast<char32_t>(arr[i]);
	}

	template<sizeT n> String(const char (&arr)[n])
	{
		sizeT size = n;
		if (arr[n-1] == '\0') {
			_charData = Vector<char32_t>(n - 1);
			size--;
		}
		for (sizeT i = 0; i < size; i++)
			_charData[i] = static_cast<char32_t>(arr[i]);
	}

	String(sizeT t_n)
	: _charData(t_n) {}

	String(sizeT t_n, char32_t t_default)
	: _charData(t_n, t_default) {}

	String(sizeT t_n, char16_t t_default)
			: _charData(t_n, static_cast<char32_t>(t_default)) {}

	String(sizeT t_n, char t_default)
			: _charData(t_n, static_cast<char32_t>(t_default)) {}

	// Wrapper functions for converting std strings to this string class
	String(std::string t_stdString)
	: _charData(t_stdString.size())
	{
		for (sizeT i = 0; i < t_stdString.size(); i++)
			_charData[i] = static_cast<char32_t>(t_stdString[i]);
	}

	String(std::wstring t_stdString)
			: _charData(t_stdString.size())
	{
		for (sizeT i = 0; i < t_stdString.size(); i++)
			_charData[i] = static_cast<char32_t>(t_stdString[i]);
	}

	// Initialize string based on vector of chars

	String(Vector<char32_t> &t_stringVector)
	: _charData(t_stringVector) {}

	String(Vector<char32_t> t_stringVector)
	: _charData(t_stringVector) {}

	char32_t * convertToCStyle() {
		char32_t * result = new char32_t[*size+1];
		for (sizeT i = 0; i < *size; i++)
			result[i] = _charData[i];
		result[*size] = U'\0';
		return result;
	}

	char32_t& operator[](int index) const
	{
		return _charData[index];
	}

	char32_t at(int index) const
	{
		return _charData.at(index);
	}

	String operator+(const String &other)
	{
		return String(_charData+other._charData);
	}

	String& operator+=(const String &other)
	{
		_charData += other._charData;
		return *this;
	}

	bool operator==(const String &other)
	{
		return _charData==other._charData;
	}

	bool operator>(const String &other)
	{
		if (*other.size < *size)
			return true;
		if (*other.size > *size)
			return false;
		for (sizeT i = 0; i < *size; i++)
			if (other._charData.at(i) > _charData.at(i))
				return false;
		return true;
	}

	bool operator<(const String &other)
	{
		if (*other.size > *size)
			return true;
		if (*other.size < *size)
			return false;
		for (sizeT i = 0; i < *size; i++)
			if (other._charData.at(i) < _charData.at(i))
				return false;
		return true;
	}

	void resize(sizeT t_n)
	{
		_charData.resize(t_n);
	}

	void clear()
	{
		_charData.clear();
	}

	long long toInt()
	{
		if (*this > U"9223372036854775807")
			throw std::runtime_error("Numeric value too big for int64.");
		if (_charData[0] == U'-' &&
			*this > U"-9223372036854775807")
			throw std::runtime_error("Numeric value too small for int64.");

		long long ret = 0;
		int len = *_charData.size;
		for (sizeT i = (_charData[0] == U'-' ? 1 : 0); i < len; i++) {
			if (_charData[i] < U'0' || _charData[i] > U'9') throw std::runtime_error("Cannot convert unnumeric string to integer.");
			ret *= 10;
			ret += _charData[i]-U'0';
		}
		if (_charData[0] == U'-')
			ret *= -1;

		return ret;
	}

	unsigned long long toUInt()
	{
		if (*this > U"18446744073709551615")
			throw std::runtime_error("Numeric value too big for int64.");
		if (_charData[0] == U'-')
			throw std::runtime_error("Cannot convert negative number to uint64.");

		unsigned long long ret = 0;
		int len = *_charData.size;
		for (sizeT i = (_charData[0] == U'-' ? 1 : 0); i < len; i++) {
			if (_charData[i] < U'0' || _charData[i] > U'9') throw std::runtime_error("Cannot convert unnumeric string to integer.");
			ret *= 10;
			ret += _charData[i]-U'0';
		}
		if (_charData[0] == U'-')
			ret *= -1;

		return ret;
	}

	long double toDouble()
	{
		long double mainPart = 0;
		sizeT i;
		for (i = (_charData[0] == U'-' ? 1 : 0); i < *_charData.size && _charData[i] != U'.' && _charData[i] != ','; i++) {
			if (_charData[i] < U'0' || _charData[i] > U'9') throw std::runtime_error("Cannot convert unnumeric string to double.");
			mainPart *= 10;
			mainPart += _charData[i]-U'0';
		}
		if (i == *_charData.size)
			return mainPart;
		long double divisor = 1;
		for (i++; i < *_charData.size; i++) {
			if (_charData[i] < U'0' || _charData[i] > U'9') throw std::runtime_error("Cannot convert unnumeric string to double.");
			mainPart *= 10;
			mainPart += (_charData[i]-U'0');
			divisor *= 10;
		}
		return mainPart/divisor;
	}

	String substr(int t_l, int t_sz) {
		if (t_l < 0 || t_l >= *_charData.size)
			throw std::runtime_error("Invalid substr location.");
		int realSize = t_sz;
		if (t_l + t_sz > *_charData.size)
			realSize = *_charData.size - t_l;
		String res(realSize);
		Algorithm::copy(_charData, t_l, res._charData, 0, realSize);
		return res;
	}
};

const int powersOf10[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

String intToString(int value) {
	String res;
	int sign = 0;
	int strptr = 0;
	if (value < 0) {
		sign = 1;
		value = -value;
	}
	int p = 1;
	while (p < 9 && value / powersOf10[p + 1] > 0)
		p++;
	res.resize(sign + p + 1);
	if (sign) res[strptr++] = '-';
	while (p >= 0) {
		res[strptr++] = '0' + (value / powersOf10[p]);
		value %= powersOf10[p];
		p--;
	}
	return res;
}

#endif // STRING_HPP

