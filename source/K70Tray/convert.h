#pragma once

#include <string>
#include <locale>
#include <codecvt>

std::wstring s2ws(const std::string& str);
std::string ws2s(const std::wstring& wstr);
std::string int_array_to_string(unsigned char int_array[], int size_of_array, int base);
std::string utf8_encode(const std::wstring &wstr);
std::wstring utf8_decode(const std::string &str);