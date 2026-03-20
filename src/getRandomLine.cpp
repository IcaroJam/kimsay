#include <fstream>
#include <random>
#include "json.hpp"

std::string getRandomLine(std::ifstream &f) {
	nlohmann::json					data = nlohmann::json::parse(f);
	auto							arr = data["txt"];
	std::random_device				rd;
	std::mt19937 					gen(rd());
	std::uniform_int_distribution<>	dist(0, arr.size() - 1);
	int								i = dist(gen);

	return arr[i].get<std::string>();
}