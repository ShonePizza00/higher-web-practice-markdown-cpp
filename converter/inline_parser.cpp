#include "inline_parser.h"

#include "utils.h"

namespace {
	bool IsEscapable(char c) {
		return std::string("\\`*_{}[]()#+-.!<>").find(c) != std::string::npos;
	}

	bool IsEscaped(const std::string &text, std::size_t pos) {
		std::size_t slashes = 0;
		while (pos > slashes && text[pos - slashes - 1] == '\\') ++slashes;
		return slashes % 2 != 0;
	}

	std::size_t FindClosing(const std::string &text, const std::string &marker, std::size_t from) {
		for (std::size_t i = from; i + marker.size() <= text.size(); ++i) {
			if (text.compare(i, marker.size(), marker) == 0 && !IsEscaped(text, i) &&
			    (marker != "*" || ((i == 0 || text[i - 1] != '*') &&
			                       (i + 1 == text.size() || text[i + 1] != '*'))))
				return i;
		}
		return std::string::npos;
	}

	std::string Parse(const std::string &text) {
		std::string out;
		for (std::size_t i = 0; i < text.size();) {
			if (text[i] == '\\' && i + 1 < text.size() && IsEscapable(text[i + 1])) {
				out += EscapeHtml(text.substr(i + 1, 1));
				i += 2;
				continue;
			}
			if (text[i] == '`') {
				auto end = FindClosing(text, "`", i + 1);
				if (end != std::string::npos) {
					out += "<code>" + EscapeHtml(text.substr(i + 1, end - i - 1)) + "</code>";
					i = end + 1;
					continue;
				}
			}
			if (text[i] == '[') {
				auto close = text.find("](", i + 1);
				auto end = close == std::string::npos ? close : text.find(')', close + 2);
				if (close != std::string::npos && end != std::string::npos &&
				    text.find_first_of("[]()", i + 1) == close) {
					auto label = text.substr(i + 1, close - i - 1);
					auto url = text.substr(close + 2, end - close - 2);
					const bool web_url = url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0;
					if (!label.empty() && web_url && url.find_first_of(" \t\r\n") == std::string::npos) {
						out += "<a href=\"" + EscapeHtml(url) + "\">" + Parse(label) + "</a>";
						i = end + 1;
						continue;
					}
				}
			}
			const std::string marker = text.compare(i, 2, "**") == 0 ? "**" : text.substr(i, 1);
			if (marker == "*" || marker == "**") {
				auto end = FindClosing(text, marker, i + marker.size());
				if (end != std::string::npos && end > i + marker.size()) {
					const auto tag = marker == "**" ? "strong" : "em";
					out += "<" + std::string(tag) + ">" + Parse(text.substr(i + marker.size(), end - i - marker.size()))
							+
							"</" + tag + ">";
					i = end + marker.size();
					continue;
				}
				if (marker == "**") {
					out += "**";
					i += 2;
					continue;
				}
			}
			out += EscapeHtml(text.substr(i, 1));
			++i;
		}
		return out;
	}
} // namespace

std::string ParseInline(const std::string &text) { return Parse(text); }
