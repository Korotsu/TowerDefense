#include <Game.h>
#include <my_put_string.h>
#include <cassert>
#include <iostream>
#include <int_to_alpha.h>
#include <string_length.h>

const int FRAMES_PER_SECOND = 60;

void game(screen* sc, Time* t)
{
	// all in the name 
    object* obj             = game_init(sc);
    bool isGameRunning      = true;
    bool is_paused     = false;
	//FPSmanager manager;
	float cooldown_for_spawn = SDL_GetTicks();
	//Initialisation of fps limitor	
	//SDL_initFramerate(&manager);
	//put throught 60fps 
	//SDL_setFramerate(&manager,60);
    SDL_Event event;
    while (isGameRunning)
    {
        //the event recorder.
		if (SDL_PollEvent(&event) == 1)
		{
			// allow the player to close the game with the escape keyboard touch.
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				isGameRunning = false;
				game_destroy(obj,sc);
				sc->sc_number = 0;
				return;
			}
		}

		if (obj->castle_life <= 0)
		{
			game_destroy(obj,sc);
			sc->sc_number = 3;
			isGameRunning = false;
			sc->wave_m_lvl = 1;
			return;
		}
		
        //make the game Pause by pressing Space;
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
		{
			is_paused = is_game_paused(is_paused,sc,t);
		}

		//the main_proccess loop of the game if he isn't pause.
		if (!is_paused)
		{
            if (isGameRunning)
            {	
				t->t = SDL_GetTicks(); // current time in milliseconds 

				//the refresh of timer each frame. 
				if (t->t > t->prevTime)
				{
					t->deltaTime = (t->t - t->prevTime) / 1000.f; // convert deltatime in seconds
					t->prevTime = t->t;
				}
				
				//call all the process of the mouse
				mouse_process(obj,t);

				//cooldown protection for wawe to spawn then spawn rate for ennemies
				if (t->t - cooldown_for_spawn >= 3000)
				{
					if ((t->t - t->spawn_time)/350.f >= 1)
					{
						create_enemy(obj,t);
					}

					//next wave caller
					if (sc->nb_enemy == 0)
					{
						sc->wave_m_lvl += sc->wave_m_lvl;
						sc->wave++;
						generate_wave(sc,obj);
    					control_panel(obj->cp);
						cooldown_for_spawn = SDL_GetTicks();
					}
					
				}
				//all in the name... again
                game_Uptdate_and_Print(sc,obj,t);
            }
        }
		// Frame Per Second controller
		//SDL_framerateDelay(&manager);
    }
}

/**********************************************************************************************************************************************************************************************************/
/******************OBJECTS***********************************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************************************************/

/******************TOWER**********************************************************************************************************************************************************************/

tower::tower()
{}

tower::tower(object*obj, unsigned int type_t)
{
	damage 						=  0;
	type 						=  type_t;
	lvl							=  1;
	target						= NULL;
	radius						= 100;
	draged 						= true;
	origin.x 					= obj->mouse.x;
	origin.y					= obj->mouse.y;
	I.x     					= 1;
	I.y 						= 0;
	J 							= normal_rotate_90_vector(I);
	rect 						= {obj->mouse.x - 10, obj->mouse.y - 10 , 20 , 20};
	fire_time 					= 0;
	upgrade_turret_cooldown 	= 0;
	is_alive					= true;

	if (type - 1 == 0)
	{
		damage = 10;
		case_char = 'F';
		fire_rate = 200;
		cost = 100;
	}

	else if (type - 1 == 1)
	{
		damage = 1;
		case_char =  'W';
		fire_rate = 50;
		cost = 80;
	}

	else if (type - 1 == 2)
	{
		damage = 50;
		case_char = 'Z';
		fire_rate = 2000;
		cost = 120;
	}
	
}

void tower::tower_fire(object* obj, Time* t, screen* sc)
{
	//target the more advanced ennemies in the map
	float advancement = 0;
	target = NULL;
	for (enemy* enemy : obj->enemy_list)
	{
		Vector2 diff;
		diff.x =  	origin.x - enemy->origin.x;
		diff.y = 	origin.y - enemy->origin.y;
		float len = vector_length(diff);
		if (len <= radius)
		{
			if (advancement < enemy->advancement )
			{
				advancement = enemy->advancement;
				target = enemy;
			}
		}
	}

	//then SHOOT HIM IN THE FACE ... 
	if (target != NULL)
	{
		if ((t->t - fire_time) >= fire_rate)
		{
			if (type == 2)
			{
				target->slow_lvl = 1;
				target->life_pts = target->life_pts - damage;
			}
			
			else if (type == 3)
			{
				for (enemy* enemy : obj->enemy_list)
				{
					Vector2 diff;
					diff.x =  	target->origin.x - enemy->origin.x;
					diff.y = 	target->origin.y - enemy->origin.y;
					float len = vector_length(diff);
					if (len <= 100)
					{
						enemy->life_pts -= damage;
						if (enemy->life_pts <= 0)
						{
							enemy->is_alive = false;
							obj->money += enemy->money_value;
						}
					}
				}	
			}

			else
			{
				target->life_pts -= damage;
			}

			SDL_SetRenderDrawColor(sc->renderer,223,74,5,1);
			SDL_RenderDrawLine(sc->renderer,origin.x,origin.y,target->origin.x,target->origin.y);
			
			if (target->life_pts <= 0)
			{
				target->is_alive = false;
				obj->money += target->money_value;
			}
			fire_time = SDL_GetTicks();
		}
	}
}

