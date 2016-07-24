#include "DisplayObjects.h"
#include "Math.h"
#include "stdlib.h"

// Fix Table 0
struct triple
{
    int y;
    int x;
    Heading heading; 
};

struct triple defaultFixes[10][14] =
{
    {
		{ 0, 0,southeast },	//0
		{ 0, 8,south },	//1
		{ 0,19,south },	//2
		{ 4,24,west },	//3
		{ 9,24,west },	//4
		{15,19,north },	//5
		{15,11,north },	//6
		{15, 2,north },	//7
		{11, 0,east },	//8
		{ 4, 0,east },	//9
		{ 4, 8,southeast },	//*#
		{ 9,19,north },	//*$
		{ 8,12,northwest },	//#
		{ 5,19,south },	//$
    },
// fix table 1
    {
		{ 0, 4,south },	//0
		{ 0,12,south },	//1
		{ 0,17,south },	//2
		{ 5,24,west },	//3
		{10,24,west },	//4
		{15,19,north },	//5
		{15,12,north },	//6
		{15, 4,north },	//7
		{10, 0,east },	//8
		{ 5, 0,east },	//9
		{ 5,12,south },	//*#
		{ 7, 6,north },	//*$
		{ 9,12,north },	//#
		{ 3, 6,south },	//$
    },
// fix table 2
    {
		{ 0, 4,south },	//0
		{ 0,10,south },	//1
		{ 0,17,south },	//2
		{ 1,24,west },	//3
		{ 8,24,west },	//4
		{15,24,northwest },	//5
		{15,17,north },	//6
		{15, 4,north },	//7
		{11, 0,east },	//8
		{ 5, 0,east },	//9
		{ 5, 5,east },	//*#
		{ 8,17,southwest },	//*$
		{ 5, 9,west },	//#
		{12,13,northeast },	//$
    },
// fix table 3
    {
		{ 0, 4,south },	//0
		{ 0,10,south },	//1
		{ 0,16,south },	//2
		{ 0,24,southwest },	//3
		{ 8,24,west },	//4
		{15,16,north },	//5
		{15,10,north },	//6
		{15, 3,north },	//7
		{ 8, 0,east },	//8
		{ 4, 0,east },	//9
		{ 4,16,west },	//*#
		{12, 6,northeast },	//*$
		{ 4,12,east },	//#
		{ 8,10,southwest },	//$
    },

// fix table 4
    {
		{ 0, 6,south },	//0
		{ 0,11,south },	//1
		{ 0,17,south },	//2
		{ 3,24,west },	//3
		{10,24,west },	//4
		{15,24,northwest },	//5
		{15,15,north },	//6
		{15, 7,north },	//7
		{12, 0,east },	//8
		{ 6, 0,east },	//9
		{ 4, 8,east },	//*#
		{ 8,13,west },	//*$
		{ 4,12,west },	//#
		{ 8, 9,east },	//$
    },

// fix table 5
    {
		{ 0, 7,south },	//0
		{ 0,12,south },	//1
		{ 0,17,south },	//2
		{ 5,24,west },	//3
		{10,24,west },	//4
		{15,15,north },	//5
		{15,10,north },	//6
		{15, 4,north },	//7
		{ 8, 0,east },	//8
		{ 3, 0,east },	//9
		{ 5, 9,east },	//*#
		{10, 9,east },	//*$
		{ 5,13,west },	//#
		{10,13,west },	//$
    },

// fix table 6
    {
		{ 0, 6,south },	//0
		{ 0,11,south },	//1
		{ 0,16,south },	//2
		{ 5,24,west },	//3
		{10,24,west },	//4
		{15,16,north },	//5
		{15,11,north },	//6
		{15, 6,north },	//7
		{10, 0,east },	//8
		{ 5, 0,east },	//9
		{ 7, 6,south },	//*#
		{ 5,11,east },	//*$
		{11, 6,north },	//#
		{ 5,16,west },	//$
    },

// fix table 7
    {
		{ 0, 0,southeast },	//0
		{ 0, 8,south },	//1
		{ 0,14,south },	//2
		{ 2,24,west },	//3
		{ 8,24,west },	//4
		{15,20,north },	//5
		{15,14,north },	//6
		{15, 5,north },	//7
		{11, 0,east },	//8
		{ 5, 0,east },	//9
		{11,18,northwest },	//*#
		{ 9, 4,northeast },	//*$
		{ 7,14,southeast },	//#
		{ 5, 8,southwest },	//$
    },

// fix table 8
    {
		{ 0, 3,south },	//0
		{ 0, 9,south },	//1
		{ 0,16,south },	//2
		{ 4,24,west },	//3
		{ 9,24,west },	//4
		{15,24,northwest },	//5
		{15,14,north },	//6
		{15, 3,north },	//7
		{10, 0,east },	//8
		{ 3, 0,east },	//9
		{ 4,15,west },	//*#
		{ 9,12,southwest },	//*$
		{ 4,10,east },	//#
		{13, 8,northeast },	//$
    },

// fix table 9
    {
		{ 0, 0,southeast },	//0
		{ 0, 8,south },	//1
		{ 0,16,south },	//2
		{ 5,24,west },	//3
		{11,24,west },	//4
		{15,18,north },	//5
		{15, 8,north },	//6
		{15, 2,north },	//7
		{11, 0,east },	//8
		{ 6, 0,east },	//9
		{ 6, 8,east },	//*#
		{ 9,13,south },	//*$
		{ 6,12,west },	//#
		{13,13,north },	//$
    },
};
void DisplayObject::Draw(HDC dc)
{
    char ch[2] = { 0 };
    ch[0] = name;
    TextOut(dc, position.x * TextWidth, position.y * TextHeight, ch, 1);
}
void Plane::Draw(HDC dc)
{
    char ch[2] = { 0 };
    ch[0] = altitude + '0';
    DisplayObject::Draw(dc);
    TextOut(dc, (position.x+1) * TextWidth, position.y * TextHeight, ch, 1);
}
int Plane::dist(int x, int y)
{
    return (int)sqrt((x - position.x) * (x - position.x) + (y - position.y )  * ( y - position.y));
}
int Plane::cdist(int x, int y)
{
    return (int)sqrt((x - circlePos.x) * (x - circlePos.x) + (y - circlePos.y )  * ( y - circlePos.y));
}
void Plane::CommandPlane(Command command, int modifier)
{
    switch (command)
    {
        case ChangeAlt:
            altitude = modifier;
            break; 
        case RotateLeft:
            Circle(-1, -1);
            rotate = -modifier;
            break;
        case RotateRight:
            Circle(-1, -1);
            rotate = modifier;
            break;
        case ClearToLand:
            clearedToLand = TRUE;
            break;
    }
}
void Plane::Circle(int x, int y)
{
    if (x == -1)
    {
        circling = FALSE;
    }
    else
    {
        circleDist = dist(x, y);
        circlePos.x = x;
        circlePos.y = y;
        if (circleDist <= 1)
            circleDist = 2;
        circling = TRUE;
    }
}
bool Plane::Move(int time)
{
    if (time >= nextMove)
    {
        fuel--;
        nextMove += speed;
        if (circling)
        {
            switch(heading)
            {
                int n;
                case east:
                    n = cdist(position.x+1, position.y);
                    if (n > circleDist)
                    {
                        n = cdist(position.x+1, position.y+1);
                        if (n > circleDist)
                            heading = northeast;
                        else
                            heading = southeast;
                    }
                    break;
                case southeast:
                    n = cdist(position.x+1, position.y+1);
                    if (n > circleDist)
                    {
                        n = cdist(position.x, position.y+1);
                        if (n > circleDist)
                            heading = east;
                        else
                            heading = south;
                    }
                    break;
                case south:
                    n = cdist(position.x, position.y+1);
                    if (n > circleDist)
                    {
                        n = cdist(position.x+1, position.y+1);
                        if (n > circleDist)
                            heading = southwest;
                        else
                            heading = southeast;
                    }
                    break;
                case southwest:
                    n = cdist(position.x-1, position.y+1);
                    if (n > circleDist)
                    {
                        n = cdist(position.x-1, position.y);
                        if (n > circleDist)
                            heading = south;
                        else
                            heading = west;
                    }
                    break;
                case west:
                    n = cdist(position.x-1, position.y);
                    if (n > circleDist)
                    {
                        n = cdist(position.x-1, position.y+1);
                        if (n > circleDist)
                            heading = northwest;
                        else
                            heading = southwest;
                    }
                    break;
                case northwest:
                    n = cdist(position.x-1, position.y-1);
                    if (n > circleDist)
                    {
                        n = cdist(position.x-1, position.y);
                        if (n > circleDist)
                            heading = north;
                        else
                            heading = west;
                    }
                    break;
                case north:
                    n = cdist(position.x, position.y-1);
                    if (n > circleDist)
                    {
                        n = cdist(position.x+1, position.y-1);
                        if (n > circleDist)
                            heading = northwest;
                        else
                            heading = northeast;
                    }
                    break;
                case northeast:
                    n = cdist(position.x+1, position.y-1);
                    if (n > circleDist)
                    {
                        n = cdist(position.x+1, position.y);
                        if (n > circleDist)
                            heading = north;
                        else
                            heading = west;
                    }
                    break;
            }
        }
        else if (rotate < 0) 
        {
            rotate++;
            heading = heading-1;
            if (heading < 0)
                heading += 8;
        }
        else if (rotate > 0)
        {
            rotate --;
            heading = heading + 1;
            if (heading >= 8)
                heading -= 8;
        }
        switch(heading)
        {
            case east:
                MoveDelta (1, 0);
                break;
            case southeast:
                MoveDelta (1, 1);
                break;
            case south:
                MoveDelta (0, 1);
                break;
            case southwest:
                MoveDelta (-1, 1);
                break;
            case west:
                MoveDelta (-1, 0);
                break;
            case northwest:
                MoveDelta (-1, -1);
                break;
            case north:
                MoveDelta (0, -1);
                break;
            case northeast:
                MoveDelta (1, -1);
                break;
        }
        return true;
    }
    return false;
}
bool Plane::OffBoard()
{
    return (int)position.x < 0 || position.x >= BoardWidth || (int)position.y < 0 || position.y >= BoardHeight;
}

