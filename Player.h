#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp> // 키보드 입력용
#include <iostream>


class Map;

enum class PlayerState {
    Idle,
    Run,
    Jump,
    Fall,
    Dash
};

class Player
{
public:
    Player();

    void handleInput(float dt);
    void update(float dt, const Map& map);
    void render(sf::RenderTarget& target);
    void renderDebug(sf::RenderTarget& target, const Map& map);


    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const;

    PlayerState currentState = PlayerState::Idle;
	void updateState();
	void updateAnimation(float dt);

    void setCheckpoint(const sf::Vector2f& pos);
    void resetToCheckpoint();

    int jumpCount = 0;                  // 지금까지 뛴 점프 횟수
    int   dashCount = 0;                // 지금까지 한 대시 횟수

private:
    void applyGravity(float dt);
    void x_move(float dt);
    void y_move(float dt);


    // 1) 멤버 순서: texture가 먼저, sprite가 그다음 (중요)
    sf::Texture texture;
    sf::Sprite sprite;

    sf::Vector2f position{ 100.f, 100.f };
    sf::Vector2f velocity{ 0.f, 0.f };

    int hp = 100;

    // ==== 점프 관련 ====
    bool isOnGround = false;            //바닥인지
	bool OnPlate = false;               //플레이트 위인지
    bool isJumping = false;             // 점프 중인지

    bool jumpKeyHeld = false;           // 스페이스를 누르고 있는지
    bool jumpKeyWasHeld = false;        // 이전 프레임에 스페이스 상태

    bool  isDashing = false;            // 대시 중인지
    int   dashDir = 0;                  // -1 왼쪽, +1 오른쪽
    float dashTime = 0.f;               // 대시 경과 시간

    float jumpTime = 0.f;               // 점프 버튼 누른 시간
    

    static constexpr float MOVE_SPEED = 200.f;      //move
    static constexpr float JUMP_SPEED = 500.f;     // 초기 점프 속도
    static constexpr float GRAVITY = 1100.f;        // 중력
    static constexpr float FALL_GRAV_MULT = 1.7f;   // 떨어질 때 중력 배수

    static constexpr float MAX_JUMP_HOLD = 0.10f;   // 길게 점프 가능 시간
    static constexpr float JUMP_CUT_FACTOR = 0.4f;  // 버튼 떼면 상승속도 줄이는 비율

    static constexpr float DASH_SPEED = 1000.f;      // 대시 속도
    static constexpr float DASH_TIME = 0.10f;         // 대시 지속 시간


    static constexpr float MAX_FALL_SPEED = 2000.f;  // ★ 최대 낙하 속도
    static constexpr int   MAX_JUMPS = 2;            // 최대 점프 횟수 (1단+2단)
    static constexpr int   MAX_DASHES = 1;          // 최대 대쉬 횟수


	sf::IntRect animationRect;  // 애니메이션용 사각형 영역
	float animationTimer = 0.f;
	float animationSpeed = 0.1f; // 프레임당 지속 시간

    int currentFrame = 0;

    int frameWidth = 0;
    int frameHeight = 0;
    
    bool facingRight = true;

    const int IDLE_START = 0;
    const int IDLE_END = 3;
    const int RUN_START = 4;
    const int RUN_END = 9;
    const int JUMP_FRAME = 10;
    const int FALL_FRAME = 11;
    const int DASH_START = 12;
    const int DASH_END   = 15;


    void animateRange(int start, int end);
    void setFrame(int frameIndex);

    sf::Vector2f checkpointPosition;

};