tower::~tower()
{}

/******************TOWER**********************************************************************************************************************************************************************/

/******************MOUSE**********************************************************************************************************************************************************************/

Mouse::Mouse()
{
	draging_a_turret = false;
	in_rect1 = false;
	in_rect2 = false;
	in_rect3 = false;
}

void Mouse::drag_a_turret(object* obj, int type)
{
	obj->turret_list.push_back(new tower(obj,type));
}

Mouse::~Mouse()
{}

/******************MOUSE**********************************************************************************************************************************************************************/

/******************MAP****************************************************************************************************************************************************************************************/

int map::get(int i, int j)
{
	if (j >= 16)
	{
		j = 15;
	}
	return (nb_of_lines - 1) * j + i;
}

void map::set(int i, int j, int a)
{
	int id = get(i,j);
	assert(id < (int)map_list.size());
	map_list[id] = a;
}

//All in the name
void map::draw_the_grid(screen* sc)
{
	unsigned int x = 0;
	for (unsigned int i = 0; i < nb_of_lines; i++)
	{
		SDL_RenderDrawLine(sc->renderer,x,0,x,832);
		x += 32 ;
	}

	x = 0;
	
	for (unsigned int i = 0; i < nb_of_collumns; i++)
	{
		SDL_RenderDrawLine(sc->renderer,0,x,1280,x);
		x += 52;
	}
}

//print the map in the console
void map::show_map()
{
	std::cout << "][";
	for (unsigned int j = 0; j < nb_of_collumns-1; j++)
	{
		for (unsigned int i = 0; i < nb_of_lines-1; i++)
		{
			std::cout << map_list[get(i,j)] << "][";
		}
		std::cout << std::endl << "][";
	}
}

//All in the name
void map::draw_the_start_point(object* obj,screen* sc,unsigned int i,unsigned int j, std::vector<int> color)
{
	obj->enemy_spawnpoint.x = map_list_blocks[get(i,j)].x;
	obj->enemy_spawnpoint.y = map_list_blocks[get(i,j)].y + 26;
	draw_a_horizontal_block(sc,i,j,color);
}

//All in the name
void map::draw_a_horizontal_block(screen* sc, unsigned int i, unsigned int j,std::vector<int> color)
{
	SDL_SetRenderDrawColor(sc->renderer, color[0],color[1],color[2],1);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y + 52 ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y + 52);
}

//All in the name
void map::draw_a_vertical_block(screen* sc, unsigned int i, unsigned int j,std::vector<int> color)
{
	SDL_SetRenderDrawColor(sc->renderer, color[0],color[1],color[2],1);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y ,map_list_blocks[get(i,j)].x, map_list_blocks[get(i,j)].y + 52);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x+32,map_list_blocks[get(i,j)].y ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y + 52);
}

//All in the name
void map::draw_a_up_angle_block(screen* sc, object* obj, unsigned int i, unsigned int j,std::vector<int> color)
{
	SDL_SetRenderDrawColor(sc->renderer, color[0],color[1],color[2],1);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y + 52 ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y + 52);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x+32,map_list_blocks[get(i,j)].y + 52 ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y);
	if (!map_init)
	{
		Vector2 vector;
		vector.x = map_list_blocks[get(i,j)].x + 16;
		vector.y = map_list_blocks[get(i,j)].y + 26;
		obj->road.push_back(vector);
		obj->road_string.push_back('U');
	}
	
}

//All in the name
void map::draw_a_left_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color)
{
	SDL_SetRenderDrawColor(sc->renderer, color[0],color[1],color[2],1);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x + 32,map_list_blocks[get(i,j)].y + 52 ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x + 32,map_list_blocks[get(i,j)].y,map_list_blocks[get(i,j)].x, map_list_blocks[get(i,j)].y);
	if (!map_init)
	{
		Vector2 vector;
		vector.x = map_list_blocks[get(i,j)].x + 16;
		vector.y = map_list_blocks[get(i,j)].y + 26;
		obj->road.push_back(vector);
		obj->road_string.push_back('L');
	}
}

//All in the name
void map::draw_a_down_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color)
{
	SDL_SetRenderDrawColor(sc->renderer, color[0],color[1],color[2],1);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x + 32,map_list_blocks[get(i,j)].y,map_list_blocks[get(i,j)].x, map_list_blocks[get(i,j)].y);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y ,map_list_blocks[get(i,j)].x, map_list_blocks[get(i,j)].y + 52);
	if (!map_init)
	{
		Vector2 vector;
		vector.x = map_list_blocks[get(i,j)].x + 16;
		vector.y = map_list_blocks[get(i,j)].y + 26;
		obj->road.push_back(vector);
		obj->road_string.push_back('D');
	}
}

