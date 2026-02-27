#include <iostream>
#include <random>
#include <sstream>
#include <iterator>
#include <fstream>

#include "TextFlow.hpp"
#include "json.hpp"

#define MAX_TXT_W 42



typedef struct kim {
	std::ifstream		file;
	int					file_w;
	int					file_h;

	std::stringstream	img;
	int					img_w;
	int					img_h;

	std::string			text;
	int					text_w;
	int					text_h;

	std::stringstream	out;
} t_kim;



int utf8len(const char *s)
{
	int len = 0;
	while(*s)
		len += (*(s++) & 0xC0) != 0x80;
	return len;
}

void processKim(t_kim &kim) {
	std::string	line;

	kim.file.open("kim");
	if (kim.file.fail()) {
		std::cerr << "Failed to open the kimFile :(" << std::endl;
		exit(1);
	}

	while (std::getline(kim.file, line)) {
		int	len = utf8len(line.c_str());
		if (len > kim.file_w)
			kim.file_w = len;
		kim.file_h++;
	}
	kim.file.clear();
	kim.file.seekg(0);

	std::string frame = "█";

	std::fill_n(std::ostream_iterator<std::string>(kim.img), kim.file_w + 4, frame);
	kim.img << "\n";
	while (std::getline(kim.file, line))
		kim.img << frame << frame << line << frame << frame << std::endl;
	std::fill_n(std::ostream_iterator<std::string>(kim.img), kim.file_w + 4, frame);
	kim.img_w = kim.file_w + 4;
	kim.img_h = kim.file_h + 2;
}

void processText(t_kim &kim, int argc, char **argv) {
	if (argc == 1) {
		// std::cin >> kim.text;
		std::ifstream f("dialog.json");
		if (f.fail()) {
			std::cerr << "Failed to open the dialog archive :(" << std::endl;
			exit(1);
		}
		nlohmann::json					data = nlohmann::json::parse(f);
		auto							arr = data["kim"];
		std::random_device				rd;
		std::mt19937 					gen(rd());
		std::uniform_int_distribution<>	dist(0, arr.size());
		int								i = dist(gen);

		kim.text = arr[i].get<std::string>();
	} else {
		for (int i = 1; i < argc; i++) {
			if (kim.text.length())
				kim.text += " ";
			kim.text += argv[i];
		}
	}

	kim.text = TextFlow::Column("KIM KITSURAGI - " + kim.text)
		.width(MAX_TXT_W).indent(2).toString().erase(0, 2);
}

void formatKim(t_kim &kim) {
	std::stringstream textStream(kim.text);
	std::string img, txt;

	std::getline(kim.img, img);
	kim.out << img << std::endl;
	while (!kim.img.eof() || !textStream.eof()) {
		std::getline(kim.img, img);
		std::getline(textStream, txt);

		if (!img.empty()) {
			kim.out << img;
			if (txt.empty())
				kim.out << std::endl;
			else
				kim.out << "  ";
		} else {
			std::fill_n(std::ostream_iterator<std::string>(kim.out), kim.img_w + 2, " ");
		}

		if (!txt.empty()) {
			kim.out << txt << std::endl;
		}

		img = txt = "";
	}
}

int main(int argc, char **argv) {
	t_kim				kim;

	// Get the file and count its width and height in utf chars
	processKim(kim);

	// Get the raw text that will be displayed and format it
	processText(kim, argc, argv);

	// Format the output buffer
	formatKim(kim);

	std::cout << kim.out.rdbuf();

	kim.file.close();
}

/*
	Text should include the character's name in mayus,
	followed by " - ". Character dialog should go between
	double quotes, the narrator is exempt from this rule.
	The text should have a 2-character tabulation. Example:

	KIM KITSURAGI - "I don't think that's our
	  current priority, detective..." The Liutenant
	  looks away. "We should get going."
*/

// TODO:
// - Flag to autopick dialog (cin/args otherwise)
// - Better customizability
// - Code cleanup because right now it's garbageeee
// - Install script