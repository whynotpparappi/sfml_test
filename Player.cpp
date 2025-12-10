#include "Player.h"
#include "Map.h"


Player::Player()
    : texture("assets/Player.png")
    , sprite(texture)
{
    if (!texture.loadFromFile("assets/Player_add.png")) //플레이어 기본 텍스처 로드
    {
        std::cout << "플레이어 텍스처 로드 실패!\n";
    }
    else {
        std::cout << "5- 플레이어 텍스처 로드 성공!\n";
    }
    int totalColumns = 12;  // 실제 칸 수에 맞게 수정

    frameWidth = static_cast<int>(texture.getSize().x) / totalColumns;
    frameHeight = static_cast<int>(texture.getSize().y); // 한 줄뿐이라면 전체 높이

    animationRect = sf::IntRect({ 0, 0 }, { frameWidth, frameHeight });
    sprite.setTexture(texture);
    sprite.setTextureRect(animationRect);

    // 좌우 반전 기준점 (왼쪽 위)
    sprite.setOrigin({ 0.f, 0.f });

    sprite.setPosition(position);

	checkpointPosition = position;
}

void Player::handleInput(float dt)
{
    // ---- 좌우 이동 (A, D) ----
    float moveDir = 0.f;
	bool ADown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
	bool DDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
	bool shiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);

    if (!isDashing) {
        if (ADown) { // 왼쪽
            moveDir -= 1.f;
            if (shiftDown && isOnGround)
                moveDir -= 0.5f;
        }
        if (DDown) { // 오른쪽
            moveDir += 1.f;
            if (shiftDown && isOnGround)
                moveDir += 0.5f;
        }
        velocity.x = moveDir * MOVE_SPEED;
    }
    if (moveDir > 0.f)
		facingRight = true;
    else if (moveDir < 0.f)
		facingRight = false;


    // ---- 점프 (Space) ----
    bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
	bool sDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    bool justPressed = spaceDown && !jumpKeyWasHeld;    // 이번 프레임에 막 눌림
    bool justReleased = !spaceDown && jumpKeyWasHeld;   // 이번 프레임에 막 뗌

	if ((justPressed && jumpCount < MAX_JUMPS) && (sDown && isOnGround)) //s누르면서 점프 (아래점프)
    {
        isJumping = true;
        jumpTime = 0.f;

        isOnGround = false;
        OnPlate = false;
        jumpCount++;
    }
    else if (justPressed && jumpCount < MAX_JUMPS )   //이번에 눌렸고 점프 카운트가 max 아래일때
    {
        isJumping = true;
        jumpTime = 0.f;

        velocity.y = -JUMP_SPEED;   // 위로 발사
        isOnGround = false;
        jumpCount++;
    }

    // 3) 점프 커팅 (버튼 떼는 순간, 아직 위로 올라가는 중이면 상승 속도 줄이기)
    if (justReleased && isJumping && velocity.y < 0.f)
    {
        velocity.y *= JUMP_CUT_FACTOR;  // 톡 누르면 짧은 점프
    }

    // 공중 대시
    
    if (!isOnGround && !isDashing && dashCount < MAX_DASHES) {

        if (shiftDown&&ADown) {
            isDashing = true;
            dashDir = -1;
            dashTime = 0.f;
            dashCount++;
        }
        else if (shiftDown&&DDown) {
            isDashing = true;
            dashDir = +1;
            dashTime = 0.f;
            dashCount++;
		}
    }
    
    jumpKeyWasHeld = spaceDown;
	jumpKeyHeld = spaceDown;
}

