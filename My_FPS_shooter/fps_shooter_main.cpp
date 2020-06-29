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

#include "my_extra_math.h"

#include "colour_enum.h"

#include "sprite.h"

#include "game_map.h"

#include "objects.h"

//#include "player_info.h"



struct obj_to_draw{

    float dist_from_player;
    float loc_x;
    float loc_y;

    sprite * spr;

    void calc_dist_from_player(float p_x, float p_y){
        dist_from_player=sqrt( (p_x-loc_x)*(p_x-loc_x) + (p_y-loc_y)*(p_y-loc_y) );
    }

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

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    //all sprite stuff;



    private:

    int num_sprites;
    sprite * curr_sprite;

    FILE * list_of_sprites_file;
    vector<sprite> sprite_mem;

    void load_sprites(){

        freopen("sprites_main_list.txt","r",stdin);

        cin>>num_sprites;
        string curr_file_name;
        sprite curr_sprite;

        for(int i=0;i<num_sprites;i++){
            cin>>curr_file_name;
            curr_sprite.load_sprite_from_file(curr_file_name.c_str());
            sprite_mem.push_back(curr_sprite);
        }
        return;
    }


    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



    private:

    game_map curr_map;

    void load_map(){
        char m_x_file_name [100]="first_map.game_map";
        curr_map.load_from_file(m_x_file_name);
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //player stats

    float p_x=10.0f;
    float p_y=10.0f;
    float p_angle=0.0f;

    float health=100.0;
    float max_health=100.0;
    int health_bar_resolution=100;


    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    //object related

    float min_dist_to_draw_obj=1.0;
    obj_to_draw new_obj_d;
    priority_queue <obj_to_draw> list_obj_to_draw;



    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //video stuff

    float FOV=PI / 4.0;
    float DOF=20.0;
    float ray_j=0.02f;

    float time_game_loop;
    float FPS_limit=30.0;
    bool is_fps_limit=true; //carefully here! Switching off may cause CPU to overload

    float rot_sensitive=1.5f; //angle per second
    float p_speed=3.0f;       //distance per second

    float * depth_array;



    void raycasting(){

        //variables for raycasting
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

        curr_map.clear_p_see();

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

    }

    //draw all objects

    void draw_objects(){

        float angle_to_obj;
        float angle_diff;

        int obj_start_x;
        int obj_size_x;

        int object_ceiling;
        int object_floor;
        int obj_size_y;

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


        return;
    }


    //draw hud

    void draw_hud(){

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

    }

    void draw_all(){

        raycasting();

        draw_objects();

        draw_hud();

        refresh_screen();

    return;
    }



    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //enemies dynamic objects


    void update_enemies(float player_x,float player_y,float passed_time){

        for(int i=0;i<curr_map.all_enemies.size();i++){
            curr_map.all_enemies.at(i)->update(player_x,player_y,&curr_map,passed_time);
            //if(){ not draw if not visible
            new_obj_d.loc_x=curr_map.all_enemies.at(i)->x_loc;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            new_obj_d.loc_y=curr_map.all_enemies.at(i)->y_loc;
            new_obj_d.calc_dist_from_player(player_x,player_y);
            new_obj_d.spr=curr_map.all_enemies.at(i)->spr;
            list_obj_to_draw.push(new_obj_d);
            //}
        }

    }



    //update enemy bullets

    void update_enemy_projectiles(float player_x,float player_y,float passed_time,float * p_h){
        enemy_projectile curr_enemy_proj(0,0,0,0,0,0,0);
        for(int i=0;i<curr_map.enemy_projectiles.size();i++){
            curr_enemy_proj=curr_map.enemy_projectiles.front();
            curr_map.enemy_projectiles.pop();
            if(!(curr_enemy_proj.is_destroyed)){

                curr_enemy_proj.update(player_x,player_y,&curr_map,passed_time,p_h);

                new_obj_d.loc_x=curr_enemy_proj.x_loc;
                new_obj_d.loc_y=curr_enemy_proj.y_loc;
                new_obj_d.calc_dist_from_player(player_x,player_y);

                new_obj_d.spr=curr_enemy_proj.spr;
                list_obj_to_draw.push(new_obj_d);

                curr_map.enemy_projectiles.push(curr_enemy_proj);
            }
        }
        return;
    }

// controls and movement

    void controls_movement(){

            //movement

            float temp_p_x;
            float temp_p_y;

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


        return;
    }



    void loading_stuff(){
        load_sprites();
        load_map();
        depth_array = new float [screen_x];
        enemy * test_enemy = new enemy_fir_proj(35.0,10.0,&sprite_mem.at(4),100,1.5,8.0,4.0,2,2,15,0.5,&sprite_mem.at(5));
        curr_map.all_enemies.push_back(test_enemy);
    }

    public:

    void launch_game(){

        loading_stuff();

        // timing
        auto t1=chrono::system_clock::now();
        auto t2=chrono::system_clock::now();


        while(1){

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

//            std::thread first (controls_movement);
//
//            std::thread second (draw_all);

            controls_movement();

            draw_all();

            update_enemies(p_x,p_y,time_game_loop);

            update_enemy_projectiles(p_x,p_y,time_game_loop,&health);

//            first.join();
//
//            second.join();

        }
    }


};


int main(){
console_game_mainframe my_game;
my_game.construct_cons(320,180,4,4);
my_game.launch_game();
return 0;
}
