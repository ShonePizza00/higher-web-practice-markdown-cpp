#include "scanner.h"

#include <cctype>
#include <sstream>

namespace {
	std::string TrimRight(std::string s) {
		while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
		return s;
	}

	bool Heading(const std::string &line, int &level, std::string &content) {
		std::size_t n = 0;
		while (n < line.size() && line[n] == '#') ++n;
		if (n == 0 || n > 3 || n == line.size() || line[n] != ' ') return false;
		level = static_cast<int>(n);
		content = line.substr(n + 1);
		return true;
	}

	bool Ordered(const std::string &line, std::string &content) {
		std::size_t i = 0;
		while (i < line.size() && std::isdigit(static_cast<unsigned char>(line[i]))) ++i;
		if (i == 0 || i + 1 >= line.size() || line[i] != '.' || line[i + 1] != ' ') return false;
		content = line.substr(i + 2);
		return true;
	}

	bool Unordered(const std::string &line, std::string &content) {
		if (line.size() < 3 || (line[0] != '-' && line[0] != '*') || line[1] != ' ') return false;
		content = line.substr(2);
		return true;
	}
} // namespace

std::vector<Block> ScanMarkdown(const std::string &markdown) {
	std::istringstream input(markdown);
	std::vector<std::string> lines;
	for (std::string line; std::getline(input, line);) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		lines.push_back(TrimRight(std::move(line)));
	}

	std::vector<Block> blocks;
	for (std::size_t i = 0; i < lines.size();) {
		if (lines[i].empty()) {
			++i;
			continue;
		}
		int level = 0;
		std::string content;
		if (Heading(lines[i], level, content)) {
			blocks.push_back({Block::Type::Heading, level, {content}});
			++i;
		} else if (Ordered(lines[i], content)) {
			Block block{Block::Type::OrderedList};
			do {
				block.lines.push_back(content);
				++i;
			} while (i < lines.size() && Ordered(lines[i], content));
			blocks.push_back(std::move(block));
		} else if (Unordered(lines[i], content)) {
			Block block{Block::Type::UnorderedList};
			do {
				block.lines.push_back(content);
				++i;
			} while (i < lines.size() && Unordered(lines[i], content));
			blocks.push_back(std::move(block));
		} else {
			Block block{Block::Type::Paragraph};
			do {
				block.lines.push_back(lines[i++]);
				if (i == lines.size() || lines[i].empty()) break;
				int ignored = 0;
				if (Heading(lines[i], ignored, content) || Ordered(lines[i], content) || Unordered(lines[i], content))
					break;
			} while (true);
			blocks.push_back(std::move(block));
		}
	}
	return blocks;
}
