#include "Tech.h"
#include "Map.h"
#include "Player.h"

Tech::Tech(TechType t, const sf::Vector2f& pos)
	: type(t)
	, position(pos)
{
	loadTexture();
	sprite.setPosition(position);
	
}

void Tech::loadTexture() {
	std::string filename;

	switch (type)
	{
	case TechType::savePoint:
		filename = "assets/Tech_savePoint.png";
		break;
	case TechType::moreDash:
		filename = "assets/Tech_moreDash.png";
		break;
	case TechType::moreJump:
		filename = "assets/Tech_moreJump.png";
		break;
	default:
		break;
	}

	if (!texture.loadFromFile(filename))
		std::cout << "Tech 텍스처 로드 실패: " << filename << "\n";

	sprite.setTexture(texture);
}

void Tech::update(float dt, Player& player, const Map& map) {
	if (consumed)
		return;

	sprite.setPosition(position);

	sf::Vector2f playerPos = player.getPosition();
	sf::Vector2f diff = playerPos - position;

	float distSq = diff.x * diff.x + diff.y * diff.y;
	if (distSq <= pickupRadius * pickupRadius) {
		applyTo(player);
		consumed = true;
	}
}
void Tech::applyTo(Player& player) {
	if (type == TechType::savePoint) {
		player.setCheckpoint(position);
		std::cout << "세이브 포인트 획득!\n";
	}
	else if (type == TechType::moreDash) {
		player.jumpCount--;
		std::cout << "대시 횟수 증가!\n";
	}
	else if (type == TechType::moreJump) {
		player.dashCount--;
		std::cout << "점프 횟수 증가!\n";
	}
}
void Tech::render(sf::RenderTarget& target) const {
	if(consumed)
		return;

	if (texture.getSize().x == 0 || texture.getSize().y == 0)
		return;

	target.draw(sprite);
}

void Tech::renderDebug(sf::RenderTarget& target) const {
	if(consumed)
		return;

	sf::CircleShape circle(pickupRadius);
	circle.setRadius(pickupRadius);
	circle.setOrigin({ pickupRadius, pickupRadius });
	circle.setPosition(position);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(1.f);
	circle.setOutlineColor(sf::Color::Yellow);

	target.draw(circle);
}


void Tech::setPosition(sf::Vector2f pos) {
	position = pos;
	sprite.setPosition(position);
}



