#pragma once 
#define DEBUG
#include <iostream> 
#include <vector>
#include <map>
#include <stdlib.h> 
#include <time.h>  
#include <math.h>
#include "data_structure.h"
using namespace std; 

int NUM_PASSES = 5;
unsigned int CUT_SET;
unsigned int MIN_CUT_SET;

// Total area used by the nodes
long int TOP_AREA_USED;
long int BOTTOM_AREA_USED;
long int MAX_TOP_AREA;
long int MAX_BOTTOM_AREA;

// vector<Net> NetList;
// vector<Node> NodeList;
vector<Net> MIN_CUT_NET;
vector<Node> MIN_CUT_NODE;
map <int, vector<Node*> > gain_bucket_map;

void output_partition(vector<Node> *NodeList);
void output_NetList(vector<Net> *NetList);
void setup_area_constraint(vector<Die> *Die_vector);
bool check_area_constraint();
bool check_availible_area(NODE_PART_NAME new_node_part, Node* node);
void update_die_area(NODE_PART_NAME new_node_part, Node* node);
void update_node_num(Net &net);
void update_cutstate(vector<Net> *NetList);
void distribute_nodes_by_area(vector<Node> *NodeList);	
void random_partition(vector<Node> *NodeList);
void initial_partition(vector<Node> *NodeList);
void output_gain_bucket();
void initialize_nodes_gain(vector<Node> *NodeList);
int swap_partition(Node *node);
Node* find_max_gain_node();
void recalculate_node_gain(Node *node);
void FM_algorithm(vector<Node> *NodeList, vector<Net> *NetList, vector<Die> *Die_vector);
void Inst_grouping(vector<Node> *NodeList, vector<Inst> *Instances);
void connecting_net_to_inst(vector<Net> *NetList);
void connecting_net_to_node(vector<Node> *NodeList, vector<Net> *NetList);
void calculate_cut(vector<Net> *NetList);
void partitioning(vector<Node> *NodeList, vector<Net> *NetList, vector<Inst> *Instances, vector<Die> *Die_vector, vector<Net*> *cutNetList);
void changing_instance_partition(vector<Node> *NodeList);
inline void quicksort(vector<Node>& a,int L,int R) ; 
inline void node_grouping(vector<Node> *NodeList, vector<Net> *NetList, vector<Inst> instances);
int cut_Node(Node node, vector<Net> *NetList);
inline void node_grouping(vector<Node> *NodeList, vector<Net> *NetList, vector<Inst> instances, vector<Net*> *cutNetList, int stage);




void output_partition(vector<Node> *NodeList){
#ifdef DEBUG
	cout << "\nNode Partition:" << endl;
	for(auto n:(*NodeList)){
		cout << n.name << " ";
		if (n.node_partition == TOP)
			cout << "TOP";
		else if(n.node_partition == BOTTOM)
			cout << "	BOTTOM";
		else{
			cout << "Error output_partition(): invalid node partition" << endl;
		}
		cout << endl;
	}
#endif
}

void output_NetList(vector<Net> *NetList){
#ifdef DEBUG
	// output NetList
	cout << "\nNetList(T/B):" << endl;
	for(auto N:(*NetList)){
		cout << N.name << " " << N.TopNodesNum << "/" << N.BottomNodesNum << (N.isCut ? " Cut " : "     ");
		for(auto node : N.NodeList)
			cout << node->name << " ";
		cout << endl;
	}
#endif
}

void setup_area_constraint(vector<Die> *Die_vector){
	Die* TopDie = &(*Die_vector)[0];
	Die* BottomDie = &(*Die_vector)[1];
	MAX_TOP_AREA = (long int)(TopDie->maxX - TopDie->minX) * (TopDie->maxY - TopDie->minY) * TopDie->DieMaxUtil/100;
	MAX_BOTTOM_AREA = (long int)(BottomDie->maxX - BottomDie->minX) * (BottomDie->maxY - BottomDie->minY) * BottomDie->DieMaxUtil/100;
#ifdef DEBUG
	cout << "MAX_TOP_AREA = " << MAX_TOP_AREA << endl;
	cout << "MAX_BOTTOM_AREA = " << MAX_BOTTOM_AREA << endl;
#endif
}

bool check_area_constraint(){
	return (TOP_AREA_USED <= MAX_TOP_AREA && BOTTOM_AREA_USED <= MAX_BOTTOM_AREA);
}

// check if the Die has enough space to put the node
bool check_availible_area(NODE_PART_NAME new_node_part, Node* node){
	int area_available = 0;
	int insert_area = 0;
	if(new_node_part == TOP){
		area_available = MAX_TOP_AREA - TOP_AREA_USED;
		insert_area = node->TOP_area;
	}
	else if(new_node_part == BOTTOM){
		area_available = MAX_BOTTOM_AREA - BOTTOM_AREA_USED;
		insert_area = node->BOTTOM_area;
	}
	return (area_available >= insert_area);
}

