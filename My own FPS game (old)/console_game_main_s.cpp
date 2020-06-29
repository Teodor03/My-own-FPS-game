#include<iostream>
#include<chrono>
#include<cmath>
#include<vector>
#include<list>
#include<thread>
#include<string>
#include<cstdio>
#include<utility>
#include<queue>
#include<algorithm>

using namespace std;

//#define _WIN32_WINNT 0x0601
#include<windows.h>
//typedef struct _CONSOLE_FONT_INFOEX
//{
//    ULONG cbSize;
//    DWORD nFont;
//    COORD dwFontSize;
//    UINT  FontFamily;
//    UINT  FontWeight;
//    WCHAR FaceName[LF_FACESIZE];
//}CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;
////the function declaration begins
//#ifdef __cplusplus
//extern "C" {
//#endif
//BOOL WINAPI SetCurrentConsoleFontEx(HANDLE hConsoleOutput, BOOL bMaximumWindow, PCONSOLE_FONT_INFOEX
//lpConsoleCurrentFontEx);
//#ifdef __cplusplus
//}
//#endif

const float PI=3.141592;


    //useful mathematical function
    float my_atan2(float x,float y){
    float r;
    if(x>0){
        r=atan(y/x);
    }
    if(y>=0 and x<0){
        r=PI+atan(y/x);
    }
    if(y<0 and x<0){
        r=-PI +atan(y/x);
    }
    if(y>0 and x==0){
        r=PI/2;
    }
    if(y<0 and x==0){
        r=-PI/2;
    }
    if(r<0){
        r=r+2*PI;
    }
    return r;
    }





enum colour{
FG_BLACK		= 0x0000,
FG_DARK_BLUE    = 0x0001,
FG_DARK_GREEN   = 0x0002,
FG_DARK_CYAN    = 0x0003,
FG_DARK_RED     = 0x0004,
FG_DARK_MAGENTA = 0x0005,
FG_DARK_YELLOW  = 0x0006,
FG_GREY			= 0x0007,
FG_DARK_GREY    = 0x0008,
FG_BLUE			= 0x0009,
FG_GREEN		= 0x000A,
FG_CYAN			= 0x000B,
FG_RED			= 0x000C,
FG_MAGENTA		= 0x000D,
FG_YELLOW		= 0x000E,
FG_WHITE		= 0x000F,
BG_BLACK		= 0x0000,
BG_DARK_BLUE	= 0x0010,
BG_DARK_GREEN	= 0x0020,
BG_DARK_CYAN	= 0x0030,
BG_DARK_RED		= 0x0040,
BG_DARK_MAGENTA = 0x0050,
BG_DARK_YELLOW	= 0x0060,
BG_GREY			= 0x0070,
BG_DARK_GREY	= 0x0080,
BG_BLUE			= 0x0090,
BG_GREEN		= 0x00A0,
BG_CYAN			= 0x00B0,
BG_RED			= 0x00C0,
BG_MAGENTA		= 0x00D0,
BG_YELLOW		= 0x00E0,
BG_WHITE		= 0x00F0,
};





class sprite{
    public:

    int x_length;
    int y_length;

    short * sprite_mem;

    bool * ignore_table;

    FILE * sprite_load;

    public:

    void Draw(int x,int y,short co = 0x00FF){
        if(-1<x and x<x_length and -1<y and y<y_length){
            sprite_mem[y*x_length+x]=co;
        }
    return;
    }

    void load_sprite_from_file(const char * file_name){
        sprite_load = fopen(file_name,"rt");
        fscanf(sprite_load,"%d %d\n",&x_length,&y_length);
        sprite_mem = new short[x_length*y_length];
        ignore_table = new bool[x_length*y_length];
        short col;
        for(int y=0;y<y_length;y++){
            for(int x=0;x<x_length;x++){
                fscanf(sprite_load,"%x\n",&col);
                Draw(x,y,col);
            }
        }
        int curr_b;
        for(int y=0;y<y_length;y++){
            for(int x=0;x<x_length;x++){
                fscanf(sprite_load,"%d\n",&curr_b);
                if(curr_b==0){
                    ignore_table[y*x_length+x]=false;
                }else{
                    ignore_table[y*x_length+x]=true;
                }
            }
        }
        return;
    }

    short take_sample(float s_x,float s_y){
        if(0<=s_x and s_x<1 and 0<=s_y and s_y<1){
            return sprite_mem[((int)(s_y*((float)y_length)))*x_length +  ((int)(s_x*((float)x_length)))];
        }else{
            return 0x0000;
        }
    }

