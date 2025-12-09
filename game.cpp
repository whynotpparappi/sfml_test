#include "Game.h"
#include "Player.h"
#include "Map.h"


Game::Game()
//  SFML 3에서는 VideoMode에 {width, height} 형태로 넘기는 걸 권장
    : window(sf::VideoMode({ 1920u, 1080u }), "Metroidvania") 
    , camera()
    , player()
    , map(0,0)       //시작 방
	, tech(TechType::savePoint, { 200.f, 200.f }) //테크 초기화
{
    window.setFramerateLimit(144);               //144framerate

    camera.setSize({ Map::ROOM_TILES_X * Map::TILE_SIZE,
        Map::ROOM_TILES_Y * Map::TILE_SIZE }); // Map 크기에 맞게 카메라 작동
    camera.setCenter({ camera.getSize().x / 2.f, //중앙
        camera.getSize().y / 2.f });

    player.setPosition({ 50.f, 300.f }); //player 위치 50 50 설정

	player.setCheckpoint(player.getPosition());
    checkpointRoomX = map.getRoomX();
    checkpointRoomX = map.getRoomY();
}

void Game::run()
{
    sf::Clock clock;                //sfml Clock클래스 호출

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds(); //델타 타임 (fps가 달라도 동일한 속도로 이동가능)
        // delta time clock객체의 시간을 0으로 리셋하고 이를 float초단위로 받음

        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents()
{
    //  SFML 3에서는 pollEvent가 std::optional<sf::Event> 를 반환
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        // 닫기 이벤트
        if (event->is<sf::Event::Closed>()) // event is
        {
            window.close();
        }
        
        //키보드
        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::R)
                resetToCheckpoint();
        }
    }
}

void Game::resetToCheckpoint()
{
    map.loadRoom(checkpointRoomX, checkpointRoomY);

    // 2. 플레이어를 체크포인트 위치로 이동
    player.resetToCheckpoint();

    // 3. 카메라도 플레이어 기준으로 맞추기
    camera.setCenter(player.getPosition());

    // 4. 적, 아이템 등도 리셋하고 싶으면 여기서 초기화
    // enemies.clear();
    // loadEnemiesForRoom(checkpointRoomX, checkpointRoomY);
}

void Game::update(float dt)
{
    //플레이어 업데이트
    player.update(dt, map);

    tech.update(dt, player, map);

    // 맵 이동
    map.updateRoomTransition(player.getPosition()); 

    // 카메라를 현재 방 중앙으로 고정
    float cx = map.getRoomX() * Map::ROOM_TILES_X * Map::TILE_SIZE
        + (Map::ROOM_TILES_X * Map::TILE_SIZE) / 2;

    float cy = map.getRoomY() * Map::ROOM_TILES_Y * Map::TILE_SIZE
        + (Map::ROOM_TILES_Y * Map::TILE_SIZE) / 2;

    camera.setCenter({ cx, cy }); // view
}

void Game::render()
{
    window.clear();

    window.setView(camera);
    map.render(window);
    player.render(window);
	tech.render(window);

    map.renderDebug(window);
	player.renderDebug(window, map);
    tech.renderDebug(window);

    window.display();

}