// update the die area after swapping node partition
void update_die_area(NODE_PART_NAME new_node_part, Node* node){
	if (new_node_part == TOP){
		TOP_AREA_USED += node->TOP_area;
		BOTTOM_AREA_USED -= node->BOTTOM_area;
	}
	else if (new_node_part == BOTTOM){
		BOTTOM_AREA_USED += node->BOTTOM_area;
		TOP_AREA_USED -= node->TOP_area;
	}
	else {
	#ifdef DEBUG
		cout << "error: invalid partition" << endl;
	#endif
	}
}

void update_node_num(Net &net){
	net.TopNodesNum = 0;
	net.BottomNodesNum = 0;
	for(auto node : net.NodeList){
		if(node->node_partition == TOP)
			net.TopNodesNum++;
		else if(node->node_partition == BOTTOM)
			net.BottomNodesNum++;
		else {
			#ifdef DEBUG
			cout << "Error update_node_num(): Invalid node partition." << endl;
			#endif
		}
	}
}

void update_cutstate(vector<Net> *NetList){
	for(int i = 0; i < NetList->size(); i++){
		update_node_num((*NetList)[i]);
		(*NetList)[i].isCut = ((*NetList)[i].TopNodesNum >= 1 && (*NetList)[i].BottomNodesNum >= 1);
	}
}

// calculating area of the node taking up on both Die
void Node::calculate_total_area(){
	// update the area in the node (TOP_area, BOTTOM_area)
	this->TOP_area = 0;
	this->BOTTOM_area = 0;
	for(auto i : this->Instlist){
		this->TOP_area += i->libCell[0]->sizeX * i->libCell[0]->sizeY;
		this->BOTTOM_area += i->libCell[1]->sizeX * i->libCell[1]->sizeY;
	}
}

// distribute the node by area priority (less area first)
void distribute_nodes_by_area(vector<Node> *NodeList){	
	#ifdef DEBUG
	cout << "--- Area Based Distribution ---" << endl;
	#endif
	// modify the node_partition in NodeList
    for(auto n : (*NodeList)){
        NODE_PART_NAME new_node_part = NO_PART;
        n.calculate_total_area();
        // assign new node to the die on which has a smaller area
        if(n.TOP_area < n.BOTTOM_area){
            new_node_part = TOP;
        }
        else {
            new_node_part = BOTTOM;
        }

        // put node on Die if availible  
        if(check_availible_area(new_node_part, &n)){
			n.node_partition = new_node_part;
		}
		else{
			// switch to the other die 
			if(new_node_part == BOTTOM){
            	new_node_part = TOP;
			}
			else if(new_node_part == TOP){
				new_node_part = BOTTOM;
			}
			// check area again
			if(check_availible_area(new_node_part, &n)){
				n.node_partition = new_node_part;
			}
			else{
				#ifdef DEBUG
				cout << "area constraint error: cannot put node on either die." << endl;
				#endif
			}
		}

		
    }
	
}

// generate a random partition for initial condition
void random_partition(vector<Node> *NodeList){
	#ifdef DEBUG
	cout << "\n--- Initial Partition ---" << endl;
	#endif
	int LARGEST_AREA = 0;
	Node* LARGEST_NODE;
	while(true){
		// clear the top and bottom area
		TOP_AREA_USED = 0;
		BOTTOM_AREA_USED = 0;
		srand( time(NULL) );
		// distribute the partition by random
		for(int i=0; i<NodeList->size(); i++){
			(*NodeList)[i].calculate_total_area();
			if(LARGEST_AREA < (*NodeList)[i].TOP_area){
				LARGEST_AREA = (*NodeList)[i].TOP_area;
				LARGEST_NODE = &(*NodeList)[i];
			}
			if(LARGEST_AREA < (*NodeList)[i].BOTTOM_area){
				LARGEST_AREA = (*NodeList)[i].BOTTOM_area;
				LARGEST_NODE = &(*NodeList)[i];
			}

			if(rand()%2){
				TOP_AREA_USED += (*NodeList)[i].TOP_area;
				(*NodeList)[i].node_partition = TOP;
				for(auto N:(*NodeList)[i].connected_nets)
					N->TopNodesNum++;
			}
			else{
				BOTTOM_AREA_USED += (*NodeList)[i].BOTTOM_area;
				(*NodeList)[i].node_partition = BOTTOM;
				for(auto N:(*NodeList)[i].connected_nets)
					N->BottomNodesNum++;
			}
		}
		// stop if satisfying area constraint
		if(check_area_constraint())
			break;
	}
	output_partition(NodeList);
	#ifdef DEBUG
		cout << "AREA = " << TOP_AREA_USED << " / " << BOTTOM_AREA_USED << endl;
	#endif
}

