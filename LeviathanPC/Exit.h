#pragma once

/**
 * Author:    Alexis Harper
 * Created:   18.10.2018
 *
 * Description: Exit checks if the player is in an exit area, and if so, load
 *              up a new arena.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "Rectangle.h"
#include "Arena.h"
#include "Player.h"

//Exit object
class Exit {

public:

	Exit (std::string location, Rectangle box, float playerX, float playerY, float cameraOffsetX, float cameraOffsetY);
	~Exit ();

	//Linked list for exits
	struct ExitList {

		Exit *exit;
		ExitList *next;

		ExitList (Exit *exit) : exit (exit) { next = NULL; };

	};

	float getPlayerX ();
	float getPlayerY ();
	float getCamPosX ();
	float getCamPosY ();

	static void createExitList (Exit *exit);
	static void addExitList (Exit *exit);
	static void deleteExitList ();

	static void testForExit (Arena **arena, Player *player);

private:

	Rectangle hitbox;

	std::string location;

	float playerX, playerY, cameraOffsetX, cameraOffsetY;

};

