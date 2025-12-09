#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
	Animation(const std::string& sheetPath,
		sf::Vector2i framsize,
		int framecount,
		float frameTime);

	void update(float dt);

	void applyToSprite(sf::Sprite& sprite);

	void reset();

	bool isValid() const { return !a_frames.empty(); }
private:
	sf::Texture a_Texture;
	std::vector<sf::IntRect> a_frames;
	int a_currentFrame = 0;
	float a_frametime = 0.1f;
	float a_elapsed = 0.f;
};