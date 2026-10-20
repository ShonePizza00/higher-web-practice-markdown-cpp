#include "converter/renderer.h"
#include "converter/scanner.h"
#include "converter/utils.h"

#include <iostream>
#include <string>

namespace {
	std::string ConvertMarkdown(const std::string &markdown) {
		return RenderHtml(ScanMarkdown(markdown));
	}

	bool TestUtf8() {
		return IsValidUtf8("Привет") && !IsValidUtf8(std::string("\xc0\xaf", 2));
	}

	bool TestEmptyInput() {
		return ConvertMarkdown("").empty() && ConvertMarkdown("  \n\t\n\n").empty();
	}

	bool TestHeadingsAndBlocks() {
		return ConvertMarkdown("# One\n## Two\n### Three\n") ==
		       "<h1>One</h1>\n<h2>Two</h2>\n<h3>Three</h3>\n" &&
		       ConvertMarkdown("# Title\r\n\r\nline one\nline two\n\n- A\n- B\n\n1. X\n2. Y\n") ==
		       "<h1>Title</h1>\n<p>line one line two</p>\n<ul>\n  <li>A</li>\n  <li>B</li>\n</ul>\n<ol>\n  <li>X</li>\n  <li>Y</li>\n</ol>\n";
	}

	bool TestInlineFormattingAndEscaping() {
		return ConvertMarkdown("*a **b** c* and `*not italic*`") ==
		           "<p><em>a <strong>b</strong> c</em> and <code>*not italic*</code></p>\n" &&
		       ConvertMarkdown("\\*literal\\* \\<tag\\> A & B \"x\"") ==
		           "<p>*literal* &lt;tag&gt; A &amp; B &quot;x&quot;</p>\n";
	}

	bool TestLinksAndUnclosedMarkup() {
		return ConvertMarkdown("[Yandex](https://yandex.ru)") ==
		           "<p><a href=\"https://yandex.ru\">Yandex</a></p>\n" &&
		       ConvertMarkdown("[") == "<p>[</p>\n" &&
		       ConvertMarkdown("[Yandex](https://yandex.ru") == "<p>[Yandex](https://yandex.ru</p>\n" &&
		       ConvertMarkdown("[open [link]") == "<p>[open [link]</p>\n" &&
		       ConvertMarkdown("[bad](javascript:alert(1))") == "<p>[bad](javascript:alert(1))</p>\n" &&
		       ConvertMarkdown("**unclosed *also unclosed") == "<p>**unclosed *also unclosed</p>\n";
	}

	bool TestLongLine() {
		const std::string line(20001, 'x');
		return ConvertMarkdown(line) == "<p>" + line + "</p>\n";
	}

	struct TestCase {
		const char *name;
		bool (*run)();
	};
} // namespace

int main() {
	const TestCase tests[] = {
		{"UTF-8", TestUtf8},
		{"empty input", TestEmptyInput},
		{"headings and blocks", TestHeadingsAndBlocks},
		{"inline formatting and escaping", TestInlineFormattingAndEscaping},
		{"links and unclosed markup", TestLinksAndUnclosedMarkup},
		{"long line", TestLongLine},
	};

	for (const TestCase &test: tests) {
		if (!test.run()) {
			std::cerr << "Failed: " << test.name << '\n';
			return 1;
		}
	}
	return 0;
}
