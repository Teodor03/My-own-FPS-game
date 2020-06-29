#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include"game_map.h"
#include"sprite.h"
#include<cmath>


class object{
public:

    float x_loc;
    float y_loc;

    sprite * spr;

    object(float x,float y,sprite * s): x_loc{x},y_loc{y},spr{s}{
}

//may add some sort of animations in the future

};

//class enemy_type{
//
//float starting_health;
//
//float movement_speed;
//
//float detect_range;
//float attack_range;
//
//float reload_time;
//
//bool is_firing_projectile;
//
//enemy_projectile
//
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
//wwww
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




#endif // OBJECTS_H_INCLUDED
