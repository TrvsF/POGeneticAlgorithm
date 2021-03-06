/*----------
	texture class
	object given to all drawable game entities so the may be drawn on screen

	a. travis 22nd dec
----------*/
#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "assets.h"
#include "math_helper.h"
#include "camera.h"

class texture
{
	protected:
		SDL_Renderer* m_renderer;
		SDL_Texture* m_texture;

		int m_width;
		int m_height;

		SDL_Rect m_renderRect;

	public:
		texture(std::string path);
		texture(std::string path, std::string text, int size, SDL_Color colour);
		virtual ~texture();

		int width();
		int height();

		virtual void render(Vector2 pos, float rotation);
};
#endif