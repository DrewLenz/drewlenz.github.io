#ifndef _WORKINGGAMECLASS_H
#define _WORKINGGAMECLASS_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#include <math.h>


class Obj{
  private:
    int _X;
    int _Y;
    int _height;
    int _width;
    int _speed;
    int _color;


  public:
    unsigned char Bitmap[];
    int X(){
      return _X;
    }
    void setX(int X){
      _X = X;
      return;
    }

    int Y() {
      return _Y;
    }

    void setY(int Y){
      _Y = Y;
      return;
    }

    int Height(){
      return _height;
    }

    int Width(){
      return _width;
    }

    int Speed(){
      return _speed;
    }

    void set_Speed(int obj_speed){
      _speed = obj_speed;
    }

    int Color(){
      return _color;
    }

    void setColor(int color){
      _color = color;
    }

    Obj(int x, int y, int width, int height, int obj_speed, int color){
      _X = x;
      _Y = y;
      _height = height;
      _width = width;
      _speed = obj_speed;
      _color = color;
    }

};

class Frog : public Obj{
  public:
    unsigned char Bitmap[8] =
    { B10000001, B01000010, B00111100, B00111100,
      B00111100, B00111100, B01000010, B10000001, };

    Frog(int x, int y, int width, int height, int obj_speed, int color) : Obj(x, y, width, height, obj_speed, color){ };

};

class Car1 : public Obj{
  public:
    unsigned char Bitmap[8] =
    { B11100111, B01000010, B01111110, B10000001,
      B10000001, B01111110, B01000010, B11100111, };

    Car1(int x, int y, int width, int height, int obj_speed, int color) : Obj(x, y, width, height, obj_speed, color){ };

};

class Car2 : public Obj{
  public:
    unsigned char Bitmap[8] =
    { B11100111, B01000010, B01111110, B11111111,
      B11111111, B01111110, B01000010, B11100111, };

    Car2(int x, int y, int width, int height, int obj_speed, int color) : Obj(x, y, width, height, obj_speed, color){ };

};

class Bus : public Obj{
  public:
    unsigned char Bitmap[16] =
    {
      B11100000, B00000111,
      B11111111, B11111111,
      B11010101, B10101011,
      B11010101, B10101011,
      B11010101, B10101011,
      B11010101, B10101011,
      B11111111, B11111111,
      B11100000, B00000111,
    };
    Bus(int x, int y, int width, int height, int obj_speed, int color) : Obj(x, y, width, height, obj_speed, color){ };
};

class Log : public Obj{
  public:
    Log(int x, int y, int width, int height, int obj_speed, int color) : Obj(x, y, width, height, obj_speed, color){ };
};

class Home {
  private:
    int _location;
    bool _occupied;
    bool _prevOccupied;

  public:
    int Location(){
      return _location;
    }

    bool isOccupied(){
      return _occupied;
    }

    bool setOccupied(bool occupied){
      _occupied = occupied;
    }

    bool prevOccupied(){
      return _prevOccupied;
    }

    void setPrevOccupied(bool prev){
      _prevOccupied = prev;
    }

    Home(int location){
      _location = location;
      _occupied = false;
      _prevOccupied = false;
    }
};

#endif // _WORKINGGAMECLASS_H
