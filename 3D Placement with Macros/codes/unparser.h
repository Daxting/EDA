#pragma once
#include <iostream>
#include <vector>
#include "data_structure.h"
using namespace std;

inline void unparser(ofstream& outfile, vector<Inst*>& topDieInstList, vector<Inst*>& bottomDieInstList, vector<Terminal>& terminalList){

	cout << "--- Genearate Output File Start ---" << endl;
	
		outfile << "TopDiePlacement " << topDieInstList.size() << endl;
	for(auto&& topDieInst : topDieInstList)
		outfile << "Inst " << topDieInst->name << " " << topDieInst->locationX << " " << topDieInst->locationY << " " << toString(topDieInst->orientation) << endl;
	outfile << "BottomDiePlacement " << bottomDieInstList.size() << endl;
	for(auto&& bottomDieInst : bottomDieInstList)
		outfile << "Inst " << bottomDieInst->name << " " << bottomDieInst->locationX << " " << bottomDieInst->locationY << " " << toString(bottomDieInst->orientation) << endl;
	outfile << "NumTerminals " << terminalList.size() << endl;
	for(auto&& terminal : terminalList)
		outfile << "Terminal " << terminal.name << " " << terminal.locationX << " " << terminal.locationY << endl;

	/* before c++11
	outfile << "TopDiePlacement " << topDieInstList.size() << endl;
	for(int i = 0; i < topDieInstList.size(); i++)
		outfile << "Inst " << topDieInstList.at(i).name << " " << topDieInstList.at(i).locationX << " " << topDieInstList.at(i).locationY << toString(topDieInstList.at(i).orientation) << endl;
	outfile << "BottomDiePlacement " << bottomDieInstList.size() << endl;
	for(int i = 0; i < bottomDieInstList.size(); i++)
		outfile << "Inst " << bottomDieInstList.at(i).name << " " << bottomDieInstList.at(i).locationX << " " << bottomDieInstList.at(i).locationY << toString(bottomDieInstList.at(i).orientation) << endl;
	outfile << "NumTerminals " << terminalList.size() << endl;
	for(int i = 0; i < terminalList.size(); i++)
		outfile << "Terminal " << terminalList.at(i).name << " " << terminalList.at(i).locationX << " " << terminalList.at(i).locationY << endl;
	*/
	cout << "--- Genearate Output File Complete ---" << endl;
	return;
}