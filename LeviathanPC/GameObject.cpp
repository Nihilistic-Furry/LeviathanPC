#include "stdafx.h"
#include "GameObject.h"

#include <map>
#include <list>

#include "Arena.h"
#include "Input.h"


#define tstBit(flag, bit) (flag & (1 << bit))
#define setBit(flag, bit) flag = (flag | (1 << bit))
#define clrBit(flag, bit) flag = (flag & ~(1 << bit))


enum Flags 
{
	AI_STATE_FLAG_STILL,
	AI_STATE_FLAG_BODY_ATTACK
};


//Initialize count to 0
unsigned int GameObject::objectCount = 0;


GameObject::GameObject () 
{
	//If there are no objects already, set up shaders
	if (GameObject::objectCount == 0) 
	{
		//Load program
		this->program = loadShaderProgram (
			"assets/shaders/enemies/enemy_default.vert", 
			"assets/shaders/enemies/enemy_default.frag");

		//Set uniforms
		this->uboost = GPU_GetUniformLocation (this->program.program, 
											   "damageBoost");
		GPU_SetUniformf (this->uboost, 0.0f);

		//Deactivate shader program
		GPU_ActivateShaderProgram (0, NULL);
	}

	//Incriment counter
	GameObject::objectCount++;
}

GameObject::~GameObject () 
{
	//If this is the last object, clear shaders
	if (GameObject::objectCount == 1) 
	{
		//Free program
		freeShaderProgram (this->program);
	}

	//Decriment counter
	GameObject::objectCount--;
}

bool GameObject::damage (int attack) 
{
	if (this->damageBoost > 0.0f)
	{
		return false;
	} 
	else 
	{
		if (attack >= this->hp)
		{
			this->death ();

			return true;
		} 
		else
		{
			//Set damage boost counter
			this->damageBoost = 60.0f;

			//Reduce HP
			this->hp -= attack;

			return false;
		}
	}
}


int GameObject::getHp () 
{
	return this->hp;
}


int GameObject::getHpMax () 
{
	return this->hpMax;
}


int GameObject::getAttack () 
{
	return this->attack;
}


int GameObject::getSpeed () 
{
	return this->speed;
}


Rectangle* GameObject::getHitbox () 
{
	return this->hitbox;
}


void GameObject::setHp (int value) 
{
	this->hp = value;

	return;
}


void GameObject::setHpMax (int value) 
{
	this->hpMax = value;

	return;
}


void GameObject::setAttack (int value) 
{
	this->attack = value;

	return;
}


void GameObject::setSpeed (int value) 
{
	this->speed = value;

	return;
}


void GameObject::executeAI (AIState &objectAIState, AIArgs args) 
{
	//If the object is on screen, execute AI
	if (this->hitbox->rectOnScreen ()) 
	{
		(this->*(this->objectAIState.currentAIAction)) (this->objectAIState, 
														args);
	}

	return;
}


void GameObject::updateDamageBoost () 
{
	//If object is damage boosting, check timer
	if (this->damageBoost > 0) 
	{
		if (this->damageBoost > Input::getDelta ()) 
		{
			//If player has more boost than delta, then remove boost time
			this->damageBoost -= (float) Input::getDelta ();
		}
		else 
		{
			//If time is up, stop counter and allow damage
			this->damageBoost = 0.0f;
		}
	}

	return;
}


void GameObject::activateDefaultShaderProgram () 
{
	//Render player model
	GPU_ActivateShaderProgram (this->program.program, &this->program.block);

	//Damage boost shader uniform
	this->uboost = GPU_GetUniformLocation (this->program.program, 
										   "damageBoost");
	GPU_SetUniformf (this->uboost, this->damageBoost);

	return;
}


