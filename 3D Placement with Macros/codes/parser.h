#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

// input data
void parser(ifstream&, int&, int&, int&, vector<Die>&, vector<Inst>&, vector<Net>&);

void parser(ifstream& infile, int& TerminalSize, int& terminalSpacing, int& TerminalCost, vector<Die>& Die, vector<Inst>& instances, vector<Net>& NetList) {
    cout <<"--- start parsing ---" <<endl;
    int NumTechnologies = 0;
    string buffer;
    infile >> buffer;
    infile >> NumTechnologies;
    vector<Tech> Technologies(0);
    for (int i = 0; i < NumTechnologies; i++) {
        infile >> buffer;
        Tech tempTech;
        int LibCellCount = 0;
        infile >> tempTech.name >> LibCellCount;
        for (int j = 0; j < LibCellCount; j++) {
            LibCell tempLibCell;
            infile >> buffer >> buffer;
            if (buffer == "Y")
                tempLibCell.isMacro = 1;
            else
                tempLibCell.isMacro = 0;
            infile >> tempLibCell.name >> tempLibCell.sizeX >> tempLibCell.sizeY;
            int pinquantity = 0;
            infile >> pinquantity;
            for (int k = 0; k < pinquantity; k++) {
                Pin temppin;
                infile >> buffer >> temppin.name >> temppin.locationX >> temppin.locationY;
                tempLibCell.pinList.push_back(temppin);
            }
            tempTech.library.push_back(tempLibCell);
            tempLibCell.pinList.clear();
        }
        Technologies.push_back(tempTech);
        tempTech.library.clear();
    }
    infile >> buffer >> Die.at(0).minX >> Die.at(0).minY >> Die.at(0).maxX >> Die.at(0).maxY;
    Die.at(1).minX = Die.at(0).minX;
    Die.at(1).minY = Die.at(0).minY;
    Die.at(1).maxX = Die.at(0).maxX;
    Die.at(1).maxY = Die.at(0).maxY;
    infile >> buffer >> Die.at(0).DieMaxUtil >> buffer >> Die.at(1).DieMaxUtil;
    for (int i = 0; i < 2; i++) {
        Row tempDieRow;
        infile >> buffer >> tempDieRow.startX >> tempDieRow.startY >> tempDieRow.rowLength >> tempDieRow.rowHeight >> tempDieRow.repeatCount;
        Die.at(i).DieRow = tempDieRow;
    }
    string TopDieTech = "", BottomDieTech = "";
    infile >> buffer >> TopDieTech >> buffer >> BottomDieTech;
    if(TopDieTech == Technologies.at(0).name && BottomDieTech == Technologies.at(0).name){
        Die.at(0).Technology = Technologies.at(0);
        Die.at(1).Technology = Technologies.at(0);
    }
    else if (TopDieTech == Technologies.at(0).name) {
        Die.at(0).Technology = Technologies.at(0);
        Die.at(1).Technology = Technologies.at(1);
    }
    else {
        Die.at(0).Technology = Technologies.at(1);
        Die.at(1).Technology = Technologies.at(0);
    }
    infile >> buffer >> TerminalSize >> buffer >> buffer >> terminalSpacing >> buffer >> TerminalCost;
    cout << "   Instances   " <<endl;
    int NumInst = 0;
    infile >> buffer >> NumInst;
    for (int i = 0; i < NumInst; i++) {
        Inst tempInstance;
        string libCellName;
        infile >> buffer >> tempInstance.name >> libCellName;
        for (int j = 0; j < Die.at(0).Technology.library.size(); j++) {
            if (libCellName == Die.at(0).Technology.library.at(j).name) {
                tempInstance.libCell.push_back(&Die.at(0).Technology.library.at(j));
                tempInstance.libCell.push_back(&Die.at(1).Technology.library.at(j));
                tempInstance.pinList = Technologies.at(0).library.at(j).pinList;
                // pin have location but not right one, remember to overwrite
            }
        }
        tempInstance.orientation = Orientation::R0;
        instances.push_back(tempInstance);
        tempInstance.libCell.clear();
        tempInstance.pinList.clear();
    }
    // parsing Nets
    cout << "   Nets   " <<endl;
    int NumNet = 0;
    infile >> buffer >> NumNet;
    for (int i = 0; i < NumNet; i++) {
        //cout << "Netnumber: "<< i <<" complete"<<endl;
        class Net tempNet;
        infile >> buffer >> tempNet.name >> tempNet.NumInstances;
        
        for (int j = 0; j < tempNet.NumInstances; j++) {

            infile >> buffer >> buffer;
            string Instname, pinname;
            int Inst_index = 0, pin_index = 0; 

            // adding each of the Instances to the Net's InstList
            Inst* InstPtr;
            Instname = buffer.substr(0, buffer.find("/"));
            Inst_index = stoi(Instname.substr(1))-1;
            InstPtr = &instances.at(Inst_index);
            tempNet.InstList.push_back(InstPtr);

            // adding the pin on the Inst to the Net's pinList
            Pin* pinPtr;
            pinname = buffer.substr(buffer.find("/")+1);
            pin_index = stoi(pinname.substr(1))-1;
            pinPtr = &instances.at(Inst_index).pinList.at(pin_index);
            tempNet.pinList.push_back(pinPtr); 

        }
        NetList.push_back(tempNet);
        tempNet.pinList.clear();
        tempNet.InstList.clear();
    }
    cout <<"--- done parsing ---" <<endl;
}