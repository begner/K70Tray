#include "stdafx.h"
#include "convert.h"

std::wstring s2ws(const std::string& str)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

std::string int_array_to_string(unsigned char int_array[], int size_of_array, int base) {
	std::string returnstring = "";
	for (int temp = 0; temp < size_of_array; temp++) {
		int tmpInt = (int)int_array[temp];
		char tmpChar[10] = "";
		_itoa_s(tmpInt, tmpChar, base);
		returnstring += std::string(tmpChar) + ", ";
	}
	return returnstring;
}

std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

void str_replace(std::string &s, const std::string &search, const std::string &replace)
{
    for (size_t pos = 0;; pos += replace.length())
	{
         pos = s.find(search, pos);
         if (pos == std::string::npos) break;

         s.erase(pos, search.length());
         s.insert(pos, replace);
     }
}


// trim from start
std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

#ifdef USE_GDIPULS
	Gdiplus::Rect rect2GdiRect(RECT &rect)
	{
		return Gdiplus::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
#endif