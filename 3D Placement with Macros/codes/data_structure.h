#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Tech;
class Row;
class Die;
class Pin;
class Terminal;
class LibCell;
class Inst;
class Net;
class Node;
enum class Orientation;

inline Orientation toOrientation(std::string& _orientation);
inline std::string toString(Orientation& _orientation);
inline void getPinPosition(Inst* inst, Pin* pin, int& posX, int& posY);

// 方向
// R90 逆時鐘轉90度
enum class Orientation : int{
	R0,
	R90,
	R180,
	R270
};

// 製程
// @param name 製程名稱
// @param library 製程下單元清單
class Tech{
public:
	string name; // 製程名稱
	vector<LibCell> library; // 製程下單元清單
	Tech() = default;
	Tech(string _name, vector<LibCell>_library) :
		name(_name), library(_library){}
};

// 列
// @param repeatCount 列數量
// @param startX,startY 列起始座標
// @param rowLength,rowHeight 列長寬
class Row{
public:
	int repeatCount; // 列數量
	int startX, startY; // 列起始座標
	int rowLength, rowHeight; // 列長寬

	Row() : repeatCount(0), startX(0), startY(0), rowLength(0), rowHeight(0){}
	Row(int _repeatCount, int _startX, int _startY, int _rowLength, int _rowHeight) :
		repeatCount(_repeatCount), startX(_startX), startY(_startY), rowLength(_rowLength), rowHeight(_rowHeight){}
};

// 晶片
// bottom = 1, Top = 0
// @param minX,minY,maxX,maxY 晶片座標
// @param DieMaxUtil 晶片最大使用率
// @param Technology 晶片製程
// @param DieRow 晶片列定義
class Die{
public:
	int minX, minY, maxX, maxY; // 晶片座標
	int DieMaxUtil; // 晶片最大使用率
	Tech Technology; // 晶片製程
	Row DieRow; // 晶片列定義

	Die() : minX(0), minY(0), maxX(0), maxY(0), DieMaxUtil(0){}
	Die(int _minX, int _minY, int _maxX, int _maxY, int _DieMaxUtil, Tech _Technology, Row _DieRow) :
		minX(_minX), minY(_minY), maxX(_maxX), maxY(_maxY), DieMaxUtil(_DieMaxUtil), Technology(_Technology), DieRow(_DieRow){}
};

// 腳位
// @param name 腳位名稱
// @param locationX,locationY 腳位座標
class Pin{
public:
	string name; // 腳位名稱
	int locationX, locationY; // 腳位座標
	Pin() : locationX(0), locationY(0){}
	Pin(string _name, int _locationX, int _locationY) :
		name(_name), locationX(_locationX), locationY(_locationY){}
};

// 通道
// @param name 通道名稱
// @param locationX,locationY 通道座標
class Terminal{
public:
	string name; // 通道名稱
	int locationX, locationY; // 通道座標
	Terminal() : locationX(0), locationY(0){}
	Terminal(string _name, int _locationX, int _locationY) :
		name(_name), locationX(_locationX), locationY(_locationY){}
};

// 單元
// @param name 單元名稱
// @param isMacro 是否為巨集
// @param sizeX,sizeY 單元大小
// @param pinList 單元下腳位清單
class LibCell{
public:
	string name; // 單元名稱
	bool isMacro; // 是否為巨集
	int sizeX, sizeY; // 單元大小
	vector<Pin> pinList; // 單元下腳位清單
	LibCell() : sizeX(0), sizeY(0){}
	LibCell(string _name, bool _isMacro, int _sizeX, int _sizeY, vector<Pin> _pinList) :
		name(_name), isMacro(_isMacro), sizeX(_sizeX), sizeY(_sizeY), pinList(_pinList){}
};

