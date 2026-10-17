#include "utils.h"

std::string EscapeHtml(const std::string &text) {
	std::string result;
	result.reserve(text.size());
	for (char c: text) {
		switch (c) {
			case '&': result += "&amp;";
				break;
			case '<': result += "&lt;";
				break;
			case '>': result += "&gt;";
				break;
			case '"': result += "&quot;";
				break;
			default: result += c;
		}
	}
	return result;
}

bool IsValidUtf8(const std::string &text) {
	for (std::size_t i = 0; i < text.size();) {
		const auto first = static_cast<unsigned char>(text[i]);
		if (first <= 0x7f) {
			++i;
			continue;
		}
		const std::size_t length = first >= 0xc2 && first <= 0xdf
			                           ? 2
			                           : first >= 0xe0 && first <= 0xef
				                             ? 3
				                             : first >= 0xf0 && first <= 0xf4
					                               ? 4
					                               : 0;
		if (length == 0 || i + length > text.size()) return false;
		for (std::size_t j = 1; j < length; ++j) {
			const auto next = static_cast<unsigned char>(text[i + j]);
			if ((next & 0xc0) != 0x80 || (j == 1 &&
			                              ((first == 0xe0 && next < 0xa0) || (first == 0xed && next >= 0xa0) ||
			                               (first == 0xf0 && next < 0x90) || (first == 0xf4 && next >= 0x90))))
				return false;
		}
		i += length;
	}
	return true;
}