//All in the name
void map::draw_a_right_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color)
{
	SDL_SetRenderDrawColor(sc->renderer, color[0],color[1],color[2],1);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y ,map_list_blocks[get(i,j)].x, map_list_blocks[get(i,j)].y + 52);
	SDL_RenderDrawLine(sc->renderer, map_list_blocks[get(i,j)].x,map_list_blocks[get(i,j)].y + 52 ,map_list_blocks[get(i,j)].x + 32, map_list_blocks[get(i,j)].y + 52);
	if (!map_init)
	{
		Vector2 vector;
		vector.x = map_list_blocks[get(i,j)].x + 16;
		vector.y = map_list_blocks[get(i,j)].y + 26;
		obj->road.push_back(vector);
		obj->road_string.push_back('R');
	}
}

//All in the name
void map::draw_the_end_block(screen* sc,object* obj, unsigned int i,unsigned int j,std::vector<int> color)
{
	draw_a_horizontal_block(sc,i,j,color);
	if (!map_init)
	{
		Vector2 vector;
		vector.x = map_list_blocks[get(i,j)].x + 16;
		vector.y = map_list_blocks[get(i,j)].y + 26;
		obj->road.push_back(vector);
		obj->road_string.push_back('E');
	}
}

//All in the name
void map::draw_tower_pick_box(screen* sc,unsigned int i,unsigned int j)
{
	SDL_RenderFillRect(sc->renderer,&(map_list_blocks[get(i,j)]));
}

//All in the name
void map::init_block_list()
{
	int x 				= 0;
	int y 				= 0;
	int w 				= 32;
	int h 				= 52;

	map_list_blocks.resize(nb_of_blocks);

	for (unsigned int j = 0; j < nb_of_collumns-1; j++)
	{
		for (unsigned int i = 0; i < nb_of_lines-1; i++)
		{
			int id 					= get(i,j);
			map_list_blocks[id].x 	= x;
			map_list_blocks[id].y 	= y;
			map_list_blocks[id].w 	= w;
			map_list_blocks[id].h 	= h;
			x += 32; 
		}
		x = 0;
		y += 52;
	}
}

map::map()
{
	//initialize the map
	nb_of_lines 					= ( 1280 / 32) + 1;
	nb_of_collumns	 				= ( 832 / 52) + 1;
	nb_of_blocks 					= ( nb_of_collumns - 1 ) * ( nb_of_lines - 1 );
	map_list 						= shape::get_map_list_blueprint();
	characters_sheet 				= "S-|ULDREABC";
	map_init 						= false;
	map_color 						= {255,1,1,255};
	show_map();
	init_block_list();
}