// partitioning nodes equally into two Dies
void initial_partition(vector<Node> *NodeList){
	#ifdef DEBUG
	cout << "\n--- Initial Partition ---" << endl;
	#endif
	for(int i=0; i<NodeList->size(); i++){
		(*NodeList)[i].calculate_total_area();
		if(i < NodeList->size()/2){
			TOP_AREA_USED += (*NodeList)[i].TOP_area;
			(*NodeList)[i].node_partition = TOP;
			for(auto N:(*NodeList)[i].connected_nets)
				N->TopNodesNum++;
		}
		else{
			BOTTOM_AREA_USED += (*NodeList)[i].BOTTOM_area;
			(*NodeList)[i].node_partition = BOTTOM;
			for(auto N:(*NodeList)[i].connected_nets)
				N->BottomNodesNum++;
		}
	}
	output_partition(NodeList);
}
	
// calaulate the gain of a single node
void Node::calculate_nodes_gain(){
    // gain = external cost - internal cost
    // insert the node into the gain bucket by their gain
    NODE_PART_NAME F_Block = this->node_partition;
    NODE_PART_NAME T_Block = NO_PART;

    if(F_Block == TOP)
        T_Block = BOTTOM; 
    else if(F_Block == BOTTOM)
        T_Block = TOP; 
    else 
	#ifdef DEBUG
        cout << "Error calculate_nodes_gain(): Invalid partition" << endl;
    #endif
    // Gain Calculation
    // For each net n connected to c 
    // if F_num = 1, g(c)++; 
    // if T_num = 0, g(c)--;
    for(int i = 0; i < this->connected_nets.size(); i++){
        int F_num = 0, T_num = 0;

        if(F_Block == TOP){
            F_num = this->connected_nets[i]->TopNodesNum;
            T_num = this->connected_nets[i]->BottomNodesNum;
        }
        else if(F_Block == BOTTOM){
            F_num = this->connected_nets[i]->BottomNodesNum;
            T_num = this->connected_nets[i]->TopNodesNum;
        }

		if (F_num == 1)
			this->node_gain++;
		if (T_num == 0)
			this->node_gain--;
    }
    gain_bucket_map[this->node_gain].push_back(this);
}

void output_gain_bucket(){
	#ifdef DEBUG
	cout << "\nGain Bucket:" << endl;
	for(auto bucket: gain_bucket_map){
		cout << "[" << bucket.first << "] ";
		for(auto node: bucket.second)
			cout << node->name << " ";
		cout << endl;
	}
	#endif
}

// initialize all the nodes' gain and print the GainBucket 
void initialize_nodes_gain(vector<Node> *NodeList){
	// cout << "\n--- Gain Initialization ---" << endl;
	//Setting gain for each node with the given initial random partition
	for (unsigned int i = 0; i < (*NodeList).size(); i++)
	{
		// Reset lock status and node gain
		(*NodeList)[i].lock_state = UNLOCKED;
		(*NodeList)[i].node_gain = 0;

		// Set proper node gain
		(*NodeList)[i].calculate_nodes_gain();
	}
#ifdef DEBUG
	output_gain_bucket();
#endif
}

// swaping a node's partition
int swap_partition(Node *node){
	// swap the partition of the max gain node and lock the node
	NODE_PART_NAME org_node_part = node->node_partition;
	NODE_PART_NAME new_node_part = (TOP == org_node_part) ? BOTTOM : TOP;
	
	// Checking the availible area
	if(check_availible_area(new_node_part, node)){
		// swaping partition
		node->node_partition = new_node_part;
		node->lock_state = LOCKED;
		update_die_area(new_node_part, node);
		// for(auto N : node->connected_nets){
		// 	N->TopNodesNum += ((new_node_part == TOP) ? 1 : -1);
		// 	N->BottomNodesNum += ((new_node_part == BOTTOM) ? 1 : -1);
		// }
#ifdef DEBUG
		cout << "swapping node: " << node->name << endl;
#endif
		return true;
	}
	else {
		node->node_partition = org_node_part;
		return false;
	}
}

// find and swap the max gain node
Node* find_max_gain_node(){
	map < int, vector<Node*> > ::reverse_iterator iter;
	unsigned int i = 0;
	bool swapped = false;
	Node *swappedNode = NULL;
	vector<Node *> *max_gain_node;

	// Get the highest gain node from the gain bucket
	for(iter = gain_bucket_map.rbegin(); iter != gain_bucket_map.rend(); iter++)
	{
		// Get highest gain node vector from the map
		max_gain_node = &iter->second;
		// Loop through the vector to get an unlocked node
		for(i=0; i<max_gain_node->size(); i++){
			// skip the locked node
			if((*max_gain_node)[i]->lock_state == LOCKED)
				continue;
			else{
				// Swap partition if the area enough, if not go to next node
				swapped = swap_partition((*max_gain_node)[i]);
				// If the condition is true that means node has been moved and locked
				if (swapped){
					swappedNode = (*max_gain_node)[i];
					if ((*max_gain_node)[i]->lock_state == LOCKED)
						max_gain_node->erase(max_gain_node->begin() + i);
					break;
				}	
			}
		}
		if (swapped){
			break;
		}
	}
	return swappedNode;
}

