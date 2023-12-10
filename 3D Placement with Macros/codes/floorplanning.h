#pragma once
#include <iostream>
#include <vector>
#include "data_structure.h"
using namespace std;
using std::cout;

void macro_in_grid_placing(vector<Inst*> MacroList, Grid &grid, Die* die, bool isBottom);
void floorplanning_in_grid(vector<Inst*> stdCellList, vector<Inst*> MacroList, Grid& grid, vector<Node> NodeList, vector<Net> &NetList, Die* Die, bool isBottom);
int spiral(int n, int start_column, int start_row, bool isX);
void stdcell_in_grid_placing(vector<Node> NodeList, Grid &grid, Die* die, bool isBottom);
void simulated_annealing(Grid &grid, Die* die, vector<Net> &NetList, bool isBottom);



void floorplanning_in_grid(vector<Inst*> stdCellList, vector<Inst*> MacroList, Grid& grid, vector<Node> NodeList, vector<Net> &NetList, Die* Die, bool isBottom){
    cout << "--- floorplanning_in_grid Start ---" << endl;
    if(!isBottom)   cout<<"(Top die mode)"<<endl;
    else            cout<<"(Bottom die mode)"<<endl;
    //macro in grid placing
    macro_in_grid_placing(MacroList, grid, &Die[isBottom], isBottom);
    //stdcell placing
    stdcell_in_grid_placing(NodeList, grid, &Die[isBottom], isBottom);
    //simulated_annealing optimization
    simulated_annealing(grid, &Die[isBottom], NetList, isBottom);
    //write grid location info to the instances address
    grid.write_address();
    cout << "--- floorplanning_in_grid finish ---" << endl;
    
}

void macro_in_grid_placing(vector<Inst*> MacroList, Grid &grid, Die* die, bool isBottom){
    //macro is transformed to grids which can accomodate it
    //macro is placed in the corner with clockwise sequence
    //for example, the first macro is placed on the left-lower corner
    //and the second one will be placed on the left-upper corner
    //the function has a limitation that if the peripheral space is not enough for macros, it will in a dead loop
    cout << "--- Macro in grid Placing Start ---" << endl;
    Location loc=Location::LeftLower;
    int column_location=0, row_location=0;
    int num=0, deadKey=0;//to prevent dead loop
    while(num<MacroList.size()){
        if(grid.macro_placing(MacroList[num], column_location, row_location, isBottom, loc)){
            num++;
            deadKey=0;
        }
        if(loc==Location::LeftLower){
            loc=Location::LeftUpper;
            deadKey++;
            for(int i=0;i<grid.column_number;i++){
                if(!grid.occupied[grid.row_number-1][i]){
                    column_location=i;
                    row_location=grid.row_number-1;
                }
            }
        }   
        else if(loc==Location::LeftUpper){
            loc=Location::RightUpper;
            deadKey++;
            for(int i=grid.row_number-1;i>=0;i--){
                if(!grid.occupied[i][grid.column_number-1]){
                    column_location=grid.column_number-1;
                    row_location=i;
                }
            }
        }  
        else if(loc==Location::RightUpper){
            loc=Location::RightLower;
            deadKey++;
            for(int i=grid.column_number-1;i>=0;i--){
                if(!grid.occupied[0][i]){
                    column_location=i;
                    row_location=0;
                }
            }
        }  
        else if(loc==Location::RightLower){
            loc=Location::LeftLower;
            deadKey++;
            for(int i=0;i<grid.row_number;i++){
                if(!grid.occupied[i][0]){
                    column_location=0;
                    row_location=i;
                }
            }
        }  
        #ifdef DEBUG
            cout<<"Macro [ "<<num<<" ] in grid placement excution"<<endl;
        #endif        
        if(deadKey>4){
            #ifdef DEBUG
                cout<<"Macro [ "<<num<<" ] can not be place in the peripheral region"<<endl;
            #endif  
            break;
        }
    }
    //find space for macros that can not placed in the peripheral region
    while(num<MacroList.size()){
        #ifdef DEBUG
            cout<<"start to place into internal region"<<endl;
        #endif  
        //can reference to next part...
        cout<<"ERROR"<<endl;
        break;
    }

    cout << "---  Macro in grid Placing finish ---" << endl;
    return;
}

int spiral(int n, int start_column, int start_row, bool isX){
    //start from the left_upper corner
    int x=0,y=0;
    if(isX){
        int count=0;
        while(true){
            if(count*count>=n)
                break;
            count++;
        }
        if(count%2==0){
            x=count/2*-1;
            if(n<count*count){
                if(n<=count*(count-1))
                    x+=count;
                else    
                    x+=count*count-n;
            }
        }
        else{
            x=count/2+1;
            if(n<count*count){
                if(n<=count*(count-1))
                    x-=count;
                else
                    x-=count*count-n;
            }
        }
        return x+start_column;
    }
    else{
        int count=0;
        while(true){
            if(count*(count+1)>=n)
                break;
            count++;
        }
        if(count%2!=0){
            y=count/2*(-1)-1;
            if(n<count*(count+1)){
                if(n<=count*count)
                    y+=count;
                else    
                    y+=count*(count+1)-n;
            }
        }
        else{
            y=count/2;
            if(n<count*(count+1)){
                if(n<=count*count)
                    y-=count;
                else
                    y-=count*(count+1)-n;
            }
        }
        return y+start_row;
    }
}

