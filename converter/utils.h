#pragma once

#include <string>

std::string EscapeHtml(const std::string &text);

bool IsValidUtf8(const std::string &text);
