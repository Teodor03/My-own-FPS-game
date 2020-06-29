#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include<cstdio>

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

#endif // SPRITE_H_INCLUDED