void stdcell_in_grid_placing(vector<Node> NodeList, Grid &grid, Die* die, bool isBottom){
    cout << "--- stdcell_in_grid_placing Start ---" << endl;
    //put the stdcell on the die by the means of spiral out
    //and the stdcell in one node will be placed in neighborhood
    int start_column;
    int start_row;
    int counter=0;
    bool no_space=false;
    //the center of the node will be lacated at not_enough region first
    //and the instance will not over the region of over_enough
    //if the not_enough region is not big enough for all the instance
    //the center of the node will be lacated at enough region latter and will not be limited by predefined region
    for(int i=0;i<NodeList.size();i++){
        if(!no_space && grid.find_space_for_stdcell(start_column, start_row, false)==false){
            no_space=true;
            #ifdef DEBUG
                cout<<"stdcell in grid placement region is extended to all the die"<<endl;
            #endif  
        }
        if(no_space=true)
            grid.find_space_for_stdcell(start_column, start_row, true);
        int j=0;
        #ifdef DEBUG
            cout<<"stdcell in grid placement Node[ "<<i<<" ] excuting"<<endl;
        #endif  
        while(j<NodeList[i].Instlist.size()){
            counter=0;
            if(no_space && grid.occupied[spiral(counter,start_column,start_row, 0)][spiral(counter,start_column,start_row, 1)]
                && spiral(counter,start_column,start_row, 1)>=grid.over_enough 
                && spiral(counter,start_column,start_row, 1)<grid.column_number-grid.over_enough
                && spiral(counter,start_column,start_row, 0)>=grid.over_enough
                && spiral(counter,start_column,start_row, 0)<grid.row_number-grid.over_enough){
                grid.address[spiral(counter,start_column,start_row, 1)][spiral(counter,start_column,start_row, 0)]=NodeList[i].Instlist[j];
                grid.occupied[spiral(counter,start_column,start_row, 1)][spiral(counter,start_column,start_row, 0)]=true;
                counter++;
                j++;
            }
            else if(!no_space && grid.occupied[spiral(counter,start_column,start_row, 1)][spiral(counter,start_column,start_row, 0)]
                && spiral(counter,start_column,start_row, 1)>=0
                && spiral(counter,start_column,start_row, 1)<grid.column_number
                && spiral(counter,start_column,start_row, 0)>=0
                && spiral(counter,start_column,start_row, 0)<grid.row_number){
                grid.address[spiral(counter,start_column,start_row, 1)][spiral(counter,start_column,start_row, 0)]=NodeList[i].Instlist[j];
                grid.occupied[spiral(counter,start_column,start_row, 1)][spiral(counter,start_column,start_row, 0)]=true;
                counter++;
                j++;
            }
            else{
                counter++;
            }
        }
    }
    
    cout << "stdcell_in_grid_placing finish" << endl;
    //print the detail of position for debuging 
    #ifdef DEBUG
        cout << "stdcell_in_grid_placing checking Start"<<endl;
        if(!isBottom)
            cout<<"topdie: "<<endl;
        else
            cout<<"bottomdie: "<<endl;
        for(int i=0;i<NodeList.size();i++){
            for(int j=0;j<NodeList[i].Instlist.size();j++){
                cout <<NodeList[i].Instlist[j]->name <<": ( " << NodeList[i].Instlist[j]->locationX << " , " << NodeList[i].Instlist[j]->locationY << " ) " << endl;
            }
        }
        cout << "stdcell_in_grid_placing checking finish"<<endl;
    #endif
    return;
}

void simulated_annealing(Grid &grid, Die* die, vector<Net> &NetList, bool isBottom){
    //current version: no tolerate for higher HPWL
    //only switch the whole row or column
    //total is time is still need to test!
    cout<<"-----simulated_annealing Start-----"<<endl;
    if(!isBottom)
        cout<<"<<<Top die>>>"<<endl;
    else
        cout<<"<<<Bottom die>>>"<<endl;

    int switch_time=0, total_time=3*(grid.column_number+grid.row_number-2);
    int row_num=0, column_num=0;
    int HpwlLength_original=0, HpwlLength=0;
    for(int time=0;time<total_time;time++){
        if(time%2){
            if(row_num=grid.row_number-1)    row_num=0;
            grid.write_address();
            for(int i=0;i<NetList.size();i++){
                HpwlLength_original+=NetList[i].getHPWlL();
            }
            grid.switch_SA(row_num, row_num+1, true);
            for(int i=0;i<NetList.size();i++){
                HpwlLength+=NetList[i].getHPWlL();
            }
            if(HpwlLength<HpwlLength_original){
                switch_time++;
                #ifdef DEBUG
                    cout<<"ROW SWITCH!"<<endl;
                #endif 
            }
            else{   // turn to original
                grid.switch_SA(row_num, row_num+1, true);
            }
            HpwlLength=0;
            HpwlLength_original=0;
            row_num++;
        }
        else{
            if(column_num=grid.column_number-1)    column_num=0;
            grid.write_address();
            for(int i=0;i<NetList.size();i++){
                HpwlLength_original+=NetList[i].getHPWlL();
            }
            grid.switch_SA(column_num, column_num+1, false);
            for(int i=0;i<NetList.size();i++){
                HpwlLength+=NetList[i].getHPWlL();
            }
            if(HpwlLength<HpwlLength_original){
                switch_time++;
                #ifdef DEBUG
                    cout<<"COLUMN SWITCH!"<<endl;
                #endif 
            }
            else{   // turn to original
                grid.switch_SA(column_num, column_num+1, false);
            }
            HpwlLength=0;
            HpwlLength_original=0;
            column_num++;
        }
        #ifdef DEBUG
            cout<<"simulated_annealing "<<switch_time<<" times SWITCH!"<<endl;
            cout<<"simulated_annealing: "<<time<<" / "<<total_time<<" times have been done"<<endl;
        #endif  
    }
    cout<<"total swich times: "<<switch_time<<endl; 
    cout<<"-----simulated_annealing Finish-----"<<endl;
}