void Player::update(float dt, const Map& map)
{
    // 1) 입력 처리
    handleInput(dt);

    // 2) 중력 적용 (가변 중력: 버튼 누르고 있을 땐 중력 약하게)
    float effectiveGravity = GRAVITY;

    // 3) 점프 길게 누르기 시간 누적
    if (isJumping && jumpKeyHeld){
        jumpTime += dt;
        if (jumpTime >= MAX_JUMP_HOLD)
            jumpKeyHeld = false;
    }

    if (isJumping && jumpKeyWasHeld && velocity.y < 0.f && jumpTime < MAX_JUMP_HOLD){
        effectiveGravity *= 0.4f;
    }
    else if (velocity.y > 0.f) {
        effectiveGravity *= FALL_GRAV_MULT;
    }

    // ==== 대시 진행 ====
    if (isDashing)
    {
        dashTime += dt;

        // 대시 동안 속도 고정 + 중력 무시
        velocity.x = dashDir * DASH_SPEED;
        velocity.y = 0.f;

        if (dashTime >= DASH_TIME)
        {
            isDashing = false;
            // 대시 끝난 후에는 중력/이동 로직이 다시 적용됨
        }
    }
    else
    {
        // 대시가 아닐 때만 중력 적용
        if (!isOnGround)
            velocity.y += effectiveGravity * dt;
    }
    
    // --- 최대 낙하 속도 제한(터미널 속도) ---
    if (velocity.y > MAX_FALL_SPEED)
        velocity.y = MAX_FALL_SPEED;

    isOnGround = false;

    // 좌우 이동
    x_move(dt);

    sprite.setPosition(position);
    sf::FloatRect bounds = sprite.getGlobalBounds();            // 플레이어의 충돌 박스 엔터티의 전역 경계 사각형을 가져옵니다

    float leftX = bounds.position.x;
    float topY = bounds.position.y;
    float width = bounds.size.x;
    float height = bounds.size.y;
    float rightX = leftX + width;
    float bottomY = topY + height;
    float centerY = topY + height * 0.5f;

    sf::Vector2f leftTop(leftX - 1.f, topY + 2.f);
    sf::Vector2f leftMid(leftX - 1.f, centerY);
    sf::Vector2f leftBot(leftX - 1.f, bottomY - 2.f);

    sf::Vector2f rightTop(rightX + 1.f, topY + 2.f);
    sf::Vector2f rightMid(rightX + 1.f, centerY);
    sf::Vector2f rightBot(rightX + 1.f, bottomY - 2.f);



    // 왼쪽 벽 충돌
    bool solidLeftTop = map.isSolidAt(leftTop);
    bool solidLeftMid = map.isSolidAt(leftMid);
    bool solidLeftBot = map.isSolidAt(leftBot);

    if (velocity.x < 0.f && (solidLeftTop || solidLeftMid || solidLeftBot))
    {
         float sampleX = leftTop.x;  // 왼쪽 샘플 x
        int   tileX   = static_cast<int>(sampleX) / Map::TILE_SIZE;
        float tileRight = (tileX + 1) * Map::TILE_SIZE;
        // 플레이어의 왼쪽이 타일 오른쪽에 딱 맞게 고정
        position.x = tileRight;
        velocity.x = 0.f;

        sprite.setPosition(position);
        bounds = sprite.getGlobalBounds();
        leftX = bounds.position.x;
        rightX = leftX + bounds.size.x;
        topY = bounds.position.y;
        bottomY = topY + bounds.size.y;
        centerY = topY + bounds.size.y * 0.5f;
    }

    // 오른쪽 벽 충돌
    bool solidRightTop = map.isSolidAt(rightTop);
    bool solidRightMid = map.isSolidAt(rightMid);
    bool solidRightBot = map.isSolidAt(rightBot);
    if (velocity.x > 0.f && (solidRightTop || solidRightMid || solidRightBot))
    {
        float sampleX = rightTop.x;  // 오른쪽 샘플 x
        int   tileX = static_cast<int>(sampleX) / Map::TILE_SIZE;
        float tileLeft = tileX * Map::TILE_SIZE;

        position.x = tileLeft - width;  // 타일 왼쪽에 딱 붙이기
        velocity.x = 0.f;

        sprite.setPosition(position);
        bounds = sprite.getGlobalBounds();
        leftX = bounds.position.x;
        rightX = leftX + bounds.size.x;
        topY = bounds.position.y;
        bottomY = topY + bounds.size.y;
        centerY = topY + bounds.size.y * 0.5f;
    }

    y_move(dt);
    sprite.setPosition(position);

    bounds = sprite.getGlobalBounds();

    leftX = bounds.position.x;
    topY = bounds.position.y;
    width = bounds.size.x;
    height = bounds.size.y;
    rightX = leftX + width;
    bottomY = topY + height;
    float centerX = leftX + width * 0.5f;

    // 발/머리 샘플 포인트
    sf::Vector2f feetL(leftX + 2.f, bottomY + 1.f);
    sf::Vector2f feetR(rightX - 2.f, bottomY + 1.f);

    sf::Vector2f headL(leftX + 2.f, topY - 1.f);
    sf::Vector2f headR(rightX - 2.f, topY - 1.f);

    
    // 바닥 충돌

    bool solidFeetL = map.isSolidAt(feetL);
    bool solidFeetR = map.isSolidAt(feetR);

	bool plateFeetL = map.isPlateAt(feetL);
    bool plateFeetR = map.isPlateAt(feetR);

    if (velocity.y >= 0.f && (solidFeetL || solidFeetR))
    {
        float sampleY = feetL.y;  // y는 둘이 거의 같으니 왼쪽 기준
        int   tileY = static_cast<int>(sampleY) / Map::TILE_SIZE;
        float tileTop = tileY * Map::TILE_SIZE;

        position.y = tileTop - height;  // 타일 위에 딱 서기
        velocity.y = 0.f;

        isOnGround = true;
        isJumping = false;
        jumpTime = 0.f;
        jumpCount = 0;

		isDashing = false;
		dashTime = 0.f;
		dashCount = 0;

        sprite.setPosition(position);
        bounds = sprite.getGlobalBounds();
        leftX = bounds.position.x;
        topY = bounds.position.y;
        width = bounds.size.x;
        height = bounds.size.y;
        rightX = leftX + width;
        bottomY = topY + height;
        centerX = leftX + width * 0.5f;
    }

    if (velocity.y >= 0.f && (plateFeetL || plateFeetR))
    {
        bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        bool sDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);

        if (spaceDown && sDown) {
            OnPlate = false;
        }
        else {
            float sampleY = feetL.y;  // y는 둘이 거의 같으니 왼쪽 기준
            int   tileY = static_cast<int>(sampleY) / Map::TILE_SIZE;
            float tileTop = tileY * Map::TILE_SIZE;

            position.y = tileTop - height;  // 타일 위에 딱 서기
            velocity.y = 0.f;

            isOnGround = true;
            isJumping = false;
            jumpTime = 0.f;
            jumpCount = 0;

            isDashing = false;
            dashTime = 0.f;
            dashCount = 0;

            sprite.setPosition(position);
            bounds = sprite.getGlobalBounds();
            leftX = bounds.position.x;
            topY = bounds.position.y;
            width = bounds.size.x;
            height = bounds.size.y;
            rightX = leftX + width;
            bottomY = topY + height;
            centerX = leftX + width * 0.5f;
        }
    }
    // 천장 충돌 (머리가 천장에 닿으면 위쪽 속도 0)

    bool solidHeadL = map.isSolidAt(headL);
    bool solidHeadR = map.isSolidAt(headR);

    if (velocity.y < 0.f && (solidHeadL || solidHeadR))
    {
        float sampleY = headL.y;
        int   tileY = static_cast<int>(sampleY) / Map::TILE_SIZE;
        float tileBottom = (tileY + 1) * Map::TILE_SIZE;

        position.y = tileBottom;  // 머리 바로 아래에 맞추기
        velocity.y = 0.f;

        sprite.setPosition(position);
        bounds = sprite.getGlobalBounds();
    }

    //상태 결정 로직
    updateState();
	updateAnimation(dt);

    // 6) 스프라이트 위치 반영
    sprite.setPosition(position);
}