void GameObject::aStar (AIState &objectAIState, AIArgs args) 
{
	//Basic var declaration
	double angle = 0.0f;
	bool doDirection = true;

	float xToPlayer = args.player->getHitbox ().getX () - this->hitbox->getX ();
	float yToPlayer = args.player->getHitbox ().getY () - this->hitbox->getY ();

	//Set test box rect
	Rectangle testBox;

	testBox.setX (this->hitbox->getX () - 0.25f);
	testBox.setY (this->hitbox->getY () - 0.25f);
	testBox.setWidth (0.5f);
	testBox.setHeight (0.5f);

	//If player is 
	if (!args.activeArena->rectInWalls (testBox)) 
	{
		//Set angle to angle to player
		angle = atan2 (-yToPlayer, xToPlayer);
	} 
	else 
	{
		//TODO Implement A* pathfinding algorithm to find _new angle

		//Lambada for A* heuristic
		auto heuristic = [](float startX, float startY, float nodeX, 
							float nodeY, float destX, float destY) 
		{
			//dx = node.x - player.x
			float dx1 = nodeX - destX;
			
			//dy = node.y - player.y
			float dy1 = nodeY - destY;
			
			//Calculate vector from start
			float dx2 = nodeX - startX;
			float dy2 = nodeY - startY;

			/*
			Calculate abs of cross product (tiebreaker that favors straight 
			lines)
			*/
			float cross = ((dx1 * dy2) - (dy1 * dx2));

			//Use abs of derivatives
			dx1 = (dx1 >= 0) ? dx1 : -dx1;
			dy1 = (dy1 >= 0) ? dy1 : -dy1;

			/*
			D * (dx + dy) + (D2 - 2 * D) * min (dx, dy) 
			where D = 1 & D2 = sqrt (2)
			*/
			float heuristic = (dx1 + dy1) - (0.4142135f * 
				((dy1 < dx1) ? dy1 : dx1));

			//Add tiebreaker (reduces number of checked tiles)
			heuristic += cross * 0.000390625f;

			return heuristic;
		};

		/* Start and finish coords
			this->hitbox->getX ()
			this->hitbox->getY ()
			args.player->getHitbox ().getX ()
			args.player->getHitbox ().getY ()
		*/

		//Map for coordinates around it
		//std::map<std::pair<int, int>, float> map;

		struct Point 
		{
			float x, y;

			float f, g;

			bool start = false;

			Point *lastPoint;

			Point (float i, float j, float k) : x (i), y (j), g (k) 
			{ lastPoint = NULL; };
		};

		//Open and closed set arrays
		std::list<struct Point> openSet;
		std::list<struct Point> closedSet;

		//Add starting position to openSet
		struct Point startingPoint = Point (this->hitbox->getX (), 
											this->hitbox->getY (), 0);
		startingPoint.f = heuristic (this->hitbox->getX (),
									 this->hitbox->getY (), 
									 this->hitbox->getX (), 
									 this->hitbox->getY (),
									 args.player->getHitbox ().getX (),
									 args.player->getHitbox ().getY ());
		startingPoint.start = true;
		openSet.push_back (startingPoint);

		while (!openSet.empty ()) 
		{
			//Find the part with the smallest f value
			std::list<struct Point>::iterator lowest = openSet.begin ();

			//Go through list and check if 
			for (std::list<struct Point>::iterator i = openSet.begin (); 
				 ++i != openSet.end (); ) 
			{
				//If i is lower than current lowest, change lowest to i
				if ((*i).f < (*lowest).f) 
				{
					lowest = i;
				}
			}

			//Check if destination is found
			if (Rectangle::rectIsColliding (Rectangle ((*lowest).x, 
				(*lowest).y, 0.031f, 0.031f), args.player->getHitbox ())) 
			{
				Point *n = (*lowest).lastPoint, *prev = NULL;

				if (n == NULL) {

					//If on player, do what flag says to do
					if (tstBit (objectAIState.state, AI_STATE_FLAG_STILL)) 
					{
						this->direction = NONE;
						doDirection = false;
					} 
					else
					{
						angle = atan2 (-yToPlayer, xToPlayer);
					}
				} 
				else if (n->start) 
				{
					angle = atan2 (n->y - this->hitbox->getY (), 
								   this->hitbox->getX () - n->x);
				} 
				else 
				{
					do 
					{
						prev = n;
						n = n->lastPoint;
					} while (n->lastPoint != NULL && !n->start && 
							 !n->lastPoint->start);

					angle = atan2 (prev->y - this->hitbox->getY (), 
								   this->hitbox->getX () - prev->x);
				}

				break;
			}

			//Move lowest from openSet to closedSet
			Point current = *lowest;
			
			closedSet.push_back (current);

			//Add neibors
			{
				std::list<struct Point> neibors;

				//Adjacent
				neibors.push_back (Point (current.x + 0.031f, current.y, 
								   current.g + 0.031f)); //Adjacent
				neibors.push_back (Point (current.x - 0.031f, current.y, 
								   current.g + 0.031f));
				neibors.push_back (Point (current.x, current.y + 0.031f, 
								   current.g + 0.031f));
				neibors.push_back (Point (current.x, current.y - 0.031f, 
								   current.g + 0.031f));

				//Diagonals
				neibors.push_back (Point (current.x + 0.031f, 
								   current.y + 0.031f, current.g + 
								   0.04384062043f)); 
				neibors.push_back (Point (current.x + 0.031f,
								   current.y - 0.031f, current.g +
								   0.04384062043f));
				neibors.push_back (Point (current.x - 0.031f, 
								   current.y + 0.031f, current.g + 
								   0.04384062043f));
				neibors.push_back (Point (current.x - 0.031f,
								   current.y - 0.031f, current.g +
								   0.04384062043f));

				/*
				See if any neibors are in closed set, and if they are, remove
				them
				*/
				for (std::list<struct Point>::iterator i = neibors.begin (); 
					 i != neibors.end (); ) 
				{
					bool existing = false;

					//If tile is in wall, forget about it
					if (args.activeArena->rectInWalls (Rectangle ((*i).x, 
						(*i).y, 0.031f, 0.031f))) 
					{
						i = neibors.erase (i);

						goto doneChecking;
					}

					//Check if in closed set, and if it is, remove it
					for (std::list<struct Point>::iterator j = 
						 closedSet.begin (); ++j != closedSet.end (); )
					{
						if ((*i).x == (*j).x && (*i).y == (*j).y)
						{
							if ((*i).g < (*j).g)
							{
								j = closedSet.erase (j);

								break;
							} 
							else
							{
								i = neibors.erase (i);

								goto doneChecking;
							}

							existing = true;
						}
					}

					//Check if in open set, and if so, keep most efficent
					if (!openSet.empty()) 
					{
						for (std::list<struct Point>::iterator k = 
							 openSet.begin (); ++k != openSet.end (); ) 
						{
							if ((*i).x == (*k).x && (*i).y == (*k).y) 
							{
								if ((*i).g < (*k).g) 
								{
									k = openSet.erase (k);

									break;
								} 
								else
								{
									i = neibors.erase (i);

									goto doneChecking;
								}

								existing = true;
							}
						}
					}

					//If it still exists, finish calculations
					if (!existing) 
					{
						(*i).f = (*i).g + heuristic (this->hitbox->getX (),
													 this->hitbox->getY (),
													 (*i).x, (*i).y, 
													 args.player->getHitbox ()
													 .getX (),
													 args.player->getHitbox ()
													 .getY ()); //Calc. f

						/*
						Odd notation, but essentially have a reversed linked
						list as well
						*/
						(*i).lastPoint = &(*lowest); 

						openSet.push_back (*i); //Add to openSet
					}

					i++;

				doneChecking:;
				}

				openSet.erase (lowest);
			}

			//If there have been enough checked, give up and just go to player;
			if (closedSet.size () >= 32) 
			{
				//std::cout << "Bad\n";

				//If given up, do what flag says to do
				if (tstBit (objectAIState.state, AI_STATE_FLAG_STILL)) 
				{
					this->direction = NONE;
					doDirection = false;
				} 
				else 
				{
					angle = atan2 (-yToPlayer, xToPlayer);
				}

				break;
			}
		}
	}

	//std::cout << doDirection << "\n" << angle << "\n";

	//Set direction enum if necessary
	if (doDirection) 
	{
		this->angleToDirection (angle);
	}

	//If close to player, attack, if not, don't
	if ((sqrt (xToPlayer * xToPlayer + yToPlayer * yToPlayer) >= 
		objectAIState.range))
	{
		if (tstBit (objectAIState.state, AI_STATE_FLAG_BODY_ATTACK)) 
		{
			objectAIState.currentAIAction = &GameObject::bodyAttack;
		}
	}

	return;
}


