#include "game_engine.h"

game_engine::game_engine()
{}

// unload all managers
game_engine::~game_engine()
{
	m_assets = NULL;
	m_graphics = NULL;
	m_gtickTimer = NULL;
	m_fpsTimer = NULL;
	m_inputs = NULL;
	m_levels = NULL;
	m_physics = NULL;
	m_agents = NULL;
}

// init game engine and all managers
bool game_engine::init()
{
	m_isRunning = false;

	// init graphics
	printf("starting graphics\n");
	m_graphics = graphics::INSTANCE();
	if (m_graphics->init("POGame", 1280, 960, false))
	{
		printf("graphics started!\n\n");
		m_isRunning = true;
	}

	// init timers
	printf("loading timers...");
	m_gtickTimer = new timer();
	m_fpsTimer = new timer();
	printf("done!\n");

	// init asset manager
	printf("loading asset manager...");
	m_assets = assets::INSTANCE();
	printf("done!\n");

	// init asset manager
	printf("loading input manager...");
	m_inputs = inputs::INSTANCE();
	printf("done!\n");

	// init level manager
	printf("loading level manager...");
	m_levels = levels::INSTANCE();
	printf("done!\n");

	// init physics manager
	printf("loading physics manager...");
	m_physics = physics::INSTANCE();
	printf("done!\n");

	// init agent manager
	printf("loading agent manager...");
	m_agents = agent_manager::INSTANCE();
	printf("done!\n");

	return true;
}

// stop the game and close down
void game_engine::stop()
{
	m_graphics->clean();
}

// main game loop
void game_engine::run()
{
	while (m_isRunning)
	{
		// tick timer
		m_gtickTimer->update();
		m_fpsTimer->update();

		while (SDL_PollEvent(&m_events) != 0)
		{
			switch (m_events.type)
			{
			case SDL_QUIT:
				m_isRunning = false;
				break;
			}
		}
		// 144 times a second refresh the renderer
		if (m_gtickTimer->deltaTime() >= 1.0f / PHYS_RATE)
		{
			doUpdates();
		}

		if (m_fpsTimer->deltaTime() >= 1.0f / FRAME_RATE)
		{
			render();
		}

	}
}

// update all inputs
void game_engine::earlyUpdate()
{
	m_gtickTimer->reset();
	m_inputs->update();
}

// update game entities
void game_engine::update()
{
	m_levels->update();
	m_agents->update();
}

// reset the timer ready for next frame
void game_engine::lateUpdate()
{
	m_inputs->updatePrev();
}

void game_engine::doUpdates()
{
	earlyUpdate();
	update();
	lateUpdate();
}

// render the new frame
void game_engine::render()
{
	m_fpsTimer->reset();
	m_graphics->clearBuffer();
	m_levels->render();
	m_graphics->render();
}