bool Fix::At(Plane &plane)
{
    int x, y;
    plane.GetPosition(x, y);
    return (x == position.x || y == position.y) && plane.GetAltitude() >= 5 && (heading ^ 4) == plane.GetHeading();
}
bool Navaid::At(Plane &plane)
{
    int x, y;
    plane.GetPosition(x, y);
    if (( x == position.x || y == position.y) && plane.ClearedToLand())
    {
        plane.SetClearedToLand(false);
        plane.SetHeading(heading);
        plane.SetAltitude(0);
        return true;
    }
    return false;
}
bool Airport::At(Plane &plane)
{
    int x, y;
    plane.GetPosition(x, y);
    if ( x == position.x && y == position.y)
        if (plane.GetHeading() == (heading ^ 4))
            if (plane.GetAltitude() == 0)
                return true;
    return false;
}
void Board::Draw(HDC dc)
{
    DrawBackground(dc); 
    for (std::vector<Navaid *>::iterator it = navAids.begin(); it != navAids.end(); ++it)
    {
        (*it)->Draw(dc);
    }
    for (std::vector<Airport *>::iterator it = airports.begin(); it != airports.end(); ++it)
    {
        (*it)->Draw(dc);
    }
    for (std::vector<Fix *>::iterator it = fixes.begin(); it != fixes.end(); ++it)
    {
        (*it)->Draw(dc);
    }
    for (std::set<Plane *>::iterator it = planesIn.begin(); it != planesIn.end(); ++it)
    {
        (*it)->Draw(dc);
    }
    DrawEntryPlanes(dc);
    DrawTime(dc);
}
void Board::DrawTime(HDC dc)
{
    char buf[256];
    sprintf(buf, ">%02d<", (maxTime - time)/60);
    TextOut(dc, 10 * TextWidth, 20 * TextHeight, buf, strlen(buf));
}
Board::~Board()
{
    for (std::set<Plane *>:: iterator it = planesIn.begin(); it != planesIn.end(); ++it)
    {
        Plane *plane = *it;
        delete plane;
    }
    for (std::set<Plane *>:: iterator it = planesOut.begin(); it != planesOut.end(); ++it)
    {
        Plane *plane = *it;
        delete plane;
    }
    for (std::set<Plane *>:: iterator it = planesDisplay.begin(); it != planesDisplay.end(); ++it)
    {
        Plane *plane = *it;
        delete plane;
    }
    for (std::set<Plane *>:: iterator it = planesDone.begin(); it != planesDone.end(); ++it)
    {
        Plane *plane = *it;
        delete plane;
    }
    for (int i=0; i < 10; i++)
    {
        Fix *fix = fixes[i];
        delete fix;
    }
    delete (navAids[0]);
    delete (navAids[1]);
    delete (airports[0]);
    delete (airports[1]);
}
void Board::Setup()
{
    int rnd = random(10);
    for (int i=0; i < 10; i++)
    {
        fixes.push_back(new Fix(i, defaultFixes[rnd][i].x, defaultFixes[rnd][i].y, defaultFixes[rnd][i].heading));

    }
    navAids.push_back(new Navaid(0, defaultFixes[rnd][10].x, defaultFixes[rnd][10].y, defaultFixes[rnd][10].heading));
    navAids.push_back(new Navaid(1, defaultFixes[rnd][11].x, defaultFixes[rnd][11].y, defaultFixes[rnd][11].heading));
    airports.push_back(new Airport(0, defaultFixes[rnd][12].x, defaultFixes[rnd][12].y, defaultFixes[rnd][12].heading));
    airports.push_back(new Airport(1, defaultFixes[rnd][13].x, defaultFixes[rnd][13].y, defaultFixes[rnd][13].heading));

    for (int i=0; i < 26; i++)
    {
        int src = random(12), dest = random(12), fast = random(2), fuel = random(20) + 31;
        int alt = random(5) + 5, start = random(maxTime - 16 * 60);
        int fix = random(12);
        if (fix < 10)
        {
            planesOut.insert(new Plane(i, fix, dest, alt, fast, start, fuel,
                defaultFixes[rnd][fix].x, defaultFixes[rnd][fix].y, defaultFixes[rnd][fix].heading));
        }
        else
        {
            planesOut.insert(new Plane(i, fix, dest, alt, fast, start, fuel,
                defaultFixes[rnd][fix+2].x, defaultFixes[rnd][fix+2].y, defaultFixes[rnd][fix+2].heading ^ 4));
        }
    }
}
void Board::DrawBackground(HDC dc)
{
    char ch[2] = { '.', 0 };
    for (int i=0; i < BoardWidth; i++)
        for (int j=0; j < BoardHeight; j++)
            TextOut(dc, i * TextWidth, j * TextHeight, ch, 1);
}
std::string Board::GetDisplayString(Plane *plane)
{
    static char *directions[] = {"E ", "SE", "S ", "SW", "W ", "NW", "N ", "NE" };
    static char dests[] = {"0123456789%$"};
    char data[256];
    int f = plane->GetFuel() * plane->GetSpeed()/ 60;
    sprintf(data, "%c%c%s%c %c TO %c %c", plane->GetName(), plane->IsJet() ? 'J' : 'P', directions[plane->GetHeading()],
        plane->GetAltitude() + '0', dests[plane->GetSource()], dests[plane->GetDestination()], f < 10 ? f + '0' : '+') ;
    return data;
}
std::string Board::GetDisplayString(int index)
{
    for (std::set<Plane *>::iterator it = planesIn.begin(); it != planesIn.end(); ++it)
    {
        Plane *plane = *it;
        if (index == plane->GetName() - 'A')
            return GetDisplayString(plane);
    }
    return "";
}
void Board::DrawEntryPlanes(HDC dc)
{
    int i = 0;
    for (std::set<Plane *>::iterator it = planesDisplay.begin(); it != planesDisplay.end(); ++it)
    {
        Plane *plane = *it;
        int x = (i/16) * 10 + BoardWidth + 2 ;
        int y = (i % 16);
        std::string str = GetDisplayString(plane);
        TextOut(dc, x * TextWidth, y * TextHeight, str.c_str(), str.size());
        i++;
    }
}
int Board::GetPlaneIndex(int x, int y)
{
    x = x/TextWidth;
    y = y/TextHeight;
    for (std::set<Plane *>::iterator it = planesIn.begin(); it != planesIn.end(); ++it)
    {
        int x1, y1;
        Plane *plane = *it;
        plane->GetPosition(x1, y1);
        if (x1 == x && y1 == y)
            return plane->GetName() - 'A';
    }
    return -1;
}
void Board::CommandPlane(int index, enum Command command, int modifier)
{
    for (std::set<Plane *>::iterator it = planesIn.begin(); it != planesIn.end(); ++it)
    {
        Plane *plane = *it;
        if (plane->GetName() == index + 'A')
        {
            if (command == CircleNavaid)
            {
                int x,y;
                navAids[0]->GetPosition(x,y);
                int n1 = plane->dist(x, y);
                navAids[1]->GetPosition(x,y);
                int n2 = plane->dist(x,y);
                if (n1 < n2)
                {
                    navAids[0]->GetPosition(x,y);
                }
                plane->Circle(x, y);
            }
            else
            {
                plane->CommandPlane(command, modifier);
            }
            break;
        }
    }
}
void Board::In()
{
    if (time % 60 == 0)
    {
        int lower = time - time %60;
        int upper = time + 60;
        for (std::set<Plane *>::iterator it = planesOut.begin(); it != planesOut.end();)
        {
            Plane *plane = *it;
            int n = plane->GetStartTime();
            if (n >= lower && n < upper)
            {
                it = planesOut.erase(it);
                planesDisplay.insert(plane);
                continue;
            }
            ++it;
        }
        
    }
}
std::string Board::Timed()
{
    std::string rv;
    In();
    for (std::set<Plane *>::iterator it = planesIn.begin(); it != planesIn.end();)
    {
        Plane *plane = *it;
        int n = plane->GetDestination();
        bool atFix; 
        if (n >= 10)
        {
            navAids[n-10]->At(*plane);
            atFix = airports[n-10]->At(*plane);
            if (atFix)
            {
                if (plane->GetHeading() == airports[n-10]->GetHeading() ^ 4)
                {
                    it = planesIn.erase(it);
                    planesDone.insert(plane);
                    continue;
                }
            }
        }
        else
        {
            atFix = fixes[n]->At(*plane);
        }
        if (plane->GetAltitude() == 0)
        {
            bool at = false;
            int x, y;
            int x1, y1;
            plane->GetPosition(x, y);
            airports[0]->GetPosition(x1,y1);
            if (x == x1 && y == y1)
                at = true;
            airports[1]->GetPosition(x1,y1);
            if (x == x1 && y == y1)
                at = true;
            if (at)
            {
                rv = "Skimmed airport without landing!";
            }
        }
        if (plane->Move(time))
        {
            if (atFix && plane->OffBoard())
            {
                it = planesIn.erase(it);
                planesDone.insert(plane);
                continue;
            }
            else if (plane->OffBoard())
            {
                // game over, exit in wrong place
                if (plane->GetAltitude() < 5)
                    rv = std::string(plane->GetName()) + " left control area at wrong altitude";
                else 
                {   
                    if ((plane->GetDestination() < 10 && plane->GetHeading() != (fixes[plane->GetDestination()]->GetHeading() ^ 4)) ||
                        (plane->GetDestination() >9 && plane->GetHeading() != (airports[plane->GetDestination()-10]->GetHeading() ^ 4)))
                        rv = std::string(plane->GetName()) + " left control area at wrong heading";
                    else
                        rv = std::string(plane->GetName()) + " left control area at wrong place";
                }
            }
        }
        ++it;
    }
    for (std::set<Plane *>::iterator it = planesDisplay.begin(); it != planesDisplay.end();)
    {
        Plane *plane = *it;
        if (plane->GetStartTime() <= time)
        {
            it = planesDisplay.erase(it);
            planesIn.insert(plane);
            continue;
        }
        ++it;
    }
    if (rv == "")
    {
        for (std::set<Plane *>::iterator it1 = planesIn.begin(); rv == "" && it1 != planesIn.end(); ++it1)
        {
            Plane *left = *it1;
            for (std::set<Plane *>::iterator it2 = it1; rv == "" && it2 != planesIn.end(); ++it2)
            {
                Plane *right = *it2;
                if (it1 != it2)
                {
                    if (left->GetAltitude() == right->GetAltitude())
                    {
                        int x,y;
                        right->GetPosition(x, y);
                        if (left->dist(x, y) < 4)
                            rv = std::string("Conflict between ") + left->GetName() + " and " + right->GetName();
                    }
                }
            }
        }
    }
    if (rv == "")
    {
        for (std::set<Plane *>::iterator it = planesIn.begin(); rv == "" && it != planesIn.end(); ++it)
        {
            Plane *plane = *it;
            if (plane->GetFuel() < 0)
            {
                rv = std::string(plane->GetName()) + " out of fuel!";
            }
        }
    }
    // game over, they win!
    if (rv == "" && planesIn.size() == 0 && planesOut.size() ==0 && planesDisplay.size() == 0)
        rv = "You Won!";
    // game over, timeout
    if (rv == "" && ++time >= maxTime)
        rv = "Time is up!";
    return rv;
}