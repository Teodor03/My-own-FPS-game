#ifndef PLAYER_INFO_H_INCLUDED
#define PLAYER_INFO_H_INCLUDED

class player_info{

    //main stuff for engine

    float p_x=10.0f;
    float p_y=10.0f;
    float p_angle=0.0f;

    //player stats

    float health=100;
    float max_health=100.0;
    int health_bar_resolution=100;

    float p_speed=3.0f;       //distance per second


};

#endif // PLAYER_INFO_H_INCLUDED
