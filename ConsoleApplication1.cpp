#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>  
#include <sstream>
#include <map>  
#include <thread>
#include <chrono>
#include "CSVRow.h"
#include "Definements.h"
#include "Node.h"
#include "Reverse.h"
#include "smart_ptr.h"
#include "targetver.h"
#include <Windows.h> 
#include <Wininet.h>
#include <filesystem>
#include <experimental/filesystem>
#include <tchar.h>
#include <urlmon.h>
#include <ctime>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")

#define VNAME(x) #x


using namespace std;

/*
The primary function of this algorithm is to input badge ID information taken from the Johns Hopkins University hospital and subsequently
detect and analyze ambulation data of patients. It inputs a csv file with parameters of badge ID, time, room number, room name, and date and
utilizes stored values of distance to calculate and output a csv file with the following information for each ambulation on individual rows:

Output by column:
1:Badge ID
2:Room Number
3:Date
4:Ambulation Number
5:Distance
6:Start Time
7:Duration
8:Average Speed
9:Last Updated (of each individual ambulation period)

The algorithm also considers several anomalies and appropriately adjusts for them, which include the misidentification of badge sensors,
the missing of a sensor signal in a sequential sequence, and the rapid triggering between two closeby sensors.

In this program, an ambulation is defined as an out-of-room sequence in which the patient triggers six or more sensors (adjusted for anomalies)
outside of the patient room.
Segments
*/

//Function to sort dates
bool sortcol(const vector<string>& v1,
	const vector<string>& v2) {
	return (v1[2] < v2[2]);
}

//Function that downloads new MRN data from the website to IData.csv file in the directory
void update()
{
}