// 實體
// @param name 實體名稱
// @param locationX, locationY 實體座標
// @param orientation 實體旋轉方向
// @param die 實體所屬晶片
// @param pinList 實體下腳位清單
// @param libCell 實體所屬單元模型
// @param connectedNet 實體連接線路
class Inst{
public:
	string name; // 實體名稱
	int locationX, locationY; // 實體座標
	Orientation orientation; // 實體旋轉方向
	bool isBottom; // 實體所屬晶片
	vector<Pin> pinList; // 實體下腳位清單
	vector<LibCell*> libCell; // 實體所屬單元模型
	vector<Net*> connectedNet; // 實體連接線路
	Inst() : locationX(0), locationY(0), orientation(Orientation::R0), isBottom(0){}
	Inst(string _name, int _locationX, int _locationY, Orientation _orientation, bool _isBottom, vector<Pin> _pinList, vector<LibCell*> _libCell) :
		name(_name), locationX(_locationX), locationY(_locationY), orientation(_orientation), isBottom(_isBottom), pinList(_pinList), libCell(_libCell){}
	void update_pin_location(int x, int y){ // 更新腳位座標 x, y方向更動
		locationX += x;
		locationY += y;
		for(int i = 0; i < pinList.size(); i++){
			pinList.at(i).locationX += x;
			pinList.at(i).locationY += y;
		}
	}
	void update_pin_location(Orientation orientation){ // 更新腳位座標 旋轉 // Inst左下座標不變
		for(int i = 0; i < pinList.size(); i++){
			pinList.at(i).locationX -= locationX;
			pinList.at(i).locationY -= locationY;
			int templocation = pinList.at(i).locationX;
			if(orientation == Orientation::R90){
				if(isBottom){
					pinList.at(i).locationX = -pinList.at(i).locationY + libCell.at(1)->sizeY;
					pinList.at(i).locationY = templocation;
				}
				else{
					pinList.at(i).locationX = -pinList.at(i).locationY + libCell.at(0)->sizeY;
					pinList.at(i).locationY = templocation;
				}
			}
			else if(orientation == Orientation::R180){
				if(isBottom){
					pinList.at(i).locationX -= ( 2 * pinList.at(i).locationX - libCell.at(1)->sizeX );
					pinList.at(i).locationY -= ( 2 * pinList.at(i).locationY - libCell.at(1)->sizeY );
				}
				else{
					pinList.at(i).locationX -= ( 2 * pinList.at(i).locationX - libCell.at(0)->sizeX );
					pinList.at(i).locationY -= ( 2 * pinList.at(i).locationY - libCell.at(0)->sizeY );
				}
			}
			else if(orientation == Orientation::R270){
				if(isBottom){
					pinList.at(i).locationX = pinList.at(i).locationY;
					pinList.at(i).locationY = -templocation + libCell.at(1)->sizeX;
				}
				else{
					pinList.at(i).locationX = pinList.at(i).locationY;
					pinList.at(i).locationY = -templocation + libCell.at(0)->sizeX;
				}
			}
			pinList.at(i).locationX += locationX;
			pinList.at(i).locationY += locationY;
		}
	}
	bool operator==(Inst instance){
		if(name == instance.name)
			return true;
		else
			return false;
	}
};

// 線路
// @param name 線路名稱
// @param isCut 是否跨越兩個Die
// @param NumInstances 線路所接實體總數
// @param InstList 線路下實體清單
// @param pinList 線路下腳位清單
class Net{
public:
	string name; // 線路名稱
	bool isCut; // 是否跨越兩個Die
	int NumInstances; // 線路所接實體總數
	int TopNodesNum, BottomNodesNum; // 晶片上線路連接節點數量
	vector<Inst*> InstList; // 線路下實體清單
	vector<Pin*> pinList; // 線路下腳位清單
	vector<Node*> NodeList; // 線路下節點清單
	Net() : NumInstances(0), TopNodesNum(0), BottomNodesNum(0){}
	Net(string _name, int _NumInstances, vector<Inst*> _InstList, vector<Pin*> _pinList) :
		name(_name), NumInstances(_NumInstances), InstList(_InstList), pinList(_pinList){}
	bool operator==(Net net){
		if(name == net.name)
			return true;
		else
			return false;
	}

	// 回傳Net下所有Pin所圈出最大矩形四點座標
	inline void getHpwlPoint(int& minX, int& minY, int& maxX, int& maxY, const int DieX, const int DieY){
		int tempMinX = DieX, tempMinY = DieY, tempMaxX = 0, tempMaxY = 0;
		int posX, posY;

		auto Inst = InstList.begin();
		auto Pin = pinList.begin();
		while(Inst == InstList.end() || Pin == pinList.end()){
			posX = ( *Pin )->locationX;
			posY = ( *Pin )->locationY;
			//getPinPosition(*Inst, *Pin, posX, posY);

			if(posX > tempMaxX) tempMaxX = posX;
			if(posX < tempMinX) tempMinX = posX;
			if(posY > tempMaxY) tempMaxY = posY;
			if(posY < tempMinY) tempMinY = posY;

			Inst++;
			Pin++;
		}

		minX = tempMinX;
		minY = tempMinY;
		maxX = tempMaxX;
		maxY = tempMaxY;
	}

