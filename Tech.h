#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Player;
class Map;

enum class TechType {
	savePoint,
	moreDash,
	moreJump
};

class Tech
{
public:
    Tech(TechType type, const sf::Vector2f& pos);

	void update(float dt, Player& player, const Map& map);

	void render(sf::RenderTarget& target) const;
	void renderDebug(sf::RenderTarget& target) const;

	void setPosition(sf::Vector2f pos);
	sf::Vector2f getPosition() const { return position; }

	TechType getType() const { return type; }
	bool isConsumed() const { return consumed; }


private:
	void loadTexture();
	void applyTo(Player& player);

	TechType type;

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Vector2f position;

	bool consumed = false; //È¹µæ ¿©ºÎ

	float pickupRadius = 24.f;
};
