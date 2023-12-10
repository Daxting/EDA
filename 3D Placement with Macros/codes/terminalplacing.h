#pragma once
#define DEBUG
#ifdef DEBUG
#define TERMINAL_PLACING_DEBUG
#endif // DEBUG

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "data_structure.h"
using namespace std;

inline bool terminal_check(Terminal& tempTerminal, vector<Terminal> &terminalList, int& terminalSpacing, int& terminalSize, Die* anyDie);

inline void terminal_placing(vector<Net*>& cutNetList, vector<Terminal>& terminalList, int &terminalSpacing, int &terminalSize, Die* anyDie){

	#ifdef TERMINAL_PLACING_DEBUG
	cout << "--- terminal Placing Start ---" << endl;
	#endif // TERMINAL_PLACING_DEBUG

	{
		int netMinX, netMinY, netMaxX, netMaxY, cutNetSize;
		vector<int> cutNetSizeList;
		vector<Net*> cutNetOrderList;
		cutNetSizeList.reserve(cutNetList.size());
		cutNetOrderList.reserve(cutNetList.size());

		// 算出各Net下Pin所框出矩形的面積，並照著cutNetList的順序排列
		for(auto&& NetPtr : cutNetList){
			( *NetPtr ).getHpwlPoint(netMinX, netMinY, netMaxX, netMaxY, ( *anyDie ).maxX, ( *anyDie ).maxY);
			cutNetSize = ( netMaxX - netMinX ) * ( netMaxY - netMinY );
			cutNetSizeList.push_back(cutNetSize);
		}

		// 根據各Net下Pin所框出矩形的面積，將對應位置的cutNetList由小到大排序到cutNetOrderList
		int i = 0, pos = 0;
		do{
			cutNetSize = 0;
			pos = 0;
			for(i = 0; i < cutNetSizeList.size(); i++){
				if(cutNetSize >= cutNetSizeList.at(i) && cutNetSizeList.at(i) != 0){
					cutNetSize = cutNetSizeList.at(i);
					pos = i;
				}
			}
			cutNetOrderList.push_back(cutNetList.at(pos));
			cutNetSizeList.at(pos) = 0;
		} while(cutNetSize != 0);

		cutNetList.swap(cutNetOrderList);
		//cutNetList = cutNetOrderList;
	}
	Terminal tempTerminal;
	 
	/*
	// 將所有cutNetList最簡單的間隔排列在Die上
	{
		int xtemp = ( *anyDie ).minX, ytemp = ( *anyDie ).minY, count = 0;;
		int num_of_terminal_in_one_row = ( ( *anyDie ).maxX - ( *anyDie ).minX - terminalSpacing ) / ( terminalSize + terminalSpacing );
		for(int i = 0; i < cutNetList.size() / num_of_terminal_in_one_row; i++){
			//determine the position
			xtemp = terminalSpacing + terminalSize / 2;
			ytemp += terminalSpacing + terminalSize / 2;
			if(i != 0)
				ytemp += terminalSize;
			for(int j = 0; j < num_of_terminal_in_one_row; j++){
				tempTerminal.name = cutNetList.at(i)->name;
				tempTerminal.locationX = xtemp;
				tempTerminal.locationY = ytemp;
				terminalList.push_back(tempTerminal);
				count += 1;
				xtemp += terminalSize + terminalSpacing;
				if(count == cutNetList.size())
					break;
			}
		}
	}
	*/
	// 新方法 待驗證
	{
		int netMinX, netMinY, netMaxX, netMaxY;
		int posX, posY;

		// 按cutNetList順序，優先將terminal擺在Net的Pin上
		for(auto&& Net : cutNetList){
			tempTerminal.name = Net->name;

			for(auto i = 0; i < Net->pinList.size(); i++){
				posX = Net->InstList.at(i)->locationX;
				posY = Net->InstList.at(i)->locationY;
				//getPinPosition(Net->InstList.at(i), Net->pinList.at(i), posX, posY);

				tempTerminal.locationX = posX - ( terminalSize / 2 );
				tempTerminal.locationY = posY - ( terminalSize / 2 );


				if(terminal_check(tempTerminal, terminalList, terminalSpacing, terminalSize, anyDie)){
					#ifdef TERMINAL_PLACING_DEBUG
					cout << "terminal placing : " << tempTerminal.name << " in " << tempTerminal.locationX << " , " << tempTerminal.locationY << "\n";
					#endif // TERMINAL_PLACING_DEBUG

					break;
				}
				else if(i == Net->pinList.size() - 1){
					// 需要處理所有Pin的位置都無法放置此Terminal問題
					#ifdef TERMINAL_PLACING_DEBUG
					cout << "terminal placing : " << tempTerminal.name << " cant not place in pin position\n";
					#endif // TERMINAL_PLACING_DEBUG

					int minX, minY, maxX, maxY;
					Net->getHpwlPoint(minX, minY, maxX, maxY, ( *anyDie ).maxX, ( *anyDie ).maxY);

					for(int x = minX; x <= maxX; x++){
						for(int y = minY; y <= maxY; y++){
							tempTerminal.locationX = x;
							tempTerminal.locationY = y;
							if(terminal_check(tempTerminal, terminalList, terminalSpacing, terminalSize, anyDie)) break;
						}
					}

				}
			}

			terminalList.push_back(tempTerminal);
		}	
	}
	

	#ifdef TERMINAL_PLACING_DEBUG
	cout << "--- terminal Placing Complete ---" << endl;
	#endif // TERMINAL_PLACING_DEBUG
}

// 回傳terminal是否滿足擺放條件
inline bool terminal_check(Terminal& tempTerminal, vector<Terminal>& terminalList, int& terminalSpacing, int& terminalSize, Die* anyDie){

	if(( tempTerminal.locationX ) - anyDie->minX < terminalSpacing) return false;
	else if(( tempTerminal.locationY ) - anyDie->minY < terminalSpacing) return false;
	else if(anyDie->maxX - ( tempTerminal.locationX ) > terminalSpacing) return false;
	else if(anyDie->maxY - ( tempTerminal.locationY ) > terminalSpacing) return false;
	else{
		int distance = 0;
		for(auto&& terminal : terminalList){
			distance = sqrt(( ( tempTerminal.locationX - terminal.locationX ) * ( tempTerminal.locationX - terminal.locationX ) ) + ( ( tempTerminal.locationY - terminal.locationY ) * ( tempTerminal.locationY - terminal.locationY ) ));
			if(distance < terminalSpacing) return false;
		}
		return true;
	}
}