#include "stdafx.h"
#include "Exit.h"

#include "CutEvent.h"


namespace 
{
	struct Exit::ExitList *first = NULL, *last = NULL;
}


Exit::Exit (std::string location, Rectangle box, float playerX, float playerY,
			float cameraOffsetX, float cameraOffsetY) 
{
	//Copy location string
	this->location = location;

	//Set hitbox of exit
	this->hitbox = box;

	//Set floats on player and camera stuff
	this->playerX = playerX;
	this->playerY = playerY;
	this->cameraOffsetX = cameraOffsetX;
	this->cameraOffsetY = cameraOffsetY;
}


Exit::~Exit () 
{

}


float Exit::getPlayerX ()
{
	return this->playerX;
}


float Exit::getPlayerY () 
{
	return this->playerY;
}


float Exit::getCamPosX () 
{
	return (this->playerX - this->cameraOffsetX - 0.5f);
}


float Exit::getCamPosY () 
{
	return (this->playerY - this->cameraOffsetY - 0.5f);
}


void Exit::createExitList (Exit *exit) 
{
	first = _new ExitList (exit);

	last = first;

	return;
}


void Exit::addExitList (Exit *exit) 
{
	last->next = _new ExitList (exit);

	last = last->next;

	return;
}


void Exit::deleteExitList () 
{
	struct ExitList *e, *m;

	e = first;

	while (e) 
	{
		delete e->exit;

		m = e->next;

		delete e;

		e = m;
	}

	first = NULL;
	last = NULL;

	return;
}


void Exit::testForExit (Arena *& arena, Player & player) 
{
	struct ExitList *exit = first;

	while (exit) 
	{
		if (Rectangle::rectIsColliding (exit->exit->hitbox, 
			player.getHitbox ())) 
		{
			//Set player position
			player.setPosition (exit->exit->getPlayerX (), 
								 exit->exit->getPlayerY ());

			//Set camera position (with offset)
			Sprite::setCamera (exit->exit->getCamPosX (), 
							   exit->exit->getCamPosY ()); 

			//Get location string
			char *buf = _new char[exit->exit->location.length () + 1];
			strcpy (buf, exit->exit->location.c_str ());

			//Replace arena
			delete arena;
			arena = new_Arena (buf); 

			//Clear out event list
			CutEvent::deleteEventList ();

			//Delete allocated buffer (prevent mem leak)
			delete[] buf;

			//Exit loop
			break;
		}

		exit = exit->next;
	}

	return;
}