void GameObject::bodyAttack (AIState &objectAIState, AIArgs args) 
{
	float xToPlayer = args.player->getHitbox ().getX () - 
		this->hitbox->getX ();
	float yToPlayer = args.player->getHitbox ().getY () - 
		this->hitbox->getY ();

	if (Rectangle::rectIsColliding (*this->hitbox, args.player->getHitbox ())) 
	{
		args.player->damage (this->objectAIState.damage, *args.healthBar);

		if (tstBit (objectAIState.state, AI_STATE_FLAG_STILL)) 
		{
			this->direction = NONE;
		} 
		else 
		{
			double angle = atan2 (-yToPlayer, xToPlayer);

			this->angleToDirection (angle);
		}
	} 
	else 
	{
		double angle = atan2 (-yToPlayer, xToPlayer);

		this->angleToDirection (angle);
	}

	if ((sqrt (xToPlayer * xToPlayer + yToPlayer * yToPlayer) < 
		objectAIState.range)) 
	{
		objectAIState.currentAIAction = &GameObject::aStar;
	}

	return;
}


void GameObject::angleToDirection (double angle) 
{
	if ((angle <= (5 * M_PI / 8) && angle >= (3 * M_PI / 8))) 
	{
		this->direction = UP;
	} 
	else if ((angle <= (3 * M_PI / 8) && angle >= (M_PI / 8))) 
	{
		this->direction = UP_RIGHT;
	}
	else if ((angle <= (M_PI / 8) && angle >= (-M_PI / 8)))
	{
		this->direction = RIGHT;
	}
	else if ((angle <= (-M_PI / 8) && angle >= (-3 * M_PI / 8)))
	{
		direction = DOWN_RIGHT;
	} 
	else if ((angle <= (-3 * M_PI / 8) && angle >= (-5 * M_PI / 8)))
	{
		this->direction = DOWN;
	} 
	else if ((angle <= (-5 * M_PI / 8) && angle >= (-7 * M_PI / 8))) 
	{
		this->direction = DOWN_LEFT;
	} 
	else if ((angle <= (-7 * M_PI / 8) || angle >= (7 * M_PI / 8)))
	{
		this->direction = LEFT;
	} 
	else if ((angle <= (7 * M_PI / 8) && angle >= (5 * M_PI / 8))) 
	{
		this->direction = UP_LEFT;
	} 
	else
	{
		this->direction = NONE;
	}

	return;
}
