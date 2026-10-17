#include "renderer.h"

#include "inline_parser.h"

std::string RenderHtml(const std::vector<Block> &blocks) {
	std::string html;
	for (const auto &block: blocks) {
		switch (block.type) {
			case Block::Type::Heading:
				html += "<h" + std::to_string(block.level) + ">" + ParseInline(block.lines.front()) + "</h" +
						std::to_string(block.level) + ">\n";
				break;
			case Block::Type::Paragraph: {
				std::string text;
				for (const auto &line: block.lines) {
					if (!text.empty()) text += ' ';
					text += line;
				}
				html += "<p>" + ParseInline(text) + "</p>\n";
				break;
			}
			case Block::Type::OrderedList:
			case Block::Type::UnorderedList: {
				const bool ordered = block.type == Block::Type::OrderedList;
				const std::string tag = ordered ? "ol" : "ul";
				html += "<" + tag + ">\n";
				for (const auto &line: block.lines) html += "  <li>" + ParseInline(line) + "</li>\n";
				html += "</" + tag + ">\n";
				break;
			}
		}
	}
	return html;
}