// recalculate the node gain on the Net connected to the swapped node
void recalculate_node_gain(Node *node){
	// recalculate the node gain on the Net connected to the swapped node
	int *F_n = NULL;
	int *T_n = NULL;
	int prev_gain = 0;
	int new_gain = 0;
	vector<Node*> *nodeList = NULL;

	NODE_PART_NAME F_Block = NO_PART;
	NODE_PART_NAME T_Block = node->node_partition;

	// From & To Block
	if(T_Block == NO_PART) {
		#ifdef DEBUG
		cout << "Error recalculate_gain(): Invalid partition." << endl;
		#endif
	}
	else
		F_Block = (T_Block == TOP) ? BOTTOM : TOP;

	// iterate through the connected Net
	for(auto N: node->connected_nets){
		if(F_Block == TOP){
			F_n = &(*N).TopNodesNum;
			T_n = &(*N).BottomNodesNum;
		}
		else if(F_Block == BOTTOM){
			F_n = &(*N).BottomNodesNum;
			T_n = &(*N).TopNodesNum;
		}
		for(auto n: N->NodeList){
			// Clearing nodeList
			if(NULL != nodeList)
				nodeList = NULL;
			if(n->lock_state == LOCKED)
				continue;
			// Calculate new gain
			prev_gain = n->node_gain;
			new_gain = n->node_gain;
			if(*T_n == 0) new_gain++;
			if(*T_n == 1) new_gain--;
			if(*F_n == 1) new_gain--;
			if(*F_n == 2) new_gain++;
			(*F_n)--;
			(*T_n)++;

			// update gain bucket
			nodeList = &gain_bucket_map[prev_gain];
			(*nodeList).erase(std::find((*nodeList).begin(), (*nodeList).end(), n));
			gain_bucket_map[new_gain].push_back((n));
			n->node_gain = new_gain;
		}
	}

	
}

void FM_algorithm(vector<Node> *NodeList, vector<Net> *NetList, vector<Die> *Die_vector){
	Node* max_node = NULL;
	int	i = 0;
	unsigned int INITIAL_CUTSET	= 0;
	unsigned int MINCUT_TOP_AREA = TOP_AREA_USED;
	unsigned int MINCUT_BOTTOM_AREA = BOTTOM_AREA_USED;
	
	int numNets = NetList->size();
	int numNodes = NodeList->size();

	
	//Setting up the area constraint
	//setup_area_constraint(Die_vector);

	//Loop for passes
	for (int j = 1; j <= NUM_PASSES; j++)
	{
		output_NetList(NetList);
		// Initialize node gain  
		initialize_nodes_gain(NodeList);
		update_cutstate(NetList);
		// Set inital cut set as minimum
		MIN_CUT_SET = CUT_SET;
		INITIAL_CUTSET = CUT_SET;
		// Set the initial partition as the MIN_CUT paritition
		MIN_CUT_NET = *NetList;
		MIN_CUT_NODE = *NodeList;

		// Loop for iterations
		while (true)
		{
			// Get node with max gain and swap
			max_node = find_max_gain_node();
			
			// output the partition after swapping
#ifdef DEBUG
			// output_partition(NodeList);
			// cout << "AREA = " << TOP_AREA_USED << " / " << BOTTOM_AREA_USED << endl;
#endif

			// No more swaps can be made - stop the algorithm
			if (NULL == max_node)
				break;
		
			// Update cut set when a change has been made
			CUT_SET -= max_node->node_gain;
			// Recalculation of node gains for the nieghbouring nodes
			recalculate_node_gain(max_node);
#ifdef DEBUG
			// cout << "Cutset: " << CUT_SET << endl;
			// output_gain_bucket();
#endif

			// If current cut set is less than minimum cutset then set min cutset to the new value
			if (CUT_SET < MIN_CUT_SET)
			{
				// Save state every time a new min cut set is obtained
				MIN_CUT_SET = CUT_SET;
				MIN_CUT_NET = *NetList;
				MIN_CUT_NODE = *NodeList;
				MINCUT_TOP_AREA = TOP_AREA_USED;
				MINCUT_BOTTOM_AREA = BOTTOM_AREA_USED;
			}
		}

		// revert to mincut state
		CUT_SET = MIN_CUT_SET;
		*NetList = MIN_CUT_NET;
		*NodeList = MIN_CUT_NODE;
		TOP_AREA_USED = MINCUT_TOP_AREA;
		BOTTOM_AREA_USED = MINCUT_BOTTOM_AREA;

		// update the isCut in NetList
		update_cutstate(NetList);
		gain_bucket_map.clear();

		// output result of current pass
#ifdef DEBUG
		cout << "Cutset: " << CUT_SET << endl;
		cout << "End Pass" << endl << endl;
		cout << "\n--- Pass " << j << " Result ---" << endl;
		output_partition(NodeList);
		cout << "AREA = " << TOP_AREA_USED << " / " << BOTTOM_AREA_USED << endl;
		cout << "\nCUTSET(initial/mincut): "<<  INITIAL_CUTSET << " / " << MIN_CUT_SET << endl;
#endif
	}
}