    bool take_sample_check_ignore(float s_x,float s_y){
        if(0<=s_x and s_x<1 and 0<=s_y and s_y<1){
            return ignore_table[((int)(s_y*((float)y_length)))*x_length + ((int)(s_x*((float)x_length)))];
        }else{
            return false;
        }
    }
};





class enemy;
class enemy_projectile;





class game_map{

    public:

    int map_x_size;
    int map_y_size;

    char * map_sym_mem;

    int * map_num_mem;

    bool * p_see;

//    bool * loaded_obj_from_map;

    FILE * load_file;

    void load_from_file(const char * file_name){
        load_file=fopen(file_name,"rt");
        fscanf(load_file,"%d %d\n",&map_x_size,&map_y_size);
        map_sym_mem = new char [map_x_size*map_y_size];
        map_num_mem = new int [map_x_size*map_y_size];
        p_see = new bool [map_x_size*map_y_size];
        //loaded_obj_from_map = new bool [map_x_size*map_y_size];
        for(int i=0;i<map_y_size;i++){
            for(int j=0;j<map_x_size;j++){
                fscanf(load_file,"%c",&map_sym_mem[i*map_x_size+j]);
            }
            fscanf(load_file,"\n");
        }
        for(int i=0;i<map_y_size;i++){
            for(int j=0;j<map_x_size;j++){
                fscanf(load_file,"%d ",&map_num_mem[i*map_x_size+j]);
            }
            fscanf(load_file,"\n");
        }
        return;
    }

    char get_symbol(int x,int y){
        if(-1<x and x<map_x_size and -1<y and y<map_y_size){
            return map_sym_mem[y*map_x_size+x];
        }
        return ' ';
    }

    int get_num(int x,int y){
        if(-1<x and x<map_x_size and -1<y and y<map_y_size){
            return map_num_mem[y*map_x_size+x];
        }
        return 0;
    }

    bool * get_p_see(int x,int y){
        if(-1<x and x<map_x_size and -1<y and y<map_y_size){
            return &p_see[y*map_x_size+x];
        }
        return 0;
    }
    //! MAY CAUSE MEMORY ISSUES
    void clear_p_see(){
        fill(p_see,p_see+map_x_size*map_y_size,false);
    }

    queue<enemy_projectile> enemy_projectiles;



    queue<enemy> enemies;

//    void update_enemies(float p_x,float p_y,float passed_time){
//        enemy curr_enemy;
//        for(int i=0;i<enemies.size();i++){
//            curr_enemy=enemies.front();
//            enemies.pop();
//            if(!curr_enemy.is_dead){
//                curr_enemy.update(p_x,p_y,this,passed_time);
//                enemies.push(curr_enemy);
//            }
//        }
//    }

};





class object{
public:

float x_loc;
float y_loc;

sprite * spr;

object(float x,float y,sprite * s): x_loc{x},y_loc{y},spr{s}{
}

//may add some sort of animations in the future

};

//enum pos_ef{
//
//health=0;
//xp=1;
//
//
//
//};

//class pickup: public object {
//};

