#pragma once

#include <fstream>

std::string getRandomLine(std::ifstream &f);

int utf8len(const char *s);

int validate_and_count_utf8(const std::string& str, const std::string & place);

std::string replaceAll(std::string const& original, std::string const& from, std::string const& to);

std::string getFormattedText(bool discoFormat, const std::string &name, const std::string &rawText, int wrap);