// grouping each single Inst as a node
void Inst_grouping(vector<Node> *NodeList, vector<Inst> *Instances){
	// grouping each single Inst as a node
	cout << "Instlist:";
	// Adding Instances to new_node and push_back to NodeList
	for(int i=0; i < Instances->size(); i++){
		Node new_node;
		#ifdef DEBUG
		cout << (*Instances)[i].name << " ";
		#endif
		new_node.name = "n"+to_string(NodeList->size()+1);
		new_node.Instlist.push_back(&(*Instances)[i]);
		// add the connected Nets into node
		for(auto inst : new_node.Instlist)
			for(auto N : inst->connectedNet)
				new_node.connected_nets.push_back(N);
		NodeList->push_back(new_node);
	}
	#ifdef DEBUG
	cout << "\nNode quantity: "<< NodeList->size() << endl;
	for(int i=0; i < NodeList->size(); i++){
		cout << (*NodeList)[i].name << " " << (*NodeList)[i].Instlist.size() << " ";
		for(auto inst: (*NodeList)[i].Instlist)
			cout << inst->name << " ";
		cout << endl;
	}
	#endif
}

void connecting_net_to_inst(vector<Net> *NetList){
	// connecting the Net to instances
	for(int i=0; i < NetList->size(); i++)
		for(auto inst : (*NetList)[i].InstList)
			inst->connectedNet.push_back(&(*NetList)[i]);
}

void connecting_net_to_node(vector<Node> *NodeList, vector<Net> *NetList){
	// connecting the Net in Netlist to the Nodes
	#ifdef DEBUG
	cout << "--- Netlist --- " << endl;
	#endif
	for(int i=0; i < NodeList->size(); i++){
		for(auto inst : (*NodeList)[i].Instlist){
			for(int j=0; j < inst->connectedNet.size(); j++){
				// Net in inst's connectedNet connect to node
				if ((*NodeList)[i].connected_nets.end() == std::find((*NodeList)[i].connected_nets.begin(), (*NodeList)[i].connected_nets.end(), inst->connectedNet[j]))
					(*NodeList)[i].connected_nets.push_back(inst->connectedNet[j]);
				if (inst->connectedNet[j]->NodeList.end() == std::find(inst->connectedNet[j]->NodeList.begin(), inst->connectedNet[j]->NodeList.end(), &(*NodeList)[i]))
					inst->connectedNet[j]->NodeList.push_back(&(*NodeList)[i]);
			}	
		}
	}
	#ifdef DEBUG
	// output NetList
	for(auto N : (*NetList)){
		cout << N.name << ": ";
		for(auto node : N.NodeList)
			cout << node->name << " ";
		cout << endl;
	}
	cout << endl;	
	#endif
}

void calculate_cut(vector<Net> *NetList){
	for(auto net : (*NetList)){
		update_node_num(net);
		if(net.TopNodesNum >= 1 && net.BottomNodesNum >= 1)
			CUT_SET++;
	}
}

void partitioning(vector<Node> *NodeList, vector<Net> *NetList, vector<Inst> *Instances, vector<Die> *Die_vector, vector<Net*> *cutNetList){
	node_grouping(NodeList, NetList, *Instances);
	//Inst_grouping(NodeList, Instances);
	connecting_net_to_inst(NetList);
	connecting_net_to_node(NodeList, NetList);
	setup_area_constraint(Die_vector);
	// distribute_nodes_by_area(NodeList);
	do{
		random_partition(NodeList); 
		calculate_cut(NetList);
		FM_algorithm(NodeList, NetList, Die_vector);
	}while(check_area_constraint() == false);
	changing_instance_partition(NodeList);
	cout << "--- Partitioning End ---\n" << endl;
}

void changing_instance_partition(vector<Node> *NodeList){
	for(auto n: (*NodeList)){
		for(auto inst: n.Instlist)
			inst->isBottom = (n.node_partition==BOTTOM);
	}
}