    inline int getHPWlL(){
        int Xmin, Xmax, Ymin, Ymax;
        getHpwlPoint(Xmin, Ymin, Xmax, Ymax, 2147483647, 2147483647);
        return Xmax-Xmin+Ymax-Ymin;
    }
};

typedef enum{
	NO_PART = -1,
	TOP = 0,
	BOTTOM = 1,
} NODE_PART_NAME;

typedef enum{
	UNLOCKED = 0,
	LOCKED = 1,

} LOCK_STATE;

class Node{
public:
	string name;
	int node_gain;
	int TOP_area, BOTTOM_area;
	LOCK_STATE lock_state;
	NODE_PART_NAME node_partition; // on which die the node locates (TOP/BOTTOM)
	vector<Inst*> Instlist; // the Inst included in this Node
	vector<Net*> connected_nets; // nets connected to this Node
	void calculate_nodes_gain();
	void calculate_total_area();
	Node() = default;
	Node(vector<Inst*> _Instlist) : Instlist(_Instlist){}
};

inline Orientation toOrientation(std::string& _orientation){
	switch(_orientation[1]){
	case '0':
		return Orientation::R0;
		break;
	case '9':
		return Orientation::R90;
		break;
	case '1':
		return Orientation::R180;
		break;
	default:
		return Orientation::R270;
		break;
	}
}

inline std::string toString(Orientation& _orientation){
	if(_orientation == Orientation::R0) return "R0";
	else if(_orientation == Orientation::R90) return "R90";
	else if(_orientation == Orientation::R180) return "R180";
	else return "R270";
}

// 獲取Pin的絕對座標(目前棄用)
inline void getPinPosition(Inst* inst, Pin* pin, int& posX, int& posY){
	if(inst->orientation == Orientation::R0){
		posX = inst->locationX + pin->locationX;
		posY = inst->locationY + pin->locationY;
	}
	else if(inst->orientation == Orientation::R90){
		posX = inst->locationX + pin->locationY;
		posY = inst->locationY + ( *inst->libCell.begin() )->sizeX - pin->locationX;
	}
	else if(inst->orientation == Orientation::R180){
		posX = inst->locationX + ( *inst->libCell.begin() )->sizeX - pin->locationX;
		posY = inst->locationY + ( *inst->libCell.begin() )->sizeY - pin->locationY;
	}
	else if(inst->orientation == Orientation::R270){
		posX = inst->locationX + ( *inst->libCell.begin() )->sizeY - pin->locationY;
		posY = inst->locationY + pin->locationX;
	}
}


enum class Location : int{//the enum is designed for macro floorplanning
    LeftUpper,
    RightUpper,
    RightLower,
    LeftLower
};

class Grid {
public:
    int column_number, row_number;
    double row_high, column_width;
    vector<vector<Inst*>> address;
    vector<vector<bool>> occupied;
    int over_enough, enough, not_enough;//concentrate

    Grid(){
        column_number=0;
        row_number=0;
        row_high=0;
        column_width=0;
        //the following two variables is transformed from the die for the convenience of floorplanning
        //Remember! address[number of row][numeber of column], it's NOT like a cartesian coordinate system
        address=vector<vector<Inst*>>(0);
        occupied=vector<vector<bool>>(0);
        over_enough=0;
        enough=0;
        not_enough=0;
    }
    Grid(vector<Inst*> stdCellList, vector<Inst*> MacroList, Die* Die, bool isBottom){
        row_high=Die[isBottom].DieRow.rowHeight;
        column_width=find_average_width(stdCellList, MacroList, Die, isBottom);
        column_number=Die[isBottom].DieRow.rowLength/column_width;
        row_number=Die[isBottom].DieRow.repeatCount;
        address=vector<vector<Inst*>>(row_number, vector<Inst*>(column_number, nullptr));
        occupied=vector<vector<bool>>(row_number, vector<bool>(column_number, 0));
        not_enough=enough_counter(column_number, row_number, stdCellList.size());
        enough=not_enough-1;
        over_enough=enough-1;
    }


