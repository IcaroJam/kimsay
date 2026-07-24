#include <algorithm>
#include <getopt.h>
#include <string>
#include <uchar.h>
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
#define ARTDIR "/kimsay/portraits/"
#define DIALOGDIR "/kimsay/dialog/"



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
	{"electrochemistry", "ELECTROCHEMISTRY"},
	{"endurance", "ENDURANCE"},
	{"halfLight", "HALF LIGHT"},
	{"painThreshold", "PAIN THRESHOLD"},
	{"physicalInstrument", "PHYSICAL INSTRUMENT"},
	{"shivers", "SHIVERS"},
	{"composure", "COMPOSURE"},
	{"handEyeCoordination", "HAND/EYE COORDINATION"},
	{"interfacing", "INTERFACING"},
	{"perception", "PERCEPTION"},
	{"reactionSpeed", "REACTION SPEED"},
	{"savoirFaire", "SAVOIR FAIRE"},
	{"harry", "HARRIER DU BOIS"},
	{"cuno", "CUNO"},
	{"joyce", "JOYCE MESSIER"},
	{"klaasje", "KLAASJE"},
	{"titus", "TITUS HARDIE"},
	{"deserter", "THE DESERTER"},
	{"garte", "GARTE, THE CAFETERIA MANAGER"},
	{"lena", "LENA, THE CRYPTOZOOLOGIST'S WIFE"},
	{"noid", "NOID"},
	{"soona", "SOONA, THE PROGRAMMER"},
	{"jean", "JEAN VICQUEMARE"},
	{"idiot", "IDIOT DOOM SPIRAL"},
	{"acele", "ACELE"},
	{"steban", "STEBAN, THE STUDENT COMMUNIST"},
	{"andre", "ANDRE"},
	{"measurehead", "MEASUREHEAD"},
	{"sunday", "SUNDAY FRIEND"},
	{"roy", "BIRD'S NEST ROY"},
	{"plaisance", "PLAISANCE"},
	{"warship", "COALITION WARSHIP ARCHER"},
	{"eggHead", "EGG HEAD"},
	{"dicemaker", "NOVELTY DICEMAKER"},
	{"gary", "GARY, THE CRYPTOFASCIST"},
	{"rene", "RENÉ ARNOUX"},
	{"lilienne", "LILIENNE, THE NET PICKER"},
	{"cunoesse", "CUNOESSE"},
	{"morell", "MORELL, THE CRYPTOZOOLOGIST"},
	{"cindy", "CINDY THE SKULL"},
	{"manana", "CALL ME MAÑANA"},
	{"dolores", "DOLORES DEI"},
	{"gaston", "GASTON MARTIN"},
	{"workerWoman", "WORKING CLASS WOMAN"},
	{"richGuy", "MEGA RICH LIGHT-BENDING GUY"},
	{"annette", "ANNETTE"},
	{"trant", "TRANT HEIDELSTAM"},
	{"washerWoman", "WASHERWOMAN"},
	{"hanged", "THE HANGED MAN"},
	{"ruby", "RUBY, THE INSTIGATOR"},
	{"racistDriver", "RACIST LORRY DRIVER"},
	{"tommy", "TOMMY LE HOMME"},
	{"siileng", "SIILENG"},
	{"smoker", "SMOKER ON THE BALCONY"},
	{"alain", "ALAIN"},
	{"eugene", "EUGENE"},
	{"echoMaker", "ECHO MAKER"},
	{"kortenaer", "KORTENAER"},
	{"phasmid", "INSULINDIAN PHASMID"},
	{"paledriver", "PALEDRIVER"},
	{"jules", "JULES PIDIEU"},
	{"notJean", "MAN WITH SUNGLASSES"},
	{"elisabeth", "ELISABETH"},
	{"tiago", "TIAGO"},
};
const static std::string	helpStr = "Usage: kimsay [-h] [-rFu] [-c character] [-w wrap] [-g gap] [-W borderWidth] [-H borderHeight] [-b borderChar] [-n name] [-f artFile] [text...]";
const static std::string	knownChars = "Default:\n\tkim\n"
								"Skills - Intellect:\n\tconceptualization\n\tdrama\n\tencyclopedia\n\tlogic\n\trhetoric\n\tvisualCalculus\n"
								"Skills - Psyche:\n\tauthority\n\tempathy\n\tespritDeCorps\n\tinlandEmpire\n\tsuggestion\n\tvolition\n"
								"Skills - Physique:\n\telectrochemistry\n\tendurance\n\thalfLight\n\tpainThreshold\n\tphysicalInstrument\n\tshivers\n"
								"Skills - Motorics:\n\tcomposure\n\thandEyeCoordination\n\tinterfacing\n\tperception\n\treactionSpeed\n\tsavoirFaire\n"
								"Characters:\n\tharry\n\tcuno\n\tjoyce\n\tklaasje\n\ttitus\n\tdeserter\n\tgarte\n\tlena\n\tnoid\n\tsoona\n\tjean\n\tidiot\n\tacele\n\tsteban\n\tandre\n\tmeasurehead\n\tsunday\n\troy\n\tplaisance\n\twarship\n\teggHead\n\tdicemaker\n\tgary\n\trene\n\tlilienne\n\tcunoesse\n\tmorell\n\tcindy\n\tmanana\n\tdolores\n\tgaston\n\tworkerWoman\n\trichGuy\n\tannette\n\ttrant\n\twasherWoman\n\thanged\n\truby\n\tracistDriver\n\ttommy\n\tsiileng\n\tsmoker\n\talain\n\teugene\n\techoMaker\n\tkortenaer\n\tphasmid\n\tpaledriver\n\tjules\n\tnotJean\n\telisabeth\n\ttiago\n";

std::string artFrom(const std::string &c) {
	return std::string(FILEDIR) + ARTDIR + c;
}

std::string dialogFrom(const std::string &c) {
	return std::string(FILEDIR) + DIALOGDIR + c + ".json";
}

typedef struct kim {
	bool				revacholianTxt = false;
	bool				frameLess = false;
	bool				discoFormat = true;
	int					wrap = 42;
	int					gap = 2;
	int					borderW = 2;
	int					borderH = 1;
	std::string			borderC = "█";
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

	while ((opt = getopt(argc, argv, "hrFuc:w:g:W:H:b:n:f:")) != -1) {
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
		case 'W':
			tmp = atoi(optarg);
			if (tmp >= 1)
				kim.borderW = tmp;
			break;
		case 'H':
			tmp = atoi(optarg);
			if (tmp >= 1)
				kim.borderH = tmp;
			break;
		case 'b':
			kim.borderC = extractFirst(optarg, kim.borderC);
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
		kim.img_w = file_w + 2 * kim.borderW;
		kim.img_h = file_h + 2 * kim.borderH;

		std::string	frameRow;
		for (int i = 0; i < kim.img_w; i++)
			frameRow.append(kim.borderC);
		std::string	framePiece;
		for (int i = 0; i < kim.borderW; i++)
			framePiece.append(kim.borderC);

		for (int i = 0; i < kim.borderH; i++)
			kim.img << frameRow << std::endl;
		while (std::getline(file, line)) {
			int len = utf8len(line.c_str());
			kim.img << framePiece << line;
			std::fill_n(std::ostream_iterator<std::string>(kim.img), file_w - len, " ");
			kim.img << framePiece << std::endl;
		}
		for (int i = 0; i < kim.borderH; i++)
			kim.img << frameRow << std::endl;
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
	t_kim	kim;

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
