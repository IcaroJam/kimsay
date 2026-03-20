#include "kimsay.hpp"
#include "TextFlow.hpp"

std::string getFormattedText(bool discoFormat, const std::string &name, const std::string &rawText, int wrap) {
	if (discoFormat)
		return TextFlow::Column(name + " - " + replaceAll(rawText, "\t", "  "))
			.width(wrap).indent(2).toString().erase(0, 2);

	return TextFlow::Column(replaceAll(rawText, "\t", "  "))
			.width(wrap).toString();
}