    bool macro_placing(Inst* madd, int column_location, int row_location, bool isBottom, Location loc){
        #ifdef DEBUG
            cout << "------ Macro placing  -----"<<endl;
        #endif


        bool ifFail=false;
        //determine if the space is enough for the macro
        if(loc==Location::LeftLower){
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location+j][column_location+i])  ifFail=true;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location+j][column_location+i])  ifFail=true;
                    }
                }

            }
        }
        else if(loc==Location::LeftUpper){
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location-j][column_location+i])  ifFail=true;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location-j][column_location+i])  ifFail=true;
                    }
                }

            }
        }
        else if(loc==Location::RightUpper){
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location-j][column_location-i])  ifFail=true;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location-j][column_location-i])  ifFail=true;
                    }
                }

            }
        }
        else{
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location+j][column_location-i])  ifFail=true;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        if(occupied[row_location+j][column_location-i])  ifFail=true;
                    }
                }

            }
        }

        //placing or not placing
        if(!ifFail){
            macro_placement(madd, column_location, row_location, isBottom, loc);
            #ifdef DEBUG
                cout<<"Macro placing succeed"<<endl;
            #endif
            return true;
        }
        else{
            #ifdef DEBUG
                cout<<"Macro placing not succeed"<<endl;
            #endif
            return false;
        }
    }

    void macro_placement(Inst* madd, int column_location, int row_location, bool isBottom, Location loc){
        //the function is called by macro_placing
        //the function will place the macro on the grid without considering if there is enough space
        #ifdef DEBUG
            cout << "------ Macro placement  -----"<<endl;
        #endif
        if(loc==Location::LeftLower){
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location+j][column_location+i]=true;
                        address[row_location+j][column_location+i]=madd;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location+j][column_location+i]=true;
                        address[row_location+j][column_location+i]=madd;
                    }
                }

            }
        }
        else if(loc==Location::LeftUpper){
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location-j][column_location+i]=true;
                        address[row_location-j][column_location+i]=madd;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location-j][column_location+i]=true;
                        address[row_location-j][column_location+i]=madd;
                    }
                }

            }
        }
        else if(loc==Location::RightUpper){
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location-j][column_location-i]=true;
                        address[row_location-j][column_location-i]=madd;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location-j][column_location-i]=true;
                        address[row_location-j][column_location-i]=madd;
                    }
                }

            }
        }
        else{
            if(madd->orientation==Orientation::R0 || madd->orientation==Orientation::R180){
                int ho= madd->libCell[isBottom]->sizeX/column_width+1;
                int vo= madd->libCell[isBottom]->sizeY/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location+j][column_location-i]=true;
                        address[row_location+j][column_location-i]=madd;
                    }
                }
            }
            else{
                int ho= madd->libCell[isBottom]->sizeY/column_width+1;
                int vo= madd->libCell[isBottom]->sizeX/row_high+1;
                for(int i=0;i<ho;i++){
                    for(int j=0;j<vo;j++){
                        occupied[row_location+j][column_location-i]=true;
                        address[row_location+j][column_location-i]=madd;
                    }
                }

            }
        }
    }

    void write_address(){
        #ifdef DEBUG
            cout << "------ write location to the instances address-----"<<endl;
        #endif
        for(int i=row_number-1;i>=0;i--){
            for(int j=column_number-1;j>=0;j--){
                if(occupied[i][j]){
                    address[i][j]->locationX=column_width*j;
                    address[i][j]->locationY=row_high*i;
                }
            }
        }
    }

    int enough_counter(int column_number, int row_number, int CellNumber){
        int length=0;
        do{
            length++;
        }while((column_number-length)*(row_number-length)>=CellNumber);
        return length;
    }

    double find_average_width(vector<Inst*> stdCellList, vector<Inst*> MacroList, Die* Die, bool isBottom){
        cout << "--- find_average_width start ---" << endl;
        //first, find the average width of all stdcells on a single die
        double AverageWidth=0, width=0;
        for(int i=0;i<stdCellList.size();i++){
            width+=stdCellList[i]->libCell[isBottom]->sizeX;
        }
        for(int i=0;i<MacroList.size();i++){
            if(MacroList[i]->libCell[isBottom]->sizeX > MacroList[i]->libCell[isBottom]->sizeY){
                MacroList[i]->orientation=Orientation::R90;
                width+=MacroList[i]->libCell[isBottom]->sizeY;
            }
            else
                width+=MacroList[i]->libCell[isBottom]->sizeX;
        }
        AverageWidth=width/double(stdCellList.size());
        #ifdef DEBUG
            cout<<"stdCell number: "<<stdCellList.size()<<endl;
            cout<<"macro number: "<<MacroList.size()+MacroList.size()<<endl;
            cout<<"Instance number: "<<stdCellList.size()+MacroList.size()<<endl;
            cout<<"total length: "<<width<<endl;
            cout<<"AverageWidth: "<<AverageWidth<<endl;
        #endif
        cout << "--- find_average_width finish ---" << endl;
    return AverageWidth;
}

    bool find_space_for_stdcell(int& locX, int& locY, bool enough=false){
        //the function is designed to find the center of a region of related stdcell
        //enough means the region of center is confined to not_enough
        #ifdef DEBUG
            cout<<"find_space_for_stdcell processing!"<<endl;
            cout<<boolalpha<<"enough region: "<<enough<<endl;
            cout<<boolalpha<<"not_enough region: "<<!enough<<endl;
        #endif        
        bool no_space=false;//is a lock to 
        if(!enough){
            for(int i=row_number-not_enough-1;i>=not_enough;i--){
                for(int j=not_enough;j<column_number-not_enough;j++){
                    if(!occupied[i][j]){
                        locX=j, 
                        locY=i;
                        return true;
                    }
                }
            }
        }
        if(!no_space){
            no_space=true;
            return false;
        }
        if(enough){
            //consider row region of over_enough
            for(int i=row_number-over_enough-1;i>=over_enough;i--){
                if(!occupied[column_number-over_enough-1][i]){
                    locX=i;
                    locY=column_number-over_enough-1;
                    return true;
                }
                if(!occupied[over_enough][i]){
                    locX=i;
                    locY=over_enough;
                    return true;
                }
            }
            //consider column region of over_enough
            for(int j=over_enough;j<column_number-over_enough;j++){
                if(!occupied[j][over_enough]){
                    locX=over_enough, 
                    locY=j;
                    return true;
                }
                if(!occupied[j][row_number-over_enough-1]){
                    locX=row_number-over_enough-1, 
                    locY=j;
                    return true;
                }
            }
        }
        cout<<"ERROR!, there is no space for stdcell to place in the over_enough area!"<<endl;
        return false;
    }

    inline void operator =(Grid grid_copy){
        column_number=grid_copy.column_number;
        row_number=grid_copy.row_number;
        row_high=grid_copy.row_high;
        column_width=grid_copy.column_width;
        over_enough=grid_copy.over_enough;
        enough=grid_copy.enough;
        not_enough=grid_copy.not_enough;
        vector<Inst*> tempinst;
        vector<bool>  tempbool;
        for(int i=0;i<grid_copy.address.size();i++){
            tempbool.clear();
            tempinst.clear();
            for(int j=0;j<grid_copy.address[i].size();j++){
                tempinst.push_back(grid_copy.address[i][j]);
                tempbool.push_back(grid_copy.occupied[i][j]);
            }
            address.push_back(tempinst);
            occupied.push_back(tempbool);
        }
    }

    void switch_SA(int num1, int num2, bool isRow){
        if(isRow){
            vector<Inst*> tempinst;
            vector<bool>  tempbool;
            for(int i=0;i<address[num1].size();i++){
                tempinst.push_back(address[num1][i]);
                tempbool.push_back(occupied[num1][i]);
            }
            address[num1].clear();
            occupied[num1].clear();
            for(int i=0;i<address[num2].size();i++){
                address[num1].push_back(address[num2][i]);
                occupied[num1].push_back(occupied[num2][i]);
            }
            address[num2].clear();
            occupied[num2].clear();
            for(int i=0;i<tempinst.size();i++){
                address[num2].push_back(tempinst[i]);
                occupied[num2].push_back(tempbool[i]);
            }
        }
        else{
            vector<Inst*> tempinst;
            vector<bool>  tempbool;
            for(int i=0;i<address.size();i++){
                tempinst.push_back(address[i][num1]);
                tempbool.push_back(occupied[i][num1]);
            }
            for(int i=0;i<address.size();i++){
                address[i][num1]=address[i][num2];
                occupied[i][num1]=occupied[i][num2];
            }
            for(int i=0;i<tempinst.size();i++){
                address[i][num2]=tempinst[i];
                occupied[i][num2]=tempbool[i];
            }
        }
    }

};