inline void quicksort(vector<Node>* a,int L,int R) { 
  int i = 0, j = 0;
  if(L < R) { 
    i = L, j = R + 1; 
    while(1) {
        while((i < R) && (a->at(++i).Instlist.size() < a->at(L).Instlist.size())) ;   
        while((j > L) && (a->at(--j).Instlist.size() > a->at(L).Instlist.size())) ;  
        if(i >= j) 
            break; 
        Node tmp; 
        tmp= a->at(i);
        a->at(i) = a->at(j);
        a->at(j) = tmp; 
    } 
    Node tmp; 
    tmp= a->at(L);
    a->at(L) = a->at(j);
    a->at(j) = tmp;
    quicksort(a , L, j - 1);    
    quicksort(a , j + 1, R);   
  } 
}

inline void node_grouping(vector<Node> *NodeList, vector<Net> *NetList, vector<Inst> instances){
	//#ifdef DEBUG
	cout << "\n--- Grouping Node ---" << endl;
	//#endif
	int stage = 0;
    int MaxNumberNet = (*NetList).size()/pow(10, (4-stage)); // temporary
    vector<Net> tempNetList = *NetList;
    Node tempgrouping_node;
    vector<int> netcounter(0);
    for(int i = 0; i< tempNetList.size(); i++){ // initialize with size = 2 net
        if(tempNetList.at(i).InstList.size() == 2){
            tempgrouping_node.Instlist.push_back(tempNetList.at(i).InstList.at(0));
            tempgrouping_node.Instlist.push_back(tempNetList.at(i).InstList.at(1));
			for(int j = 0; j< (*NetList).size(); j++){
				if((*NetList).at(j).name == tempNetList.at(i).name)
					tempgrouping_node.connected_nets.push_back(&(*NetList).at(j));
			}
            NodeList->push_back(tempgrouping_node);
            tempgrouping_node.Instlist.clear();
			tempgrouping_node.connected_nets.clear();
            netcounter.push_back(1);
            tempNetList.erase(tempNetList.begin() + i);
            i--;
        }
    }
    int netsize = 3;
    while(tempNetList.size() != 0){
        for(int i = 0; i< tempNetList.size(); i++){
            if(tempNetList.at(i).InstList.size() == netsize){
                int position = -1, maxSimilar = 0, samePosition = -1; 
				bool isSame = false;
                for(int j = 0; j < NodeList->size(); j++){
                    if(netcounter.at(j) < MaxNumberNet){
						int similar = 0;
                        for(int k = 0; k < NodeList->at(j).Instlist.size(); k++){
							//cout << i<<" "<< j<<" "<<k <<" "<<similar<<" "<< position<< endl; 
							if(find(tempNetList.at(i).InstList.begin(), tempNetList.at(i).InstList.end(), NodeList->at(j).Instlist.at(k)) == tempNetList.at(i).InstList.end())
                                continue;
                            else if(k == (NodeList->at(j).Instlist.size() - 1) && similar == k){
								similar++;
								maxSimilar = similar;
								//cout << "all the same"<< endl;
								if(!isSame){
									for(int l = 0; l< tempNetList.at(i).InstList.size(); l++){
                                		if(find(NodeList->at(j).Instlist.begin(), NodeList->at(j).Instlist.end(), tempNetList.at(i).InstList.at(l)) == NodeList->at(j).Instlist.end())
                                    		NodeList->at(j).Instlist.push_back(tempNetList.at(i).InstList.at(l));
                            		}
									for(int l = 0; l< (*NetList).size(); l++){
										if((*NetList).at(l).name == tempNetList.at(i).name)
											NodeList->at(j).connected_nets.push_back(&(*NetList).at(l));
									}
									netcounter.at(j)++;
									isSame = true;
									samePosition = j;
									break;
								}
								else{
									netcounter.at(samePosition)++;
									NodeList->erase(NodeList->begin() + j);
									netcounter.erase(netcounter.begin() + j);
									j--;
								}
                            }
                            else{
								similar++;
								//cout << "similar++"<<endl;
								if(maxSimilar <= similar && samePosition == -1){
									position = j;
									maxSimilar = similar;
								}
							}
                        }
                    }
                    if(j == (NodeList->size()-1) && samePosition == -1){
                        if(position == -1){ // open new node
                            for(int k = 0; k< tempNetList.at(i).InstList.size(); k++)
                                tempgrouping_node.Instlist.push_back(tempNetList.at(i).InstList.at(k));
							for(int k = 0; k< (*NetList).size(); k++){
								if((*NetList).at(k).name == tempNetList.at(i).name)
									tempgrouping_node.connected_nets.push_back(&(*NetList).at(k));
							}
                            NodeList->push_back(tempgrouping_node);
							tempgrouping_node.connected_nets.clear();
                            tempgrouping_node.Instlist.clear();
                            netcounter.push_back(1);
							break;
                       	}
                        else{ // combine
                            for(int k = 0; k< tempNetList.at(i).InstList.size(); k++){
                                if(find(NodeList->at(position).Instlist.begin(), NodeList->at(position).Instlist.end(), tempNetList.at(i).InstList.at(k)) == NodeList->at(position).Instlist.end())
                                    NodeList->at(position).Instlist.push_back(tempNetList.at(i).InstList.at(k));
                            }
							for(int k = 0; k< (*NetList).size(); k++){
								if((*NetList).at(k).name == tempNetList.at(i).name)
									NodeList->at(position).connected_nets.push_back(&(*NetList).at(k));
							}  
                            netcounter.at(position)++;
                        } 
                    }   
                }
                tempNetList.erase(tempNetList.begin() + i);
                i--;
            }
        }
        netsize++;
    }
    quicksort(NodeList, 0, (NodeList->size()-1)); // node依照instance數量由小到大排序 //快速排序法
	int instance_size = instances.size();
    for(int i = (NodeList->size() - 1); i >= 0; i--){ // 刪除多餘的instance
        for(int j = 0; j< NodeList->at(i).Instlist.size(); j++){
            if(find(instances.begin(), instances.end(), *(*NodeList).at(i).Instlist.at(j)) == instances.end()){
                NodeList->at(i).Instlist.erase(NodeList->at(i).Instlist.begin() + j);
                j--;
                if(NodeList->at(i).Instlist.size() == 0){
                    NodeList->erase(NodeList->begin() + i);
                    break;
                }
            }
            else
                instances.erase(find(instances.begin(), instances.end(), *NodeList->at(i).Instlist.at(j)));
        }
    }

	int MinNumberInst = instance_size/pow(10, (2+stage));
	for(int i = 0; i< NodeList->size(); i++){
		if(NodeList->at(i).Instlist.size() < MinNumberInst){
			#ifdef DEBUG
				cout <<"Node "<< i <<" Inst quantity: "<< NodeList->at(i).Instlist.size() << endl;
			#endif
			for(int j = i; j< NodeList->size(); j++){
				if(NodeList->at(j).Instlist.size() <= (MinNumberInst - NodeList->at(i).Instlist.size()) && i != j){
					NodeList->at(i).Instlist.insert(NodeList->at(i).Instlist.end(), NodeList->at(j).Instlist.begin(), NodeList->at(j).Instlist.end());
					for(int k = 0; k< NodeList->at(j).connected_nets.size(); k++){
						if(find(NodeList->at(i).connected_nets.begin(), NodeList->at(i).connected_nets.end(), NodeList->at(j).connected_nets.at(k)) == NodeList->at(i).connected_nets.end())
						NodeList->at(i).connected_nets.push_back(NodeList->at(j).connected_nets.at(k));
					}
					NodeList->erase(NodeList->begin() + j);
					j--;
					if(NodeList->at(i).Instlist.size() >= MinNumberInst)
						break;
					}
			}
 		}
	}
	//#ifdef DEBUG
    for(int i = 0; i< NodeList->size(); i++){
		NodeList->at(i).name = "n" + to_string(i + 1);
        cout << "n" << i + 1 << " ";
        for(int j = 0; j< NodeList->at(i).Instlist.size(); j++){
            cout <<NodeList->at(i).Instlist.at(j)->name << " ";
        }
        cout << endl;
    }
	//#endif
}
int cut_Node(Node node, vector<Net> *NetList){
	int netlocation = 0;
	int cut = abs(node.connected_nets.at(0)->TopNodesNum - node.connected_nets.at(0)->BottomNodesNum);
	int total = node.connected_nets.at(0)->TopNodesNum + node.connected_nets.at(0)->BottomNodesNum;
	for(int i = 1; i< node.connected_nets.size(); i++){
		if(node.connected_nets.at(i)->TopNodesNum != 0 && node.connected_nets.at(i)->BottomNodesNum != 0){
			if(cut == 0 || (cut > abs(node.connected_nets.at(i)->TopNodesNum - node.connected_nets.at(i)->BottomNodesNum) 
			&& total > node.connected_nets.at(i)->TopNodesNum + node.connected_nets.at(i)->BottomNodesNum)){
				netlocation = i;
				cut = abs(node.connected_nets.at(i)->TopNodesNum - node.connected_nets.at(i)->BottomNodesNum);
				total = node.connected_nets.at(i)->TopNodesNum + node.connected_nets.at(i)->BottomNodesNum;
			}		
		}
	}
	return netlocation;
}
inline void node_grouping(vector<Node> *NodeList, vector<Net> *NetList, vector<Inst> instances, vector<Net*> *cutNetList, int stage){
	//#ifdef DEBUG
	cout << "\n--- Grouping Node ---" << endl;
	//#endif
    int MaxNumberNet = (*NetList).size()/pow(10, (4-stage));
	int MinNumberInst = instances.size()/pow(10, (2+stage));
    vector<Node> tempNodeList = *NodeList;
    Node tempgrouping_node;
	for(int i = 0; tempNodeList.size(); i++){
		if(tempNodeList.at(i).connected_nets.size() > MaxNumberNet){
			int netlocation = cut_Node(tempNodeList.at(i), NetList);
			for(int j = 0; j< tempNodeList.at(i).connected_nets.at(netlocation)->InstList.size(); j++){
				if(find(tempNodeList.at(i).Instlist.begin(), tempNodeList.at(i).Instlist.end(), tempNodeList.at(i).connected_nets.at(netlocation)->InstList.at(j)) != tempNodeList.at(i).Instlist.end()){
					tempgrouping_node.Instlist.push_back(tempNodeList.at(i).connected_nets.at(netlocation)->InstList.at(j));
					tempNodeList.at(i).Instlist.erase(find(tempNodeList.at(i).Instlist.begin(), tempNodeList.at(i).Instlist.end(), tempNodeList.at(i).connected_nets.at(netlocation)->InstList.at(j)));
				}		
			}
			tempgrouping_node.connected_nets.push_back(tempNodeList.at(i).connected_nets.at(netlocation));
			tempNodeList.at(i).connected_nets.erase(tempNodeList.at(i).connected_nets.begin() + netlocation);
			if(tempNodeList.at(i).connected_nets.size() > MaxNumberNet)
				--i;
			else{
				tempgrouping_node.node_partition = tempNodeList.at(i).node_partition;
				tempNodeList.push_back(tempgrouping_node);
				tempgrouping_node.connected_nets.clear();
				tempgrouping_node.Instlist.clear();
			}
		}
	}
	vector <int> is_cutNet((*NetList).size());
    quicksort(NodeList, 0, (NodeList->size()-1)); // node依照instance數量由小到大排序 //快速排序法
	int instance_size = instances.size();
    for(int i = (NodeList->size() - 1); i >= 0; i--){ // 刪除多餘的instance
        for(int j = 0; j< NodeList->at(i).Instlist.size(); j++){
            if(find(instances.begin(), instances.end(), *(*NodeList).at(i).Instlist.at(j)) == instances.end()){
				if(stage == 2) {
					for(int k = 0; k< (*NetList).size(); k++){
						if(find((*NetList).at(k).InstList.begin(), (*NetList).at(k).InstList.end(), (*NodeList).at(i).Instlist.at(j)) != (*NetList).at(k).InstList.end() && is_cutNet.at(k) == 0){
							if(find(NodeList->at(i).connected_nets.begin(), NodeList->at(i).connected_nets.end(), &((*NetList).at(k))) != NodeList->at(i).connected_nets.end()){
								cutNetList->push_back(&((*NetList).at(k)));
								is_cutNet.at(k) = 1;
							}
						}
					}
				}
                NodeList->at(i).Instlist.erase(NodeList->at(i).Instlist.begin() + j);
                j--;
                if(NodeList->at(i).Instlist.size() == 0){
                    NodeList->erase(NodeList->begin() + i);
                    break;
                }
            }
            else
                instances.erase(find(instances.begin(), instances.end(), *NodeList->at(i).Instlist.at(j)));
        }
    }

	for(int i = 0; i< NodeList->size(); i++){
		if(NodeList->at(i).Instlist.size() < MinNumberInst){
			#ifdef DEBUG
				cout <<"Node "<< i <<" Inst quantity: "<< NodeList->at(i).Instlist.size() << endl;
			#endif
			for(int j = i; j< NodeList->size(); j++){
				if(NodeList->at(j).Instlist.size() <= (MinNumberInst - NodeList->at(i).Instlist.size()) && i != j){
					NodeList->at(i).Instlist.insert(NodeList->at(i).Instlist.end(), NodeList->at(j).Instlist.begin(), NodeList->at(j).Instlist.end());
					for(int k = 0; k< NodeList->at(j).connected_nets.size(); k++){
						if(find(NodeList->at(i).connected_nets.begin(), NodeList->at(i).connected_nets.end(), NodeList->at(j).connected_nets.at(k)) == NodeList->at(i).connected_nets.end())
						NodeList->at(i).connected_nets.push_back(NodeList->at(j).connected_nets.at(k));
					}
					NodeList->erase(NodeList->begin() + j);
					j--;
					if(NodeList->at(i).Instlist.size() >= MinNumberInst)
						break;
					}
			}
 		}
	}
    for(int i = 0; i< NodeList->size(); i++){
		NodeList->at(i).name = "n" + to_string(i + 1);
        cout << "n" << i + 1 << " ";
        for(int j = 0; j< NodeList->at(i).Instlist.size(); j++){
            cout <<NodeList->at(i).Instlist.at(j)->name << " ";
        }
        cout << endl;
    }
	#ifdef DEBUG
	cout<< "cutNet: "<<endl;
	for(int i = 0; i<cutNetList->size(); i++){	
		cout<< (cutNetList->at(i))->name<< "   ";
	}
	#endif
}

