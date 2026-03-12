#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <iterator>

#include "TextFlow.hpp"
#include "json.hpp"


#ifndef FILEDIR
	#define FILEDIR "/usr/share"
#endif



typedef struct kim {
	bool				revacholianTxt = false;
	bool				frameLess = false;
	bool				discoFormat = true;
	int					wrap = 42;
	int					gap = 2;
	std::string			artFile = FILEDIR "/kimsay/portraits/kim";
	std::string			discoFile = FILEDIR "/kimsay/dialog/kim.json";

	std::stringstream	img;
	int					img_w = 0;
	int					img_h = 0;

	std::string			rawText;
	std::stringstream	text;
	int					text_w = 0;
	int					text_h = 0;

	std::stringstream	out;
} t_kim;

int utf8len(const char *s) {
	int len = 0;
	while(*s)
		len += (*(s++) & 0xC0) != 0x80;
	return len;
}

int validate_and_count_utf8(const std::string& str, const std::string & place) {
    const uint8_t*	bytes = reinterpret_cast<const uint8_t*>(str.data());
    size_t			length = str.size();
    size_t			i = 0;
	int				chars = 0;

    while (i < length) {
        uint8_t byte = bytes[i];

        if (byte <= 0x7F) {
            // 1-byte: 0xxxxxxx
            i++;
        }
        else if ((byte & 0xE0) == 0xC0) { // 0b110xxxxx (2-byte)
            if (i + 1 >= length) return false;
            uint8_t byte2 = bytes[++i];
            if ((byte2 & 0xC0) != 0x80) return false;
            uint32_t code_point = ((byte & 0x1F) << 6) | (byte2 & 0x3F);
            if (code_point < 0x80) return false; // Overlong
            i++;
        }
        else if ((byte & 0xF0) == 0xE0) { // 0b1110xxxx (3-byte)
            if (i + 2 >= length) return false;
            uint8_t byte2 = bytes[++i];
            uint8_t byte3 = bytes[++i];
            if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80) return false;
            uint32_t code_point = ((byte & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
            if (code_point < 0x800 || (code_point >= 0xD800 && code_point <= 0xDFFF)) return false; // Overlong/surrogate
            i++;
        }
        else if ((byte & 0xF8) == 0xF0) { // 0b11110xxx (4-byte)
            if (i + 3 >= length) return false;
            uint8_t byte2 = bytes[++i];
            uint8_t byte3 = bytes[++i];
            uint8_t byte4 = bytes[++i];
            if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80 || (byte4 & 0xC0) != 0x80) return false;
            uint32_t code_point = ((byte & 0x07) << 18) | ((byte2 & 0x3F) << 12) | 
                                  ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
            if (code_point > 0x10FFFF) return false; // Exceeds Unicode limit
            i++;
        }
        else {
            // Invalid leading byte (e.g., 0x80-0xBF, 0xF8-0xFF)
			std::cerr << "Invalid UTF-8 sequence in the provided " << place << std::endl;
			exit(EXIT_FAILURE);
        }

		// Counts a single utf8 chat each loop
		chars++;
    }

    return chars; // All bytes validated
}

std::string replaceAll(std::string const& original, std::string const& from, std::string const& to) {
    std::string					results;
    std::string::const_iterator	end = original.end();
    std::string::const_iterator	current = original.begin();
    std::string::const_iterator	next = std::search(current, end, from.begin(), from.end());
    while (next != end) {
        results.append(current, next);
        results.append(to);
        current = next + from.size();
        next = std::search(current, end, from.begin(), from.end());
    }
    results.append(current, next);
    return results;
}



void processArgs(t_kim &kim, int argc, char **argv) {
	int	opt;
	int tmp;

	while ((opt = getopt(argc, argv, "rFnw:g:f:")) != -1) {
		switch (opt)
		{
		case 'r':
			kim.revacholianTxt = true;
			break;
		case 'F':
			kim.frameLess = true;
			break;
		case 'n':
			kim.discoFormat = false;
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
		case 'f':
			kim.artFile = optarg;
			break;
		default:
			std::cerr << "Usage: kimsay [-rFn] [-w wrap] [-g gap] [-f artFile] [text...]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// The getopt function permutes argv so nonoptions are
	// left at the end. The global var optind holds the index of the
	// first of this nonoptions
	if (argc > 1 && optind <= argc) {
		if (!kim.revacholianTxt) {
			for (int i = optind; i < argc; i++) {
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
		std::ifstream f(kim.discoFile);
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

	// Validate the text
	validate_and_count_utf8(kim.rawText, "input text");

	std::string formatted = kim.discoFormat ?
		TextFlow::Column("KIM KITSURAGI - " + replaceAll(kim.rawText, "\t", "  "))
			.width(kim.wrap).indent(2).toString().erase(0, 2) :
		TextFlow::Column(replaceAll(kim.rawText, "\t", "  "))
			.width(kim.wrap).toString();

	kim.text = std::stringstream(formatted);
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
