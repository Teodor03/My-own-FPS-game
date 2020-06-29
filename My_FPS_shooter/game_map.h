#ifndef GAME_MAP_H_INCLUDED
#define GAME_MAP_H_INCLUDED

#include<cstdio>
#include<algorithm>
#include<queue>

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


    vector<enemy *> all_enemies;


    queue<enemy_projectile> enemy_projectiles;

    ~game_map(){

        delete[] map_sym_mem;

        delete[] map_num_mem;

        delete[] p_see;

        for(int i=0;i<all_enemies.size();i++){
            delete all_enemies.at(i);
        }

    }

};

#endif // GAME_MAP_H_INCLUDED