class enemy: public object{
public:

float health;
bool is_dead=false;

void do_damage(float damage){
health-=damage;
if(health<=0){
    health=0;
    is_dead=true;
}
return;
}

float mov_speed;

bool have_detected_player=false;
float detect_range;

bool check_if_in_detect_range(float p_x,float p_y){
    if(sqrt((x_loc-p_x)*(x_loc-p_x) + (y_loc-p_y)*(y_loc-p_y))<detect_range){
        return true;
    }
    return false;
}



void goto_pos(float x_target,float y_target,float pass_time_in_sec, game_map * curr_map){
if( x_target<0.0 or x_target>((float)curr_map->map_x_size) or y_target<0.0 or y_target>((float)curr_map->map_y_size) or curr_map->get_symbol((int)x_target,(int)y_target)=='#'){
    return;
}
float new_x,new_y,x_r,y_r,dist_to_new,actual_dist;

x_r=x_target-x_loc;
y_r=y_target-y_loc;

dist_to_new=sqrt(x_r*x_r+y_r*y_r);
actual_dist=mov_speed*pass_time_in_sec;
if(actual_dist>=dist_to_new){
    x_loc=x_target;
    y_loc=y_target;
    return;
}
new_x=x_loc + x_r*actual_dist/dist_to_new;
new_y=y_loc + y_r*actual_dist/dist_to_new;

//may cause enemies to got through walls if speed or time between frames is too hight but not CPU intensive
if( curr_map->get_symbol(((int)new_x),((int)new_y))!='#' ){
    x_loc=new_x;
    y_loc=new_y;
}
return;
}



void update_undetected(){
//for now do nothing
return;
}

float attack_range;

float reload_time;
float time_to_reload=0;

virtual void attack(float p_x,float p_y,game_map * curr_map)=0;

//call this for update of enemy actions: main enemy AI
void update(float p_x,float p_y,game_map * curr_map,float pass_time){
//check if player is already been detected
if(have_detected_player){
    float dist_to_player=sqrt( (x_loc-p_x)*(x_loc-p_x)+(y_loc-p_y)*(y_loc-p_y) );

    if(time_to_reload>0){
        time_to_reload-=pass_time;
        if(time_to_reload<0){
            time_to_reload=0;
        }
    }

    if(dist_to_player<attack_range){
        if(time_to_reload==0){
            attack(p_x,p_y,curr_map);
            time_to_reload=reload_time;
        }
    }else{
        goto_pos(p_x,p_y,pass_time,curr_map);
    }

}else{
    if( (* curr_map->get_p_see((int)x_loc,(int)y_loc)) and check_if_in_detect_range(p_x,p_y) ){
        have_detected_player=true;
    }else{
        update_undetected();
    }
}
return;
}

enemy(float x,float y,sprite * s,float hea,float mov_sp,float det_range,float att_range,float rel_time):object{x,y,s},health{hea},mov_speed{mov_sp},detect_range{det_range},attack_range{att_range},reload_time{rel_time}{
}

};

//class enemy_not_fir_proj: public enemy {
//
//    void attack(p_x,p_y){
//
//
//    }
//
//};





class enemy_projectile: public object{

private:

float proj_a;
float sin_a;
float cos_a;

float velocity;

float blast_radius;

float damage;

public:

bool is_destroyed=false;

void update(float p_x,float p_y,game_map * curr_map,float pass_time,float * p_health){
    //quite similar whit goto_pos
    if(!is_destroyed){
    x_loc+=cos_a*velocity*pass_time;
    y_loc+=sin_a*velocity*pass_time;
    if((curr_map->get_symbol((int)x_loc,(int)y_loc)=='#') or x_loc<0 or y_loc<0 or x_loc>((float)curr_map->map_x_size) or y_loc>((float)curr_map->map_y_size)){
        is_destroyed=true;
    }else{
        if(sqrt((p_x-x_loc)*(p_x-x_loc)+(p_y-y_loc)*(p_y-y_loc))<blast_radius){
            (*p_health)-=damage;
            is_destroyed=true;
        }
    }
    }
    return;
}

enemy_projectile(float x_start,float y_start,float angle,sprite * proj_sprite,float vel,float bl_rad,float dam): object{x_start,y_start,proj_sprite},velocity{vel},blast_radius{bl_rad},damage{dam},proj_a{angle}{
    sin_a=sin(proj_a);
    cos_a=cos(proj_a);
}

};





class enemy_fir_proj: public enemy {

public:

float vel,dam,bl_rad;

sprite * proj_sprite;

void attack(float p_x,float p_y,game_map * curr_map){

float angle=my_atan2(p_x-x_loc,p_y-y_loc);
enemy_projectile new_pr(x_loc,y_loc,angle,proj_sprite,vel,bl_rad,dam);
curr_map->enemy_projectiles.push(new_pr);

}


enemy_fir_proj(float x,float y,sprite * s,float hea,float mov_sp,float det_range,float att_range,float rel_time,float vel,float dam,float bl_rad,sprite * proj_spr): enemy{x,y,s,hea,mov_sp,det_range,att_range,rel_time},vel{vel},dam{dam},bl_rad{bl_rad},proj_sprite{proj_spr}{
}

};





struct obj_to_draw{
    float dist_from_player;
    float loc_x;
    float loc_y;

    sprite * spr;

    bool operator <(obj_to_draw other)const{
        return dist_from_player<other.dist_from_player;
    }
};





class console_game_mainframe{

    //all console related stuff

    private:

    int screen_x;
    int screen_y;

    HANDLE h_console_in;
    HANDLE h_console;

    SMALL_RECT s_rect_window;

    CHAR_INFO * screen_buffer;

