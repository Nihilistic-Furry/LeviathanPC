#include "stdafx.h"
#include "Sprite.h"

#include "ErrorEnum.h"

#include "Input.h"
#include "Font.h"


namespace 
{
	float screenWidth, screenHeight;
	float cameraX, cameraY;
};


GPU_Image* loadTexture (char *filename) 
{
	GPU_Image *texture = GPU_LoadImage(filename);

	if (texture == NULL)
	{
		std::cout << "[-] SDL: " << SDL_GetError () << "\n";

		exit (ERROR_GPU_TEXTURE_LOAD);
	}

	return texture;
}


Sprite::Sprite () 
{
	//Default constructor
}


Sprite::Sprite (char *filename)
{
	//Load texture
	this->texture = loadTexture (filename);

	//Set to nearest
	GPU_SetImageFilter (this->texture, GPU_FILTER_NEAREST);

	//Get info
	int w, h;

	w = this->texture->texture_w;
	h = this->texture->texture_h;

	this->pWidth = w;
	this->pHeight = h;

	this->width = w / 960.0f;
	this->height = h / 720.0f;

	//Set things
	this->texture->anchor_x = 0.0f; //Center on edge
	this->texture->anchor_y = 0.0f;
}


Sprite::~Sprite ()
{
	GPU_FreeImage (this->texture);
}


void Sprite::updateScreenDimentions (float w, float h)
{
	screenWidth = w;
	screenHeight = h;

	Font::updateScreenDimentions (w, h);
	Rectangle::updateScreenDimentions (w, h);

	return;
}


void Sprite::translateCamera (float x, float y) 
{
	cameraX += x;
	cameraY += y;

	Font::setCamera (cameraX, cameraY);
	Rectangle::setCamera (cameraX, cameraY);

	return;
}


void Sprite::setCamera (float x, float y) 
{
	cameraX = x;
	cameraY = y;

	Font::setCamera (cameraX, cameraY);
	Rectangle::setCamera (cameraX, cameraY);

	return;
}


void Sprite::render (GPU_Target *screen, float x, float y, float scale,
					 GPU_Rect *clip) 
{
	GPU_Rect renderQuad = { (x - cameraX) * screenWidth, (y + cameraY) *
		screenHeight, this->width * screenWidth, this->height * screenHeight };

	if (clip != NULL) 
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	renderQuad.w = renderQuad.w * scale * 1.33333333333f;
	renderQuad.h = renderQuad.h * scale;

	GPU_BlitRect (this->texture, clip, screen, &renderQuad);

	return;
}


void Sprite::srender (GPU_Target *screen, float x, float y, float scale,
					  GPU_Rect *clip) 
{
	GPU_Rect renderQuad = { x * screenWidth, y * screenHeight, this->width * 
		screenWidth, this->height * screenHeight };

	if (clip != NULL) 
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	renderQuad.w = renderQuad.w * scale * 1.33333333333f;
	renderQuad.h = renderQuad.h * scale;

	GPU_BlitRect (this->texture, clip, screen, &renderQuad);

	return;
}


void Sprite::setBlendMode (GPU_BlendPresetEnum mode)
{
	GPU_SetBlendMode (this->texture, mode);

	return;
}


void Sprite::setFilter (GPU_FilterEnum filter) 
{
	GPU_SetImageFilter (this->texture, filter);

	return;
}


void Sprite::setColor (Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
	GPU_SetRGBA (this->texture, r, g, b, a);

	this->alpha = alpha;

	return;
}


void Sprite::setAlpha (Uint8 a)
{
	GPU_SetRGBA (this->texture, 255, 255, 255, a);

	this->alpha = alpha;

	return;
}


void Sprite::crescereAlpha (int8_t alpha)
{
	this->alpha += alpha;

	this->setAlpha (this->alpha);

	return;
}


void Sprite::crescereAlpha_s (int8_t alpha) 
{
	if (this->alpha + alpha <= 0)
	{
		this->alpha = 0;
	} 
	else
	{
		this->alpha += alpha;
	}

	this->setAlpha (this->alpha);

	return;
}


float Sprite::getWidth ()
{
	return this->width;
}


float Sprite::getHeight () 
{
	return this->height;
}


int Sprite::getPWidth ()
{
	return this->pWidth;
}


int Sprite::getPHeight () 
{
	return this->pHeight;
}


SpriteSheet::SpriteSheet () 
{
	//Default constructor
}


SpriteSheet::SpriteSheet (char *filename, int x, int y) 
{
	this->sprite = _new Sprite (filename);

	this->imagesX = x;
	this->imagesY = y;

	this->resX = (int) (this->sprite->getPWidth() / x);
	this->resY = (int) (this->sprite->getPHeight() / y);
}


SpriteSheet::~SpriteSheet () 
{
	delete this->sprite;
}


void SpriteSheet::render (GPU_Target *screen, float x, float y, float scale,
						  int indexX, int indexY) 
{
	GPU_Rect clip;

	clip.x = (float) (this->resX * indexX);
	clip.y = (float) (this->resY * indexY);
	clip.w = (float) this->resX;
	clip.h = (float) this->resY;

	this->sprite->render (screen, x, y, scale, &clip);

	return;
}


void SpriteSheet::srender (GPU_Target *screen, float x, float y, float scale,
						   int indexX, int indexY)
{
	GPU_Rect clip;

	clip.x = (float) (this->resX * indexX);
	clip.y = (float) (this->resY * indexY);
	clip.w = (float) this->resX;
	clip.h = (float) this->resY;

	this->sprite->srender (screen, x, y, scale, &clip);

	return;
}


void SpriteSheet::setBlendMode (GPU_BlendPresetEnum mode)
{
	this->sprite->setBlendMode (mode);

	return;
}


void SpriteSheet::setFilter (GPU_FilterEnum filter) 
{
	this->sprite->setFilter (filter);

	return;
}


void SpriteSheet::setColor (Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
	this->sprite->setColor (r, g, b, a);

	return;
}


void SpriteSheet::setAlpha (Uint8 a)
{
	this->sprite->setAlpha (a);

	return;
}


void SpriteSheet::crescereAlpha (int8_t alpha)
{
	this->sprite->crescereAlpha (alpha);

	return;
}


void SpriteSheet::crescereAlpha_s (int8_t alpha) 
{
	this->sprite->crescereAlpha_s (alpha);

	return;
}