void map::print_map(screen* sc, object* obj)
{
	//draw the grid ...
	draw_the_grid(sc);

	//search in the list if you have a special block then draw it
	for (unsigned int j = 0; j < nb_of_collumns-1; j++)
	{
		for (unsigned int i = 0; i < nb_of_lines-1; i++)
		{
			if (map_list[get(i,j)] ==  characters_sheet[0])
			{
				draw_the_start_point(obj,sc,i,j,map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[1])
			{
				draw_a_horizontal_block(sc, i, j, map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[2])
			{
				draw_a_vertical_block(sc, i, j, map_color);	
			}

			else if (map_list[get(i,j)] == characters_sheet[3])
			{
				draw_a_up_angle_block(sc,obj,i,j, map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[4])
			{
				draw_a_left_angle_block(sc,obj,i,j, map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[5])
			{
				draw_a_down_angle_block(sc,obj,i,j, map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[6])
			{
				draw_a_right_angle_block(sc,obj,i,j, map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[7])
			{
				draw_the_end_block(sc,obj,i,j,map_color);
			}

			else if (map_list[get(i,j)] == characters_sheet[8])
			{
				SDL_SetRenderDrawColor(sc->renderer,255,255,255,255);
				draw_tower_pick_box(sc,i,j);
			}

			else if (map_list[get(i,j)] == characters_sheet[9])
			{
				SDL_SetRenderDrawColor(sc->renderer,1,1,255,255);
				draw_tower_pick_box(sc,i,j);
			}

			else if (map_list[get(i,j)] == characters_sheet[10])
			{
				SDL_SetRenderDrawColor(sc->renderer,255,1,1,255);
				draw_tower_pick_box(sc,i,j);
			}
		}
	}

	//My BEAUTIFULL color swap process 0<0
	int x = 2;
	if (map_color[0] == 255 && map_color[2] == 1 && map_color[1] < 255)
	{
		map_color[1] += x;
	}
	
	else if (map_color[1] >= 255 && map_color[0] > 1)
	{
		map_color[0] -= x;
	}

	else if (map_color[1] >= 255 && map_color[0] <= 1 && map_color[2] < 255)
	{
		map_color[2] += x;
	}

	else if (map_color[2] >= 255 && map_color[1] > 1)
	{
		map_color[1] -= x;
	}

	else if (map_color[2] >= 255 && map_color[1] <= 1 && map_color[0] < 255)
	{
		map_color[0] += x;
	}

	else if (map_color[0] >= 255)
	{
		map_color[2] -= x;
	}

	map_init = true;	
}

map::~map()
{
	map_color.erase(map_color.begin(),map_color.end()-1);
	map_list_blocks.erase(map_list_blocks.begin(),map_list_blocks.end()-1);
}

/******************MAP****************************************************************************************************************************************************************************************/

/******************enemy**********************************************************************************************************************************************************************/

enemy::enemy( unsigned int size, unsigned int type_m, object* obj)
{
	//initialize all the variables
    size = size ;
    type_m = type_m;
    origin.x = obj->enemy_spawnpoint.x;
    origin.y = obj->enemy_spawnpoint.y;
    is_alive = true;
	advancement = 0;
	is_a_healer = false;
	slow_lvl = 0;
	turn_cooldown = 0;

	//create differents types of ennemies
	if (type_m - 1 == 0)
	{
		money_value = 4;
		life_pts 	= 100;
		speed 		= 2;
		damage		= 2;
	}

	else if (type_m - 1 == 1)
	{
		money_value = 8;
		life_pts 	= 300;
		speed 		= 1;
		damage 		= 3;
	}

	else if (type_m - 1 == 2)
	{
		money_value = 12;
		life_pts 	= 100;
		speed 		= 1;
		heal_radius	= 30;
		is_a_healer = true;
		damage		= 1;
	}
	
	for (unsigned int i = 0; i < 4; i++)
	{
		if (size - 1 > i)
		{
	    	money_value	*= 10;
			life_pts  	*= size;
			damage += damage;
		}
	}

	base_life_pts = life_pts;

    //Give them a collector skin made with love by ME ! (in blueprints.cpp)
    std::vector<float> sword = shape::get_sword_blueprint();	

    std::vector<float> shield = shape::get_shield_blueprint();

    std::vector<float> cross = shape::get_cross_blueprint();

    std::vector<float> coords_by_type[] = {sword,
                                shield,
                                cross,
    };

    int coords_size_by_type[3] = {35,34,27};

    std::vector<float> coords = coords_by_type[type_m-1];

    int coords_size = coords_size_by_type[type_m-1];
	unsigned int k = 0;
	poly.size = coords_size; 
	poly.l_list.resize(poly.size);
	poly.w_list.resize(poly.size);

    for ( int j = 0 ; j < poly.size/2 ; j++)
	{
			poly.l_list[j].x = (coords[k]) * (0.25 * (size + 1)) ;
			poly.l_list[j].y = (coords[k + 1]) * (0.25 * (size + 1)) ;
			k += 2; //two by two.
	}
    I.x     = 1;
	I.y 	= 0;
	U.x 	= I.x;
	U.y 	= I.y;
	J = normal_rotate_90_vector(I);
	road_size = obj->road.size();
	coords.erase(coords.begin(), coords.end() - 1);
	coords_by_type->erase(coords_by_type->begin(), coords_by_type->end() - 1);
	sword.erase(sword.begin(), sword.end() - 1);
	shield.erase(shield.begin(), shield.end() - 1);
	cross.erase(cross.begin(), cross.end() - 1);
}

enemy::enemy()
{}

void enemy::movement(object* obj, Time* t)
{
	//check if an enemy reach an angle then change his direction and obviusly make him walk
	if (t->t - turn_cooldown >= 100)
	{
		for (unsigned int i = 0; i < obj->road.size() ; i++)
		{
			Vector2 diff;
			diff.x = obj->road[i].x - origin.x;
			diff.y = obj->road[i].y - origin.y;
			float len = vector_length(diff);

			if (len <= 1.9)
			{
				turn_cooldown = SDL_GetTicks();
				if ( obj->road_string[i] == 'U' )
				{
					if (U.x == 1 && U.y == 0)
					{
						U.x = 0;
						U.y = -1;
					}

					else if (U.x == 0 && U.y == 1)
					{
						U.x = -1;
						U.y = 0;
					}
				}

				else if (obj->road_string[i] == 'L')
				{
					if (U.x == 0 && U.y == -1)
					{
						U.x = -1;
						U.y = 0;
					}

					else if ( U.x == -1 && U.y == 0 )
					{
						U.x = 0;
						U.y = 1;
					}
				}

				else if (obj->road_string[i] == 'D')
				{
					if (U.x == -1 && U.y == 0)
					{
						U.x = 0;
						U.y = 1;
					}

					else if ( U.x == 0 && U.y == -1)
					{
						U.x = 1;
						U.y = 0;
					}
				}

				else if (obj->road_string[i] == 'R')
				{
					if (U.x == 0 && U.y == 1)
					{
						U.x = 1;
						U.y = 0;
					}

					else if ( U.x == -1 && U.y == 0)
					{
						U.x = 0;
						U.y = -1;
					}
				}

				else if (obj->road_string[i] == 'E')
				{
					obj->castle_life -= damage;
					is_alive = false;
					U.x = 0;
					U.y = 0;
				}
			}
		}
	}
	origin.x += (speed * pow(0.50,slow_lvl)) * U.x;
	origin.y += (speed * pow(0.50,slow_lvl)) * U.y;
	advancement += speed;
}

void enemy::healing(object* obj)
{
	//check if other ennemies are in the range of the healer if yes then heal them
	for (enemy* enemy : obj->enemy_list)
	{
		Vector2 diff;
		diff.x =  	origin.x - enemy->origin.x;
		diff.y = 	origin.y - enemy->origin.y;
		float len = vector_length(diff);
		if (len <= heal_radius)
		{
			float heal = 0.50 * (size+1);
			if (enemy->life_pts + heal <= enemy->base_life_pts )
			{
				enemy->life_pts += heal;
			}

			if (life_pts + heal <= base_life_pts )
			{
				life_pts += heal;
			}
		}
	}
}

void enemy::is_dead(object* obj, screen* sc)
{
	//check if 1 or more ennemies are dead and if they are erase them
	auto it = obj->enemy_list.begin();
	while (it != obj->enemy_list.end())
	{
		enemy& enemy = *(*it);
		if(!enemy.is_alive)
		{
			it = obj->enemy_list.erase(it);
			sc->nb_enemy--;
		}

		else
		{
			it++;
		}
	}
}

enemy::~enemy()
{}

/******************enemy**********************************************************************************************************************************************************************/
object::object()
{
	money = 200;
	castle_life = 100;
}

object::~object()
{}

/**********************************************************************************************************************************************************************************************************/
/******************OBJECTS***********************************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************************************************/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////GAME INIT//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_screen(screen* sc)
{
   	//clear the screen
	SDL_SetRenderDrawColor(sc->renderer, 0, 0, 0, 255);
	SDL_RenderClear(sc->renderer);
	//Then SURPRISE.... re-initialize the screen...
	SDL_SetWindowTitle(sc->sc, "((+_+))((+_+))TowerDefense((+_+))((+_+))");
	if(TTF_Init() == -1)
	{
		my_put_string("Erreur d'initialisation de TTF_Init : %s\n");
		my_put_string(TTF_GetError());
	}
	sc->font  = TTF_OpenFont("./media/Fonts/font.ttf", 300); 
	if( Mix_PlayingMusic() == 1 )
    {
		Mix_HaltMusic();
    }
	sc->music = Mix_LoadMUS( "./media/Sound/Music/Pentakill - Mortal Reminder.mp3" );
	Mix_PlayMusic( sc->music, -1 );
	Mix_VolumeMusic(20);
}

void generate_wave(screen* screen, object* object)
{
	//Create and initiate the control list to 0
	std::vector<int> Control_list(9) ;
	object->cp->Control_list = Control_list;
	for (unsigned int i = 1; i < 9; i++)
	{
		object->cp->Control_list[i] = 0;
	}
	
	//spawn min 1 monster
	object->cp->Control_list[0] =  1; 
	int temp = screen->wave_m_lvl;
	int temp_x;
	int x[] = {4800,1600,800,240,80,40,12,4,1};
	unsigned int i = 0;

	//algorithm to proceduraly create ennemies
	while ( temp != 0 )
	{
		if ( temp - x[i] >= 0 )
		{
			temp_x = object->cp->Control_list[8-i];
			if((8-i)%3 == 0)
			{
				object->cp->Control_list[8-i] = temp_x + 5 ;
			}
			else
			{
				object->cp->Control_list[8-i] = temp_x + 2 ;
			}
			temp -= x[i];
		}

		else
		{
			i++;
		} 
	}

	//keep the total number of ennemies
	for (unsigned int i = 0; i < 9; i++)
	{
		screen->nb_enemy += object->cp->Control_list[i];
	}
}

void control_panel(Control_Panel* panel)
{
	//just a control panel and its his initialization.
    panel->number_of_S3_T1 = panel->Control_list[6];
    panel->number_of_S2_T1 = panel->Control_list[3];
    panel->number_of_S1_T1 = panel->Control_list[0];
    panel->number_of_S3_T2 = panel->Control_list[7];
    panel->number_of_S2_T2 = panel->Control_list[4];
    panel->number_of_S1_T2 = panel->Control_list[1];
    panel->number_of_S3_T3 = panel->Control_list[8];
    panel->number_of_S2_T3 = panel->Control_list[5];
    panel->number_of_S1_T3 = panel->Control_list[2];
	panel->Control_list.erase(panel->Control_list.begin(),panel->Control_list.end() - 1);
}

void create_enemy(object* obj, Time* t)
{
	//Check all the control panel then create an enemy if control panel tell him
	t->spawn_time = SDL_GetTicks();
	if (obj->cp->number_of_S1_T1 >= 1)
	{
		obj->enemy_list.push_back(new enemy(1, 1, obj));
		obj->cp->number_of_S1_T1--;
	}

	else if(obj->cp->number_of_S1_T2 >= 1)
	{
		obj->enemy_list.push_back(new enemy(1, 2, obj));
		obj->cp->number_of_S1_T2--;
	}

	else if(obj->cp->number_of_S1_T3 >= 1)
	{
		obj->enemy_list.push_back(new enemy(1, 3, obj));
		obj->cp->number_of_S1_T3--;
	}

	else if(obj->cp->number_of_S2_T1 >= 1)
	{
		obj->enemy_list.push_back(new enemy(2, 1, obj));
		obj->cp->number_of_S2_T1--;
	}

	else if(obj->cp->number_of_S2_T2 >= 1)
	{
		obj->enemy_list.push_back(new enemy(2, 2, obj));
		obj->cp->number_of_S2_T2--;
	}

	else if(obj->cp->number_of_S2_T3 >= 1)
	{
		obj->enemy_list.push_back(new enemy(2, 3, obj));
		obj->cp->number_of_S2_T3--;
	}

	else if(obj->cp->number_of_S3_T1 >= 1)
	{
		obj->enemy_list.push_back(new enemy(3, 1, obj));
		obj->cp->number_of_S3_T1--;
	}

	else if(obj->cp->number_of_S3_T2 >= 1)
	{
		obj->enemy_list.push_back(new enemy(3, 2, obj));
		obj->cp->number_of_S3_T2--;
	}

	else if(obj->cp->number_of_S3_T3 >= 1)
	{
		obj->enemy_list.push_back(new enemy(3, 3, obj));
		obj->cp->number_of_S3_T3--;
	}
}

// What can do this function OH i know inititialize the game ...
object* game_init(screen* sc)
{
    init_screen(sc);
    object* obj;
    obj = new object;
    obj->cp = new Control_Panel;
	obj->pic.print_map(sc,obj);
    generate_wave(sc,obj);
    control_panel(obj->cp);
    return obj;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////GAME_INIT//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////MAIN_LOOP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	~~~~~~~~~~~~~~~PAUSE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//print the word pause on the screen ... obviously
	void text_pause(screen* screen)
	{
		SDL_Color color = { 255, 255, 255, 255 };
		char* word = (char*) "Pause";
		SDL_Surface* surface = TTF_RenderText_Blended(screen->font, word, color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
		SDL_FreeSurface(surface);
		SDL_Rect rect = { 334, 250, 356, 100};

		SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, 255);
		SDL_RenderCopy(screen->renderer, texture, NULL, &(rect));
		SDL_RenderPresent(screen->renderer);
	}

	//i really need to explain it ??? .... check if the game is paused or not
	bool is_game_paused(bool is_game_paused, screen* screen, Time* t)
	{
		t->time_paused = SDL_GetTicks();

		if (is_game_paused == true)
		{
			return false;
		}

		else if (is_game_paused == false)
		{
			text_pause(screen);
			return true;
		}

		return is_game_paused;
	}

//	~~~~~~~~~~~~~~~PAUSE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//  ~~~~~~~~~~~~~~~MOUSE_PROCESS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	void remove_tower(object* obj)
	{
		//travel the list then check if one or more turret is destroy then erase it
		auto it = obj->turret_list.begin();
		while (it != obj->turret_list.end())
		{
			tower& tower = *(*it);
			if(!tower.is_alive)
			{
				it = obj->turret_list.erase(it);
			}

			else
			{
				it++;
			}
		}
	}


	void mouse_process(object* obj, Time* t)
	{
		//record states of mouse
		Uint32 mouseState = SDL_GetMouseState(&obj->mouse.x, &obj->mouse.y);
		bool leftClicked = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
		bool rightClicked = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);	
		
		//check if the mouse is in one of the three turret spawner then drag a tower
		if (obj->mouse.y>= 780 && obj->mouse.y <= 832 && obj->mouse.x >= 608 && obj->mouse.x <= 640)
			obj->mouse.in_rect1 = true;
		else
			obj->mouse.in_rect1 = false;

		if (obj->mouse.in_rect1 && leftClicked && !obj->mouse.draging_a_turret)
		{
			obj->mouse.draging_a_turret = true;
			obj->mouse.drag_a_turret(obj,2);
		}

		if (obj->mouse.y>= 780 && obj->mouse.y <= 832 && obj->mouse.x >= 640 && obj->mouse.x <= 672)
			obj->mouse.in_rect2 = true;
		else
			obj->mouse.in_rect2 = false;

		if (obj->mouse.in_rect2 && leftClicked && !obj->mouse.draging_a_turret)
		{
			obj->mouse.draging_a_turret = true;
			obj->mouse.drag_a_turret(obj,1);
		}

		if (obj->mouse.y>= 780 && obj->mouse.y <= 832 && obj->mouse.x >= 672 && obj->mouse.x <= 704)
			obj->mouse.in_rect3 = true;
		else
			obj->mouse.in_rect3 = false;

		if (obj->mouse.in_rect3 && leftClicked && !obj->mouse.draging_a_turret)
		{
			obj->mouse.draging_a_turret = true;
			obj->mouse.drag_a_turret(obj,3);
		}
		
		//get the block under the current cursor position
		int index = obj->pic.get(obj->mouse.x/32,obj->mouse.y/52);
		char tile = obj->pic.map_list[index];

		for (tower* tower : obj->turret_list)
		{
			//detect if the cursor is on a tower or not
			if (obj->mouse.x >= tower->origin.x - 16 && obj->mouse.x <= tower->origin.x + 16 && obj->mouse.y >= tower->origin.y - 26 && obj->mouse.y <= tower->origin.y + 26 && !obj->mouse.draging_a_turret)
			{

				//Upgrade of tower
				if (leftClicked && obj->money >= tower->cost * 3 && tower->lvl && t->t - tower->upgrade_turret_cooldown >= 200)
				{
					tower->lvl++;
					tower->cost *= 3;
					tower->damage *= 2;
					obj->money -= tower->cost;
					tower->upgrade_turret_cooldown = SDL_GetTicks();
				}
				
				//salvage of tower
				else if (rightClicked)
				{
					tower->is_alive = false;
					obj->money += tower->cost/2;
					obj->pic.map_list[index] = '#';
					remove_tower(obj);
				}
				
			}
		}

		//actualization of the draged tower rectangle
		if (obj->mouse.draging_a_turret)
		{
			obj->turret_list[obj->turret_list.size()-1]->rect = {obj->mouse.x - 10, obj->mouse.y - 10 , 20, 20};
		}

		//check if you have at least one tower
		if (obj->turret_list.size() != 0)
		{
			tower* last_turret = obj->turret_list[obj->turret_list.size()-1];
			//actualization of the draged tower position
			if (last_turret->draged == true )
			{
				last_turret->origin.x = obj->mouse.x;
				last_turret->origin.y = obj->mouse.y;
			}

			if (obj->mouse.draging_a_turret && !leftClicked)
			{
				//if you stop clicking left mouse button, you have enough money and its a free space then drop the tower to the location
				if (tile == '#' && obj->money >= last_turret->cost)
				{
					last_turret->rect = obj->pic.map_list_blocks[index];
					obj->mouse.draging_a_turret = false;
					obj->pic.map_list[index] = last_turret->case_char;
					last_turret->draged = false;
					obj->money -= last_turret->cost;
				}

				//if not then destroy the turret
				else
				{
					obj->mouse.draging_a_turret = false;
					obj->turret_list.pop_back();
				}
			}
		}
	}

//  ~~~~~~~~~~~~~~~MOUSE_PROCESS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//  ~~~~~~~~~~~~~~~GAME_UPDATE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	void tower_process(object* obj, Time* t,screen* sc)
	{
		for (tower* tower : obj->turret_list)
		{
			if (!tower->draged)
				tower->tower_fire(obj,t,sc);
		}
		
	}

	void enemy_process(object* object, screen* screen, Time* t)
	{
		for (enemy* enemy : object->enemy_list)
		{
			//Convert polygon from local to world position
			enemy->poly.w_list = localToWorld_list(enemy->poly.l_list,
								enemy->poly.w_list,
								enemy->origin, 
								enemy->I, 
								enemy->J, 
								enemy->poly.size);

			if (enemy->is_alive)
			{
				if (enemy->is_a_healer)
				{
					enemy->healing(object);
				}
				
				//create life bar and call the enemy movement
				int range = 50*(enemy->life_pts/enemy->base_life_pts);
				SDL_Rect rect = {static_cast <int>(enemy->origin.x)-25,static_cast <int>(enemy->origin.y)-20,50,5};
				SDL_Rect rect2 = {static_cast <int>(enemy->origin.x)-25,static_cast <int>(enemy->origin.y)-20,range,5};
				enemy->movement(object,t);
				SDL_SetRenderDrawColor(screen->renderer,255,255,255,1);
				SDL_RenderDrawLines(screen->renderer, &enemy->poly.w_list[0], (enemy->poly.size / 2));
				SDL_SetRenderDrawColor(screen->renderer,255,255,255,1);
				SDL_RenderDrawRect(screen->renderer,&rect);
				SDL_SetRenderDrawColor(screen->renderer,255,0,0,1);
				SDL_RenderFillRect(screen->renderer,&rect);
				SDL_SetRenderDrawColor(screen->renderer,255,255,255,1);
				SDL_RenderDrawRect(screen->renderer,&rect2);
				SDL_SetRenderDrawColor(screen->renderer,0,255,0,1);
				SDL_RenderFillRect(screen->renderer,&rect2);
			}

			else
			{
				//Erease the dead enemy
				enemy->is_dead(object,screen);
			}
		}
	}

	void gameUpdate(screen* sc , object* obj, Time* t)
	{
		tower_process(obj,t,sc);
		enemy_process(obj,sc,t);
	}

//  ~~~~~~~~~~~~~~~GAME_UPDATE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// 	~~~~~~~~~~~~~~~GAME_PRINT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	SDL_Point localToWorld_pts(SDL_Point pts, Point origin , Vector2 I, Vector2 J) 
	{
		return SDL_Point { (int)origin.x + ( ( pts.x * (int)I.x ) + ( pts.y * (int)J.x )),
					 		(int)origin.y + ( ( pts.x * (int)I.y ) + ( pts.y * (int)J.y ))};
	}

	std::vector<SDL_Point> localToWorld_list(std::vector<SDL_Point> list , std::vector<SDL_Point> w_list, Point origin, Vector2 I, Vector2 J, unsigned int size)
	{
		for ( unsigned int index = 0 ; index < size ; index++)
		{
			w_list[index] = localToWorld_pts(list[index], origin, I, J);
		}
		return w_list;
	}

	void print_towers(object* object, screen* sc)
	{
		int index = object->pic.get(object->mouse.x/32,object->mouse.y/52);
		char tile = object->pic.map_list[index];
			
		for (tower* tower : object->turret_list)
		{
			// the process to control the color of the draged tower to show to player if he can drop the turret here or not
			if (tower->draged)
			{
				if (tile == '#' && tower->cost <= object->money)
					SDL_SetRenderDrawColor(sc->renderer, 1, 255, 1, 255);
				else
					SDL_SetRenderDrawColor(sc->renderer, 255, 1, 1, 255);
			}

			//then draw the different color tower
			else if(tower->case_char == 'F')
			{
				if (tower->lvl == 3)
					SDL_SetRenderDrawColor(sc->renderer, 100, 100, 100, 255);

				else if (tower->lvl == 2)
					SDL_SetRenderDrawColor(sc->renderer, 200, 200, 200, 255);

				else
					SDL_SetRenderDrawColor(sc->renderer, 255, 255, 255, 255);
			}
			else if(tower->case_char == 'W')
			{
				if (tower->lvl == 3)
					SDL_SetRenderDrawColor(sc->renderer, 1, 255, 255, 255);

				else if (tower->lvl == 2)
					SDL_SetRenderDrawColor(sc->renderer, 1, 115, 255, 255);

				else
					SDL_SetRenderDrawColor(sc->renderer, 1, 1, 255, 255);
			}
			else if(tower->case_char == 'Z')
			{
				if (tower->lvl == 3)
					SDL_SetRenderDrawColor(sc->renderer, 110, 50, 1, 255);

				else if (tower->lvl == 2)
					SDL_SetRenderDrawColor(sc->renderer, 145, 65, 1, 255);

				else
					SDL_SetRenderDrawColor(sc->renderer, 190, 85, 1, 255);
			}
			SDL_RenderFillRect(sc->renderer, &(tower->rect));
			SDL_RenderDrawRect(sc->renderer, &(tower->rect));
		}
		
	}

	//read the name please
	void print_money(object* object, screen* screen)
	{
		const char* money_text = int_to_alpha(object->money);
		SDL_Color color1 = { 238, 155, 47, 255};
		SDL_Surface* surface = TTF_RenderText_Blended(screen->font, money_text, color1);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
		SDL_FreeSurface(surface);
		SDL_Rect rect_s = {130 , 0, 50 * static_cast<int>(string_length(money_text)), 40};
		const char* word = "MONEY :";
		SDL_Surface* text_surface = TTF_RenderText_Blended(screen->font, word, color1);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(screen->renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_Rect rect = { 2, 0, 130, 40 };
		SDL_RenderCopy(screen->renderer,text_texture,NULL,&rect);
		SDL_RenderCopy(screen->renderer,texture, NULL, &rect_s);
		SDL_DestroyTexture(text_texture);
		SDL_DestroyTexture(texture);
	}

	//same 
	void print_castle_life(object* object, screen* screen)
	{
		const char* castle_life_text = int_to_alpha(object->castle_life);
		SDL_Color color1 = { 255, 1, 1, 255};
		SDL_Surface* surface = TTF_RenderText_Blended(screen->font, castle_life_text, color1);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
		SDL_FreeSurface(surface);
		SDL_Rect rect_s = {1120 , 40, 50 * static_cast<int>(string_length(castle_life_text)), 40};
		const char* word = "LIFE :";
		SDL_Surface* text_surface = TTF_RenderText_Blended(screen->font, word, color1);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(screen->renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_Rect rect = { 980, 40, 130, 40 };
		SDL_RenderCopy(screen->renderer,text_texture,NULL,&rect);
		SDL_RenderCopy(screen->renderer,texture, NULL, &rect_s);
		SDL_DestroyTexture(text_texture);
		SDL_DestroyTexture(texture);
	}

	//same++
	void print_wawe_number(screen* screen)
	{
		const char* wave_text = int_to_alpha(screen->wave);
		SDL_Color color1 = { 255, 255, 255, 255};
		SDL_Surface* surface = TTF_RenderText_Blended(screen->font, wave_text, color1);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
		SDL_FreeSurface(surface);
		SDL_Rect rect_s = {1120 , 0, 50 * static_cast<int>(string_length(wave_text)), 40};
		const char* word = "WAVE :";
		SDL_Surface* text_surface = TTF_RenderText_Blended(screen->font, word, color1);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(screen->renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_Rect rect = { 980, 0, 130, 40 };
		SDL_RenderCopy(screen->renderer,text_texture,NULL,&rect);
		SDL_RenderCopy(screen->renderer,texture, NULL, &rect_s);
		SDL_DestroyTexture(text_texture);
		SDL_DestroyTexture(texture);
	}

	//(same++)++
	void game_Uptdate_and_Print(screen* sc, object* obj, Time* t)
	{
	    SDL_SetRenderDrawColor(sc->renderer, 0, 0, 0, 255);
		SDL_RenderClear(sc->renderer);
		SDL_SetRenderDrawColor(sc->renderer, 41, 39, 39, 255);
		obj->pic.print_map(sc,obj);
		gameUpdate(sc,obj,t);
		SDL_SetRenderDrawColor(sc->renderer, 255, 255, 255, 255);
		print_towers(obj,sc);
		print_money(obj,sc);
		print_castle_life(obj,sc);
		print_wawe_number(sc);
	    SDL_RenderPresent(sc->renderer);  
	}

// 	~~~~~~~~~~~~~~~GAME_PRINT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A function that destroy the game WHOUAAA AMAZING INCREADIBLE I'm sure you did not find it (I took a month to find what this function could serve)
void game_destroy(object* obj,screen* sc)
{

	for (enemy* enemy : obj->enemy_list)
	{
		enemy->is_alive = false;
	}
	
	if (obj->enemy_list.size() >= 1)
	{
		obj->enemy_list[0]->is_dead(obj,sc);
	}
	obj->turret_list.clear();

	delete obj->cp;
	delete obj;
	TTF_Quit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////MAIN_LOOP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////