#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

struct Rect {
	uint16_t type;
	uint32_t width;
	uint32_t height;
	uint32_t y;
	uint32_t x;
};

struct ImageData {
	std::string path;
	std::vector<Rect> rects;
};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

void ReadFile(std::string path, std::vector<ImageData>& vecData) {
	std::ifstream ifs(path, std::ios::in);
	if (ifs.is_open()) {
		ifs.seekg(0, ifs.beg);

		while (!ifs.eof()) {
			ImageData img;
			
			std::string rawData;
			std::getline(ifs, rawData);

			std::vector<std::string> splited = split(rawData, ';');
			if (splited.size() > 0) {
				img.path = splited[0];

				for (int i = 1; i < splited.size(); ++i) {
					Rect rect;
					memset(&rect, 0, sizeof(Rect));
					std::vector<std::string> values = split(splited[i], ':');

					rect.type = std::stoi(values[0]);
					rect.x = std::stoi(values[1]);
					rect.y = std::stoi(values[2]);
					rect.width = std::stoi(values[3]);
					rect.height = std::stoi(values[4]);

					img.rects.push_back(rect);
				}

				vecData.push_back(img);
			}		
 		}
	}
}

void main() {
	std::vector<ImageData> vecData;
	ReadFile("I:\\RonCohen\\Workspace\\Main Projects\\LabelerCV\\LabelerCV\\LabelerCV\\Full\\trainval.txt", vecData);
}