#include "agent.h"

void agent::moveForward()
{
	if (velocity() >= -MAX_VEL)
		m_tickVelocity -= 0.2f;
}

void agent::moveBackward()
{
	if (velocity() <= MAX_VEL)
		m_tickVelocity += 0.2f;
}

void agent::turnLeft()
{
	m_boostIndex++;
	rotation(rotation() - 3.5f);
}

void agent::turnRight()
{
	m_boostIndex++;
	rotation(rotation() + 3.5f);
}

void agent::boost()
{
	// if can boost, booost
	if (m_canBoost)
	{
		velocity(velocity() * BOOST_MUL[m_boostIndex]);
		m_hasBoosted = true;
	}

	// if player is moving while boosting cancel the boost
	// [mimick bhoping]
	// TODO : ADD CHECK IF PLAYER IS MOVING FORWARD
	if (true)
		m_canBoost = false;
}

void agent::checkBoostCooldown()
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

void agent::cancelBoost()
{
	if (abs(velocity()) > MAX_VEL)
		m_canBoost = false;
}

void agent::doMovement()
{
	Vector2 movementVec = getMovementVector();

	BoundingBox nextFrameBB = bb() + movementVec;

	// if player is going to collide with another object
	if (physics::INSTANCE()->isGoingToCollideWithBB(nextFrameBB))
	{
		// cancel the player's boost
		cancelBoost();

		// try and move only 1 axis (for the sliding against the wall effect
		Vector2 xVec = Vector2(movementVec.x, 0);
		Vector2 yVec = Vector2(0, movementVec.y);

		BoundingBox xbb = bb() + xVec;
		BoundingBox ybb = bb() + yVec;

		// can move x
		if (!physics::INSTANCE()->isGoingToCollideWithBB(xbb))
		{
			movePlayer(xVec * 2);
		}
		// can move y
		if (!physics::INSTANCE()->isGoingToCollideWithBB(ybb))
		{
			movePlayer(yVec * 2);
		}

		// only a little bit of slide
		return;
	}
	// if not
	else
	{
		movePlayer(movementVec);
	}
}

Vector2 agent::getMovementVector()
{
	velocity(calcVelocity());
	Vector2 movement = Vector2(0, calcVelocity());
	return RotateVector(movement, rotation());
}

float agent::calcVelocity()
{
	float vel = velocity() + m_tickVelocity;
	float velMag = abs(vel);

	if (velMag > MAX_VEL_BOOST)
	{
		vel = vel > 0 ? MAX_VEL_BOOST : -MAX_VEL_BOOST;
	}

	if (m_tickVelocity == 0)
	{
		if (velMag < 0.05f)
		{
			return 0;
		}
		return vel > 0 ? vel - 0.05f : vel + 0.05f;
	}

	if (velMag > MAX_VEL && !m_hasBoosted)
	{
		return vel > 0 ? MAX_VEL : -MAX_VEL;
	}

	return vel;
}

void agent::movePlayer(Vector2 movementVec)
{
	translate(movementVec);
}

bool agent::handleGeneInputs(int tick)
{
	gene* tempGene = m_genome->getGeneAtIndex(tick);

	if (tempGene == NULL)
	{
		return false;
	}

	if (tempGene->shouldMoveForward())
	{
		moveForward();
	}

	if (tempGene->shouldMoveBackward())
	{
		moveBackward();
	}

	if (tempGene->shouldTurnRight())
	{
		turnRight();
	}

	if (tempGene->shouldTrunLeft())
	{
		turnLeft();
	}

	if (tempGene->shouldBoost())
	{
		boost();
	}

	return true;
}

void agent::shouldHighlight(bool flag)
{
	m_shouldHighligt = flag;
}

agent::agent(Vector2 position, goal* goal)
{
	pos(position);
	rotation(270);
	m_defaultPos = position;

	setTexture("enemy.png");

	m_goal = goal;

	m_tickVelocity = 0;

	m_projCount = 0;
	m_boostIndex = 0;
	m_canBoost = true;
	m_hasBoosted = false;
	m_shouldHighligt = false;
	m_boostCooldownCount = 0;

	m_genome = new genome();
	m_genome->populate();

	physics::INSTANCE()->addEntityNoCollison(this);
}

agent::~agent()
{
}

void agent::resetPos()
{
	pos(m_defaultPos);
	rotation(270);
	setBB();
}

genome* agent::gnome()
{
	return m_genome;
}

void agent::gnome(genome* g)
{
	m_genome = g;
}

float agent::getFitness()
{
	float dtg = 1.0f / distnaceBetweenVecs(pos(), m_goal->pos());
	return dtg; 
}

std::string agent::getGeneString()
{
	return m_genome->getGeneAtIndex(m_tick)->getAsString();
}

void agent::update(int tick)
{
	if (!active())
		return;

	m_tick = tick;

	// does gene inputs
	if (!handleGeneInputs(tick))
	{
		printf("\n\nERROR HANDLING GENES IN AGENT\n\n");
		return;
	}

	// check the boost cooldown meter
	checkBoostCooldown();

	// do the actual moving part
	doMovement();

	m_boostIndex = 0;
	m_tickVelocity = 0;
	m_hasBoosted = false;
}

void agent::render()
{
	renderTexture();

	if (m_shouldHighligt)
		drawDebugBB();
}
