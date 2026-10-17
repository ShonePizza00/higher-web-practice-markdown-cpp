#pragma once

#include <string>
#include <vector>

struct Block {
	enum class Type { Heading, Paragraph, OrderedList, UnorderedList } type;

	int level = 0;
	std::vector<std::string> lines;
};

std::vector<Block> ScanMarkdown(const std::string &markdown);