//Main function - essentially runs the ambulation calculator on 1 file
vector<vector<string>> analyzeData(string fileName, vector<vector<string>> inputInfo, vector<vector<int>> &ambuData, vector<vector<string>> &segData)
{
	
	//Node information - Room number/ID, and list of adjacent rooms with their distances following after
Node n14354(0,14354,14449,18,14312,22,0,0,0,0,0,0,14329,14330,14331,0,0);
Node n14449(1,14449,14362,38,14354,18,0,0,0,0,0,0,14331,14332,0,0,0);
Node n14362(2,14362,14361,29.3,14354,38,0,0,0,0,0,0,14332,14333,14334,14335,0);
Node n14361(3,14361,14355,58,14362,29.3,0,0,0,0,0,0,14335,14336,14337,14338,14339);
Node n14355(4,14355,14359,27.8,14361,58,14317,26,0,0,0,0,14338,14339,14340,14341,0);
Node n14359(5,14359,14360,16.1,14355,27.8,0,0,0,0,0,0,14341,14342,14343,0,0);
Node n14360(6,14360,14318,30,14359,16.1,0,0,0,0,0,0,14343,14344,14345,0,0);
Node n14318(7,14318,14309,53.3,14360,30,14316,42.4,0,0,0,0,14345,14346,14347,14348,0);
Node n14309(8,14309,14302,32,14318,53.3,14316,52,14437,10.5,0,0,14347,14348,14349,14326,14327);
Node n14302(9,14302,14300,19,14309,32,0,0,0,0,0,0,14349,14326,14327,14328,0);
Node n14300(10,14300,14324,33.9,14302,19,14278,33.4,0,0,0,0,14325,0,0,0,0);
Node n14324(11,14324,14295,32.9,14300,33.9,14278,30.6,0,0,0,0,14325,14323,14322,0,0);
Node n14295(12,14295,14229,35,14324,32.9,0,0,0,0,0,0,14322,14296,14228,0,0);
Node n14229(13,14229,14230,26,14295,35,14199,33.4,0,0,0,0,14228,14227,14226,0,0);
Node n14230(14,14230,14231,28,14229,26,14199,37.3,0,0,0,0,14226,14225,14224,0,0);
Node n14231(15,14231,14216,18,14230,28,0,0,0,0,0,0,14224,14223,14222,0,0);
Node n14216(16,14216,14223,16,14231,18,0,0,0,0,0,0,14222,14221,0,0,0);
Node n14233(17,14233,14209,21,14216,16,0,0,0,0,0,0,14220,14219,14218,14217,0);
Node n14209(18,14209,14206,22.4,14223,21,0,0,0,0,0,0,14218,14217,0,0,0);
Node n14206(19,14206,14198,48.4,14209,22.4,0,0,0,0,0,0,0,0,0,0,0);
Node n14198(20,14198,14199,15.2,14206,48.4,0,0,0,0,0,0,0,0,0,0,0);
Node n14199(21,14199,14298,26.7,14198,15.2,14229,33.4,14230,37.3,0,0,0,0,0,0,0);
Node n14298(22,14298,14299,48.5,14199,26.7,0,0,0,0,0,0,0,0,0,0,0);
Node n14299(23,14299,14278,12.2,14298,48.5,0,0,0,0,0,0,0,0,0,0,0);
Node n14278(24,14278,14308,35,14299,12.2,14277,38,14300,33.4,14324,30.6,0,0,0,0,0);
Node n14308(25,14308,14321,40,14278,35,0,0,0,0,0,0,0,0,0,0,0);
Node n14321(26,14321,14316,27.1,14308,40,0,0,0,0,0,0,0,0,0,0,0);
Node n14316(27,14316,14313,34.2,14321,27.1,14318,42.4,14309,52,14317,32,0,0,0,0,0);
Node n14313(28,14313,14314,16,14316,34.2,0,0,0,0,0,0,0,0,0,0,0);
Node n14314(29,14314,14312,20,14313,16,14311,32.8,0,0,0,0,0,0,0,0,0);
Node n14312(30,14312,14354,22,14314,20,14311,34.5,0,0,0,0,0,0,0,0,0);
Node n14317(4.5,14317,14355,26,14316,32,0,0,0,0,0,0,0,0,0,0,0);
Node n14277(24,14277,14278,38,0,0,0,0,0,0,0,0,0,0,0,0,0);
Node n14311(29.5,14311,14312,34.5,14314,32.8,0,0,0,0,0,0,0,0,0,0,0);
Node n14437(8,14437,14302,10.5,0,0,0,0,0,0,0,0,0,0,0,0,0);
Node n14329(0,14329,0,0,0,0,0,0,0,0,0,0,14354,0,0,0,0);
Node n14330(0,14330,0,0,0,0,0,0,0,0,0,0,14354,0,0,0,0);
Node n14331(0.5,14331,0,0,0,0,0,0,0,0,0,0,14354,14449,0,0,0);
Node n14332(1.5,14332,0,0,0,0,0,0,0,0,0,0,14449,14362,0,0,0);
Node n14333(2,14333,0,0,0,0,0,0,0,0,0,0,14362,0,0,0,0);
Node n14334(2,14334,0,0,0,0,0,0,0,0,0,0,14362,0,0,0,0);
Node n14335(2.5,14335,0,0,0,0,0,0,0,0,0,0,14362,14361,0,0,0);
Node n14336(3,14336,0,0,0,0,0,0,0,0,0,0,14361,0,0,0,0);
Node n14337(3,14337,0,0,0,0,0,0,0,0,0,0,14361,0,0,0,0);
Node n14338(3.5,14338,0,0,0,0,0,0,0,0,0,0,14361,14355,0,0,0);
Node n14339(3.5,14339,0,0,0,0,0,0,0,0,0,0,14361,14355,0,0,0);
Node n14340(4,14340,0,0,0,0,0,0,0,0,0,0,14355,0,0,0,0);
Node n14341(4.5,14341,0,0,0,0,0,0,0,0,0,0,14355,14359,0,0,0);
Node n14342(5,14342,0,0,0,0,0,0,0,0,0,0,14359,0,0,0,0);
Node n14343(5.5,14343,0,0,0,0,0,0,0,0,0,0,14359,14360,0,0,0);
Node n14344(6,14344,0,0,0,0,0,0,0,0,0,0,14360,0,0,0,0);
Node n14345(6.5,14345,0,0,0,0,0,0,0,0,0,0,14360,14318,0,0,0);
Node n14346(7,14346,0,0,0,0,0,0,0,0,0,0,14318,0,0,0,0);
Node n14347(7.5,14347,0,0,0,0,0,0,0,0,0,0,14318,14309,14316,0,0);
Node n14348(7.5,14348,0,0,0,0,0,0,0,0,0,0,14318,14309,14316,0,0);
Node n14349(8.5,14349,0,0,0,0,0,0,0,0,0,0,14309,14302,0,0,0);
Node n14326(8.5,14326,0,0,0,0,0,0,0,0,0,0,14309,14302,0,0,0);
Node n14327(8.5,14327,0,0,0,0,0,0,0,0,0,0,14309,14302,0,0,0);
Node n14328(9,14328,0,0,0,0,0,0,0,0,0,0,14302,0,0,0,0);
Node n14325(10.5,14325,0,0,0,0,0,0,0,0,0,0,14300,14324,14278,0,0);
Node n14323(11,14323,0,0,0,0,0,0,0,0,0,0,14324,0,0,0,0);
Node n14322(11.5,14322,0,0,0,0,0,0,0,0,0,0,14324,14295,0,0,0);
Node n14296(12,14296,0,0,0,0,0,0,0,0,0,0,14295,0,0,0,0);
Node n14228(12.5,14228,0,0,0,0,0,0,0,0,0,0,14295,14229,14199,0,0);
Node n14227(13,14227,0,0,0,0,0,0,0,0,0,0,14229,0,0,0,0);
Node n14226(13.5,14226,0,0,0,0,0,0,0,0,0,0,14229,14230,0,0,0);
Node n14225(14,14225,0,0,0,0,0,0,0,0,0,0,14230,0,0,0,0);
Node n14224(14.5,14224,0,0,0,0,0,0,0,0,0,0,14230,14231,0,0,0);
Node n14223(15,14223,0,0,0,0,0,0,0,0,0,0,14231,0,0,0,0);
Node n14222(15.5,14222,0,0,0,0,0,0,0,0,0,0,14231,14216,0,0,0);
Node n14221(16,14221,0,0,0,0,0,0,0,0,0,0,14216,0,0,0,0);
Node n14220(17,14220,0,0,0,0,0,0,0,0,0,0,14223,0,0,0,0);
Node n14219(17,14219,0,0,0,0,0,0,0,0,0,0,14223,0,0,0,0);
Node n14218(17.5,14218,0,0,0,0,0,0,0,0,0,0,14223,0,0,0,0);
Node n14217(17.5,14217,0,0,0,0,0,0,0,0,0,0,14223,0,0,0,0);

	map<double, Node> funcMap =
	{
{0,n14354},
{1,n14449},
{2,n14362},
{3,n14361},
{4,n14355},
{5,n14359},
{6,n14360},
{7,n14318},
{8,n14309},
{9,n14302},
{10,n14300},
{11,n14324},
{12,n14295},
{13,n14229},
{14,n14230},
{15,n14231},
{16,n14216},
{17,n14223},
{18,n14209},
{19,n14206},
{20,n14198},
{21,n14199},
{22,n14298},
{23,n14299},
{24,n14278},
{25,n14308},
{26,n14321},
{27,n14316},
{28,n14313},
{29,n14314},
{30,n14312},
{4.5,n14317},
{24.5,n14277},
{29.5,n14311},
{7.5,n14437},
{0,n14329},
{0,n14330},
{0.5,n14331},
{1.5,n14332},
{2,n14333},
{2,n14334},
{2.5,n14335},
{3,n14336},
{3,n14337},
{3.5,n14338},
{3.5,n14339},
{4,n14340},
{4.5,n14341},
{5,n14342},
{5.5,n14343},
{6,n14344},
{6.5,n14345},
{7,n14346},
{7.5,n14347},
{7.5,n14348},
{8.5,n14349},
{8.5,n14326},
{8.5,n14327},
{9,n14328},
{10.5,n14325},
{11,n14323},
{11.5,n14322},
{12,n14296},
{12.5,n14228},
{13,n14227},
{13.5,n14226},
{14,n14225},
{14.5,n14224},
{15,n14223},
{15.5,n14222},
{16,n14221},
{17,n14220},
{17,n14219},
{17.5,n14218},
{17.5,n14217},
{31,n14354},
{32,n14449},
{33,n14362},
{34,n14361},
{-3,n14313},
{-2,n14314},
{-1,n14312}

	};
	map<int, int> funcMap2 =
	{
{14329,1},
{14330,2},
{14331,3},
{14332,4},
{14333,5},
{14334,6},
{14335,7},
{14336,8},
{14337,9},
{14338,10},
{14339,11},
{14340,12},
{14341,13},
{14342,14},
{14343,15},
{14344,16},
{14345,17},
{14346,18},
{14347,19},
{14348,20},
{14349,21},
{14326,22},
{14327,23},
{14328,24},
{14325,25},
{14323,26},
{14322,27},
{14296,28},
{14228,29},
{14227,30},
{14226,31},
{14225,32},
{14224,33},
{14223,34},
{14222,35},
{14221,36},
{14220,37},
{14219,38},
{14218,39},
{14217,40}

	};
	int validSensorID[33] = { 
		14362,
14361,
14355,
14359,
14360,
14318,
14309,
14302,
14300,
14324,
14295,
14229,
14230,
14231,
14216,
14223,
14209,
14206,
14198,
14199,
14298,
14299,
14278,
14308,
14321,
14316,
14313,
14314,
14312,
14354,
14317,
14277,
14311

	};
	cout << "HI2"; //hhlhflehlfhlh

	const int numRows = 1000;
	const int numCols = 12;
	int numAmbulation = 0;

	//Input file
	ifstream input(fileName);
	CSVRow row;
	string inputData[numRows][numCols];
	int count = 0;
	while (input >> row)
	{
		for (int i = 0; i < numCols; i++)
		{
			inputData[count][i] = row[i];
		}
		count++;
	}

	int parsedRows = 0;
	for (int i = 0; i < numRows; ++i)
	{
		if (inputData[i][0].empty())
		{
			break;
		}
		parsedRows++;
	}

	int time[numRows];
	int endTime2[numRows];
	int sensorID[numRows];
	string hours, minutes, seconds;
	for (int i = 0; i < parsedRows; ++i)
	{
		hours = inputData[i][7].substr(11, 2);
		minutes = inputData[i][7].substr(14, 2);
		seconds = inputData[i][7].substr(17, 2);
		time[i] = atoi(hours.c_str()) * 3600 + atoi(minutes.c_str()) * 60 + atoi(seconds.c_str());
		hours = inputData[i][8].substr(11, 2);
		minutes = inputData[i][8].substr(14, 2);
		seconds = inputData[i][8].substr(17, 2);
		endTime2[i] = atoi(hours.c_str()) * 3600 + atoi(minutes.c_str()) * 60 + atoi(seconds.c_str());
		sensorID[i] = atoi(inputData[i][5].c_str());
	}


	//function find room number
	int r14329 = 0,
r14330 = 0,
r14331 = 0,
r14332 = 0,
r14333 = 0,
r14334 = 0,
r14335 = 0,
r14336 = 0,
r14337 = 0,
r14338 = 0,
r14339 = 0,
r14340 = 0,
r14341 = 0,
r14342 = 0,
r14343 = 0,
r14344 = 0,
r14345 = 0,
r14346 = 0,
r14347 = 0,
r14348 = 0,
r14349 = 0,
r14326 = 0,
r14327 = 0,
r14328 = 0,
r14325 = 0,
r14323 = 0,
r14322 = 0,
r14296 = 0,
r14228 = 0,
r14227 = 0,
r14226 = 0,
r14225 = 0,
r14224 = 0,
r14223 = 0,
r14222 = 0,
r14221 = 0,
r14220 = 0,
r14219 = 0,
r14218 = 0,
r14217 = 0;
	for (int i = 0; i < parsedRows; ++i)
	{
		switch (sensorID[i])
		{
case 14329 : r14329++;
	break;
case 14330 : r14330++;break;
case 14331 : r14331++;break;
case 14332 : r14332++;break;
case 14333 : r14333++;break;
case 14334 : r14334++;break;
case 14335 : r14335++;break;
case 14336 : r14336++;break;
case 14337 : r14337++;break;
case 14338 : r14338++;break;
case 14339 : r14339++;break;
case 14340 : r14340++;break;
case 14341 : r14341++;break;
case 14342 : r14342++;break;
case 14343 : r14343++;break;
case 14344 : r14344++;break;
case 14345 : r14345++;break;
case 14346 : r14346++;break;
case 14347 : r14347++;break;
case 14348 : r14348++;break;
case 14349 : r14349++;break;
case 14326 : r14326++;break;
case 14327 : r14327++;break;
case 14328 : r14328++;break;
case 14325 : r14325++;break;
case 14323 : r14323++;break;
case 14322 : r14322++;break;
case 14296 : r14296++;break;
case 14228 : r14228++;break;
case 14227 : r14227++;break;
case 14226 : r14226++;break;
case 14225 : r14225++;break;
case 14224 : r14224++;break;
case 14223 : r14223++;break;
case 14222 : r14222++;break;
case 14221 : r14221++;break;
case 14220 : r14220++;
	break;
case 14219 : r14219++;
	break;
case 14218 : r14218++;
	break;
case 14217 : r14217++;
	break;

		}
	}

	cout << "HI"; //hhlhflehlfhlh

	int tempSensor = 0;
	int roomNumber;
	//Finds patient room number
if (r14329 > tempSensor) { roomNumber = 14329; tempSensor = r14329; }
if (r14330 > tempSensor) { roomNumber = 14330; tempSensor = r14330; }
if (r14331 > tempSensor) { roomNumber = 14331; tempSensor = r14331; }
if (r14332 > tempSensor) { roomNumber = 14332; tempSensor = r14332; }
if (r14333 > tempSensor) { roomNumber = 14333; tempSensor = r14333; }
if (r14334 > tempSensor) { roomNumber = 14334; tempSensor = r14334; }
if (r14335 > tempSensor) { roomNumber = 14335; tempSensor = r14335; }
if (r14336 > tempSensor) { roomNumber = 14336; tempSensor = r14336; }
if (r14337 > tempSensor) { roomNumber = 14337; tempSensor = r14337; }
if (r14338 > tempSensor) { roomNumber = 14338; tempSensor = r14338; }
if (r14339 > tempSensor) { roomNumber = 14339; tempSensor = r14339; }
if (r14340 > tempSensor) { roomNumber = 14340; tempSensor = r14340; }
if (r14341 > tempSensor) { roomNumber = 14341; tempSensor = r14341; }
if (r14342 > tempSensor) { roomNumber = 14342; tempSensor = r14342; }
if (r14343 > tempSensor) { roomNumber = 14343; tempSensor = r14343; }
if (r14344 > tempSensor) { roomNumber = 14344; tempSensor = r14344; }
if (r14345 > tempSensor) { roomNumber = 14345; tempSensor = r14345; }
if (r14346 > tempSensor) { roomNumber = 14346; tempSensor = r14346; }
if (r14347 > tempSensor) { roomNumber = 14347; tempSensor = r14347; }
if (r14348 > tempSensor) { roomNumber = 14348; tempSensor = r14348; }
if (r14349 > tempSensor) { roomNumber = 14349; tempSensor = r14349; }
if (r14326 > tempSensor) { roomNumber = 14326; tempSensor = r14326; }
if (r14327 > tempSensor) { roomNumber = 14327; tempSensor = r14327; }
if (r14328 > tempSensor) { roomNumber = 14328; tempSensor = r14328; }
if (r14325 > tempSensor) { roomNumber = 14325; tempSensor = r14325; }
if (r14323 > tempSensor) { roomNumber = 14323; tempSensor = r14323; }
if (r14322 > tempSensor) { roomNumber = 14322; tempSensor = r14322; }
if (r14296 > tempSensor) { roomNumber = 14296; tempSensor = r14296; }
if (r14228 > tempSensor) { roomNumber = 14228; tempSensor = r14228; }
if (r14227 > tempSensor) { roomNumber = 14227; tempSensor = r14227; }
if (r14226 > tempSensor) { roomNumber = 14226; tempSensor = r14226; }
if (r14225 > tempSensor) { roomNumber = 14225; tempSensor = r14225; }
if (r14224 > tempSensor) { roomNumber = 14224; tempSensor = r14224; }
if (r14223 > tempSensor) { roomNumber = 14223; tempSensor = r14223; }
if (r14222 > tempSensor) { roomNumber = 14222; tempSensor = r14222; }
if (r14221 > tempSensor) { roomNumber = 14221; tempSensor = r14221; }
if (r14220 > tempSensor) { roomNumber = 14220; tempSensor = r14220; }
if (r14219 > tempSensor) { roomNumber = 14219; tempSensor = r14219; }
if (r14218 > tempSensor) { roomNumber = 14218; tempSensor = r14218; }
if (r14217 > tempSensor) { roomNumber = 14217; tempSensor = r14217; }



	int inRoom2[numRows];
	int timeCount2 = 0;
	inRoom2[0] = 0;
	timeCount2++;
	for (int i = 0; i < parsedRows; ++i)
	{
		if (roomNumber == sensorID[i])
		{
			inRoom2[timeCount2] = i;
			timeCount2++;
		}
	}

	//If the path of an ambulation goes off the floor, ambulation is not recored. This checks for that
	bool dont;
	bool checkArray[numRows];
	for (int i = 0; i < timeCount2; i++)
	{
		dont = false;
		for (int j = inRoom2[i]; j < inRoom2[i + 1]; ++j)
		{
			if (sensorID[j] != 14354 &&
sensorID[j] != 14449 &&
sensorID[j] != 14362 &&
sensorID[j] != 14361 &&
sensorID[j] != 14355 &&
sensorID[j] != 14359 &&
sensorID[j] != 14360 &&
sensorID[j] != 14318 &&
sensorID[j] != 14309 &&
sensorID[j] != 14302 &&
sensorID[j] != 14300 &&
sensorID[j] != 14324 &&
sensorID[j] != 14295 &&
sensorID[j] != 14229 &&
sensorID[j] != 14230 &&
sensorID[j] != 14231 &&
sensorID[j] != 14216 &&
sensorID[j] != 14223 &&
sensorID[j] != 14209 &&
sensorID[j] != 14206 &&
sensorID[j] != 14198 &&
sensorID[j] != 14199 &&
sensorID[j] != 14298 &&
sensorID[j] != 14299 &&
sensorID[j] != 14278 &&
sensorID[j] != 14308 &&
sensorID[j] != 14321 &&
sensorID[j] != 14316 &&
sensorID[j] != 14313 &&
sensorID[j] != 14314 &&
sensorID[j] != 14312 &&
sensorID[j] != 14317 &&
sensorID[j] != 14277 &&
sensorID[j] != 14311 &&
sensorID[j] != 14437 &&
sensorID[j] != 14329 &&
sensorID[j] != 14330 &&
sensorID[j] != 14331 &&
sensorID[j] != 14332 &&
sensorID[j] != 14333 &&
sensorID[j] != 14334 &&
sensorID[j] != 14335 &&
sensorID[j] != 14336 &&
sensorID[j] != 14337 &&
sensorID[j] != 14338 &&
sensorID[j] != 14339 &&
sensorID[j] != 14340 &&
sensorID[j] != 14341 &&
sensorID[j] != 14342 &&
sensorID[j] != 14343 &&
sensorID[j] != 14344 &&
sensorID[j] != 14345 &&
sensorID[j] != 14346 &&
sensorID[j] != 14347 &&
sensorID[j] != 14348 &&
sensorID[j] != 14349 &&
sensorID[j] != 14326 &&
sensorID[j] != 14327 &&
sensorID[j] != 14328 &&
sensorID[j] != 14325 &&
sensorID[j] != 14323 &&
sensorID[j] != 14322 &&
sensorID[j] != 14296 &&
sensorID[j] != 14228 &&
sensorID[j] != 14227 &&
sensorID[j] != 14226 &&
sensorID[j] != 14225 &&
sensorID[j] != 14224 &&
sensorID[j] != 14223 &&
sensorID[j] != 14222 &&
sensorID[j] != 14221 &&
sensorID[j] != 14220 &&
sensorID[j] != 14219 &&
sensorID[j] != 14218 &&
sensorID[j] != 14217

				)
			{
				dont = true;
				break;
			}
		}
		if (dont == true)
		{
			checkArray[i + 1] = 1;
		}
		else
		{
			checkArray[i + 1] = 0;
		}
	}

	//Checks for re-exit of room within 60 seconds, which extends ambulation instead of creating a new instance
	int inRoom[numRows];
	int timeCount = 0;
	inRoom[0] = 0;
	timeCount++;
	int timeCountRef = 1;
	bool daCheck;
	for (int i = 0; i < parsedRows; ++i)
	{
		if (roomNumber == sensorID[i])
		{
			daCheck = true;
			if (i != 0 && i < parsedRows - 1)
			{
				if (time[i + 1] - time[i] <= 60 && checkArray[timeCountRef] == 0)
				{
					daCheck = false;
				}
			}
			if (daCheck == true)
			{
				inRoom[timeCount] = i;
				timeCount++;
			}
			timeCountRef++;
		}
	}
	cout << "HIJI" << timeCount << "      " << roomNumber; //hhlhflehlfhlh

	//Main algorithm of the program. Reads in room IDs from the csv file and calculates distance, while considering for anomalies
	int OORStart = 0;
	int badgesHit;
	int tempSizeSeg;
	double tempMap, tempMap2;
	vector<int> ambulationStart;
	vector<int> ambulationEnd;
	vector<string> lmao;
	int total_anom = 0;
	double finalDistance[numRows];
	double finalDuration[numRows];
	int lastSensorID, oldSize;
	int temp1, temp2, temp3;
	double temp4, temp5, temp6;
	double tempValue, cumDistance, speedSecond, distanceLast, timeLast, firstDistance, firstTime, cTime, tempSpeedd;
	bool pass, hitSecond;
	vector<double> values, timez, valuez;
	vector<int> locations, locationz, ambCoor;
	vector<double> tempValues1, tempValues2, tempValues3, tempValues4, tempValues5, tempValues6, tempValues7, tempValues8, tempValues9, tempValues10, tempValues11, tempValues12, tempValues13, tempValues14, tempValues15;
	vector<double> locationsChange;
	bool isTrue = false;
	int rapgod, noLoop;
	int startTime, endTime;
	int humma = 0;
	string realID, oldID;
	realID = "";
	vector<string> segmentTemp;
	int initialSegSize;
	double thelastseg, thefirstseg;
	double segmentTemporaryDistance = 0;
	double segmentTemporaryTime = 0;
	for (int i = 0; i < timeCount; ++i)
	{
		tempSizeSeg = segData.size();
		thelastseg = 0;
		thefirstseg = 0;
		startTime = 0;
		endTime = 0;
		isTrue = false;
		lastSensorID = 0;
		badgesHit = 0;
		speedSecond = 0;
		distanceLast = 0;
		timeLast = 0;
		firstDistance = 0;
		firstTime = 0;
		tempValue = 0;
		noLoop = 0;
		cTime = 0;
		hitSecond = false;
		values.clear();
		valuez.clear();
		locations.clear();
		locationsChange.clear();
		timez.clear();
		oldSize = 0;
		cumDistance = -11;
		rapgod = -1;
		initialSegSize = segData.size();	//refreshed every time
		for (int j = inRoom[i]; j < inRoom[i + 1]; ++j)
		{
			if (sensorID[j] != 14354 &&
sensorID[j] != 14449 &&
sensorID[j] != 14362 &&
sensorID[j] != 14361 &&
sensorID[j] != 14355 &&
sensorID[j] != 14359 &&
sensorID[j] != 14360 &&
sensorID[j] != 14318 &&
sensorID[j] != 14309 &&
sensorID[j] != 14302 &&
sensorID[j] != 14300 &&
sensorID[j] != 14324 &&
sensorID[j] != 14295 &&
sensorID[j] != 14229 &&
sensorID[j] != 14230 &&
sensorID[j] != 14231 &&
sensorID[j] != 14216 &&
sensorID[j] != 14223 &&
sensorID[j] != 14209 &&
sensorID[j] != 14206 &&
sensorID[j] != 14198 &&
sensorID[j] != 14199 &&
sensorID[j] != 14298 &&
sensorID[j] != 14299 &&
sensorID[j] != 14278 &&
sensorID[j] != 14308 &&
sensorID[j] != 14321 &&
sensorID[j] != 14316 &&
sensorID[j] != 14313 &&
sensorID[j] != 14314 &&
sensorID[j] != 14312 &&
sensorID[j] != 14317 &&
sensorID[j] != 14277 &&
sensorID[j] != 14311 &&
sensorID[j] != 14437 &&
sensorID[j] != 14329 &&
sensorID[j] != 14330 &&
sensorID[j] != 14331 &&
sensorID[j] != 14332 &&
sensorID[j] != 14333 &&
sensorID[j] != 14334 &&
sensorID[j] != 14335 &&
sensorID[j] != 14336 &&
sensorID[j] != 14337 &&
sensorID[j] != 14338 &&
sensorID[j] != 14339 &&
sensorID[j] != 14340 &&
sensorID[j] != 14341 &&
sensorID[j] != 14342 &&
sensorID[j] != 14343 &&
sensorID[j] != 14344 &&
sensorID[j] != 14345 &&
sensorID[j] != 14346 &&
sensorID[j] != 14347 &&
sensorID[j] != 14348 &&
sensorID[j] != 14349 &&
sensorID[j] != 14326 &&
sensorID[j] != 14327 &&
sensorID[j] != 14328 &&
sensorID[j] != 14325 &&
sensorID[j] != 14323 &&
sensorID[j] != 14322 &&
sensorID[j] != 14296 &&
sensorID[j] != 14228 &&
sensorID[j] != 14227 &&
sensorID[j] != 14226 &&
sensorID[j] != 14225 &&
sensorID[j] != 14224 &&
sensorID[j] != 14223 &&
sensorID[j] != 14222 &&
sensorID[j] != 14221 &&
sensorID[j] != 14220 &&
sensorID[j] != 14219 &&
sensorID[j] != 14218 &&
sensorID[j] != 14217
				)
			{
				locations.clear();
				break;
			}
			pass = false;
			for (int k = 0; k < 33; k++)
			{
				if (validSensorID[k] == sensorID[j])
				{
					pass = true;
				}
			}
			//If everything is valid, puts the relative location and time for every badge sensor hit
			if (pass == true)
			{
				values.push_back((funcMap[sensorID[j]]).getRoomID());
				locations.push_back(j);
				timez.push_back(time[j]);
			}
		}
		if (locations.size() > 1)
		{
			humma += locations.size();
			//Calculates MRN number
			for (int j = inputInfo.size() - 1; j >= 0; j--)
			{
				if (inputInfo[j][0].empty())
				{
					break;
				}
				if (inputInfo[j][1] == inputData[inRoom[i]][0])
				{
					if (inputInfo[j][2].compare(inputData[inRoom[i]][7]) <= 0) //MRN
					{
						realID = inputInfo[j][0];
						break;
					}
				}
			}
			if (realID == "")
			{
				realID = "0";
			}
			//Erases duplicate values immediately after eachother
			cout << locations.size(); //hhlhflehlfhlh
			for (int j = 0; j < locations.size() - 1;)
			{
				if (values[j] == values[j + 1])
				{
					locations.erase(locations.begin() + j + 1);
					values.erase(values.begin() + j + 1);
				}
				else if (time[locations[j]] == time[locations[j + 1]])
				{
					if (time[locations[j]] == endTime2[locations[j]])
					{
						locations.erase(locations.begin() + j);
						values.erase(values.begin() + j);
					}
					else if (time[locations[j + 1]] == endTime2[locations[j + 1]])
					{
						locations.erase(locations.begin() + j + 1);
						values.erase(values.begin() + j + 1);
					}
				}
				else
				{
					j++;
				}
			}
			cout << locations.size(); //hhlhflehlfhlh
			for (int j = 0; j < locations.size() - 1;)
			{
				if (values[j] == values[j + 1])
				{
					total_anom++;
					locations.erase(locations.begin() + j + 1);
					values.erase(values.begin() + j + 1);
				}
				else
				{
					j++;
				}
			}
			cout << locations.size(); //hhlhflehlfhlh
			//Special adjustments for situations where badges have more than 1 "adjacent" node. Basically, adjusts the relative position value
			//based on which way the patient travels through the special locations (for instance, traveling left to right in the hallway in
			//the middle of the room
			for (int j = 0; j < locations.size();j++)
			{
				if (values[j] == 29.5)
				{
					if (j + 1 == locations.size())
					{
						if (values[j - 1] > 29.5)
						{
							locationsChange.push_back(j);
							locationsChange.push_back(-0.5);
						}
						else
						{
							locationsChange.push_back(j);
							locationsChange.push_back(0.5);
						}
					}
					else if (j == 0)
					{
						if (values[1] > 29.5)
						{
							locationsChange.push_back(0);
							locationsChange.push_back(-0.5);
							locationsChange.push_back(1);
							locationsChange.push_back(0.5);
						}
						else if (values[1] < 29.5)
						{
							locationsChange.push_back(0);
							locationsChange.push_back(0.5);
							locationsChange.push_back(1);
							locationsChange.push_back(-0.5);
						}
						//29.5->28 29.5->29
					}
					else
					{
						if ((values[j - 1] <= 28 && values[j + 1] <= 28))
						{
							locationsChange.push_back(j);
							locationsChange.push_back(0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-0.5);
						}
						else if ((values[j - 1] >= 29 && values[j + 1] >= 29))
						{
							locationsChange.push_back(j);
							locationsChange.push_back(-0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(0.5);
						}
						else if (values[j - 1] <= 28 && values[j + 1] >= 29)
						{
							locationsChange.push_back(j);
							locationsChange.push_back(0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(0.5);
						}
						else
						{
							locationsChange.push_back(j);
							locationsChange.push_back(-0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-0.5);

						}
					}
				}
				
				else if (values[j] == 3.5) //6, 7, 13, 14, begin, end
				{
					if (j + 1 == locations.size())
					{
						if (values[j - 1] < 3 || values[j - 1] == 4 || values[j - 1] == 5)
						{
							locationsChange.push_back(j);
							locationsChange.push_back(0.5);

						}
						else
						{
							locationsChange.push_back(j);
							locationsChange.push_back(23.5);
						}
					}
					else if (j == 0) //3.5
					{
						if ((values[j - 1] < 3 || values[j - 1] == 4 || values[j - 1] == 5) && values[j + 2] > values[j + 1])
						{
							locationsChange.push_back(j);
							locationsChange.push_back(-1.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(1.5);
						}
						else if ((values[j - 1] < 3 || values[j - 1] == 4 || values[j - 1] == 5) && values[j + 2] > values[j + 1])
						{
							locationsChange.push_back(j);
							locationsChange.push_back(-0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(0.5);
						}
						else if (values[j + 2] > values[j + 1])
						{
							locationsChange.push_back(j);
							locationsChange.push_back(21.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-21.5);
						}
						else
						{
							locationsChange.push_back(j);
							locationsChange.push_back(23.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-23.5);
						}
					}
					else
					{
						if ((values[j - 1] < 3 || values[j - 1] == 4 || values[j - 1] == 5) && (values[j + 1] < 3 || values[j + 1] == 4 || values[j + 1] == 5)) 
						{
							locationsChange.push_back(j);
							locationsChange.push_back(0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-0.5);
						}
						else if ((values[j - 1] < 3 || values[j - 1] == 4 || values[j - 1] == 5))
						{
							locationsChange.push_back(j);
							locationsChange.push_back(0.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-21.5);
						}
						else if ((values[j + 1] < 3 || values[j + 1] == 4 || values[j + 1] == 5))
						{
							locationsChange.push_back(j);
							locationsChange.push_back(23.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(1.5);
						}
						else
						{
							locationsChange.push_back(j);
							locationsChange.push_back(23.5);
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-23.5);
						}
						
					}
				}
				
				else if (values[j] == 6)
				{
				    if (j + 1 != locations.size())
					{
						if (values[j + 1] >= 24)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-19);

						}
					}
				}
				
				else if (values[j] == 7)
				{
				    if (j + 1 != locations.size())
					{
						if (values[j + 1] >= 24)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-20);

						}
					}
				}
			
				else if (values[j] == 26)
				{
					if (j + 1 != locations.size())
					{
						if (values[j + 1] <= 6)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(19);

						}
						else if (values[j + 1] == 7 || values[j + 1] == 8 || values[j + 1] == 7.5 || values[j + 1] == 9)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(20);

						}
					}
				}
				
				else if (values[j] == 9)
				{
				    if (j + 1 != locations.size())
					{
						if (values[j + 1] >= 20)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-13);

						}
					}
				}
				
				else if (values[j] == 10)
				{
				    if (j + 1 != locations.size())
					{
						if (values[j + 1] >= 20)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-14);

						}
					}
				}
			
				else if (values[j] == 23)
				{
					if (j + 1 != locations.size())
					{
						if (values[j + 1] <= 9)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(15);

						}
						else if (values[j + 1] == 10 || values[j + 1] == 11 || values[j + 1] == 12 || values[j + 1] == 13)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(14);

						}
					}
				}
				
								else if (values[j] == 12)
				{
				    if (j + 1 != locations.size())
					{
						if (values[j + 1] >= 17)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-7);

						}
					}
				}
				
				else if (values[j] == 13)
				{
				    if (j + 1 != locations.size())
					{
						if (values[j + 1] >= 17)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(-8);

						}
					}
				}
			
				else if (values[j] == 20)
				{
					if (j + 1 != locations.size())
					{
						if (values[j + 1] <= 12)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(7);

						}
						else if (values[j + 1] == 13 || values[j + 1] == 14 || values[j + 1] == 15)
						{
							locationsChange.push_back(j + 1);
							locationsChange.push_back(8);

						}
					}
				}
				
				
				
				
			}
			cout << "HaasfI!" << locationsChange.size(); //hhlhflehlfhlh

			//Adjusts to ensure values are within -9 to 9, so that subsequent algorithms based on relative position work
			for (int z = 0; z < locationsChange.size(); z += 2)
			{
				for (int m = locationsChange[z]; m < locations.size(); m++)
				{
					values[m] += locationsChange[z + 1];
				}
			}
			tempMap = funcMap[roomNumber].getRoomID();
			tempMap2 = values[0];
			if (tempMap < -16)
			{
				tempMap += 31;
			}
			if (tempMap > 16)
			{
				tempMap -= 31;
			}
			if (tempMap2 < -16)
			{
				tempMap2 += 31;
			}
			if (tempMap2 > 16)
			{
				tempMap2 -= 31;
			}
			if (abs(tempMap - tempMap2) <= 0.5)
			{
				tempValue = values[0];
			}
			else if (abs(tempMap - tempMap2) <= 1.5)
			{
				tempValue = values[0] + 1;
			}
			else if (abs(tempMap - tempMap2) <= 2.5)
			{
				tempValue = values[0] + 2;

			}
			else if (abs(tempMap - tempMap2) <= 3.5)
			{
				tempValue = values[0] + 3;
			}
		}
		cout << "HaasfI?" << locations.size(); //hhlhflehlfhlh
		for (int k = 0; k < locations.size();)
		{
			total_anom++;
			tempValues1.clear();
			tempValues2.clear();
			tempValues3.clear();
			tempValues4.clear();
			tempValues5.clear();
			tempValues6.clear();
			tempValues7.clear();
			tempValues8.clear();
			tempValues9.clear();
			tempValues10.clear();
			tempValues11.clear();
			tempValues12.clear();
			tempValues13.clear();
			tempValues14.clear();
			tempValues15.clear();
			//Adjusts all vaues of locations by the offset factor of the most recently calculated sensor location
			if (isTrue == true)
			{
				tempValue = values[k];
				locationsChange.push_back(k);
				locationsChange.push_back(-tempValue);
			}
			else
			{
				tempValue = funcMap[roomNumber].getRoomID();
				locationsChange.push_back(k);
				locationsChange.push_back(-tempValue);
			}
			for (int h = k; h < locations.size(); h++)
			{
				values[h] -= tempValue;
				if (values[h] < -16)
				{
					values[h] += 31;
				}
				if (values[h] < -16)
				{
					values[h] += 31;
				}
				if (values[h] < -16)
				{
					values[h] += 31;
				}
				if (values[h] > 16)
				{
					values[h] -= 31;
				}
				if (values[h] > 16)
				{
					values[h] -= 31;
				}
				if (values[h] > 16)
				{
					values[h] -= 31;
				}
			}
			//Creates vectors for size comparisons
			if (values.size() - k> 1)
			{
				for (int r = 0; r < 2; r++)
				{
					tempValues1.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 2)
			{

				for (int r = 0; r < 3; r++)
				{
					tempValues2.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 3)
			{
				for (int r = 0; r < 4; r++)
				{
					tempValues3.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 4)
			{
				for (int r = 0; r < 5; r++)
				{
					tempValues4.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 5)
			{
				for (int r = 0; r < 6; r++)
				{
					tempValues5.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 7)
			{
				for (int r = 0; r < 8; r++)
				{
					tempValues6.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 8)
			{
				for (int r = 0; r < 9; r++)
				{
					tempValues7.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 9)
			{
				for (int r = 0; r < 10; r++)
				{
					tempValues8.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 10)
			{
				for (int r = 0; r < 11; r++)
				{
					tempValues9.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 13)
			{
				for (int r = 0; r < 14; r++)
				{
					tempValues10.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 12)
			{
				for (int r = 0; r < 13; r++)
				{
					tempValues11.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 30)
			{
				for (int r = 0; r < 17; r++)
				{
					tempValues12.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 6)
			{
				for (int r = 0; r < 7; r++)
				{
					tempValues13.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 20)
			{
				for (int r = 0; r < 21; r++)
				{
					tempValues14.push_back(values[k + r]);
				}
			}
			if (values.size() - k> 22)
			{
				for (int r = 0; r < 23; r++)
				{
					tempValues15.push_back(values[k + r]);
				}
			}
			cout << "HaasfI4"; //hhlhflehlfhlh
			valuez = values;
			for (int z = 0; z < locationsChange.size(); z += 2)
			{
				for (int m = locationsChange[z]; m < locations.size(); m++)
				{
					valuez[m] -= locationsChange[z + 1];
				}
			}
			for (int z = 0; z < valuez.size(); z++)
			{
				while (valuez[z] > 30)
				{
					valuez[z] -= 31;
				}
				while (valuez[z] < 0)
				{
					valuez[z] += 31;
				}

			}

			if (values.size() - k> 2)
			{
				temp4 = valuez[k + 2];
			}
			else
			{
				temp4 = 0;
			}
			if (values.size() - k> 1)
			{
				temp5 = valuez[k + 1];
			}
			else
			{
				temp5 = 0;
			}
			if (values.size() - k> 3)
			{
				temp6 = valuez[k + 3];
			}
			else
			{
				temp6 = 0;
			}
			//Calculates distance and time based on position of subsequent relative locations. Most of these are anomalies, but this also
			//includes some normal progressions of sensor badges. There is special code for beginning and ending situations, but do not adjust them.
			//Formatting of adding an additional anomaly:
			/*
			else if ((tempValues5 == vector<double>{0, 1, 2, 1, 2, 3} || tempValues5 == vector<double>{0, -1, -2, -1, -2, -3}) && temp6 != 13.5)
			{
			if (k == 0)
			{
			startTime = time[locations[k]];
			firstTime = time[locations[k + 1]] - time[locations[k]];
			firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
			speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 1]]);
			}
			if (hitSecond == true)
			{
			speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
			hitSecond = false;
			}
			cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
			>> INSERT SEGMENT RECORDING PORTION HERE, SEE BELOW
			cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());
			>> INSERT SEGMENT RECORDING PORTION HERE, SEE BELOW
			cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID());
			>> INSERT SEGMENT RECORDING PORTION HERE, SEE BELOW
			distanceLast = (funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID()));
			timeLast = (time[locations[k + 5]] - time[locations[k + 4]]);
			k += 5;
			badgesHit += 3;
			}
			Check if tempValues(number sensors for the anomaly) == the anomaly pattern, where 0 is the initial badge, 1 is the next, and -1 is the opposite direction
			Do this for the negative opposite version of the sequence too, so || the top 2 choices

			If that tempValues(number sensors for the anomaly) doesn't exist, create it

			Now, create 2 subsections: if k == 0 and hitSecond == true
			if k == 0, set the startTime to time[locations[k]], the firstTime to the first valid badge sensor - firstTime and the
			first distance for the first proper segment. Then calcuclate the secondspeed (distance of second proper segment / time of second proper segment)

			for hitsecond == true
			calcuclate the secondspeed (distance of second proper segment / time of second proper segment)
			and set hitsecond to false.

			Then, for EACH segment that should be recored, add all the distances to the cumulative distance in this format:
			cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
			Where k is the initial value, so k + 1 is the second value, etc.
			Set distanceLast to ONLY the final distance segment that is added: do this for time as well

			Now, for each segment of information you must follow the below format to output it for the segment analysis IMMEDIATELY after cumulating some distance
			Repeat (copy/paste) this code and adjust it for EVERY segment. So if at the end if there is  badgesHit +=3, there should be 3 of these.

			segmentTemporaryDistance = cumDistance;
			segmentTemporaryTime = cTime;
			cumDistance
			segmentTemp.push_back(inputData[0][7].substr(0,10));					//Date
			segmentTemp.push_back(inputData[0][0]);					//Badge ID
			segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));	//STARTING SENSOR ID <- MUST CHANGE
			segmentTemp.push_back(to_string(numAmbulation + 1));		//Ambulation #
			cTime += time[locations[k + 1]] - time[locations[k]];   //ADD SEGMENT DURATION <- MUST CHANGE
			segmentTemp.push_back(to_string(cTime));								//Cumulative time (sec) (calculate time)
			segmentTemp.push_back(to_string(cumDistance));								//Cumulative distance (ft) (calculate distance)
			if (cTime - segmentTemporaryTime != 0)
			{
			tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
			segmentTemp.push_back(to_string(tempSpeedd));
			}
			else
			{
			segmentTemp.push_back("N/A");
			}
			segmentTemp.push_back(realID);

			segData.push_back(segmentTemp);
			segmentTemp.clear();



			Then, for each the k badges that were scanned, cumulate that value to k (k += 5 if the anomaly has 5 sensors in the sequence)
			Finally, for each SEGMENT that was recorded (basically, each time something was added to cumDistance, add that value to badgesHit (badgesHit+=3)
			*/

			if (tempValues3 == vector<double>{0, -1, 1, 2} || tempValues3 == vector<double>{0, 1, -1, -2})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 2]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 2]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID()));
				timeLast = (time[locations[k + 3]] - time[locations[k + 2]]);
				//adjust k, k+1, k+2, k+3 to their actual values using a formula
				//then funcMap k, take in k+1, get distance
				//cumulate distance to cumDistance
				k += 3;
				badgesHit += 2;
			}
			else if (tempValues3 == vector<double>{0, -1, 0, 2} || tempValues3 == vector<double>{0, 1, 0, -2})
			{
				if (values[k + 3] == 2)
				{
					temp1 = valuez[k + 3] - 1;
				}
				else if (values[k + 3] == -2)
				{
					temp1 = valuez[k + 3] + 1;
				}
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[valuez[k]].getID());
				if (k == 0)
				{
					startTime = time[locations[k + 2]];
					firstTime = time[locations[k + 3]] - time[locations[k + 2]];
					firstDistance += (funcMap[valuez[k + 2]].getDistance(funcMap[temp1].getID()));
					firstDistance += (funcMap[temp1].getDistance(funcMap[valuez[k + 3]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[temp1].getID()) + funcMap[valuez[k + 3]].getDistance(funcMap[temp1].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += (funcMap[valuez[k + 2]].getDistance(funcMap[temp1].getID()));
				cumDistance += (funcMap[temp1].getDistance(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 0 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID()));
				timeLast = (time[locations[k + 3]] - time[locations[k + 2]]);
				//adjust k, k+1, k+2, k+3 to their actual values using a formula
				//then funcMap k, take in k+1, get distance
				//cumulate distance to cumDistance
				k += 3;
				badgesHit += 2;
			}
			else if ((tempValues4 == vector<double>{0, -1, 0, 1, 2 } || tempValues4 == vector<double>{0, 1, 0, -1, -2}) && temp4 != 13.5 && k == 0)
			{
				if (k == 0)
				{
					startTime = time[locations[k + 2]];
					firstTime = time[locations[k + 3]] - time[locations[k + 2]];
					firstDistance += (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID()));
					speedSecond = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 3]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 0 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 3]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 0 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 4]] - time[locations[k + 3]]);
				k += 4;
				badgesHit += 2;
			}
			else if ((tempValues6 == vector<double>{0, 1, 0, 1, 2, 1, 2, 3} || tempValues6 == vector<double>{0, -1, 0, -1, -2, -1, -2, -3}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID())) / (time[locations[k + 6]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 6]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 6]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 6]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 7]] - time[locations[k + 6]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 6]].getDistance(funcMap[valuez[k + 7]].getID()));
				timeLast = (time[locations[k + 7]] - time[locations[k + 6]]);
				k += 7;
				badgesHit += 3;
			}
			else if ((tempValues11 == vector<double>{0, 1, 2, 3, 2, 3, 2, 3, 2, 1, 2, 1, 0} || tempValues11 == vector<double>{0, -1, -2, -3, -2, -3, -2, -3, -2, -1, -2, -1, 0}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 7]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 8]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 7]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 8]] - time[locations[k + 7]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 8]].getDistance(funcMap[valuez[k + 11]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 8]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 11]] - time[locations[k + 8]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 11]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 11]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 12]] - time[locations[k + 11]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 11]].getID()));
				timeLast = (time[locations[k + 12]] - time[locations[k + 11]]);
				k += 12;
				badgesHit += 6;
			}
			else if ((tempValues14 == vector<double>{0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0} || tempValues14 == vector<double>{0, -1, -2, -3, -2, -3, -2, -3, -2, -3, -2, -1, -2, -1, -2, -1, -2, -1, -2, -1, 0}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 3 2 1 2 1 2 1 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 3 2 1 2 1 2 1 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 7]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 3 2 1 2 1 2 1 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 10]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 7]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 10]] - time[locations[k + 7]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 3 2 1 2 1 2 1 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 10]].getDistance(funcMap[valuez[k + 19]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 10]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 19]] - time[locations[k + 10]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 3 2 1 2 1 2 1 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 19]].getDistance(funcMap[valuez[k + 20]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 19]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 20]] - time[locations[k + 19]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 2 3 2 3 2 1 2 1 2 1 2 1 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 20]].getDistance(funcMap[valuez[k + 19]].getID()));
				timeLast = (time[locations[k + 20]] - time[locations[k + 19]]);
				k += 20;
				badgesHit += 6;
			}
			else if ((tempValues12 == vector<double>{0, 1, 2, 1, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 1, 0} || tempValues12 == vector<double>{0, -1, -2, -1, -2, -3, -2, -3, -2, -3, -2, -3, -2, -3, -2, -1, 0}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 2 3 2 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 2 3 2 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 2 3 2 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 5]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 12]] - time[locations[k + 5]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 2 3 2 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 15]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 12]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 15]] - time[locations[k + 12]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 2 3 2 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 16]].getDistance(funcMap[valuez[k + 15]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 15]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 16]] - time[locations[k + 15]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 2 3 2 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 16]].getDistance(funcMap[valuez[k + 15]].getID()));
				timeLast = (time[locations[k + 16]] - time[locations[k + 15]]);
				k += 16;
				badgesHit += 6;
			}
			else if ((tempValues4 == vector<double>{0, 1, 0, 1, 2 } || tempValues4 == vector<double>{0, -1, 0, -1, -2}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k + 2]];
					firstTime = time[locations[k + 3]] - time[locations[k + 2]];
					firstDistance += (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID()));
					speedSecond = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 3]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 3]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 4]] - time[locations[k + 3]]);
				k += 4;
				badgesHit += 2;
			}
			else if ((tempValues5 == vector<double>{0, 1, 2, 0, 1, 0 } || tempValues5 == vector<double>{0, -1, -2, 0, -1, 0}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 0 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 0 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 0 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 0 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 5]] - time[locations[k + 4]]);
				k += 5;
				badgesHit += 4;
			}
			else if ((tempValues4 == vector<double>{0, 1, 0, 1, 0 } || tempValues4 == vector<double>{0, -1, 0, -1, 0}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 3]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 3]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 3]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 0 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 4]] - time[locations[k + 3]]);
				k += 4;
				badgesHit += 2;
			}
			else if ((tempValues5 == vector<double>{0, -1, -2, -3, -1, 0 } && temp4 == 2.5))
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 -2 -3 -1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 -2 -3 -1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 -2 -3 -1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -1 -2 -3 -1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 5]] - time[locations[k + 4]]);
				k += 5;
				badgesHit += 4;
			}
			else if (k == 0 && temp5 != 13.5 && temp4 != 13.5 && (tempValues3 == vector<double>{1, 0, 1, 2} || tempValues3 == vector<double>{-1, 0, -1, -2}))
			{
				if (k == 0)
				{
					startTime = time[locations[k + 1]];
					firstTime = time[locations[k + 2]] - time[locations[k + 1]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("1 0 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("1 0 1 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID()));
				timeLast = (time[locations[k + 3]] - time[locations[k + 2]]);
				k += 3;
				badgesHit += 2;
			}
			else if (tempValues1 == vector<double>{1, 2})
			{
				temp1 = valuez[k] - 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[valuez[k]].getID());
				cumDistance += firstDistance;
				badgesHit += 1;
			}
			else if (tempValues1 == vector<double>{-1, -2})
			{
				temp1 = valuez[k] + 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[valuez[k]].getID());
				cumDistance += firstDistance;
				badgesHit += 1;
			}
			else if (tempValues1 == vector<double>{2, 3})
			{
				temp1 = valuez[k] - 2;
				temp2 = valuez[k] - 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				firstDistance += funcMap[temp2].getDistance(funcMap[valuez[k]].getID());
				cumDistance += firstDistance;
				badgesHit += 2;
			}
			else if (tempValues1 == vector<double>{-2, -3})
			{
				temp1 = valuez[k] + 2;
				temp2 = valuez[k] + 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				firstDistance += funcMap[temp2].getDistance(funcMap[valuez[k]].getID());
				cumDistance += firstDistance;
				badgesHit += 2;
			}
			else if (tempValues1 == vector<double>{3, 4})
			{
				temp1 = valuez[k] - 3;
				temp2 = valuez[k] - 2;
				temp3 = valuez[k] - 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				if (temp3 > 30)
				{
					temp3 -= 31;
				}
				if (temp3 < 0)
				{
					temp3 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				firstDistance += funcMap[temp2].getDistance(funcMap[temp3].getID());
				firstDistance += funcMap[temp3].getDistance(funcMap[valuez[k]].getID());
				cumDistance += firstDistance;
				badgesHit += 3;
			}
			else if (tempValues1 == vector<double>{-3, -4})
			{
				temp1 = valuez[k] + 3;
				temp2 = valuez[k] + 2;
				temp3 = valuez[k] + 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				if (temp3 > 30)
				{
					temp3 -= 31;
				}
				if (temp3 < 0)
				{
					temp3 += 31;
				}
				firstDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				firstDistance += funcMap[temp2].getDistance(funcMap[temp3].getID());
				firstDistance += funcMap[temp3].getDistance(funcMap[valuez[k]].getID());
				cumDistance += firstDistance;
				badgesHit += 3;
			}
			else if (tempValues1 == vector<double>{0, 3})
			{
				temp1 = valuez[k] + 1;
				temp2 = valuez[k] + 2;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				cumDistance += funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 3;
			}
			else if (tempValues1 == vector<double>{0, -3})
			{
				temp1 = valuez[k] - 1;
				temp2 = valuez[k] - 2;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				cumDistance += funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 3;
			}
			else if (tempValues1 == vector<double>{0, 4})
			{
				temp1 = valuez[k] + 1;
				temp2 = valuez[k] + 2;
				temp3 = valuez[k] + 3;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				if (temp3 > 30)
				{
					temp3 -= 31;
				}
				if (temp3 < 0)
				{
					temp3 += 31;
				}
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID()) + funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID()) + funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				cumDistance += funcMap[temp2].getDistance(funcMap[temp3].getID());
				cumDistance += funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID()) + funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 4;
			}
			else if (tempValues1 == vector<double>{0, -4})
			{
				temp1 = valuez[k] - 1;
				temp2 = valuez[k] - 2;
				temp3 = valuez[k] - 3;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (temp2 > 30)
				{
					temp2 -= 31;
				}
				if (temp2 < 0)
				{
					temp2 += 31;
				}
				if (temp3 > 30)
				{
					temp3 -= 31;
				}
				if (temp3 < 0)
				{
					temp3 += 31;
				}
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID()) + funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID()) + funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
				cumDistance += funcMap[temp2].getDistance(funcMap[temp3].getID());
				cumDistance += funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID()) + funcMap[temp3].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 4;
			}
			else if (tempValues8 == vector<double>{0, 1, 2, 1, 2, 1, 2, 1, 2, 3} || tempValues8 == vector<double>{0, -1, -2, -1, -2, -1, -2, -1, -2, -3})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 8]].getID())) / (time[locations[k + 8]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 8]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 8]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 8]].getDistance(funcMap[valuez[k + 9]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 8]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 9]] - time[locations[k + 8]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 8]].getDistance(funcMap[valuez[k + 9]].getID()));
				timeLast = (time[locations[k + 9]] - time[locations[k + 8]]);
				k += 9;
				badgesHit += 3;
			}
			else if (tempValues6 == vector<double>{0, 1, 2, 1, 2, 1, 2, 3} || tempValues6 == vector<double>{0, -1, -2, -1, -2, -1, -2, -3})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID())) / (time[locations[k + 6]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 6]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 6]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 6]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 7]] - time[locations[k + 6]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 6]].getDistance(funcMap[valuez[k + 7]].getID()));
				timeLast = (time[locations[k + 7]] - time[locations[k + 6]]);
				k += 7;
				badgesHit += 3;
			}
			else if (tempValues13 == vector<double>{0, 1, 2, 3, 2, 3, 4} || tempValues13 == vector<double>{0, -1, -2, -3, -2, -3, -4})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID())) / (time[locations[k + 6]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 6]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 5]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 6]] - time[locations[k + 5]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 6]].getID()));
				timeLast = (time[locations[k + 6]] - time[locations[k + 5]]);
				k += 6;
				badgesHit += 4;
			}
			else if (tempValues10 == vector<double>{0, 1, 2, 1, 2, 3, 2, 3, 2, 3, 4, 3, 4, 5} || tempValues10 == vector<double>{0, -1, -2, -1, -2, -3, -2, -3, -2, -3, -4, -3, -4, -5})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 9]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 9]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 9]].getDistance(funcMap[valuez[k + 12]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 9]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 12]] - time[locations[k + 9]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 13]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 12]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 13]] - time[locations[k + 12]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 13]].getID()));
				timeLast = (time[locations[k + 13]] - time[locations[k + 12]]);
				k += 13;
				badgesHit += 5;
			}
			else if (tempValues7 == vector<double>{0, 1, 2, 1, 2, 3, 2, 3, 4} || tempValues7 == vector<double>{0, -1, -2, -1, -2, -3, -2, -3, -4})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID())) / (time[locations[k + 6]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 6]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 6]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 6]].getDistance(funcMap[valuez[k + 7]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 6]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 7]] - time[locations[k + 6]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 7]].getDistance(funcMap[valuez[k + 8]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 7]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 8]] - time[locations[k + 7]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 7]].getDistance(funcMap[valuez[k + 8]].getID()));
				timeLast = (time[locations[k + 8]] - time[locations[k + 7]]);
				k += 8;
				badgesHit += 4;
			}
			else if ((tempValues15 == vector<double>{0, 1, 2, 1, 2, 3, 2, 3, 2, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 2, 1, 0} || tempValues15 == vector<double>{0, -1, -2, -1, -2, -3, -2, -3, -2, -3, -4, -3, -4, -3, -4, -3, -4, -3, -4, -3, -2, -1, 0}) && temp4 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 10]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 10]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 10]].getDistance(funcMap[valuez[k + 16]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 10]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 16]] - time[locations[k + 10]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 16]].getDistance(funcMap[valuez[k + 19]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 16]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 19]] - time[locations[k + 16]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 19]].getDistance(funcMap[valuez[k + 20]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 19]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 20]] - time[locations[k + 19]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 20]].getDistance(funcMap[valuez[k + 21]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 20]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 21]] - time[locations[k + 20]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 22]].getDistance(funcMap[valuez[k + 21]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 21]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 22]] - time[locations[k + 21]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4 3 4 3 4 3 4 3 4 3 2 1 0");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 22]].getDistance(funcMap[valuez[k + 21]].getID()));
				timeLast = (time[locations[k + 22]] - time[locations[k + 21]]);
				k += 22;
				badgesHit += 8;
			}
			else if (tempValues9 == vector<double>{0, 1, 2, 1, 2, 3, 2, 3, 2, 3, 4} || tempValues9 == vector<double>{0, -1, -2, -1, -2, -3, -2, -3, -2, -3, -4})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 10]].getID());
				distanceLast = (funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 10]].getID()));
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 5]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 10]] - time[locations[k + 5]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				timeLast = (time[locations[k + 10]] - time[locations[k + 5]]);
				k += 10;
				badgesHit += 4;
			}
			else if (tempValues10 == vector<double>{0, 1, 2, 1, 2, 3, 2, 4, 3, 2, 4, 3, 4, 5} || tempValues10 == vector<double>{0, -1, -2, -1, -2, -3, -2, -4, -3, -2, -4, -3, -4, -5})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 4 3 2 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 4 3 2 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 4 3 2 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 12]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 5]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 12]] - time[locations[k + 5]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 4 3 2 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 13]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 12]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 13]] - time[locations[k + 12]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3 2 4 3 2 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 12]].getDistance(funcMap[valuez[k + 13]].getID()));
				timeLast = (time[locations[k + 13]] - time[locations[k + 12]]);
				k += 13;
				badgesHit += 5;
			}
			else if ((tempValues4 == vector<double>{0, 1, 2, 1, 3} || tempValues4 == vector<double>{0, -1, -2, -1, -3}) && temp6 != 13.5)// main 3 
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 4]] - time[locations[k + 2]]);
				k += 4;
				badgesHit += 3;
			}
			else if ((tempValues4 == vector<double>{0, 1, 2, 1, 4} || tempValues4 == vector<double>{0, -1, -2, -1, -4}) && temp6 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 2]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				temp1 = valuez[k] + 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 2]].getDistance(funcMap[temp1].getID())) + (funcMap[temp1].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 4]] - time[locations[k + 2]]);
				k += 4;
				badgesHit += 3;
			}
			else if (tempValues8 == vector<double>{0, 2, 1, 2, 3, 2, 3, 2, 3, 4} || tempValues8 == vector<double>{0, -2, -1, -2, -3, -2, -3, -2, -3, -4})// main 3 
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 2]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 5]].getID())) / (time[locations[k + 5]] - time[locations[k + 2]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 8]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 5]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 8]] - time[locations[k + 5]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 8]].getDistance(funcMap[valuez[k + 9]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 8]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 9]] - time[locations[k + 8]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 2 3 2 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 8]].getDistance(funcMap[valuez[k + 9]].getID()));
				timeLast = (time[locations[k + 9]] - time[locations[k + 8]]);
				k += 9;
				badgesHit += 4;
			}
			else if (tempValues7 == vector<double>{0, 2, 1, 2, 3, 4, 3, 4, 5} || tempValues7 == vector<double>{0, -2, -1, -2, -3, -4, -3, -4, -5})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 2]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 3]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 7]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 7]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 7]].getDistance(funcMap[valuez[k + 8]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 7]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 8]] - time[locations[k + 7]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4 5");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 7]].getDistance(funcMap[valuez[k + 8]].getID()));
				timeLast = (time[locations[k + 8]] - time[locations[k + 7]]);
				k += 8;
				badgesHit += 5;
			}
			else if ((tempValues6 == vector<double>{0, 2, 1, 2, 3, 4, 3, 4} || tempValues6 == vector<double>{0, -2, -1, -2, -3, -4, -3, -4}) /*&& (k + 7 == values.size())*/)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 2]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 3]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3 4 3 4");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID()));
				timeLast = (time[locations[k + 5]] - time[locations[k + 4]]);
				badgesHit += 4;
				k += 7;
			}
			else if ((tempValues4 == vector<double>{0, 2, 1, 2, 3} || tempValues4 == vector<double>{0, -2, -1, -2, -3}) && temp6 != 13.5)// main 3 
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 2]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID()));
					speedSecond = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID())) / (time[locations[k + 3]] - time[locations[k + 2]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID())) / (time[locations[k + 2]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 2]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 3]] - time[locations[k + 2]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 3]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 3]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 4]].getID()));
				timeLast = (time[locations[k + 4]] - time[locations[k + 3]]);
				k += 4;
				badgesHit += 3;
			}
			else if ((tempValues5 == vector<double>{0, 1, 2, 1, 2, 3} || tempValues5 == vector<double>{0, -1, -2, -1, -2, -3}) && temp6 != 13.5)
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID()));
					speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID())) / (time[locations[k + 4]] - time[locations[k + 1]]);
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 4]] - time[locations[k + 1]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k + 4]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 5]] - time[locations[k + 4]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 1 2 1 2 3");

				segData.push_back(segmentTemp);
				segmentTemp.clear();
				distanceLast = (funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 5]].getID()));
				timeLast = (time[locations[k + 5]] - time[locations[k + 4]]);
				k += 5;
				badgesHit += 3;
			}
			//Place here
			else if (tempValues1 == vector<double>{0, 2})
			{
				temp1 = valuez[k] + 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}
			else if (tempValues1 == vector<double>{0, -2})
			{
				temp1 = valuez[k] - 1;
				if (temp1 > 30)
				{
					temp1 -= 31;
				}
				if (temp1 < 0)
				{
					temp1 += 31;
				}
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
				cumDistance += funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -2");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}

			else if (tempValues1 == vector<double>{0, 6})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[13.5].getID());
				cumDistance += funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 6");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}
			else if (tempValues1 == vector<double>{0, -6})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[13.5].getID());
				cumDistance += funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -6");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}

			else if (tempValues1 == vector<double>{0, 7})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[13.5].getID());
				cumDistance += funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID());
				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 7");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}

			else if (tempValues1 == vector<double>{0, -7})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[13.5].getID());
				cumDistance += funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -7");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}

			else if (tempValues1 == vector<double>{0, 8})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[13.5].getID());
				cumDistance += funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 8");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}

			else if (tempValues1 == vector<double>{0, -8})
			{
				if (k == 0)
				{
					startTime = time[locations[k]];
					firstTime = time[locations[k + 1]] - time[locations[k]];
					firstDistance += (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
					hitSecond = true;
				}
				if (hitSecond == true)
				{
					speedSecond = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
					hitSecond = false;
				}
				segmentTemporaryDistance = cumDistance;
				segmentTemporaryTime = cTime;
				cumDistance += funcMap[valuez[k]].getDistance(funcMap[13.5].getID());
				cumDistance += funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID());

				segmentTemp.push_back(inputData[0][7].substr(0, 10));
				segmentTemp.push_back(inputData[0][0]);
				segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
				segmentTemp.push_back(to_string(numAmbulation + 1));
				cTime += time[locations[k + 1]] - time[locations[k]];
				segmentTemp.push_back(to_string(cTime));
				segmentTemp.push_back(to_string(cumDistance));
				if (cTime - segmentTemporaryTime != 0)
				{
					tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
					segmentTemp.push_back(to_string(tempSpeedd));
				}
				else
				{
					segmentTemp.push_back("N/A");
				}
				segmentTemp.push_back(realID);
				segmentTemp.push_back(to_string(total_anom));
				segmentTemp.push_back("0 -8");

				segData.push_back(segmentTemp);
				segmentTemp.clear();

				distanceLast = (funcMap[valuez[k]].getDistance(funcMap[13.5].getID()) + funcMap[13.5].getDistance(funcMap[valuez[k + 1]].getID()));
				timeLast = (time[locations[k + 1]] - time[locations[k]]);
				k += 1;
				badgesHit += 2;
			}
			else
			{
				if ((tempValues3 == vector<double>{0, 2, 1, 2} || tempValues3 == vector<double>{0, -2, -1, -2}) && values.size() - k - 3 < 2)
				{
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					firstDistance = funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 2]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 2]] - time[locations[k]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);

					segData.push_back(segmentTemp);
					segmentTemp.clear();
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					cumDistance += funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 3]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k + 2]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 3]] - time[locations[k + 2]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 2 1 2");

					segData.push_back(segmentTemp);
					segmentTemp.clear();

					distanceLast = (funcMap[valuez[k + 3]].getDistance(funcMap[valuez[k + 2]].getID()));
					timeLast = (time[locations[k + 3]] - time[locations[k + 2]]);
					badgesHit += 2;
					k += 3;
				}
				else if ((tempValues5 == vector<double>{0, 1, 3, 1, 3, 6} || tempValues5 == vector<double>{0, -1, -3, -1, -3, -6}) && values.size() - k - 5 < 2)
				{
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					firstDistance = funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 1]] - time[locations[k]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 1 3 1 3 6");

					segData.push_back(segmentTemp);
					segmentTemp.clear();
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 5]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 5]] - time[locations[k + 1]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 1 3 1 3 6");

					segData.push_back(segmentTemp);
					segmentTemp.clear();

					distanceLast = (funcMap[valuez[k + 5]].getDistance(funcMap[valuez[k + 1]].getID()));
					timeLast = (time[locations[k + 5]] - time[locations[k + 1]]);
					badgesHit += 2;
					k += 5;
				}
				else if ((tempValues4 == vector<double>{0, 1, 2, 1, 2} || tempValues4 == vector<double>{0, -1, -2, -1, -2}) && values.size() - k - 4 < 2)
				{
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					firstDistance = funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 1]] - time[locations[k]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 1 2 1 2");

					segData.push_back(segmentTemp);
					segmentTemp.clear();
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 4]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 4]] - time[locations[k + 1]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 1 2 1 2");

					segData.push_back(segmentTemp);
					segmentTemp.clear();

					distanceLast = (funcMap[valuez[k + 4]].getDistance(funcMap[valuez[k + 1]].getID()));
					timeLast = (time[locations[k + 4]] - time[locations[k + 1]]);
					badgesHit += 2;
					k += 4;
				}
				else if ((tempValues3 == vector<double>{0, 1, 2, 1} || tempValues3 == vector<double>{0, -1, -2, -1}) && values.size() - k - 3 < 2)
				{
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					firstDistance = funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 1]] - time[locations[k]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 1 2 1");

					segData.push_back(segmentTemp);
					segmentTemp.clear();
					segmentTemporaryDistance = cumDistance;
					segmentTemporaryTime = cTime;
					cumDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k + 2]].getID());

					segmentTemp.push_back(inputData[0][7].substr(0, 10));
					segmentTemp.push_back(inputData[0][0]);
					segmentTemp.push_back(to_string(funcMap[valuez[k + 1]].getID()));
					segmentTemp.push_back(to_string(numAmbulation + 1));
					cTime += time[locations[k + 2]] - time[locations[k + 1]];
					segmentTemp.push_back(to_string(cTime));
					segmentTemp.push_back(to_string(cumDistance));
					if (cTime - segmentTemporaryTime != 0)
					{
						tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
						segmentTemp.push_back(to_string(tempSpeedd));
					}
					else
					{
						segmentTemp.push_back("N/A");
					}
					segmentTemp.push_back(realID);
					segmentTemp.push_back(to_string(total_anom));
					segmentTemp.push_back("0 1 2 1");

					segData.push_back(segmentTemp);
					segmentTemp.clear();

					distanceLast = (funcMap[valuez[k + 2]].getDistance(funcMap[valuez[k + 1]].getID()));
					timeLast = (time[locations[k + 2]] - time[locations[k + 1]]);
					badgesHit += 2;
					k += 3;
				}
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 0.5 && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					badgesHit += values.size() - k - 1;
					total_anom--;
					k += 100;
				}
				//Special ending code, do not touch if possible
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 1.5 && (0 > valuez.back() - funcMap[roomNumber].getRoomID() >= -1.5) && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					temp1 = valuez[k] - 1;
					if (temp1 > 30)
					{
						temp1 -= 31;
					}
					if (temp1 < 0)
					{
						temp1 += 31;
					}
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					distanceLast += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					/*segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + cumDistance - segmentTemporaryDistance);
					if (cTime - segmentTemporaryTime != 0)
					{
						segData[segData.size() - 1][6] = to_string((cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime));
					}*/



					badgesHit += values.size() - k;
					k += 100;
				}
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 1.5 && (0 < valuez.back() - funcMap[roomNumber].getRoomID() <= 1.5) && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					temp1 = valuez[k] + 1;
					if (temp1 > 30)
					{
						temp1 -= 31;
					}
					if (temp1 < 0)
					{
						temp1 += 31;
					}
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					distanceLast += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					/*segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + cumDistance - segmentTemporaryDistance);
					if (cTime - segmentTemporaryTime != 0)
					{
						segData[segData.size() - 1][6] = to_string((cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime));
					}*/



					badgesHit += values.size() - k;
					k += 100;
				}
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 2.5 && (0 > valuez.back() - funcMap[roomNumber].getRoomID() >= -2.5) && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					temp1 = valuez[k] - 1;
					temp2 = valuez[k] - 2;
					if (temp1 > 30)
					{
						temp1 -= 31;
					}
					if (temp1 < 0)
					{
						temp1 += 31;
					}
					if (temp2 > 30)
					{
						temp2 -= 31;
					}
					if (temp2 < 0)
					{
						temp2 += 31;
					}
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
					/*segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + cumDistance - segmentTemporaryDistance);
					if (cTime - segmentTemporaryTime != 0)
					{
						segData[segData.size() - 1][6] = to_string((cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime));
					}*/



					distanceLast += funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID());
					badgesHit += values.size() - k + 1;
					k += 100;
				}
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 2.5 && (0 < valuez.back() - funcMap[roomNumber].getRoomID() <= 2.5) && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					temp1 = valuez[k] + 1;
					temp2 = valuez[k] + 2;
					if (temp1 > 30)
					{
						temp1 -= 31;
					}
					if (temp1 < 0)
					{
						temp1 += 31;
					}
					if (temp2 > 30)
					{
						temp2 -= 31;
					}
					if (temp2 < 0)
					{
						temp2 += 31;
					}
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
					/*segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + cumDistance - segmentTemporaryDistance);
					if (cTime - segmentTemporaryTime != 0)
					{
						segData[segData.size() - 1][6] = to_string((cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime));
					}*/


					distanceLast += funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID());
					badgesHit += values.size() - k + 1;
					k += 100;
				}
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 3.5 && (0 > valuez.back() - funcMap[roomNumber].getRoomID() >= -3.5) && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					temp1 = valuez[k] - 1;
					temp2 = valuez[k] - 2;
					temp3 = valuez[k] - 3;
					if (temp1 > 30)
					{
						temp1 -= 31;
					}
					if (temp1 < 0)
					{
						temp1 += 31;
					}
					if (temp2 > 30)
					{
						temp2 -= 31;
					}
					if (temp2 < 0)
					{
						temp2 += 31;
					}
					if (temp3 > 30)
					{
						temp3 -= 31;
					}
					if (temp3 < 0)
					{
						temp3 += 31;
					}
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
					cumDistance += funcMap[temp2].getDistance(funcMap[temp3].getID());
					/*segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + cumDistance - segmentTemporaryDistance);
					if (cTime - segmentTemporaryTime != 0)
					{
						segData[segData.size() - 1][6] = to_string((cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime));
					}*/


					distanceLast += funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID());
					badgesHit += values.size() - k + 2;
					k += 100;
				}
				else if (abs(valuez.back() - funcMap[roomNumber].getRoomID()) <= 3.5 && (0 < valuez.back() - funcMap[roomNumber].getRoomID() <= 3.5) && values.size() - k < 2)
				{
					if (k == 0)
					{
						startTime = time[locations[k]];
					}
					temp1 = valuez[k] + 1;
					temp2 = valuez[k] + 2;
					temp3 = valuez[k] + 3;
					if (temp1 > 30)
					{
						temp1 -= 31;
					}
					if (temp1 < 0)
					{
						temp1 += 31;
					}
					if (temp2 > 30)
					{
						temp2 -= 31;
					}
					if (temp2 < 0)
					{
						temp2 += 31;
					}
					if (temp3 > 30)
					{
						temp3 -= 31;
					}
					if (temp3 < 0)
					{
						temp3 += 31;
					}
					cumDistance += funcMap[valuez[k]].getDistance(funcMap[temp1].getID());
					cumDistance += funcMap[temp1].getDistance(funcMap[temp2].getID());
					cumDistance += funcMap[temp2].getDistance(funcMap[temp3].getID());

					/*segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + cumDistance - segmentTemporaryDistance);
					if (cTime - segmentTemporaryTime != 0)
					{
						segData[segData.size() - 1][6] = to_string((cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime));
					}*/


					distanceLast += funcMap[valuez[k]].getDistance(funcMap[temp1].getID()) + funcMap[temp1].getDistance(funcMap[temp2].getID()) + funcMap[temp2].getDistance(funcMap[temp3].getID());

					badgesHit += values.size() - k + 2;
					k += 100;
				}
				else
				{
					if (values[0] != 0 && k == 0)
					{
					}
					else if (k + 1< values.size())
					{
						if (k == 0)
						{
							startTime = time[locations[k]];
							firstTime = time[locations[k + 1]] - time[locations[k]];
							firstDistance += funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID());
							hitSecond = true;
						}
						if (hitSecond == true)
						{
							speedSecond = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID())) / (time[locations[k + 1]] - time[locations[k]]);
							hitSecond = false;
						}


						segmentTemporaryDistance = cumDistance;
						segmentTemporaryTime = cTime;
						cumDistance += funcMap[valuez[k]].getDistance(funcMap[valuez[k + 1]].getID());
						distanceLast = (funcMap[valuez[k + 1]].getDistance(funcMap[valuez[k]].getID()));
						timeLast = (time[locations[k + 1]] - time[locations[k]]);

						segmentTemp.push_back(inputData[0][7].substr(0, 10));
						segmentTemp.push_back(inputData[0][0]);
						segmentTemp.push_back(to_string(funcMap[valuez[k]].getID()));
						segmentTemp.push_back(to_string(numAmbulation + 1));
						cTime += time[locations[k + 1]] - time[locations[k]];
						segmentTemp.push_back(to_string(cTime));
						segmentTemp.push_back(to_string(cumDistance));
						if (cTime - segmentTemporaryTime != 0)
						{
							tempSpeedd = (cumDistance - segmentTemporaryDistance) / (cTime - segmentTemporaryTime);
							segmentTemp.push_back(to_string(tempSpeedd));
						}
						else
						{
							segmentTemp.push_back("N/A");
						}
						segmentTemp.push_back(realID);
						segmentTemp.push_back(to_string(total_anom));
						segmentTemp.push_back("  ");

						segData.push_back(segmentTemp);
						segmentTemp.clear();

						cout << "HI5"; //hhlhflehlfhlh

						badgesHit += 1;
						k += 1;
						total_anom--;
					}
				}
			}
			isTrue = true;
			noLoop++;
			if (noLoop > 50)
			{
				k++;
			}
			//cout << to_string(funcMap[valuez[k]].getID()) << endl;
		}
		badgesHit += 1;

		//Outputs ambulation data, if it is considered to be an ambulation
		if (badgesHit >= 6)
		{
			cumDistance += 11;
			ambulationStart.push_back(locations.front());
			ambulationEnd.push_back(locations.back());
			finalDistance[numAmbulation] = cumDistance;
			endTime = time[locations.back()];
			if (speedSecond == 0 || timeLast == 0)
			{
				finalDuration[numAmbulation] = endTime - startTime;
			}
			else {

				thelastseg = (double)((int)(11 / ((distanceLast / timeLast)*0.68181818181818181818181818181818)));
				thefirstseg = (double)((int)((firstDistance - 11) / (speedSecond * 0.68181818181818181818181818181818)));
				finalDuration[numAmbulation] = endTime - startTime + thefirstseg - firstTime + thelastseg;
			}
			ambCoor.push_back(i);
			numAmbulation++;



		}

		//Outputs whether something is an ambulation or OOR
		for (int i = segData.size() - 1; i >= initialSegSize; i--)
		{
			if (badgesHit >= 6)
			{
				segData[i].push_back("Ambulation");
				segData[i][4] = to_string(stod(segData[i][4]) - firstTime + thefirstseg);
			}
			else
			{
				segData[i].push_back("Out of Room");
				segData[i][5] = to_string(stod(segData[i][5]) + 11);
				segData[i][3] = "N/A";
			}
		}
		if (badgesHit >= 6)
		{
			if (speedSecond != 0)
			{
				segData[initialSegSize][6] = to_string(speedSecond);
				segData[initialSegSize][4] = to_string(thefirstseg);
			}
			segData.push_back(segData[segData.size() - 1]);
			segData[segData.size() - 1][4] = to_string(stod(segData[segData.size() - 1][4]) + thelastseg);
			segData[segData.size() - 1][5] = to_string(stod(segData[segData.size() - 1][5]) + 11);
			//segData[segData.size() - 1][6] = ((distanceLast / timeLast)*0.68181818181818181818181818181818);
		}
		//push back new line with 11 and last speed
		if (segData.size() != tempSizeSeg)
		{
			segData.push_back(lmao); //whether it is ambulation or not.
		}
		

		OORStart = inRoom[i] + 1;
	}
	//Out of room detector
	int OORBegin[numRows];
	int OOREnd[numRows];
	int numOOR = 0;
	int middleTime;
	int originalI;
	bool inMiddle = false;
	for (int i = 1; i < timeCount; ++i)
	{
		if (time[inRoom[i]] > time[inRoom[i - 1] + 1] + 3)
		{
			if (time[inRoom[i]] < time[inRoom[i] + 1] - 60 && inMiddle == false)
			{
				OORBegin[numOOR] = inRoom[i - 1];
				OOREnd[numOOR] = inRoom[i];
				numOOR++;
			}
			if (time[inRoom[i]] < time[inRoom[i] + 1] - 60 && inMiddle == true)
			{
				if (middleTime < time[inRoom[i] + 1] - 60)
				{
					OOREnd[numOOR] = inRoom[originalI];
					numOOR++;
					OORBegin[numOOR] = inRoom[i - 1];
					OOREnd[numOOR] = inRoom[i];
				}
				else
				{
					OOREnd[numOOR] = inRoom[i];
				}
				inMiddle = false;
				numOOR++;
			}
			if (time[inRoom[i]] >= time[inRoom[i] + 1] - 60 && inMiddle == false)
			{
				OORBegin[numOOR] = inRoom[i - 1];
				inMiddle = true;
				originalI = i;
				middleTime = time[inRoom[i]];
			}
		}
	}

	vector<vector<string>> outputData;
	vector<string> outputTemp;
	string stringDuration;
	int iminutes, iseconds, finalD;
	double tempSpeed;
	int roomDisplay;


	int offffset = 0;
	//Calcalutes MRN number, then outputs all calcualted information to file
	for (int i = 0; i < numAmbulation; ++i)
	{
		oldID = realID;
		realID = "";
		for (int j = inputInfo.size() - 1; j >= 0; j--)
		{
			if (inputInfo[j][0].empty())
			{
				break;
			}
			if (inputInfo[j][1] == inputData[inRoom[ambCoor[i]]][0])
			{
				if (inputInfo[j][2].compare(inputData[inRoom[ambCoor[i]]][7]) <= 0) //might be other way
				{
					realID = inputInfo[j][0];
					break;
				}
			}
		}
		if (realID == "")
		{
			realID = "0";
		}
		if (realID != oldID)
		{
			offffset = i;
		}
		roomDisplay = funcMap2[roomNumber];
		outputTemp.clear();
		tempSpeed = finalDistance[i] / finalDuration[i] * 0.68181818181818181818181818181818;
		finalD = finalDuration[i];
		iminutes = finalD / 60;
		iseconds = finalD % 60;
		stringDuration = to_string(iminutes) + " minutes " + to_string(iseconds) + " seconds";
		//Patient #, Patient room #, Start date/time, Duration, Total Distance, Average speed, total ambulation, Ambulationnum, goal, goalupdatetime, data update time & date
		//outputTemp.push_back(inputData[0][0]);								//BadgeID
		outputTemp.push_back(realID);													//ID
		outputTemp.push_back(to_string(roomDisplay));							//Room
		outputTemp.push_back(inputData[0][7].substr(0, 10) + " " + inputData[ambulationStart[i]][7].substr(11, 8));					//Startdate/Time
																																	//outputTemp.push_back(inputData[0][7].substr(0, 10));					//Date
		outputTemp.push_back(to_string(finalD));								//Duration
		outputTemp.push_back(to_string(finalDistance[i]));						//Finaldistance
		outputTemp.push_back(to_string(tempSpeed));								//Speed
		outputTemp.push_back("1");												//Total Ambulation Placeholder
		outputTemp.push_back(to_string(i + 1 - offffset));						//AmbulationNum
		outputTemp.push_back("0");												//Goal Placeholder
		outputTemp.push_back("0");												//Goal Update Time Placeholder
																				//outputTemp.push_back(inputData[ambulationStart[i]][7].substr(11, 8));	//Start time
		outputTemp.push_back(inputData[0][7].substr(0, 10) + " " + inputData[ambulationEnd[i]][7].substr(11, 8));		//End time
		outputTemp.push_back(to_string(total_anom));
		outputData.push_back(outputTemp);
	}

	//Ambulationanalyzer code
	vector<int> ambuTemp;
	if (realID == "")
	{
		realID = "0";
	}

	ambuTemp.push_back(stoi(realID));
	if (numAmbulation > 0)
	{
		ambuTemp.push_back(1);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuData.push_back(ambuTemp);
	}
	else if (numOOR > 0)
	{
		ambuTemp.push_back(0);
		ambuTemp.push_back(1);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuData.push_back(ambuTemp);
	}
	else if (humma > 0)
	{
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(1);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuData.push_back(ambuTemp);
	}
	else
	{
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuTemp.push_back(1);
		ambuTemp.push_back(0);
		ambuTemp.push_back(0);
		ambuData.push_back(ambuTemp);
	}

	return outputData;

}

