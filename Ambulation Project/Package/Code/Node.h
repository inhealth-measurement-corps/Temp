#define VNAME(x) #x
using namespace std;

class Node {
	double ID;
	double actualRoom;
	int adj1;
	int adj2;
	int adj3;
	int adj4;
	int room1;
	int room2;
	int room3;
	int room4;
	int room5;
	double adjd1;
	double adjd2;
	double adjd3;
	double adjd4;
public:
	Node();
	Node (double, int, int, double, int, double, int, double, int, double, int, int, int, int);
	double getRoomID();
	bool adjacent(Node);
	bool adjacent(int);
	double getDistance(double);
	double getID();
	int getadj1();
	int getadj2();
	int getadj3();
	int getadj4();
	int getroom1();
	int getroom2();
	int getroom3();
	int getroom4();
	int getroom5();
	double getadjd1();
	double getadjd2();
	double getadjd3();
	double getadjd4();
};

Node::Node()
{
}

Node::Node(double id, int ar, int i1, double a1, int i2, double a2, int i3, double a3, int i4, double a4, int i5, int i6, int i7, int i8)
{
	ID = id;
	actualRoom = ar;
	adj1 = i1;
	adj2 = i2;
	adj3 = i3;
	adj4 = i4;
	room1 = i5;
	room2 = i6;
	room3 = i7;
	room4 = i8;
	adjd1 = a1;
	adjd2 = a2;
	adjd3 = a3;
	adjd4 = a4;
	
}

double Node::getID()
{
	return actualRoom;
}
double Node::getRoomID()
{
	return ID;
}
int Node::getadj1()
{
	return adj1;
}
int Node::getadj2()
{
	return adj2;
}
int Node::getadj3()
{
	return adj3;
}
int Node::getadj4()
{
	return adj4;
}
int Node::getroom1()
{
	return room1;
}
int Node::getroom2()
{
	return room2;
}
int Node::getroom3()
{
	return room3;
}
int Node::getroom4()
{
	return room4;
}
int Node::getroom5()
{
	return room5;
}
double Node::getadjd1()
{
	return adjd1;
}
double Node::getadjd2()
{
	return adjd2;
}
double Node::getadjd3()
{
	return adjd3;
}
double Node::getadjd4()
{
	return adjd4;
}
bool Node::adjacent(Node input)
{
	string name = VNAME(input);
	name = name.substr(1, 5);
	int nameint = stoi(name);
	if (nameint == adj1 || nameint == adj2 || nameint == adj3 || nameint == adj4 || nameint == room1 || nameint == room2 || nameint == room3 || nameint == room4 || nameint == room5)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Node::adjacent(int input)
{
	string name = VNAME(input);
	name = name.substr(1, 5);
	int nameint = stoi(name);
	if (nameint == adj1 || nameint == adj2 || nameint == adj3 || nameint == adj4 || nameint == room1 || nameint == room2 || nameint == room3 || nameint == room4 || nameint == room5)
	{
		return true;
	}
	else
	{
		return false;
	}
}

double Node::getDistance(double input)
{
	double output = 0;
	if (input == adj1)
	{
		output = adjd1;
	}
	else if (input == adj2)
	{
		output = adjd2;
	}
	else if (input == adj3)
	{
		output = adjd3;
	}
	else if (input == adj4)
	{
		output = adjd4;
	}
	else
	{
		/*cout << "ERROR!!!" << endl;
		cout << "OUTPUT" << input << endl;
		cout << "FOR THIS NOD:" << actualRoom << ID << endl;*/
	}
	return output;
}

//adjacent node, getter functions
//use the +-1 scheme, rooms use seperate scheme, if its +-1 adjacent, if its an adjacency it is adjacent, if not then check adjancies of adjacent, do if statement that also breaks everything.