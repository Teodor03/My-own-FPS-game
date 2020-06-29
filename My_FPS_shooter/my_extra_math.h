#ifndef MY_EXTRA_MATH_H_INCLUDED
#define MY_EXTRA_MATH_H_INCLUDED


// P
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

#endif // MY_EXTRA_MATH_H_INCLUDED