    colour fg_col;
    colour bg_col;

    public:

    void construct_cons(int width,int height,int char_x,int char_y){

        //setting screen sizes - Only location where you can change them !
        screen_x=width;
        screen_y=height;

        //setting handlers
        h_console_in=GetStdHandle(STD_INPUT_HANDLE);
        h_console=GetStdHandle(STD_OUTPUT_HANDLE);

        //some magic to make resizing work properly
        s_rect_window = {0,0,1,1};
        SetConsoleWindowInfo(h_console, TRUE, &s_rect_window);

        //setting screen_buffer_size
        COORD coord ={(short) screen_x,(short) screen_y};
        SetConsoleScreenBufferSize(h_console,coord);
        SetConsoleActiveScreenBuffer(h_console);

        //setting font sizes
        CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = char_x;
		cfi.dwFontSize.Y = char_y;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		//wcscpy_c(cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(h_console,false, &cfi);

		//completing the resize
		s_rect_window={0,0,(short)screen_x - 1, (short)screen_y - 1};
		SetConsoleWindowInfo(h_console, TRUE, &s_rect_window);

		//things for mouse which I still not need
		/*

		SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		*/

		//allocating memory for screen buffer
		screen_buffer = new CHAR_INFO[screen_x*screen_y];
		memset(screen_buffer, 0,sizeof(CHAR_INFO)*screen_x*screen_y);

    }

    void refresh_screen(){
        WriteConsoleOutput(h_console, screen_buffer, { (short)screen_x, (short)screen_y }, { 0,0 }, &s_rect_window);
    }

    void Draw(int x,int y,short ch = 0x2588,short co = 0x00F0){
        if(-1<x and x<screen_x and -1<y and y<screen_y){
            screen_buffer[y*screen_x+x].Char.UnicodeChar = ch;
            screen_buffer[y*screen_x+x].Attributes = co;
        }
    }

    //all sprite stuff;

    private:

    int num_sprites;
    sprite * curr_sprite;

    FILE * list_of_sprites_file;
    vector<sprite> sprite_mem;

    void load_sprites(){
//        list_of_sprites_file=fopen("sprites_main_list.txt","rt");
//        char curr_name_of_file [50];
//        fscanf(list_of_sprites_file,"%d\n",&num_sprites);
//        sprite th;
//        string s;
//        for(int i=0;i<num_sprites;i++){
//            fscanf(list_of_sprites_file,"%[^\n]s\n",curr_name_of_file);
//            s+=curr_name_of_file;
//            th.load_sprite_from_file(curr_name_of_file);
//            sprite_mem.push_back(th);
//        }
//        char const *p_n_l = s.c_str();
//        SetConsoleTitle(p_n_l);
        sprite th;
        num_sprites=6;
        th.load_sprite_from_file("wall_mossy_stone_brick.sprite");
        sprite_mem.push_back(th);
        th.load_sprite_from_file("wall_brick_classic.sprite");
        sprite_mem.push_back(th);
        th.load_sprite_from_file("flower_butterfly.sprite");
        sprite_mem.push_back(th);
        th.load_sprite_from_file("obelisk_mossy.sprite");
        sprite_mem.push_back(th);
        th.load_sprite_from_file("robot_ugly.sprite");
        sprite_mem.push_back(th);
        th.load_sprite_from_file("fireball.sprite");
        sprite_mem.push_back(th);
        return;
    }

    //map related

    private:

    game_map curr_map;

    void load_map(){
        char m_x_file_name [100]="first_map.game_map";
        curr_map.load_from_file(m_x_file_name);
    }

    //main game needed for objects

    float p_x=10.0f;
    float p_y=10.0f;
    float p_angle=0.0f;

    //player stats

    float health;
    float max_health=100.0;
    int health_bar_resolution=100;

    float xp;

    //object related

    float min_dist_to_draw_obj=1.0;
    obj_to_draw new_obj_d;
    priority_queue <obj_to_draw> list_obj_to_draw;


    //enemies dynamic objects

