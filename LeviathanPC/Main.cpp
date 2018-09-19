#include "stdafx.h"

#include "ErrorEnum.h"

#include <chrono>

#include "Input.h"
#include "Player.h"
#include "Arena.h"

#include "Sprite.h"
#include "Audio.h"

using namespace std;

constexpr chrono::nanoseconds timestep (16ms); //60 ticks per sec

int main(int argc, char *args[]) {

	//Window pointer
	SDL_Window* window = NULL;

	//Initialize SDL Video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		cout << "[-] SDL: " << SDL_GetError() << "\n";

		return ERROR_SDL_INIT_VIDEO;

	} else {

		cout << "[+] SDL: Video initialized.\n";

	}

	//Initialize SDL Audio
	if (SDL_Init (SDL_INIT_AUDIO) < 0) {

		cout << "[-] SDL: " << SDL_GetError () << "\n";

		return ERROR_SDL_INIT_AUDIO;

	} else {

		cout << "[+] SDL: Audio initialized.\n";

	}

	//Get SDL 
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	int w = displayMode.w; //Get monitor dimentions in two variables
	int h = displayMode.h;

	int windowResX, windowResY;

	//Set dimentions of window to max sandard size for widescreen (I wrote this in Java and then recreated all the variables with the same name before this and copied and pasted. I love simiar programming languages)
	if (h < 240 && w < 256) {

		//144p 4:3
		windowResX = 192;
		windowResY = 144;

	} else if (h < 240) {

		//144p 16:9
		windowResX = 256;
		windowResY = 144;

	} else if (h < 360 && w < 426) {

		//240p 4:3
		windowResX = 320;
		windowResY = 240;

	} else if (h < 360) {

		//240p 16:9
		windowResX = 426;
		windowResY = 240;

	} else if (h < 480 && w < 640) {

		//360p 4:3
		windowResX = 480;
		windowResY = 360;

	} else if (h < 480) {

		//360p 16:9
		windowResX = 640;
		windowResY = 360;

	} else if (h < 720 && w < 854) {

		//480p 4:3
		windowResX = 640;
		windowResY = 480;

	} else if (h < 720) {

		//480p 16:9
		windowResX = 854;
		windowResY = 480;

	} else if (h < 1080) {

		//720p 16:9
		windowResX = 1280;
		windowResY = 720;

	} else if (h < 2160) {

		//1080p 16:9
		windowResX = 1920;
		windowResY = 1080;

	} else {

		//4K
		windowResX = 3840;
		windowResY = 2160;

	}

	Sprite::updateScreenDimentions (windowResX, windowResY);

	window = SDL_CreateWindow ("Leviathan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowResX, windowResY, SDL_WINDOW_SHOWN);

	//Window options
	SDL_SetWindowResizable (window, (SDL_bool) true);

	if (window == NULL) {

		cout << "[-] SDL: " << SDL_GetError () << "\n";

		return ERROR_SDL_WINDOW_FAILED_TO_LOAD;

	}

	SDL_Renderer *renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor (renderer, 0x00, 0x00, 0x00, 0xFF);

	Arena *activeArena = new Arena ();

	Player player = Player (renderer);

	Audio::Music *song = new Audio::Music ((char*) "assets/Ambient_Hell.wav");
	song->pause (0);

	//SDL Events
	bool exit = false;
	SDL_Event sdlEvent;

	auto startTime = chrono::high_resolution_clock::now (); //Get time right after initialization (initial time before first start time reset later on)

	while (!exit) {

		auto deltaTime = chrono::high_resolution_clock::now () - startTime; //Calculates delta (time since last frame)

		while (SDL_PollEvent (&sdlEvent)) {

			switch (sdlEvent.type) {

			case SDL_QUIT:

				exit = true;

				break;

			case SDL_WINDOWEVENT:

				if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {

					windowResX = sdlEvent.window.data1;
					windowResY = sdlEvent.window.data2;

					Sprite::updateScreenDimentions (windowResX, windowResY);

				}

				break;

			}

		}

		//Limit FPS to 60 Hz
		if (chrono::duration_cast<chrono::nanoseconds>(deltaTime) >= timestep) {

			startTime = chrono::high_resolution_clock::now();

			Input::setDelta ((double) (chrono::duration_cast<chrono::duration<double, nano>>(deltaTime) / chrono::duration_cast<chrono::duration<double, nano>>(timestep))); //Set delta

			//Input
			Input::update ();

			//Update

			//cout << "Direction: " << Input::eightDirection () << "\n\n";

			player.eightDirection (activeArena->canMove (player.getHixbox ())); //Get if player can wall

			player.update ();

			//Render

			song->render ();

			SDL_RenderClear (renderer);

			//Render things
			player.render(renderer);

			SDL_RenderPresent (renderer);

		}

	}

	//Close program

	delete song;

	//Destroy window
	SDL_DestroyWindow (window);

	//Quit SDL subsystems
	SDL_CloseAudio ();
	SDL_Quit ();

	return 0;

}