void Player::renderDebug(sf::RenderTarget& target, const Map& map)
{
    sf::FloatRect bounds = sprite.getGlobalBounds();
    
    // 샘플 충돌 체크 포인트 시각화
    float leftX = bounds.position.x;
    float topY = bounds.position.y;
    float width = bounds.size.x;;
	float height = bounds.size.y;
    float rightX = leftX + width;
    float bottomY = topY + height;
    float centerX = leftX + width / 2.f;
    float centerY = topY + height / 2.f;

    // 플레이어 외곽선 박스
    sf::RectangleShape box;
    box.setPosition(bounds.position);
    box.setSize(bounds.size);
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineThickness(1.f);
    box.setOutlineColor(sf::Color::Green);
    target.draw(box);

    sf::CircleShape p(1.f);
    p.setFillColor(sf::Color::Red);

    // 발
    p.setPosition({ leftX + 1.f, bottomY + 1.f }); target.draw(p);
    p.setPosition({ rightX - 1.f, bottomY + 1.f}); target.draw(p);

    // 머리
    p.setPosition({ leftX + 1.f, topY - 1.f }); target.draw(p);
    p.setPosition({ rightX - 1.f, topY - 1.f }); target.draw(p);

    // 왼쪽
    //p.setPosition({ leftX - 5.f, topY + 2.f });  target.draw(p);
    p.setPosition({ leftX - 1.f, centerY });     target.draw(p);
    //p.setPosition({ leftX - 5.f, bottomY - 5.f }); target.draw(p);

    // 오른쪽
    //p.setPosition({ rightX + 2.f, topY + 2.f });  target.draw(p);
    p.setPosition({ rightX + 1.f, centerY });     target.draw(p);
    //p.setPosition({ rightX + 2.f, bottomY - 5.f }); target.draw(p);
}