    void update_enemy_projectiles(float player_x,float player_y,float passed_time,float * p_h){
        enemy_projectile curr_enemy_proj(0,0,0,0,0,0,0);

        for(int i=0;i<curr_map.enemy_projectiles.size();i++){
            curr_enemy_proj=curr_map.enemy_projectiles.front();
            curr_map.enemy_projectiles.pop();
            if(!(curr_enemy_proj.is_destroyed)){
                curr_enemy_proj.update(player_x,player_y,&curr_map,passed_time,p_h);

                new_obj_d.loc_x=curr_enemy_proj.x_loc;
                new_obj_d.loc_y=curr_enemy_proj.y_loc;
                new_obj_d.dist_from_player=sqrt( (new_obj_d.loc_x-player_x)*(new_obj_d.loc_x-player_x) + (new_obj_d.loc_y-player_y)*(new_obj_d.loc_y-player_y) );
                new_obj_d.spr=curr_enemy_proj.spr;
                list_obj_to_draw.push(new_obj_d);

                curr_map.enemy_projectiles.push(curr_enemy_proj);
            }
        }
    }

    //all main game related stuff

    float FOV=PI / 4.0;
    float DOF=20.0;
//    float ray_j=0.02f;

    float time_game_loop;
    float FPS_limit=30.0;
    bool is_fps_limit=true; //carefully here! Switching off may cause CPU to overload

    float rot_sensitive=1.5f; //angle per second
    float p_speed=3.0f;       //distance per second

    float * depth_array;

    public:

