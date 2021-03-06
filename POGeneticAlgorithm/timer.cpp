#include "timer.h"

timer::timer()
{
	reset();
	m_elapsedTicks = 0;
	m_deltaTime = 0.0f;
}

timer::~timer()
{
}

void timer::reset()
{
	m_startTicks = SDL_GetTicks();
}

float timer::deltaTime()
{
	return m_deltaTime;
}

void timer::update()
{
	m_elapsedTicks = SDL_GetTicks() - m_startTicks;
	m_deltaTime = m_elapsedTicks * 0.001f;
}