void Player::render(sf::RenderTarget& target)
{
    if (texture.getSize().x == 0 || texture.getSize().y == 0)
        return; // 텍스처가 비어있으면 그리지 않기
    target.draw(sprite);
}

void Player::applyGravity(float dt)
{
    if (!isOnGround)
        velocity.y += GRAVITY * dt;
}


void Player::x_move(float dt)
{
    position.x += velocity.x * dt;
}

void Player::y_move(float dt)
{
    position.y += velocity.y * dt;
}

void Player::setPosition(sf::Vector2f pos)
{
    position = pos;
}

sf::Vector2f Player::getPosition() const
{
    return position;
}

void Player::updateState()
{
    if (isDashing) {
        currentState = PlayerState::Dash;
        return;
    }

    if (isOnGround)
    {
        if (velocity.x == 0.f)
            currentState = PlayerState::Idle;
        else
            currentState = PlayerState::Run;
    }
    else
    {
        if (velocity.y < 0.f)
            currentState = PlayerState::Jump;
        else
            currentState = PlayerState::Fall;
    }
}

void Player::updateAnimation(float dt)
{
    animationTimer += dt;

    switch (currentState)
    {
    case PlayerState::Idle:
        animationSpeed = 0.15f;          // 느리게
        animateRange(IDLE_START, IDLE_END);
        break;

    case PlayerState::Run:
        animationSpeed = 0.08f;          // 빨리
        animateRange(RUN_START, RUN_END);
        break;

    case PlayerState::Jump:
        setFrame(JUMP_FRAME);
        break;

    case PlayerState::Fall:
        setFrame(FALL_FRAME);
        break;

    case PlayerState::Dash:
        // 대쉬 전용 프레임이 있으면 animateRange(DASH_START, DASH_END);
        // 없다면 Run 프레임 재사용
        animationSpeed = 0.05f;
        animateRange(RUN_START, RUN_END);
        break;
    }

    // === 마지막에 좌우 반전 처리 ===
    if (facingRight) {
        sprite.setScale({ 1.f, 1.f });
        sprite.setOrigin({ 0.f, 0.f });
    }
    else {
        sprite.setScale({ -1.f, 1.f });
        sprite.setOrigin({ static_cast<float>(frameWidth), 0.f });
    }
}

void Player::animateRange(int start, int end) {
    // start~end 사이 프레임을 순환
    if (animationTimer >= animationSpeed) {
        animationTimer = 0.f;

        currentFrame++;
        if (currentFrame > end)
            currentFrame = start;
    }

    // 현재 프레임 인덱스를 Rect로 변환
    animationRect = sf::IntRect(
        { currentFrame * frameWidth, // left
        0 },                         // top (한 줄이라고 가정)
        { frameWidth,
        frameHeight }
    );
    sprite.setTextureRect(animationRect);
}

void Player::setFrame(int frameIndex)
{
    currentFrame = frameIndex;
    animationRect = sf::IntRect(
        { currentFrame * frameWidth,
        0 },
        { frameWidth,
        frameHeight }
    );
    sprite.setTextureRect(animationRect);
}

void Player::setCheckpoint(const sf::Vector2f& pos)
{
    checkpointPosition = pos;
}

void Player::resetToCheckpoint()
{
    position = checkpointPosition;
    velocity = { 0.f, 0.f };       // 속도 초기화
    isOnGround = true;             // 필요하면 상태도 초기화
    // 상태머신 쓰면 currentState = PlayerState::Idle; 이런 것도 넣어도 좋고

    sprite.setPosition(position);

}