#include "converter/renderer.h"
#include "converter/scanner.h"
#include "converter/utils.h"

#include <cstdio>
#include <iostream>
#include <string>

namespace {
	void PrintUsage(std::ostream &out) {
		out << "Usage: MarkdownToHTML --in <input.md> [--out <output.html>]\n";
	}

	bool ReadMarkdown(const std::string &input_path, std::string &markdown) {
		std::FILE *input = std::fopen(input_path.c_str(), "rb");
		if (!input) {
			std::cerr << "Cannot open input file: " << input_path << '\n';
			return false;
		}

		char buffer[8192];
		for (std::size_t size; (size = std::fread(buffer, 1, sizeof(buffer), input)) != 0;) {
			markdown.append(buffer, size);
		}
		const bool read_failed = std::ferror(input);
		std::fclose(input);
		if (read_failed) {
			std::cerr << "Cannot read input file: " << input_path << '\n';
			return false;
		}
		if (!IsValidUtf8(markdown)) {
			std::cerr << "Input file is not valid UTF-8: " << input_path << '\n';
			return false;
		}
		if (markdown.compare(0, 3, "\xef\xbb\xbf") == 0) {
			markdown.erase(0, 3);
		}
		return true;
	}

	std::string ConvertMarkdown(const std::string &markdown) {
		return RenderHtml(ScanMarkdown(markdown));
	}

	bool WriteHtml(const std::string &output_path, const std::string &html) {
		if (output_path.empty()) {
			return std::fwrite(html.data(), 1, html.size(), stdout) == html.size();
		}

		std::FILE *output = std::fopen(output_path.c_str(), "wb");
		if (!output) {
			std::cerr << "Cannot write output file: " << output_path << '\n';
			return false;
		}
		bool write_failed = std::fwrite(html.data(), 1, html.size(), output) != html.size();
		write_failed = std::fclose(output) != 0 || write_failed;
		if (write_failed) {
			std::cerr << "Cannot write output file: " << output_path << '\n';
		}
		return !write_failed;
	}
} // namespace

int main(int argc, char *argv[]) {
	std::string input_path;
	std::string output_path;
	for (int i = 1; i < argc; ++i) {
		const std::string option = argv[i];
		if ((option == "--in" || option == "--out") && i + 1 < argc) {
			const std::string value = argv[++i];
			if (option == "--in") {
				input_path = value;
			} else {
				output_path = value;
			}
		} else {
			PrintUsage(std::cerr);
			return 2;
		}
	}
	if (input_path.empty()) {
		PrintUsage(std::cerr);
		return 2;
	}

	std::string markdown;
	if (!ReadMarkdown(input_path, markdown)) {
		return 1;
	}
	const std::string html = ConvertMarkdown(markdown);
	if (!WriteHtml(output_path, html)) {
		return 1;
	}
	return 0;
}
