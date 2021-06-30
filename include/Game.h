//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef GAME_H
#define GAME_H

// include all the needed librairies
#include <main_process.h>
#include <my_maths.h>
#include <Blueprints.h>

class polygon
{
public:
	int 		size;
	std::vector<SDL_Point> 	w_list;
	std::vector<SDL_Point> 	l_list;
};

class Control_Panel
{
public:
	std::vector<int> 			Control_list;
	unsigned int 				number_of_S3_T1;
	unsigned int 				number_of_S2_T1;
	unsigned int 				number_of_S1_T1;
	unsigned int 				number_of_S3_T2;
	unsigned int 				number_of_S2_T2;
	unsigned int 				number_of_S1_T2;
	unsigned int 				number_of_S3_T3;
	unsigned int 				number_of_S2_T3;
	unsigned int 				number_of_S1_T3;
};

class object;

class map
{
public:
	unsigned int 			nb_of_lines;
	unsigned int			nb_of_collumns;
	unsigned int 			nb_of_blocks;
	std::vector<SDL_Rect> 	map_list_blocks;
	std::string				map_list;
	std::string 			characters_sheet;
	bool					map_init;
	std::vector<int>		map_color;
	map						();
	~map					();
	int 					get(int i, int j);
	void 					set(int i, int j, int a);
	void 					draw_the_grid(screen* sc);
	void 					show_map();
	void 					init_block_list();
	void					print_map(screen* sc, object* obj);
	void 					draw_the_start_point(object* obj,screen* sc,unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_a_horizontal_block(screen* sc, unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_a_vertical_block(screen* sc, unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_a_up_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_a_left_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_a_down_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_a_right_angle_block(screen* sc,object* obj, unsigned int i, unsigned int j,std::vector<int> color);
	void 					draw_the_end_block(screen* sc,object* obj, unsigned int i,unsigned int j,std::vector<int> color);
	void 					draw_tower_pick_box(screen* sc,unsigned int i,unsigned int j);
};

class enemy;

class tower
{
public:
	int					damage;
	unsigned int 		type;
	unsigned int 		lvl;
	Point 				origin;
	Vector2 			I;
	Vector2 			J;
	SDL_Rect  			rect;
	enemy* 				target;
	int 				radius;
	float 				fire_time;
	bool 				draged;
	char 				case_char;
	unsigned int 		fire_rate;
	int 				cost;
	float				upgrade_turret_cooldown;
	bool 				is_alive;
	tower();
	tower(object*obj, unsigned int type);
	void tower_fire(object* obj, Time* t,screen* sc);
	~tower();
};

class enemy
{
private:
	float 				speed;
	Vector2				U;
	int 				road_size;
	float				heal_radius;
	unsigned int 		type_m;

public:
	unsigned int 		size;
	bool 				is_a_healer;
	Point 				origin;
	Vector2 			I;
	Vector2 			J;
	bool 				is_alive;
	polygon  			poly;
	float				life_pts;
	SDL_Rect    		life_bar;
	float 				base_life_pts;
	float				advancement;
	unsigned int 		slow_lvl;
	float				turn_cooldown;
	int 				money_value;
	int 				damage;

	enemy();
	enemy(unsigned int size, unsigned int type_m, object* obj);
	~enemy();
	void movement(object* obj, Time* t);
	void healing(object* obj);
	void is_dead(object* obj, screen* sc);
};

class Mouse
{
public:
	int x;
	int y;
	bool in_rect1;
	bool in_rect2;
	bool in_rect3;
	bool draging_a_turret;
	Mouse();
	void drag_a_turret(object* obj, int type);
	~Mouse();
};

class object
{
public:
	Point 					enemy_spawnpoint;
	Mouse 					mouse;
	Control_Panel* 			cp;
	map 					pic;
	std::vector<enemy*> 	enemy_list;
	std::vector<Vector2> 	road;
	std::string				road_string;
	std::vector<tower*>		turret_list;
	int						money;
	int 					castle_life;		
	object();
	~object();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////GAME INIT//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_screen								(screen* sc);

void generate_wave								(screen* screen, object* object);

void control_panel								(Control_Panel* panel);

void create_enemy								(object* obj, Time* t);

object* game_init								(screen* sc);

void 		game(screen* sc, Time* time);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////GAME_INIT//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////MAIN_LOOP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	~~~~~~~~~~~~~~~PAUSE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	void text_pause								(screen* screen);

	bool is_game_paused							(bool is_game_paused, screen* screen, Time* t);

//	~~~~~~~~~~~~~~~PAUSE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//  ~~~~~~~~~~~~~~~MOUSE_PROCESS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	void remove_tower							(object* obj);
							
	void mouse_process							(object* obj, Time* t);

//  ~~~~~~~~~~~~~~~MOUSE_PROCESS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void create_enemy								(object* obj, Time* t);

//  ~~~~~~~~~~~~~~~GAME_UPDATE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	void tower_process							(object* obj, Time* t, screen* sc);

	void enemy_process							(object* object, screen* screen, Time* t);

	void gameUpdate								(screen* sc , object* obj, Time* t);

//  ~~~~~~~~~~~~~~~GAME_UPDATE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// 	~~~~~~~~~~~~~~~GAME_PRINT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	SDL_Point localToWorld_pts					(SDL_Point pts, Point origin , Vector2 I, Vector2 J);

	std::vector<SDL_Point> localToWorld_list	(std::vector<SDL_Point> list, std::vector<SDL_Point> w_list, Point origin, Vector2 I, Vector2 J, unsigned int size);

	void print_towers							(object* object, screen* sc);

	void print_money							(object* object, screen* screen);

	void print_castle_life						(object* object, screen* screen);

	void print_wawe_number						(screen* screen);

	void game_Uptdate_and_Print					(screen* sc, object* obj, Time* t);

// 	~~~~~~~~~~~~~~~GAME_PRINT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void game_destroy(object* obj,screen* sc);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////MAIN_LOOP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~