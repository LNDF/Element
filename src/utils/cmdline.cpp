#include "cmdline.h"

#include <regex>

using namespace element;

static const std::regex escape_backslash("\\\\", std::regex_constants::optimize);
static const std::regex escape_quote("\"", std::regex_constants::optimize);

std::string element::process_string_for_cmdline(const std::string& str) {
    std::string result = std::regex_replace(str, escape_backslash, "\\\\");
    result = std::regex_replace(result, escape_quote, "\\\"");
    return "\"" + result + "\"";
}