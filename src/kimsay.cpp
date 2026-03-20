#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <map>

#include "kimsay.hpp"



#ifndef FILEDIR
	#define FILEDIR "/usr/local/share"
#endif



const static std::map<std::string, std::string> characters {
	{"kim", "KIM KITSURAGI"},
	{"conceptualization", "CONCEPTUALIZATION"},
	{"drama", "DRAMA"},
	{"encyclopedia", "ENCYCLOPEDIA"},
	{"logic", "LOGIC"},
	{"rhetoric", "RHETORIC"},
	{"visualCalculus", "VISUAL CALCULUS"},
	{"authority", "AUTHORITY"},
	{"empathy", "EMPATHY"},
	{"espritDeCorps", "ESPRIT DE CORPS"},
	{"inlandEmpire", "INLAND EMPIRE"},
	{"suggestion", "SUGGESTION"},
	{"volition", "VOLITION"},
};

std::string artFrom(const std::string &c) {
	return FILEDIR + std::string("/kimsay/portraits/") + c;
}

std::string dialogFrom(const std::string &c) {
	return FILEDIR + std::string("/kimsay/dialog/") + c + ".json";
}

typedef struct kim {
	bool				revacholianTxt = false;
	bool				frameLess = false;
	bool				discoFormat = true;
	int					wrap = 42;
	int					gap = 2;
	std::string			name = characters.at("kim");
	std::string			artFile = artFrom("kim");
	std::string			dialogFile = dialogFrom("kim");

	std::stringstream	img;
	int					img_w = 0;
	int					img_h = 0;

	std::string			rawText;
	std::stringstream	text;
	int					text_w = 0;
	int					text_h = 0;

	std::stringstream	out;
} t_kim;



void processArgs(t_kim &kim, int argc, char **argv) {
	int			opt;
	int 		tmp;
	std::string	helpStr = "Usage: kimsay [-h] [-rFu] [-c character] [-w wrap] [-g gap] [-n name] [-f artFile] [text...]";
	std::string	knownChars = "Default:\n\tkim\n"
								"Skills - Intellect:\n\tconceptualization\n\tdrama\n\tencyclopedia\n\tlogic\n\trhetoric\n\tvisualCalculus\n"
								"Skills - Psyche:\n\tauthority\n\tempathy\n\tespritDeCorps\n\tinlandEmpire\n\tsuggestion\n\tvolition\n";

	while ((opt = getopt(argc, argv, "hrFuc:w:g:n:f:")) != -1) {
		switch (opt) {
		case 'h':
			std::cout << helpStr << std::endl;
			exit(EXIT_SUCCESS);
		case 'r':
			kim.revacholianTxt = true;
			break;
		case 'F':
			kim.frameLess = true;
			break;
		case 'u':
			kim.discoFormat = false;
			break;
		case 'c':
			if (!characters.count(optarg)) {
				std::cerr << "Unknown character '" << optarg << "'." << std::endl
					<< "Available characters are:" << std::endl << knownChars << std::endl;
				exit(EXIT_FAILURE);
			}
			kim.name = characters.at(optarg);
			kim.artFile = artFrom(optarg);
			kim.dialogFile = dialogFrom(optarg);
			break;
		case 'w':
			tmp = atoi(optarg);
			if (tmp >= 12)
				kim.wrap = tmp;
			break;
		case 'g':
			tmp = atoi(optarg);
			if (tmp >= 0)
				kim.gap = atoi(optarg);
			break;
		case 'n':
			kim.name = optarg;
			break;
		case 'f':
			kim.artFile = optarg;
			break;
		default:
			std::cerr << helpStr << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// The getopt function permutes argv so nonoptions are
	// left at the end. The global var optind holds the index of the
	// first of this nonoptions
	if (argc > 1 && optind < argc) {
		if (!kim.revacholianTxt) {
			for (int i = optind; i < argc; i++) {
				if (kim.rawText.length())
					kim.rawText += " ";
				kim.rawText += argv[i];
			}
		}
	} else if (!kim.revacholianTxt) {
		kim.rawText = std::string( // Read until EOF
			std::istream_iterator<char>(std::cin >> std::noskipws),
			std::istream_iterator<char>()
		);
	}
}

void processKim(t_kim &kim) {
	std::ifstream	file;
	int				file_w = 0;
	int				file_h = 0;

	std::string		line;

	// Open the file with the art
	file.open(kim.artFile);
	if (file.fail()) {
		std::cerr << "Failed to open the kimFile :(" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Validate the input and count its size in unicode chars
	while (std::getline(file, line)) {
		int	len = validate_and_count_utf8(line.c_str(), "art file");
		if (len > file_w)
			file_w = len;
		file_h++;
	}
	// Reset the file reader
	file.clear();
	file.seekg(0);

	// Build the image, framed or not
	if (kim.frameLess) {
		while (std::getline(file, line)) {
			int len = utf8len(line.c_str());
			kim.img << line;
			std::fill_n(std::ostream_iterator<std::string>(kim.img), file_w - len, " ");
			kim.img << std::endl;
		}
	} else {
		std::string frame = "█";
		kim.img_w = file_w + 4;
		kim.img_h = file_h + 2;

		std::fill_n(std::ostream_iterator<std::string>(kim.img), kim.img_w, frame);
		kim.img << "\n";
		while (std::getline(file, line)) {
			int len = utf8len(line.c_str());
			kim.img << frame << frame << line;
			std::fill_n(std::ostream_iterator<std::string>(kim.img), file_w - len, " ");
			kim.img << frame << frame << std::endl;
		}
		std::fill_n(std::ostream_iterator<std::string>(kim.img), kim.img_w, frame);
	}

	file.close();
}

void processText(t_kim &kim) {
	if (kim.revacholianTxt) {
		std::ifstream f(kim.dialogFile);
		if (f.fail()) {
			std::cerr << "Failed to open the dialog archive :(" << std::endl;
			exit(EXIT_FAILURE);
		}

		kim.rawText = getRandomLine(f);
	}

	// Validate the text
	validate_and_count_utf8(kim.rawText, "input text");

	kim.text = std::stringstream(getFormattedText(kim.discoFormat, kim.name, kim.rawText, kim.wrap));
}

void formatKim(t_kim &kim) {
	std::string img, txt;

	// Start the text one line after the image
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
				std::fill_n(std::ostream_iterator<std::string>(kim.out), kim.gap, " ");
		}

		// If there's text left put it in
		if (!txt.empty()) {
			// If the text is taller than the image fill with
			// empty padding to keep alignment
			if (img.empty())
				std::fill_n(std::ostream_iterator<std::string>(kim.out), kim.img_w + kim.gap, " ");
			kim.out << txt << std::endl;
		}
		// Reset the tmps so ifs are handled correctly next loop
		img = txt = "";
	}

	kim.out << std::endl;
}

int main(int argc, char **argv) {
	t_kim				kim;

	// Handle and depurate command line arguments
	processArgs(kim, argc, argv);

	// Get the file and count its width and height in utf chars
	processKim(kim);

	// Get the formatted text that will be displayed
	processText(kim);

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
// - [X] Install script
// - [X] Man page
