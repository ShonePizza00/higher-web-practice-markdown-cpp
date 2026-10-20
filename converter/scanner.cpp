#include "scanner.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace {
	std::string TrimRight(std::string text) {
		while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
			text.pop_back();
		}
		return text;
	}

	bool CheckHeading(const std::string &line, int &level, std::string &content) {
		std::size_t marker_count = 0;
		while (marker_count < line.size() && line[marker_count] == '#') {
			++marker_count;
		}
		if (
			marker_count == 0 ||
			marker_count > 3 ||
			marker_count == line.size() ||
		    line[marker_count] != ' ') {
			return false;
		}
		level = static_cast<int>(marker_count);
		content = line.substr(marker_count + 1);
		return true;
	}

	bool CheckOrdered(const std::string &line, std::string &content) {
		std::size_t marker_end = 0;
		while (marker_end < line.size() && std::isdigit(static_cast<unsigned char>(line[marker_end]))) {
			++marker_end;
		}
		if (marker_end == 0 ||
			marker_end + 1 >= line.size() ||
			line[marker_end] != '.' ||
		    line[marker_end + 1] != ' ') {
			return false;
		}
		content = line.substr(marker_end + 2);
		return true;
	}

	bool CheckUnordered(const std::string &line, std::string &content) {
		if (line.size() < 3 ||
			(line[0] != '-' && line[0] != '*') ||
			line[1] != ' ') {
			return false;
		}
		content = line.substr(2);
		return true;
	}
} // namespace

std::vector<Block> ScanMarkdown(const std::string &markdown) {
	std::istringstream input(markdown);
	std::vector<std::string> lines;
	lines.reserve(std::count(markdown.begin(), markdown.end(), '\n') + 1);
	for (std::string line; std::getline(input, line);) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		lines.push_back(TrimRight(std::move(line)));
	}

	std::vector<Block> blocks;
	blocks.reserve(lines.size());
	for (std::size_t i = 0; i < lines.size();) {
		if (lines[i].empty()) {
			++i;
			continue;
		}
		int level = 0;
		std::string content;
		if (CheckHeading(lines[i], level, content)) {
			blocks.push_back({Block::Type::Heading, level, {content}});
			++i;
		} else if (CheckOrdered(lines[i], content)) {
			Block block{Block::Type::OrderedList};
			do {
				block.lines.push_back(content);
				++i;
			} while (i < lines.size() && CheckOrdered(lines[i], content));
			blocks.push_back(std::move(block));
		} else if (CheckUnordered(lines[i], content)) {
			Block block{Block::Type::UnorderedList};
			do {
				block.lines.push_back(content);
				++i;
			} while (i < lines.size() && CheckUnordered(lines[i], content));
			blocks.push_back(std::move(block));
		} else {
			Block block{Block::Type::Paragraph};
			do {
				block.lines.push_back(lines[i]);
				++i;
				if (i == lines.size() || lines[i].empty()) {
					break;
				}

				int ignored_level = 0;
				if (CheckHeading(lines[i], ignored_level, content) ||
					CheckOrdered(lines[i], content) ||
				    CheckUnordered(lines[i], content)) {
					break;
				}
			} while (true);
			blocks.push_back(std::move(block));
		}
	}
	return blocks;
}
