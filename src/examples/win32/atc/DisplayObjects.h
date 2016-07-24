#ifndef DisplayObjects
#define DisplayObjects

#define NOMINMAX
#include <Windows.h>
#include <set>
#include <vector>
#include <string>

const int BoardWidth = 25;
const int BoardHeight = 16;
const int TextWidth = 12;
const int TextHeight = 16;

const int PlaneDisplayLeft = 35;

class DisplayObject
{
public:
    DisplayObject(char nm, int x=0, int y=0) : name(nm) {
        position.x = x;
        position.y = y;
    }
    virtual void Draw(HDC dc);
    void MoveDelta(int dx, int dy)
    {
        position.x += dx;
        position.y += dy;
    }
    void GetPosition(int &x, int &y) { x = position.x; y = position.y; }
    char GetName() { return name; }
protected:

    char name;
    POINT position;
};
enum Heading { east, southeast,south, southwest,west,northwest, north, northeast };
enum Command { ChangeAlt, RotateLeft, RotateRight, CircleNavaid, ClearToLand };

class Plane : public DisplayObject
{
public:
    Plane(int index, int Src, int Dest, int alt, bool fast, int start, int Fuel, int x=0, int y=0, Heading head) 
        : DisplayObject('A' + index, x, y), src(Src), dest(Dest),
        speed(fast ? 15 : 30), altitude(alt), current(0), rotate(0), fuel(Fuel), 
            startTime(start), circleDist(1), circling(false), clearedToLand(false), 
            nextMove(startTime + speed), heading(head)
     { 
     }
    void CommandPlane(Command command, int modifier);
    void Rotate(int n)
    {
        rotate = Rotate;
    }
    void Circle(int x, int y);
    void SetHeading(enum Heading n) { heading = n; Circle(-1, -1); }
    enum Heading GetHeading() { return heading; }
    void SetAltitude(int n) { altitude = n; }
    int GetAltitude() { return altitude; }
    int GetDestination() { return dest; }    
    int GetSource() { return src; }
    int GetSpeed() { return speed; }
    bool IsJet() { return speed == 15; }
    bool Move(int time);
    bool OffBoard();
    bool AT(int x1, int y1) { return x == x1 && y == y1; }
    bool ClearedToLand() { return clearedToLand; }
    void SetClearedToLand(bool state) { clearedToLand = state; }
    int GetStartTime() { return startTime; }
    int GetFuel() { return fuel; }
    virtual void Draw(HDC dc);
    int dist(int x, int y);
protected:
    int cdist(int x, int y);
    int altitude;
    int speed;
    int startTime;
    int fuel;
    char dest, src;
    enum Heading heading;
private:
    char clearedToLand;
    int circleDist;
    bool circling;
    int current;
    int rotate;
    POINT last, circlePos;
    int nextMove;
};
class Fix : public DisplayObject
{
public:
    Fix(int index, int x=0, int y=0, Heading head=east) : DisplayObject(index + '0', x, y),
        heading(head)
    {
    }
    bool At(Plane &plane);
    int GetHeading() { return heading; }
private:
    Heading heading;
};
class Navaid : public DisplayObject
{
public:
    Navaid(int index, int x=0, int y=0, Heading head = east) : DisplayObject('*', x, y), heading(head)
    {
    }
    bool At(Plane &plane);
protected:
    int heading;
};
class Airport : public DisplayObject
{
public:
    Airport(int index, int x = 0, int y = 0, Heading head = east) : DisplayObject(index ? '$' : '%', x, y),
        heading(head)
    {
    }
    Heading GetHeading() { return heading; }
    bool At(Plane &plane);
protected:
    Heading heading;
};

class Board
{
public:
    Board(int maxtime) : time(0), maxTime(maxtime * 60 + 16 * 60) { Setup(); }
    ~Board() ;
    std::string Timed();
    std::string GetDisplayString(int index);
    void Draw(HDC dc);
    int GetPlaneIndex(int x, int y);
    void CommandPlane(int index, enum Command command, int modifier);
protected:
    std::string GetDisplayString(Plane *plane);
    void In();
    void DrawBackground(HDC hdc);
    void DrawEntryPlanes(HDC hdc);
    void DrawTime(HDC hdc);
    void Setup();
private:
    int time;
    int maxTime;
    std::set<Plane *> planesDone;
    std::set<Plane *> planesIn;
    std::set<Plane *> planesOut;
    std::set<Plane *> planesDisplay;
    std::vector<Navaid *> navAids;
    std::vector<Fix *> fixes;
    std::vector<Airport *> airports;

};
#endif