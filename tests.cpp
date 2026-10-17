#include "converter/renderer.h"
#include "converter/scanner.h"
#include "converter/utils.h"

#include <iostream>
#include <string>

#define CHECK(condition) \
	do { \
		if (!(condition)) { \
			std::cerr << "Test failed at line " << __LINE__ << '\n'; \
			return 1; \
		} \
	} while (false)

int main() {
	CHECK(IsValidUtf8("Привет"));
	CHECK(!IsValidUtf8(std::string("\xc0\xaf", 2)));
	CHECK(RenderHtml(ScanMarkdown("")).empty());
	CHECK(RenderHtml(ScanMarkdown("  \n\t\n\n")).empty());
	CHECK(RenderHtml(ScanMarkdown("# One\n## Two\n### Three\n")) ==
		"<h1>One</h1>\n<h2>Two</h2>\n<h3>Three</h3>\n");
	CHECK(RenderHtml(ScanMarkdown("# Title\r\n\r\nline one\nline two\n\n- A\n- B\n\n1. X\n2. Y\n")) ==
		"<h1>Title</h1>\n<p>line one line two</p>\n<ul>\n  <li>A</li>\n  <li>B</li>\n</ul>\n<ol>\n  <li>X</li>\n  <li>Y</li>\n</ol>\n");
	CHECK(RenderHtml(ScanMarkdown("*a **b** c* and `*not italic*`")) ==
		"<p><em>a <strong>b</strong> c</em> and <code>*not italic*</code></p>\n");
	CHECK(RenderHtml(ScanMarkdown("\\*literal\\* \\<tag\\> A & B \"x\"")) ==
		"<p>*literal* &lt;tag&gt; A &amp; B &quot;x&quot;</p>\n");
	CHECK(RenderHtml(ScanMarkdown("[Yandex](https://yandex.ru)")) ==
		"<p><a href=\"https://yandex.ru\">Yandex</a></p>\n");
	CHECK(RenderHtml(ScanMarkdown("[")) == "<p>[</p>\n");
	CHECK(RenderHtml(ScanMarkdown("[Yandex](https://yandex.ru")) ==
		"<p>[Yandex](https://yandex.ru</p>\n");
	CHECK(RenderHtml(ScanMarkdown("[open [link]")) == "<p>[open [link]</p>\n");
	CHECK(RenderHtml(ScanMarkdown("[bad](javascript:alert(1))")) ==
		"<p>[bad](javascript:alert(1))</p>\n");
	CHECK(RenderHtml(ScanMarkdown("**unclosed *also unclosed")) ==
		"<p>**unclosed *also unclosed</p>\n");
	const std::string long_line(20001, 'x');
	CHECK(RenderHtml(ScanMarkdown(long_line)) == "<p>" + long_line + "</p>\n");
}