    void launch_game(){

        load_sprites();
        load_map();

        depth_array = new float [screen_x];

        //starting player stats
        health=max_health;

        enemy_fir_proj test_enemy(35.0,10.0,&sprite_mem.at(4),100,1.5,8.0,4.0,2,2,15,0.5,&sprite_mem.at(5));
//        enemy test_enemy(35.0,10.0,&sprite_mem.at(4),100,1.5,8.0,4.0);
        obj_to_draw enemy_dr;

//        test_enemy.x_loc=35.0;
//        test_enemy.y_loc=10.0;
//        test_enemy.spr=&sprite_mem.at(4);
//        test_enemy.mov_speed=1.5;
//        test_enemy.health=100;
//        test_enemy.detect_range=8.0;
//        test_enemy.attack_range=4.0;


        float ray_a;
        float ray_sin;
        float ray_cos;

        float ray_x;
        float ray_y;

        float ray_d;

        int ray_map_x;
        int ray_map_y;

        int ray_map_x_b;
        int ray_map_y_b;

        int step_add_x;
        int step_add_y;

        float delta_dist_x;
        float delta_dist_y;
        float dist_next_side_x;
        float dist_next_side_y;

        bool ray_f_w;
        bool side_seen;

        int y_ceiling;
        int y_floor;

        float sprite_sample_x;
        float sprite_sample_y;

        float temp_p_x;
        float temp_p_y;


        float angle_to_obj;
        float angle_diff;

        int obj_start_x;
        int obj_size_x;

        int object_ceiling;
        int object_floor;
        int obj_size_y;

//        for(int y=0;y<screen_y;y++){
//            for(int x=0;x<screen_x;x++){
//                Draw(x,y,0x0020,(fg_col=FG_BLACK) | (bg_col=BG_BLACK));
//            }
//        }

        auto t1=chrono::system_clock::now();
        auto t2=chrono::system_clock::now();

        //main game loop
        while(true){

            //timing
            t2 = chrono::system_clock::now();
            chrono::duration<float> el_time = t2-t1;
            t1=t2;
            time_game_loop=el_time.count();

            //FPS limit
            if(is_fps_limit){
                if(time_game_loop<(1.0/FPS_limit)){
                std::this_thread::sleep_for (std::chrono::milliseconds((int)((1000.0/FPS_limit)-time_game_loop*1000)));
                }
            }

            //camera moving
            if(GetAsyncKeyState((unsigned short)'O') & 0x8000){
                p_angle -= rot_sensitive*time_game_loop;
                if(p_angle>2*PI){
                    p_angle-=2*PI;
                }
                if(p_angle<0){
                    p_angle+=2*PI;
                }
            }
            if(GetAsyncKeyState((unsigned short)'P') & 0x8000){
                p_angle += rot_sensitive*time_game_loop;
                if(p_angle>2*PI){
                    p_angle-=2*PI;
                }
                if(p_angle<0){
                    p_angle+=2*PI;
                }
            }

            //movement
            if(GetAsyncKeyState((unsigned short)'W') & 0x8000){
                temp_p_x=p_x+cos(p_angle)*p_speed*time_game_loop;
                temp_p_y=p_y+sin(p_angle)*p_speed*time_game_loop;
                if(0.0<temp_p_x and temp_p_x<curr_map.map_x_size and 0.0<temp_p_y and temp_p_y<curr_map.map_y_size and curr_map.get_symbol((int) temp_p_x,(int) temp_p_y)!='#'){
                    p_x=temp_p_x;
                    p_y=temp_p_y;
                }
            }
            if(GetAsyncKeyState((unsigned short)'S') & 0x8000){
                temp_p_x=p_x-cos(p_angle)*p_speed*time_game_loop;
                temp_p_y=p_y-sin(p_angle)*p_speed*time_game_loop;
                if(0.0<temp_p_x and temp_p_x<curr_map.map_x_size and 0.0<temp_p_y and temp_p_y<curr_map.map_y_size and curr_map.get_symbol((int) temp_p_x,(int) temp_p_y)!='#'){
                    p_x=temp_p_x;
                    p_y=temp_p_y;
                }
            }
            if(GetAsyncKeyState((unsigned short)'D') & 0x8000){
                temp_p_x=p_x-sin(p_angle)*p_speed*time_game_loop;
                temp_p_y=p_y+cos(p_angle)*p_speed*time_game_loop;
                if(0.0<temp_p_x and temp_p_x<curr_map.map_x_size and 0.0<temp_p_y and temp_p_y<curr_map.map_y_size and curr_map.get_symbol((int) temp_p_x,(int) temp_p_y)!='#'){
                    p_x=temp_p_x;
                    p_y=temp_p_y;
                }
            }
            if(GetAsyncKeyState((unsigned short)'A') & 0x8000){
                temp_p_x=p_x+sin(p_angle)*p_speed*time_game_loop;
                temp_p_y=p_y-cos(p_angle)*p_speed*time_game_loop;
                if(0.0<temp_p_x and temp_p_x<curr_map.map_x_size and 0.0<temp_p_y and temp_p_y<curr_map.map_y_size and curr_map.get_symbol((int) temp_p_x,(int) temp_p_y)!='#'){
                    p_x=temp_p_x;
                    p_y=temp_p_y;
                }
            }

            curr_map.clear_p_see();

            //Rendering collum by collum
            for(int x=0;x<screen_x;x++){
                //setting needed values
                ray_a=(p_angle-FOV/2.00)+((float)x / (float)screen_x)*FOV;

                if(ray_a>2*PI){
                    ray_a-=2*PI;
                }
                if(ray_a<0){
                    ray_a+=2*PI;
                }

                ray_d=0.0f;
                ray_x=p_x;
                ray_y=p_y;
                ray_sin=sin(ray_a);
                ray_cos=cos(ray_a);
                ray_f_w=false;

                if(ray_sin==0){
                    ray_sin=0.0000001f;
                }

                if(ray_cos==0){
                    ray_cos=0.0000001f;
                }

                ray_map_x=(int)ray_x;
                ray_map_y=(int)ray_y;

                delta_dist_x=abs(1/ray_cos);
                delta_dist_y=abs(1/ray_sin);

                if(ray_cos>0){
                    step_add_x=1;
                    dist_next_side_x=(((float)(ray_map_x+1))-ray_x)*delta_dist_x;
                }else{
                    step_add_x=-1;
                    dist_next_side_x=(ray_x-((float)ray_map_x))*delta_dist_x;
                }

                if(ray_sin>0){
                    step_add_y=1;
                    dist_next_side_y=((float)(ray_map_y+1)-ray_y)*delta_dist_y;
                }else{
                    step_add_y=-1;
                    dist_next_side_y=(ray_y-(float)(ray_map_y))*delta_dist_y;
                }

                while(ray_map_x>-1 and ray_map_x<curr_map.map_x_size and ray_map_y>-1 and ray_map_y<curr_map.map_y_size and ray_d<DOF){
                    if(dist_next_side_x<dist_next_side_y){
                        //go side x
                        ray_d+=dist_next_side_x;
                        ray_x+=dist_next_side_x*ray_cos;
                        ray_y+=dist_next_side_x*ray_sin;
                        ray_map_x_b=ray_map_x;
                        ray_map_y_b=ray_map_y;
                        ray_map_x+=step_add_x;
                        dist_next_side_x=delta_dist_x;

                        if(ray_sin>0){
                            dist_next_side_y=((float)(ray_map_y+1)-ray_y)*delta_dist_y;
                        }else{
                            dist_next_side_y=(ray_y-(float)(ray_map_y))*delta_dist_y;
                        }

                    }else{
                        //go side y
                        ray_d+=dist_next_side_y;
                        ray_x+=dist_next_side_y*ray_cos;
                        ray_y+=dist_next_side_y*ray_sin;
                        ray_map_x_b=ray_map_x;
                        ray_map_y_b=ray_map_y;
                        ray_map_y+=step_add_y;
                        dist_next_side_y=delta_dist_y;

                        if(ray_cos>0){
                            dist_next_side_x=((float)(ray_map_x+1)-ray_x)*delta_dist_x;
                        }else{
                            dist_next_side_x=(ray_x-(float)(ray_map_x))*delta_dist_x;
                        }
                    }

                    switch(curr_map.get_symbol(ray_map_x,ray_map_y)){
                        case '#':
                            ray_f_w=true;
                            goto end_while_ray_mov;
                        case 'o':

                            if(!(*curr_map.get_p_see(ray_map_x,ray_map_y))){
                                new_obj_d.loc_x=(float)ray_map_x+0.5;
                                new_obj_d.loc_y=(float)ray_map_y+0.5;
                                new_obj_d.dist_from_player=sqrt( (p_x-new_obj_d.loc_x)*(p_x-new_obj_d.loc_x)+(p_y-new_obj_d.loc_y)*(p_y-new_obj_d.loc_y) );
                                new_obj_d.spr=&sprite_mem.at(curr_map.get_num(ray_map_x,ray_map_y));
                                list_obj_to_draw.push(new_obj_d);
                                //(*curr_map.get_load_obj_from_map(ray_map_x,ray_map_y))=true;
                            }
                            break;
                    }
                    (*curr_map.get_p_see(ray_map_x,ray_map_y))=true;

                }
                end_while_ray_mov:;

                depth_array [x]=ray_d;

                if(ray_f_w){

                    if( ray_map_x_b==ray_map_x and ray_map_y_b>ray_map_y ){
                        sprite_sample_x=ray_x-((float)((int) ray_x));
                    }

                    if( ray_map_x_b>ray_map_x and ray_map_y_b==ray_map_y ){
                        sprite_sample_x=(1-ray_y+((float)((int) ray_y)));
                    }

                    if( ray_map_x_b==ray_map_x and ray_map_y_b<ray_map_y ){
                        sprite_sample_x=(1-ray_x+((float)((int) ray_x)));
                    }

                    if( ray_map_x_b<ray_map_x and ray_map_y_b==ray_map_y ){
                        sprite_sample_x=ray_y-((float)((int) ray_y));
                    }

                }

                    y_ceiling=(int)((float)(screen_y/2.00)-((float)screen_y)/max(ray_d,0.0001f));
                    y_floor=screen_y-y_ceiling;

                    for(int y=0;y<screen_y;y++){


                        if(y<y_ceiling){
                            //draw sky
                            Draw(x,y,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_CYAN));
                        }else if(y<y_floor){
                        //draw wall
                        if(ray_f_w){
                            sprite_sample_y=((float)(y-y_ceiling))/((float)(y_floor-y_ceiling));
                            try{
                                Draw(x,y,0x0020,sprite_mem[curr_map.get_num(ray_map_x,ray_map_y)].take_sample(sprite_sample_x,sprite_sample_y));
                            }catch(const std::exception& e){}
                        }else{
                            Draw(x,y,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_CYAN));
                        }
                        }else{
                            //draw floor
                            Draw(x,y,0x2022,(fg_col=FG_GREEN) | (bg_col=BG_DARK_GREEN));
                        }

                    }



            }

