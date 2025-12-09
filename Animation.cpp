#include "Animation.h"
#include <iostream>


Animation::Animation(const std::string& sheetPath,
	sf::Vector2i framsize,
	int framecount,
	float frameTime)
	: a_frametime(frameTime)
{
	if (!a_Texture.loadFromFile(sheetPath)) {
		std::cout << "애니메이션 텍스처 로드 실패: " << sheetPath << std::endl;
		return;
	}
	for (int i = 0; i < framecount; ++i) {
		int x = (i * framsize.x) % a_Texture.getSize().x;
		int y = (i * framsize.x) / a_Texture.getSize().x * framsize.y;
		a_frames.emplace_back(x, y, framsize.x, framsize.y);
	}
}