#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <iterator>

#include "TextFlow.hpp"
#include "json.hpp"



typedef struct kim {
	bool				revacholianTxt = false;
	bool				frameLess = false;
	int					wrap = 42;
	int					gap = 2;
	std::string			artFile = "art/kim";

	std::stringstream	img;
	int					img_w = 0;
	int					img_h = 0;

	std::string			rawText;
	std::stringstream	text;
	int					text_w = 0;
	int					text_h = 0;

	std::stringstream	out;
} t_kim;

int utf8len(const char *s)
{
	int len = 0;
	while(*s)
		len += (*(s++) & 0xC0) != 0x80;
	return len;
}

std::string
replaceAll( std::string const& original, std::string const& from, std::string const& to )
{
    std::string results;
    std::string::const_iterator end = original.end();
    std::string::const_iterator current = original.begin();
    std::string::const_iterator next = std::search( current, end, from.begin(), from.end() );
    while ( next != end ) {
        results.append( current, next );
        results.append( to );
        current = next + from.size();
        next = std::search( current, end, from.begin(), from.end() );
    }
    results.append( current, next );
    return results;
}



void processArgs(t_kim &kim, int argc, char **argv) {
	int	opt;

	while ((opt = getopt(argc, argv, "rFw:g:f:")) != -1) {
		switch (opt)
		{
		case 'r':
			kim.revacholianTxt = true;
			break;
		case 'F':
			kim.frameLess = true;
			break;
		case 'w':
			kim.wrap = atoi(optarg);
			break;
		case 'g':
			kim.gap = atoi(optarg);
			break;
		case 'f':
			kim.artFile = optarg;
			break;
		default:
			std::cerr << "Usage: kimsay [-rF] [-w wrap] [-g gap] [-f artFile] [text...]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// The getopt function permutes argv so nonoptions are
	// left at the end. The global var optind holds the index of the
	// first of this nonoptions
	if (argc > 1) {
		if (!kim.revacholianTxt) {
			for (int i = 1; i < argc; i++) {
				if (kim.rawText.length())
					kim.rawText += " ";
				kim.rawText += argv[i];
			}
		}
	} else {
		kim.rawText = std::string( // Read until EOF
			std::istream_iterator<char>(std::cin >> std::noskipws),
			std::istream_iterator<char>()
		);
	}
}

void processKim(t_kim &kim) {
	std::ifstream	file;
	int				file_w;
	int				file_h;

	std::string		line;

	// Open the file with the art
	file.open(kim.artFile);
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
		std::ifstream f("dialog/kim.json");
		if (f.fail()) {
			std::cerr << "Failed to open the dialog archive :(" << std::endl;
			exit(EXIT_FAILURE);
		}
		nlohmann::json					data = nlohmann::json::parse(f);
		auto							arr = data["txt"];
		std::random_device				rd;
		std::mt19937 					gen(rd());
		std::uniform_int_distribution<>	dist(0, arr.size() - 1);
		int								i = dist(gen);

		kim.rawText = arr[i].get<std::string>();
	}

	kim.text = std::stringstream(
		TextFlow::Column("KIM KITSURAGI - " + replaceAll(kim.rawText, "\t", "  "))
			.width(kim.wrap).indent(2).toString().erase(0, 2)
	);
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
// - [ ] Install script
// - [X] Man page