            //drawing objects

//            obj_to_draw add_t;
//            add_t.loc_x=4.0;
//            add_t.loc_y=4.0;
//            add_t.dist_from_player=sqrt((p_x-add_t.loc_x)*(p_x-add_t.loc_x) + (p_y-add_t.loc_y)*(p_y-add_t.loc_y));
//            add_t.spr=&sprite_mem[2];
//            list_obj_to_draw.push(add_t);

            test_enemy.update(p_x,p_y,&curr_map,time_game_loop);
            enemy_dr.dist_from_player=sqrt( (p_x-test_enemy.x_loc)*(p_x-test_enemy.x_loc) + (p_y-test_enemy.y_loc)*(p_y-test_enemy.y_loc) );
            enemy_dr.loc_x=test_enemy.x_loc;
            enemy_dr.loc_y=test_enemy.y_loc;
            enemy_dr.spr=test_enemy.spr;
            list_obj_to_draw.push(enemy_dr);

            update_enemy_projectiles(p_x,p_y,time_game_loop,& health);

                    string s;
                    s="FPS: "+to_string(1.00/time_game_loop)+" Player x: "+to_string(p_x)+" Player y: "+to_string(p_y)+" Player angle: "+to_string(p_angle);
                    char const *p_n_l = s.c_str();
                    SetConsoleTitle(p_n_l);

