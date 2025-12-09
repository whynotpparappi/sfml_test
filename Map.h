#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

class Player;

class Map
{
public:
    Map(int roomX, int roomY);

    void loadRoom(int roomX, int roomY);
    void updateRoomTransition(const sf::Vector2f& playerPos);
    void render(sf::RenderTarget& target);
    void renderDebug(sf::RenderTarget& target);
	void renderBackground(sf::RenderTarget& target);

    int getRoomX() const { return currentRoomX; }
    int getRoomY() const { return currentRoomY; }

    const std::vector<std::vector<int>>& getTilemap() const { return tilemap; }
    bool isSolidTile(int x, int y) const;
    bool isSolidAt(const sf::Vector2f& worldPos) const;   // 위치

    bool isPlateTile(int x, int y) const;
    bool isPlateAt(const sf::Vector2f& worldPos) const;   // 위치

    static constexpr int TILE_SIZE = 32;       // 타일 크기

    static constexpr int ROOM_TILES_X = 32;    // 방 가로 타일 수
    static constexpr int ROOM_TILES_Y = 18;    // 방 세로 타일 수

private:
    sf::Texture tileset;
    sf::Sprite  tileSprite;

    std::vector<sf::Texture> tileTextures;   // 타일번호 → Texture

    std::vector<std::vector<int>> tilemap;

    std::vector<bool> solidTable;   // [타일ID] → true/false
	std::vector<bool> plateTable;   // [타일ID] → true/false

    int currentRoomX;
    int currentRoomY;

    void loadFromFile(const std::string& filename);
    void loadTileTextures();
    void loadTile(int tileID, const std::string& filename);

};
