#pragma once

#include <SFML/Graphics.hpp>
#include <optional>   
#include "Player.h"
#include "Map.h"
#include "Tech.h"


class Game
{
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::View camera;
    Player player;
    Map map;
	Tech tech;

    std::vector<Tech> techs;

	int checkpointRoomX = 0;
	int checkpointRoomY = 0;

    void processEvents();
    void update(float dt);
    void render();

	void resetToCheckpoint();
};
