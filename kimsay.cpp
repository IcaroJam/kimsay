#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <iterator>

#include "TextFlow.hpp"
#include "json.hpp"

#define MAX_TXT_W 42



typedef struct kim {
	bool				revacholianTxt;

	std::stringstream	img;
	int					img_w;
	int					img_h;

	std::string			rawText;
	std::stringstream	text;
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



void processArgs(t_kim &kim, int argc, char **argv) {
	int	opt;

	while ((opt = getopt(argc, argv, "r")) != -1) {
		switch (opt)
		{
		case 'r':
			kim.revacholianTxt = true;
			break;
		default:
			std::cerr << "Usage: kimsay [-r] [text]..." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// The getopt function permutes argv so nonoptions are
	// left at the end. The global var optind holds the index of the
	// first of this nonoptions
	if (argc > 1) {
		if (!kim.revacholianTxt) {
			std::string	text;

			for (int i = 1; i < argc; i++) {
				if (text.length())
					text += " ";
				text += argv[i];
			}
			kim.rawText = text;
		}
	} else {
		std::cin >> kim.rawText;
	}
}

void processKim(t_kim &kim) {
	std::ifstream	file;
	int				file_w;
	int				file_h;

	std::string		line;

	// Open the file with the art
	file.open("kim");
	if (file.fail()) {
		std::cerr << "Failed to open the kimFile :(" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Count its size in unicode chars
	while (std::getline(file, line)) {
		int	len = utf8len(line.c_str());
		if (len > file_w)
			file_w = len;
		file_h++;
	}
	// Reset the file reader
	file.clear();
	file.seekg(0);

	// Build the framed image
	std::string frame = "█";
	kim.img_w = file_w + 4;
	kim.img_h = file_h + 2;

	std::fill_n(std::ostream_iterator<std::string>(kim.img), kim.img_w, frame);
	kim.img << "\n";
	while (std::getline(file, line))
		kim.img << frame << frame << line << frame << frame << std::endl;
	std::fill_n(std::ostream_iterator<std::string>(kim.img), kim.img_w, frame);

	file.close();
}

void processText(t_kim &kim, int argc, char **argv) {
	if (kim.revacholianTxt) {
		std::ifstream f("dialog.json");
		if (f.fail()) {
			std::cerr << "Failed to open the dialog archive :(" << std::endl;
			exit(EXIT_FAILURE);
		}
		nlohmann::json					data = nlohmann::json::parse(f);
		auto							arr = data["kim"];
		std::random_device				rd;
		std::mt19937 					gen(rd());
		std::uniform_int_distribution<>	dist(0, arr.size() - 1);
		int								i = dist(gen);

		kim.rawText = arr[i].get<std::string>();
	}

	kim.text = std::stringstream(
		TextFlow::Column("KIM KITSURAGI - " + kim.rawText)
			.width(MAX_TXT_W).indent(2).toString().erase(0, 2)
	);
}

void formatKim(t_kim &kim) {
	std::string img, txt;

	// Start the text after the top of the frame
	std::getline(kim.img, img);
	kim.out << std::endl << img << std::endl;

	// Build the output with image and text side by side,
	// separated by some padding and filling any empty space
	// with whitespace
	while (!kim.img.eof() || !kim.text.eof()) {
		std::getline(kim.img, img);
		std::getline(kim.text, txt);

		if (!img.empty()) {
			// If there's image left put it in and either skip or
			// put the padding in if there's also text
			kim.out << img;
			if (txt.empty())
				kim.out << std::endl;
			else
				kim.out << "  ";
		} else {
			// If the text is taller than the image fill with
			// empty padding to keep alignment
			std::fill_n(std::ostream_iterator<std::string>(kim.out), kim.img_w + 2, " ");
		}
		// If there's text left put it in
		if (!txt.empty()) {
			kim.out << txt << std::endl;
		}
		// Reset the tmps so ifs are handled correctly next loop
		img = txt = "";
	}
}

int main(int argc, char **argv) {
	t_kim				kim;

	// Handle and depurate command line arguments
	processArgs(kim, argc, argv);

	// Get the file and count its width and height in utf chars
	processKim(kim);

	// Get the raw text that will be displayed and format it
	processText(kim, argc, argv);

	// Format the output buffer
	formatKim(kim);

	std::cout << kim.out.rdbuf();
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
// - [X] Flag to autopick dialog (cin/args otherwise)
// - [ ] Better customizability (alt frames, imgs)
// - [ ] Code cleanup because right now it's garbageeee
// - [ ] Install script
// - [ ] Man page