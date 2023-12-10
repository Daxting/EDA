#pragma once
#define DEBUG
#ifdef DEBUG
#define GRID_UNFOLDING_DEBUG
#endif // DEBUG

#include <iostream>
#include <vector>
#include "data_structure.h"
using namespace std;

inline void Macro_placer(int row, vector<int>& RowPos, vector<int>& RowGridPos, Grid* grid, Die* die);

inline void grid_unfold(vector<Inst*>& stdCellList, vector<Inst*>& MacroList, Grid* grid, Die* die){

	// 存放各Row擺放的最右側座標
	vector<int> RowPos(grid->address.size(), 0);
	// 存放各Row已處理的Grid最右側格數
	vector<int> RowGridPos(grid->address.size(), 0);


	for(int row = 0; row < grid->address.size(); row++){

		for(int inst = 0; inst < grid->address.at(row).size(); inst++){

			// 必須是該格有物體且尚未被擺放
			if(grid->occupied[row][inst] && inst > RowGridPos[row]){

				// 已經超出Die範圍的
				// if(RowPos[row] + grid->address[row][inst]->libCell.at(grid->address[row][inst]->isBottom)->sizeX > die->maxX){
				// 	// 需要一個處理方式
				// }
				// 遇到Macro時
				if(grid->address[row][inst]->libCell.at(grid->address[row][inst]->isBottom)->isMacro){

					Macro_placer(row, RowPos, RowGridPos, grid, die);

				}
				// 遇到StdCell時
				else{
					grid->address[row][inst]->locationX = RowPos[row];
					grid->address[row][inst]->locationY = row * grid->row_high;

					RowPos[row] += grid->address[row][inst]->libCell.at(grid->address[row][inst]->isBottom)->sizeX;
					RowGridPos[row] += 1;
				}

			}

		}

	}
}

inline void Macro_placer(int row, vector<int> &RowPos, vector<int> &RowGridPos, Grid* grid, Die* die){

	int &targetGridPos = RowGridPos[row];
	Inst*& targetMacro = grid->address[row][targetGridPos];

	int maxPos = 0;

	// 向上查找此Macro跨過的Row
	for(int macroRow = row; macroRow <= ( die->DieRow.repeatCount - 1) && grid->address[macroRow][targetGridPos] == targetMacro; macroRow++){

		// 擺好Macro左側的所有Inst
		for(int gridPos = RowGridPos[macroRow]; gridPos < targetGridPos; gridPos++){

			if(grid->address[macroRow][gridPos]->libCell.at(grid->address[macroRow][gridPos]->isBottom)->isMacro){
				Macro_placer(macroRow, RowPos, RowGridPos, grid, die);
			}
			else{
				grid->address[macroRow][gridPos]->locationX = RowPos[row];
				grid->address[macroRow][gridPos]->locationY = row * grid->row_high;

				RowPos[macroRow] += grid->address[macroRow][gridPos]->libCell.at(grid->address[macroRow][gridPos]->isBottom)->sizeX;
				RowGridPos[macroRow] += 1;
			}

		}

		// 紀錄各MacroRow中的最右點
		if(RowPos[macroRow] > maxPos) maxPos = RowPos[macroRow];
	}

	// 更新該Macro左下角座標
	targetMacro->locationX = maxPos;
	targetMacro->locationY = row * grid->row_high;

	// 計算Macro寬度占了多少Grid
	int width = 0;
	for(int gridPos = targetGridPos; ( gridPos - targetGridPos + 1 ) * grid->column_width <= targetMacro->libCell.at(targetMacro->isBottom)->sizeX; gridPos++){
		width++;
	}

	// 將各MacroRow的RowPos、RowGridPos對齊，並紀錄Macro已擺置
	for(int macroRow = row; macroRow <= ( die->DieRow.repeatCount - 1 ) && grid->address[macroRow][targetGridPos] == targetMacro; macroRow++){
		RowPos[macroRow] = targetMacro->locationX + targetMacro->libCell.at(targetMacro->isBottom)->sizeX;
		RowGridPos[macroRow] += width;
	}

}