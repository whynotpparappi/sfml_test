#include "Map.h"
#include "Player.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

Map::Map(int roomX, int roomY)
    : tileset()
    , tileSprite(tileset)
{
    loadTileTextures();
    loadRoom(roomX, roomY);
}

// ----------------------------
// room 파일 로드
// ----------------------------
void Map::loadRoom(int roomX, int roomY)
{
    currentRoomX = roomX;
    currentRoomY = roomY;

    std::stringstream ss;
    ss << "assets/maps/room_" << roomX << "_" << roomY << ".txt";
    std::string filename = ss.str();
    std::cout << filename << "로딩" << std::endl;

    loadFromFile(filename);
}

// ----------------------------
// txt 파일 -> tilemap 로드
// ----------------------------
void Map::loadFromFile(const std::string& filename)
{
    tilemap.clear();

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "맵 파일 로드 실패: " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int tile;
        std::vector<int> row;

        while (ss >> tile)
            row.push_back(tile);

        tilemap.push_back(row);
    }
}

// ----------------------------
// 충돌 체크
// ----------------------------
bool Map::isSolidTile(int x, int y) const
{
    if (y < 0 || y >= static_cast<int>(tilemap.size()))
        return false;
    if (x < 0 || x >= static_cast<int>(tilemap[y].size()))
        return false;

    int tileID = tilemap[y][x];

    // 0은 빈칸
    if (tileID == 0)
        return false;

	// 타일ID 범위 체크
     if (tileID < 0 || tileID >= static_cast<int>(solidTable.size()))
         return false;

    return solidTable[tileID];
}

bool Map::isSolidAt(const sf::Vector2f& worldPos) const
{
    // 현재 방의 왼쪽 위 월드 좌표
    float roomOriginX = currentRoomX * ROOM_TILES_X * TILE_SIZE;
    float roomOriginY = currentRoomY * ROOM_TILES_Y * TILE_SIZE;

    // 방 안에서의 로컬 타일 좌표
    int localX = static_cast<int>((worldPos.x - roomOriginX) / TILE_SIZE);
    int localY = static_cast<int>((worldPos.y - roomOriginY) / TILE_SIZE);

    return isSolidTile(localX, localY);
}

// 발판 충돌 체크
bool Map::isPlateTile(int x, int y) const
{
    if (y < 0 || y >= static_cast<int>(tilemap.size()))
        return false;
    if (x < 0 || x >= static_cast<int>(tilemap[y].size()))
        return false;

    int tileID = tilemap[y][x];

    // 0은 빈칸
    if (tileID == 0)
        return false;

	// 타일ID 범위 체크
    if (tileID < 0 || tileID >= static_cast<int>(plateTable.size()))
        return false;

    return plateTable[tileID];
}

// worldPos 기준 발판 위치 체크
bool Map::isPlateAt(const sf::Vector2f& worldPos) const
{
    // 현재 방의 왼쪽 위 월드 좌표
    float roomOriginX = currentRoomX * ROOM_TILES_X * TILE_SIZE;
    float roomOriginY = currentRoomY * ROOM_TILES_Y * TILE_SIZE;

    // 방 안에서의 로컬 타일 좌표
    int localX = static_cast<int>((worldPos.x - roomOriginX) / TILE_SIZE);
    int localY = static_cast<int>((worldPos.y - roomOriginY) / TILE_SIZE);

    if (!isPlateTile(localX, localY))
        return false;

    // plate 타일의 world Y 위치
    float tileTop = currentRoomY * ROOM_TILES_Y * TILE_SIZE + localY * TILE_SIZE;

    // worldPos.y가 tileTop ~ tileTop+1 사이일 때만 true
    return (worldPos.y >= tileTop && worldPos.y < tileTop + 5.f);

    return isPlateTile(localX, localY);
}


// 방 전환 처리
void Map::updateRoomTransition(const sf::Vector2f& pos)
{
    float px = pos.x;
    float py = pos.y;

    float roomWidth = ROOM_TILES_X * TILE_SIZE;
    float roomHeight = ROOM_TILES_Y * TILE_SIZE;

    // 오른쪽 이동
    if (px > (currentRoomX + 1) * roomWidth)
        loadRoom(currentRoomX + 1, currentRoomY);

    // 왼쪽 이동
    else if (px < currentRoomX * roomWidth)
        loadRoom(currentRoomX - 1, currentRoomY);

    // 아래 이동
    if (py > (currentRoomY + 1) * roomHeight)
        loadRoom(currentRoomX, currentRoomY +  1);
    
    // 위로 이동
    else if (py < currentRoomY * roomHeight)
        loadRoom(currentRoomX, currentRoomY - 1);
}