            obj_to_draw curr_obj_d;

            while(!list_obj_to_draw.empty()){
                curr_obj_d=list_obj_to_draw.top();
                list_obj_to_draw.pop();

                angle_to_obj=my_atan2((curr_obj_d.loc_x-p_x),(curr_obj_d.loc_y-p_y));

                angle_diff=abs(p_angle-angle_to_obj);
                angle_diff=min(angle_diff,(2*PI-angle_diff));

                if(angle_diff<FOV/2 and curr_obj_d.dist_from_player>min_dist_to_draw_obj){

                    float tr_angle=p_angle-angle_diff;

                    if(tr_angle<0){
                        tr_angle+=2*PI;
                    }

                    if(abs(angle_to_obj-tr_angle)<0.0001){
                        obj_start_x=screen_x/2-((int)(((float)screen_x)*angle_diff/(FOV)));
                    }else{
                        obj_start_x=screen_x/2+((int)(((float)screen_x)*angle_diff/(FOV)));
                    }


                    object_ceiling=(int)( ((float)screen_y)/2.0-((float)screen_y)/curr_obj_d.dist_from_player);
                    object_floor=screen_y-object_ceiling;
                    obj_size_y=object_floor-object_ceiling;
                    obj_size_x=(int)(((float)obj_size_y)*((float)(curr_obj_d.spr->x_length))/((float)(curr_obj_d.spr->y_length)));

                    obj_start_x-=obj_size_x/2;

//                    string s;
//                    s="Player angle: "+to_string(p_angle)+" Angle to obj: "+to_string(angle_to_obj)+" Angle diff: "+to_string(angle_diff)+" Tr angle: "+to_string(tr_angle)+" Obj_start_x: "+to_string(obj_start_x);
//                    char const *p_n_l = s.c_str();
//                    SetConsoleTitle(p_n_l);

                    bool should_not_draw;
                    short colour_to_draw;

                    for(int x=0;x<obj_size_x;x++){
                        if((x+obj_start_x)>-1 and (x+obj_start_x)<screen_x){
                            if(depth_array[x+obj_start_x]>=curr_obj_d.dist_from_player){
                                for(int y=0;y<obj_size_y;y++){
                                    should_not_draw=curr_obj_d.spr->take_sample_check_ignore( ((float)x)/((float)obj_size_x),((float)y)/((float)obj_size_y));
                                    if(!should_not_draw){
                                        colour_to_draw=curr_obj_d.spr->take_sample(((float)x)/((float)obj_size_x),((float)y)/((float)obj_size_y));
                                        Draw((x+obj_start_x),(y+object_ceiling),0x0020,colour_to_draw);
                                    }
                                }
                            }
                        }

                    }

                }
            }

            //draw hud

            //draw health bar
            for(int i=0;i<health_bar_resolution+2;i++){
                Draw(i,0,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
                Draw(i,5,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            }
            int health_rem=(int)( health/max_health*((float)health_bar_resolution) );
            health_rem++;
            for(int i=0;i<health_bar_resolution;i++){
                if(i<=health_rem){
                    Draw(i+1,1,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_RED));
                    Draw(i+1,2,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_RED));
                    Draw(i+1,3,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_RED));
                    Draw(i+1,4,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_RED));
                }else{
                    Draw(i+1,1,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_DARK_RED));
                    Draw(i+1,2,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_DARK_RED));
                    Draw(i+1,3,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_DARK_RED));
                    Draw(i+1,4,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_DARK_RED));
                }
            }
            Draw(0,1,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(0,2,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(0,3,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(0,4,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(health_bar_resolution+1,1,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(health_bar_resolution+1,2,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(health_bar_resolution+1,3,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));
            Draw(health_bar_resolution+1,4,0x0020,(fg_col=BG_BLACK) | (bg_col=BG_GREY));


            //refresh screen
            refresh_screen();

        }


    return;
    }


};





int main(){
//cout<<"Hello world!";
console_game_mainframe my_game;
my_game.construct_cons(300,180,4,4);
my_game.launch_game();

return 0;
}
