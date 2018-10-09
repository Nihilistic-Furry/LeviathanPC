#pragma once

SDL_Texture* loadTexture (SDL_Renderer *renderer, char *filename);

//Renders SDL Sprite 
class Sprite {

public:

	Sprite ();
	Sprite (SDL_Renderer *renderer, char *filename);
	~Sprite ();

	//Call on window size change to correct movement
	static void updateScreenDimentions (int x, int y);
	//Translate camera position
	static void translateCamera (float x, float y);
	//Set camera position
	static void setCamera (float x, float y);

	//Render with camera
	void render (SDL_Renderer *renderer, float x, float y, float scale, SDL_Rect *clip);
	//Render without camera
	void srender (SDL_Renderer *renderer, float x, float y, float scale, SDL_Rect *clip);

	//Set blending
	void setBlendMode (SDL_BlendMode mode);
	//Set alpha
	void setAlpha (Uint8 alpha);

	//Increment or decrement (crescere) alpha value
	void crescereAlpha (int8_t alpha);
	//Increment or decrement (crescere) alpha value (with safety of negatives replaced with 0)
	void crescereAlpha_s (int8_t alpha);

	float getWidth ();
	float getHeight ();

	int getPWidth ();
	int getPHeight ();

private:

	SDL_Texture *texture;

	float width, height;
	int pWidth, pHeight;

	int alpha;

};

//Automatically parses spritesheet and renders it
class SpriteSheet {

public:

	SpriteSheet ();
	SpriteSheet (SDL_Renderer *renderer, char *filename, int w, int h);
	~SpriteSheet ();

	//Render with camera
	void render (SDL_Renderer *renderer, float x, float y, float scale, int indexX, int indexY);
	//Render without camera
	void srender (SDL_Renderer *renderer, float x, float y, float scale, int indexX, int indexY);

	//Set blending
	void setBlendMode (SDL_BlendMode mode);
	//Set alpha
	void setAlpha (Uint8 alpha);

	//Increment or decrement (crescere) alpha value
	void crescereAlpha (int8_t alpha);
	//Increment or decrement (crescere) alpha value (with safety of negatives replaced with 0)
	void crescereAlpha_s (int8_t alpha);

private:

	Sprite *sprite;

	int imagesX, imagesY, resX, resY;

};
