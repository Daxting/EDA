#include "filepath.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#ifdef WIN
#include "library\\data_structure.h"
#include "library\\floorplanning.h"
#include "library\\parser.h"
#include "library\\unparser.h"
#include "library\\partitioning.h"
#include "library\\grid_unfold.h"
#include "library\\terminalplacing.h"
#endif

#ifdef MAC
#include "library/data_structure.h"
#include "library/floorplanning.h"
#include "library/parser.h"
#include "library/unparser.h"
#include "library/partitioning.h"
#include "library/grid_unfold.h"
#include "library/terminalplacing.h"
#endif

#ifdef TSRI
#include "data_structure.h"
#include "floorplanning.h"
#include "parser.h"
#include "unparser.h"
#include "partitioning.h"
#include "grid_unfold.h"
#include "terminalplacing.h"
#endif

using namespace std;

int main(int argc, char *argv[]){
    // filepath format in different OS
    #ifdef WIN
    string file_location = "testcases\\ProblemB_case2.txt";
    ifstream infile(file_location, std::ios::in);
    #endif
    #ifdef MAC
    string file_location = "testcases/ProblemB_case2.txt";
    ifstream infile(file_location, std::ios::in);
    #endif
    #ifdef TSRI
    ifstream infile(argv[1], std::ios::in);
    #endif

    if(!infile){
        cout << "The file is not found. Please input one more time." << endl;
        return 0;
    }

    int terminalSize = 0, terminalSpacing = 0, TerminalCost = 0;
    vector<Die> Die(2);
    vector<Inst> instances(0);
    vector<Net> NetList(0);
    parser(infile, terminalSize, terminalSpacing, TerminalCost, Die, instances, NetList);
    ////////////////////////
    //categorizing the vector<inst>Instances to two die
    //this part may be done by partition
    vector<Inst*> topDieCellList;
    vector<Inst*> topDieMacroList;
    vector<Inst*> topDieStdCellList;
    vector<Inst*> bottomDieCellList;
    vector<Inst*> bottomDieMacroList;
    vector<Inst*> bottomDieStdCellList;
    vector<Node> NodeList;
    vector<Node> topNodeList;
    vector<Node> bottomNodeList;
    vector<Net*> cutNetList;
    partitioning(&NodeList, &NetList, &instances, &Die, &cutNetList);
    //注意!! 目前只單純將Macro放在topDie、stdCell放在bottomDie上
    // for(auto&& Inst : instances){
    //     if(!Inst.libCell[1]->isMacro){
    //         bottomDieCellList.push_back(&Inst);
    //         bottomDieStdCellList.push_back(&Inst);
    //     }  
    //     else{
    //         topDieCellList.push_back(&Inst);
    //         topDieMacroList.push_back(&Inst);
    //     }
    // }
    //
    //test2, try to partition into half by random
    // for(int i=0;i<instances.size();i++){
    //     if(i%2)
    //         instances[i].isBottom=0;
    //     else
    //         instances[i].isBottom=1;
    // }
    //end of the partition
    //


    //categorizing
    for(int i=0;i<NodeList.size();i++){
        if(NodeList[i].node_partition==TOP){
            topNodeList.push_back(NodeList[i]);
            for(int j=0;j<NodeList[i].Instlist.size();j++){
                topDieCellList.push_back(NodeList[i].Instlist[j]);
                if(NodeList[i].Instlist[j]->libCell[0]->isMacro){
                    topDieMacroList.push_back(NodeList[i].Instlist[j]);
                }
                else{
                    topDieStdCellList.push_back(NodeList[i].Instlist[j]);
                }
            }
        }
        else{
            bottomNodeList.push_back(NodeList[i]);
            for(int j=0;j<NodeList[i].Instlist.size();j++){
                bottomDieCellList.push_back(NodeList[i].Instlist[j]);
                if(NodeList[i].Instlist[j]->libCell[1]->isMacro){
                    bottomDieMacroList.push_back(NodeList[i].Instlist[j]);
                }
                else{
                    bottomDieStdCellList.push_back(NodeList[i].Instlist[j]);
                }
            }
        }
    }
    Grid topGrid(topDieStdCellList, topDieMacroList, &Die[0], 0);
        //grid construction
    Grid bottomGrid(bottomDieStdCellList, bottomDieMacroList, &Die[1], 1);
    //floorplanning
    floorplanning_in_grid(topDieStdCellList, topDieMacroList, topGrid, NodeList, NetList, &Die[0], 0);
    floorplanning_in_grid(bottomDieStdCellList, bottomDieMacroList, bottomGrid, NodeList, NetList, &Die[1], 1);

    grid_unfold(topDieStdCellList, topDieMacroList, &topGrid, &Die[0]);
    grid_unfold(bottomDieStdCellList, bottomDieMacroList, &bottomGrid, &Die[1]);

    vector<Terminal> terminalList;
    terminal_placing(cutNetList, terminalList, terminalSpacing, terminalSize, &Die[0]);



	#ifdef WIN
	string outfile_location = "testcases\\result.txt";
    ofstream outfile(outfile_location);
	#endif
	#ifdef MAC
	string outfile_location = "testcases/result.txt";
    ofstream outfile(outfile_location);
	#endif
    #ifdef TSRI
    ofstream outfile(argv[2]);
	#endif

    unparser(outfile, topDieCellList, bottomDieCellList, terminalList);
    cout << "--- Program Complete ---" << endl;
    return 0;
}



