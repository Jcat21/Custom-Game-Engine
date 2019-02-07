#include "stdafx.h"
#include "components\player.h"
#include "animation.h"
#include "game.h"
#include "jsonUtil.h"
#include "physics.h"
#include "skinnedObj.h"
#include "stringUtil.h"


static const float s_accel = 900.0f;
static const float s_moveSpeed = 300.0f;
static const float s_turnSpeed = Math::ToRadians(720.0f);
static const float s_gravity = 500.0f;
static const float s_jumpStart = 300.0f;


Player::Player(SkinnedObj *pObj)
    : Character(pObj)
	, mMoveSpeed(0.0f)
    , mState(STATE_UNKNOWN)
    , mVel(Vector3::Zero)
    , mPrevAnimTime(0.0f)
{
	Matrix4 mat = mObj->mObjectData.modelToWorld;
    mPos = mat.GetTranslation();
    Vector3 fwd = mat.GetXAxis();
	mHeading = atan2f(fwd.y, fwd.x);
}

void Player::Update(float deltaTime)
{
	Vector3 move = Vector3::Zero;
	Game* pGame = mObj->GetGame();
	if (pGame->IsKeyHeld(VK_UP))
		move.x += 1.0f;
	if (pGame->IsKeyHeld(VK_DOWN))
		move.x -= 1.0f;
	if (pGame->IsKeyHeld(VK_RIGHT))
		move.y += 1.0f;
	if (pGame->IsKeyHeld(VK_LEFT))
		move.y -= 1.0f;

	float throttle = move.Length();
	if (throttle > 1.0f)
		throttle = 1.0f;

	if (throttle > 0.0f)
	{	// turn to heading
		float heading = atan2f(move.y, move.x);
		float angDelta = heading - mHeading;
		if (angDelta > Math::Pi)		//mrwTODO wrap angle
			angDelta -= Math::TwoPi;
		if (angDelta < -Math::Pi)
			angDelta += Math::TwoPi;
		float angSpd = s_turnSpeed * deltaTime;
		angDelta = Math::Clamp(angDelta, -angSpd, angSpd);
		mHeading += angDelta;
		if (mHeading > Math::Pi)		//mrwTODO wrap angle
			mHeading -= Math::TwoPi;
		if (mHeading < -Math::Pi)
			mHeading += Math::TwoPi;
	}

	{	// accelerate
		float speed = throttle * s_moveSpeed;
		float spdDelta = speed - mMoveSpeed;
		float accel = s_accel * deltaTime;
		spdDelta = Math::Clamp(spdDelta, -accel, accel);
		mMoveSpeed += spdDelta;
	}

    mPrevAnimTime = mAnimationTime;
    Character::Update(deltaTime);

    ChangeState();
    UpdateState(deltaTime);
}

bool Player::CheckGround(float* groundHeight)
{
#if 0
    *groundHeight = 0.0f;
    return true;
#else
    Physics::Ray ray(mPos + Vector3(0.0f, 0.0f, 10.0f), mPos + Vector3(0.0f, 0.0f, -10.0f));
    Vector3 hitPoint;
    bool hit = mObj->GetGame()->mPhysics.RayCast(ray, &hitPoint);
    if (hit)
        *groundHeight = hitPoint.z;
    return hit;
#endif
}

void Player::SetState(State newState)
{
    switch (newState)
    {
    case STATE_IDLE:
        SetAnim("idle");
        mVel.z = 0.0f;
        break;
    case STATE_FALL:
        SetAnim("fall");
        break;
    case STATE_WALK:
        SetAnim("run");
        mVel.z = 0.0f;
        break;
    case STATE_JUMP:
        SetAnim("jumpStart");
        mVel.z = s_jumpStart;
        break;
    case STATE_LAND:
        SetAnim("land");
        mVel.z = 0.0f;
        break;
    }
    mState = newState;
}

void Player::ChangeState()
{
    float groundHeight = 0.0f;
    bool isOverGround = CheckGround(&groundHeight);
    bool isOnGround = isOverGround && mPos.z <= groundHeight;

    switch (mState)
    {
    case STATE_UNKNOWN:
        SetState(STATE_IDLE);
        break;
    case STATE_LAND:
        if (mAnimationTime < mPrevAnimTime)
            SetState(STATE_IDLE);
    case STATE_IDLE:
        if (mObj->GetGame()->IsKeyHeld(VK_SPACE))
            SetState(STATE_JUMP);
        else if (false == isOnGround)
            SetState(STATE_FALL);
        else if (mVel.LengthSq() > 0.0001f)
            SetState(STATE_WALK);
        break;
    case STATE_WALK:
        if (mObj->GetGame()->IsKeyHeld(VK_SPACE))
            SetState(STATE_JUMP);
        else if (false == isOnGround)
            SetState(STATE_FALL);
        else if (mVel.LengthSq() < 0.0001f)
            SetState(STATE_IDLE);
        break;
    case STATE_JUMP:
        if (mAnimationTime < mPrevAnimTime)
            SetAnim("jump");
    case STATE_FALL:
        if (mVel.z < 0.0f && isOnGround)
        {
            mPos.z = groundHeight;
            SetState(STATE_LAND);
        }
        break;
    }
}

void Player::UpdateState(float deltaTime)
{
    Matrix4 mat = Matrix4::CreateRotationZ(mHeading);
    Vector3 vel = mMoveSpeed * mat.GetXAxis();
    mVel.x = vel.x;
    mVel.y = vel.y;

    switch (mState)
    {
    case STATE_IDLE:
    case STATE_LAND:
        break;
    case STATE_WALK:
        break;
    case STATE_FALL:
    case STATE_JUMP:
        mVel.z -= s_gravity * deltaTime;
        break;
    }

    mPos += mVel * deltaTime;
    mObj->mObjectData.modelToWorld = mat
        * Matrix4::CreateTranslation(mPos);
}
