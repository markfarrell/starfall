#include "Strings.h"

bool Strings::EndsWith(const string& a, const string& b) {
	if (b.size() > a.size()) return false;
		return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

void Strings::ReplaceAll(string& str, string from, string to) {
	//Replace all; taken from [stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string]
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}