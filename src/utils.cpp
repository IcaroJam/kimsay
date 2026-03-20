#include <cstdint>
#include <algorithm>
#include <iostream>

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