//Main program will analysis algorithm is executed. Takes in IData.csv (MRNs) and database.csv (Old data), and runs
//the algorithm on all valid/applicable files in the folder directory specified.
int main()
{
	ofstream output;
	vector<unsigned long long> fileSize;
	vector<unsigned long long> oldfileSize;
	vector<string> fileName;
	vector<string> oldfileName;
	vector<vector<string>> data;
	vector<vector<string>> tempIn;
	stringstream buffer;
	string path;

	CSVRow row;
	vector<string> inputLine;
	vector<vector<string>> inputData;
	vector<vector<string>> oldData;
	vector<vector<string>> segData;
	vector<vector<int>> ambuData;
	vector<string> toScanID, toScanStart;

	cout << "Please enter the name of the directory to analyze: ";
	std::getline(std::cin, path);
	cout << "\n";

	while (true) {
		//Updates MRN badge data with a download from the website
		update();

		ifstream input("IData.csv");
		inputData.clear();
		segData.clear();
		ambuData.clear();
		toScanStart.clear();
		toScanID.clear();
		oldData.clear();
		ifstream old("database.csv");
		//input scan a bunch of stuff
		while (old >> row)
		{
			for (int i = 0; i <= 10; i++)
			{
				inputLine.push_back(row[i]);
			}
			oldData.push_back(inputLine);
			inputLine.clear();
		}
		old.close();
		bool checkkkk = true;
		while (input >> row)
		{
			checkkkk = true;
			for (int i = 0; i < 4; i++)
			{
				if (row[0].empty() || row[0] == "<values>" || row[0] == "</values>") //HERE
				{
					checkkkk = false;
					break;
				}
				inputLine.push_back(row[i]);
			}
			if (checkkkk == true)
			{
				inputData.push_back(inputLine);
				inputLine.clear();
			}
		}
		input.close();
		sort(inputData.begin(), inputData.end(), sortcol);

		for (int i = inputData.size() - 1; i >= 0; i--)
		{
			if (inputData[i][2] == "N/A")
			{
				for (int j = i; j >= 0; j--)
				{
					if (inputData[i][0] == inputData[j][0] && inputData[j][3] == "N/A</br>")
					{
						inputData[j][3] = inputData[i][3];
						inputData.erase(inputData.begin() + i);
						break;
					}
				}
			}
		}

		for (int i = inputData.size() - 1; i >= 0; i--)
		{
			if (inputData[i][3] == "N/A</br>")
			{
				toScanID.push_back(inputData[i][1]);
				toScanStart.push_back(inputData[i][2]);
			}
		}






		data.clear();
		for (auto & p : experimental::filesystem::recursive_directory_iterator(path)) {
			buffer << p;
			cout << buffer.str() << endl;
			int OPP = buffer.str().find('-') - 4;
			//series of filters to check whether or not to scan analyze a file
			if (buffer.str().substr(buffer.str().size() - 5) == ("g.csv"))
			{
				if (buffer.str().substr(buffer.str().size() - 19) != ("306724-TrackLog.csv"))
				{
					for (int i = 0; i < toScanID.size(); i++)
					{
						if (buffer.str().substr(buffer.str().size() - 19, 6) == toScanID[i])
						{
							//cout << toScanStart[i].substr(0, 10) << endl;
							//cout << buffer.str().substr(OPP, 10) << endl;
							if (toScanStart[i].substr(0, 10).compare(buffer.str().substr(OPP, 10)) <= 0) // Erase this line, it ignores end dates
							{
								ifstream file(buffer.str(), ios::binary | ios::ate);
								tempIn = analyzeData(buffer.str(), inputData, ambuData, segData);
								buffer.str("");
								for (int i = 0; i < tempIn.size(); i++)
								{
									data.push_back(tempIn[i]);
								}
								break;
							}
						}
					}



				}
			}
			buffer.str(std::string());
		}

		sort(data.begin(), data.end(), sortcol);

		int totalAmb, dayAmb, totalAmbPlus, dayAmbPlus;

		//Calculates total vs daily ambulation numbers for ambulation results
		for (int i = data.size() - 1; i >= 0; i--)
		{
			totalAmb = 1;
			dayAmb = 1;
			totalAmbPlus = 0;
			dayAmbPlus = 0;
			for (int j = i - 1; j >= 0; j--)
			{
				if (data[i][0] == data[j][0])
				{
					totalAmbPlus++;
					if (data[i][2].substr(0, 10) == data[j][2].substr(0, 10))
					{
						dayAmbPlus++;
					}
				}
			}
			totalAmb += totalAmbPlus; //6
			dayAmb += dayAmbPlus; //7	
			data[i][6] = to_string(totalAmb);
			data[i][7] = to_string(dayAmb);
		}

		//Moves ambuData vector to an adjusted new one for output later
		vector<vector<int>> ambuDataNext;
		bool dadaCheck = false;
		int tempInt;
		for (int z = 0; z < ambuData.size(); z++)
		{
			dadaCheck = false;
			tempInt = 0;
			for (int y = 0; y < ambuDataNext.size(); y++)
			{
				if (ambuDataNext[y][0] == ambuData[z][0])
					dadaCheck = true;
				tempInt = y;
			}
			if (dadaCheck == true)
			{
				ambuDataNext[tempInt][1] += ambuData[z][1];
				ambuDataNext[tempInt][2] += ambuData[z][2];
				ambuDataNext[tempInt][3] += ambuData[z][3];
				ambuDataNext[tempInt][4] += ambuData[z][4];
			}
			else
			{
				ambuDataNext.push_back(ambuData[z]);
			}
		}

		int realTime;
		time_t t = time(0);
		struct tm * now = localtime(&t);



		//Calculates some remaining data fields for ambulation analyzer
		for (int z = 0; z < ambuDataNext.size(); z++)
		{
			realTime = 0;

			for (int i = 0; i < inputData.size(); ++i)
			{
				for (int j = inputData.size() - 1; j >= 0; j--)
				{
					if (inputData[j][0].empty())
					{
						break;
					}
					if (stoi(inputData[j][0]) == ambuDataNext[z][0])
					{
						int year1 = stoi(inputData[i][2].substr(2, 2)); // 1 start, 2 end
						int year2 = now->tm_year - 100;
						int month1 = stoi(inputData[i][2].substr(5, 2));
						int month2 = now->tm_mon + 1;
						int day1 = stoi(inputData[i][2].substr(8, 2));
						int day2 = now->tm_mday;
						realTime = (365 * (year2 - year1) + 30 * (month2 - month1) + (day2 - day1) + 1);
						break;
					}
				}
			}
			ambuDataNext[z][6] = realTime;
			ambuDataNext[z][5] = ambuDataNext[z][6] - ambuDataNext[z][1] - ambuDataNext[z][2] - ambuDataNext[z][3] - ambuDataNext[z][4];
		}
		bool check = false;
		//Updates the "old data" from the database.csv file. Adds new lines, replaces lines that are being updated (aka live situations)
		for (int j = 0; j < data.size(); j++)
		{
			check = false;
			for (int i = oldData.size() - 1; i >= 0; i--)
			{
				if (oldData[i][0] == data[j][0] && oldData[i][2] == data[j][2])
				{
					oldData[i] = data[j];
					check = true;
					break;
				}
			}
			if (check == false)
			{
				oldData.push_back(data[j]);
			}
		}

		//Opens 4 files for output and writes to them
		output.open("ambulationAnalysis.csv");
		for (int i = 0; i < ambuDataNext.size(); ++i)
		{
			for (int j = 0; j < ambuDataNext[i].size(); ++j)
			{
				output << ambuDataNext[i][j];
				//if (j != 7)
				//{
					output << ",";
				//}
			}
			output << endl;
		}
		output.close();

		output.open("segmentAnalysis.csv");
		for (int i = 0; i < segData.size(); ++i)
		{
			for (int j = 0; j < segData[i].size(); ++j)
			{
				output << segData[i][j];
				//if (j != 8)
				//{
					output << ",";
				//}
			}
			output << endl;
		}
		output.close();

		output.open("database.csv");
		for (int i = 0; i < oldData.size(); ++i)
		{
			for (int j = 0; j < oldData[i].size(); ++j)
			{
				output << oldData[i][j];
				//if (j != 11)
				//{
					output << ",";
				//}
			}
			output << endl;
		}
		output.close();

		output.open("live.csv");
		for (int i = 0; i < data.size(); ++i)
		{
			for (int j = 0; j < data[i].size(); ++j)
			{
				output << data[i][j];
				//if (j != 11)
				//{
					output << ",";
				//}
			}
			output << endl;
		}
		output.close();
		cout << "Scan complete!\n";
		this_thread::sleep_for(chrono::seconds(10));
	}
	return 0;
}
/*
Database Output:
0: Patient MRN #
1: Patient room #
2: Start date/time
3: Duration
4: Total Distance
5: Average speed
6: Total ambulation
7: Ambulation number
8: Goal (placeholder 0)
9: Goal update time (placeholder 0)
10: Data update time & date

Assumptions: (note: all can be adjusted later)
leaving room 20002 does not always trigger sensor 20004 only 1 or 4
leaving room 20033 does not always trigger sensor 20030
leaving room 20000 should always trigger sensor 20001
leaving room 20005 should always trigger sensor 20004
leaving room 20007 should always trigger sensor 20010
leaving room 20008 should always trigger sensor 20010
leaving room 20012 should always trigger sensor 20013
leaving room 20016 should always trigger sensor 20015
leaving room 20018 should always trigger sensor 20017
leaving room 20020 should always trigger sensor 20021
leaving room 20026 should always trigger sensor 20025
leaving room 20040 should always trigger sensor 20045
leaving room 20041 should always trigger sensor 20045
leaving room 20043 should always trigger sensor 20044
leaving room 20053 should always trigger sensor 20052
leaving room 20057 should always trigger sensor 20056
leaving room 20060 should always trigger sensor 20059



SegData
Date	Badge ID	Sensor ID	Ambulation #	Cumulative time(sec)	Cumulative distance (ft)	Segment speed    //MRN (will have to c/p that code earlier) AmbulationorNot Total anomalies anomaly type

//Repeat as many times necessary




AmbuData
patient # (MRN)
≥1 ambulation									if ambnum > 0
no ambulation, ≥1 out of room					if amb num = 0, oor > 0
no ambulation, no out of room, ≥1 hall sensor	if both = 0, //probably end up using size of locationz or something like that
no ambulation, no out of room, ≥1 room sensor	all 3 = 0, //probably end up using size of locationz or something like that
no ambulation, no out of room, no sensor		Total days minus all above //HERE, use locaions.size() cumulative
Total days on 10W							    Take from other data file thingy (date in slot 4 - date in slot 3 + 1)


1/2nd speed is the same (set it 1 time at the end if it is an ambulation)
Make everything -11 distance (start out at -11, then if it is OOR add 11 for all, otherwise only add it at the end)
Additional extra segment with last speed and 11 distance (add it 1 time at the end if it is an ambulation)
Finish writing for all cases (manuallabor)
Write instructions for segments (consider reformatting entire function to be more efficient).

*/

