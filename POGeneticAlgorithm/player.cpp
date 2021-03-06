#include "player.h"

player::player(Vector2 position)
{
	pos(position);

	m_inputs = inputs::INSTANCE();
	m_camera = new camera();

	m_camera->bb((int)position.x, (int)position.y, 400, 400);

	setTexture("character.png");

	m_tickVelocity = 0;

	m_projCount = 0;
	m_boostIndex = 0;
	m_canBoost = true;
	m_hasBoosted = false;
	m_boostCooldownCount = 0;
}

player::~player()
{
}

void player::playerInput()
{
	if (m_inputs->keyDown(SDL_SCANCODE_W))
	{
		moveForward();
	}
	if (m_inputs->keyDown(SDL_SCANCODE_S))
	{
		moveBackward();
	}
	if (m_inputs->keyDown(SDL_SCANCODE_D))
	{
		turnRight();
	}
	if (m_inputs->keyDown(SDL_SCANCODE_A))
	{
		turnLeft();
	}
	if (m_inputs->keyPressed(SDL_SCANCODE_SPACE))
	{
		agent_manager::INSTANCE()->pauseResume();
	}
	if (m_inputs->mouseButtonPressed(inputs::left))
	{
		agent_manager::INSTANCE()->onClick(m_inputs->mousePos());
	}
}

void player::checkBoostCooldown()
{
	if (!m_canBoost)
	{
		m_boostCooldownCount++;
		if (m_boostCooldownCount > BOOST_COOLDOWN)
		{
			m_canBoost = true;
			m_boostCooldownCount = 0;
		}
	}
}

// calculate new player velocity given some ammount to add
float player::calcVelocity()
{
	// get what the velocity would be
	float vel = velocity() + m_tickVelocity;
	float velMag = abs(vel);
	// if is going too fast even for boost clamp vel
	if (velMag > MAX_VEL_BOOST)
	{
		vel = vel > 0 ? MAX_VEL_BOOST : -MAX_VEL_BOOST;
	}
	// if no velocity is being added then either remain at rest
	// or start to slow the character down
	if (m_tickVelocity == 0)
	{
		// if vel is close to 0 set to 0
		if (velMag < 0.05f)
		{
			return 0;
		}
		// else return the player's velocity but slowed down
		return vel > 0 ? vel - 0.05f : vel + 0.05f;
	}
	// if calculated velocity is over the max then return max veloicty (- or +)
	if (velMag > MAX_VEL && !m_hasBoosted)
	{
		return vel > 0 ? MAX_VEL : -MAX_VEL;
	}
	// else return the newly calculated velocity
	return vel;
}

void player::turnRight()
{
	m_boostIndex++;
	rotation(rotation() + 3.5f);
}

void player::turnLeft()
{
	m_boostIndex++;
	rotation(rotation() - 3.5f);
}

void player::moveForward()
{
	if (velocity() >= -MAX_VEL)
		m_tickVelocity -= 0.2f;
}

void player::moveBackward()
{
	if (velocity() <= MAX_VEL)
		m_tickVelocity += 0.2f;
}

void player::boost()
{
	// if can boost, booost
	if (m_canBoost)
	{
		velocity(velocity() * BOOST_MUL[m_boostIndex]);
		m_hasBoosted = true;
	}

	// if player is moving while boosting cancel the boost
	// [mimick bhoping]
	if (m_inputs->keyDown(SDL_SCANCODE_W) || m_inputs->keyDown(SDL_SCANCODE_S))
		m_canBoost = false;
}

bool player::shouldMoveObjects(Vector2 movementVec)
{
	BoundingBox checkBB = m_camera->bb() + movementVec;
	if (checkBB.isOutOfBounds(1024, 786))
	{
		return true;
	}
	return false;
}

void player::movePlayer(Vector2 movementVec)
{
	if (shouldMoveObjects(movementVec))
	{
		physics::INSTANCE()->translateEntitiesNotPlayer(-movementVec);
	}
	else
	{
		translate(movementVec);
		m_camera->translate(movementVec);
	}
}

Vector2 player::getMovement()
{
	velocity(calcVelocity());
	Vector2 movement = Vector2(0, calcVelocity());
	return RotateVector(movement, rotation());
}

void player::cancelBoost()
{
	if (abs(velocity()) > MAX_VEL)
		m_canBoost = false;
}

void player::update()
{
	// check the player's input
	playerInput();

	// check if cooldown is over
	checkBoostCooldown();

	// move the player
	movePlayer(getMovement());

	// printf("vel : %f | pos %.1f, %.1f | rot %.1f | boost : %d\n", velocity(), pos().x, pos().y, rotation(), m_canBoost);

	// reset tick based vars
	m_boostIndex = 0;
	m_tickVelocity = 0;
	m_hasBoosted = false;
}

void player::render()
{
	// drawDebugBB();
	// m_camera->render();
	renderTexture();
}