//tilemap 디버그 렌더링
void Map::renderDebug(sf::RenderTarget& target)
{
    for (int y = 0; y < (int)tilemap.size(); ++y)
    {
        for (int x = 0; x < (int)tilemap[y].size(); ++x)
        {
            int tile = tilemap[y][x];
            if (tile == 0) continue; // 빈칸은 그리지 않음

            float worldX = currentRoomX * ROOM_TILES_X * TILE_SIZE + x * TILE_SIZE;
            float worldY = currentRoomY * ROOM_TILES_Y * TILE_SIZE + y * TILE_SIZE;

            sf::RectangleShape rect;
            rect.setPosition({ worldX, worldY });
            rect.setSize({ (float)TILE_SIZE, (float)TILE_SIZE });

            // 충돌 타일은 빨간 테두리, 비충돌은 파란 테두리
            if (solidTable.size() > (std::size_t)tile && solidTable[tile])
                rect.setOutlineColor(sf::Color::Red);
            else
                rect.setOutlineColor(sf::Color::Blue);

            rect.setOutlineThickness(0.5);
            rect.setFillColor(sf::Color::Transparent);

            target.draw(rect);
        }
    }
}

// 타일 PNG 로딩
void Map::loadTileTextures()
{
    tileTextures.resize(9);

    loadTile(1, "assets/tiles/1_1.png");
    loadTile(2, "assets/tiles/1_2.png");
    loadTile(3, "assets/tiles/1_3.png");
    loadTile(4, "assets/tiles/2_1.png");
    loadTile(5, "assets/tiles/2_2.png");
    loadTile(6, "assets/tiles/2_3.png");
    loadTile(7, "assets/tiles/3_1.png");
    loadTile(8, "assets/tiles/3_2.png");
    loadTile(9, "assets/tiles/3_3.png");
    loadTile(10, "assets/tiles/0_1.png"); //발판
    loadTile(11, "assets/tiles/0_2.png"); //발판
    loadTile(12, "assets/tiles/0_3.png"); //발판

    solidTable.assign(13, false);  // 1~9 번 solid타일까지 있다고 가정
	plateTable.assign(13, false); // 1~3 번 plate타일까지 있다고 가정

    for(int id = 1 ; id <= 9 ; ++id)
		solidTable[id] = true;

	plateTable[10] = true;
    plateTable[11] = true;
	plateTable[12] = true;
}

// ----------------------------
// tileID → vector index 로 저장
// ----------------------------
void Map::loadTile(int tileID, const std::string& filename)
{
    int index = tileID - 1;
    if (index < 0)
        return;

    if (tileTextures.size() <= index)
        tileTextures.resize(index + 1);

    if (!tileTextures[index].loadFromFile(filename))
    {
        std::cout << "타일 로드 실패: " << filename << "\n";
    }

}
 

void Map::renderBackground(sf::RenderTarget& target)
{
    const int centerX = 2;
    const int centerY = 0;

    // 기준 방과의 거리 (맨해튼 거리 사용)
    int dx = std::abs(getRoomX() - centerX);
    int dy = std::abs(getRoomY() - centerY);
    int dist = dx + dy + 1; // (2,0) 에서는 0, 한 칸 옆은 1, 대각 2 ...

  
	float colorValue = 255*(1 - (1 / dist));
	target.clear(sf::Color(colorValue, colorValue, colorValue)); // 배경색 설정
    return;

}

// 최종 안전 처리된 타일 렌더링
void Map::render(sf::RenderTarget& target)
{
    //renderBackground(target);
    if (tilemap.empty())
        return;

    for (int y = 0; y < static_cast<int>(tilemap.size()); ++y)
    {
        if (tilemap[y].empty())
            continue;

        for (int x = 0; x < static_cast<int>(tilemap[y].size()); ++x)
        {
            int tile = tilemap[y][x];
            if (tile <= 0)
                continue;

            int index = tile - 1;

            // 1. 벡터 범위 체크 (필수)
            if (index < 0 || index >= static_cast<int>(tileTextures.size()))
                continue;



            //std::vector<sf::Sprite>& vectorSprites = tileSprites;
            sf::Sprite spr(tileTextures[index]); // 복사



            // 2. 스프라이트 텍스처 체크 (로드 실패 대비)     
            //const sf::Texture* tex = &spr.getTexture();
            //if (!tex || tex->getSize().x == 0 || tex->getSize().y == 0)
            //    continue;

            // 3. 타일 위치 설정
            float fX = static_cast<float>((currentRoomX * ROOM_TILES_X * TILE_SIZE + x * TILE_SIZE));
            float fY = static_cast<float>((currentRoomY * ROOM_TILES_Y * TILE_SIZE + y * TILE_SIZE));
            spr.setPosition({ fX,fY });

            target.draw(spr);
        }
    }
}