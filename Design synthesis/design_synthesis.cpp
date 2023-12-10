#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>
using namespace std;

class state{    //to store all the data systemmatically
    public:
        vector<char>state_name;     //state name
        int state_number;                       //state number
        vector<vector<bool>>state_voltage;  //next state
        vector<char>input_name;
        int input_number;
        vector<char>output_name;
        int output_number;
        vector<vector<bool>>output_voltage;
};

void state_table_receiver(class state&,char *); //this function is designed to receive the first file and store it to table
void make_kmap_in(class state);     //this function is designed to make the kmap_in file
void top();     //offered by TA
void circuit_file(class state,char * );     //designed to output the last file
int power(int ,int );       //just like pow in cmath

int main(int argc,char *argv[])
{
    state table;        //create a space for state table
    state_table_receiver(table,argv[1]);    //receive the data
    make_kmap_in(table);    //make file
    top();  //kmap
    circuit_file(table,argv[2]);    //output file
    return 0;
}

void state_table_receiver(class state& table,char * filename){

    ifstream infile(filename,ios::in);
    string str;
    infile>>str>>str>>str;  //something I don't need
    vector<bool>trash;  //create space
    table.state_number=0;
    for(int i=0;i<str.size();i++){
        if(str[ i ]==','||str[ i ]=='('){
                if(str[ i+1 ]=='0') break;  //there is no state
                table.state_number++;
                table.state_voltage.push_back(trash);
                table.state_name.push_back(str[ i+1 ]);
        }
    }
    vector<vector<bool>>present_state;  //for present state
    for(int i=0;i<table.state_number;i++){
        present_state.push_back(trash);     //create space
    }
    int i=0;    //to store data to corresponding space
    while(1){
        infile>>str;
        if(str.size()!=1)   break;     //if input of voltage ends
        if(str=="0")    present_state[ i ].push_back(0);
        else present_state[ i ].push_back(1);
        if(i==table.state_number-1)    i=0;
        else i++;
    }

    table.input_number=0;
    for(int i=0;i<str.size();i++){
        if(str[ i ]==','||str[ i ]=='('){
                if(str[ i+1 ]=='0') break;
                table.input_number++;
                table.input_name.push_back(str[ i+1 ]);
        }
    }
    vector<vector<bool>>input_series;   //for all input
    for(int i=0;i<table.input_number;i++){
        input_series.push_back(trash);
    }
    i=0;
    while(1){       //same logic as before
        infile>>str;
        if(str.size()!=1)   break;
        if(str=="0")    input_series[ i ].push_back(0);
        else input_series[ i ].push_back(1);
        if(i==table.input_number-1)    i=0;
        else i++;
    }

    i=0;
    while(1){   //same logic as before
        infile>>str;
        if(str.size()!=1)   break;
        if(str=="0")    table.state_voltage.at(i).push_back(0);
        else table.state_voltage.at(i).push_back(1);
        if(i==table.state_number-1)    i=0;
        else i++;
    }

    table.output_number=0;
     for(int i=0;i<str.size();i++){
        if(str[ i ]==','||str[ i ]=='('){   //same logic as before
                if(str[ i+1 ]=='0') break;
                table.output_number++;
                table.output_voltage.push_back(trash);
                table.output_name.push_back(str[ i+1 ]);
        }
    }
    i=0;
    while(!infile.eof()){   //if the file come to an end
        infile>>str;
        if(str.size()!=1)   break;
        if(str=="0")    table.output_voltage[ i ].push_back(0);
        else table.output_voltage[ i ].push_back(1);
        if(i==table.output_number-1)    i=0;
        else i++;
    }

    vector<int>total;   //store nodes
    int counter;
    int confirm;    //to make sure the value is usable
    if(input_series.size()==0)  confirm=present_state[0].size();
    else confirm=input_series[0].size();
    for(int i=0;i<confirm;i++){
        counter=0;
        for(int j=0;j<table.state_number;j++){
            counter+=present_state[ j ][ i ]*pow(2,(table.state_number+table.input_number-1-j));    //to make a number for every nodes
        }
        for(int j=0;j<table.input_number;j++){
            counter+=input_series[ j ][ i ]*pow(2,(table.input_number-1-j));    //to make a number for every nodes
        }
        total.push_back(counter);
    }

    int bol=1;
    while(bol==1){      //this loop is bubble sort
        bol=0;
        for(int i=0;i<total.size()-1;i++){
                if(total[ i ]>total[ i+1 ]){    //to prevent the order is not sorted
                    bol=1;
                    swap(total[ i ],total[ i+1]);
                    for(int j=0;j<table.state_number;j++){
                        swap(table.state_voltage[ j ][ i ],table.state_voltage[ j ][ i+1 ]);
                    }
                    for(int j=0;j<table.output_number;j++){
                        swap(table.output_voltage[ j ][ i ],table.output_voltage[ j ][ i+1 ]);
                    }
                }
        }
    }
}

void make_kmap_in(class state table){
        ofstream outfile("kmap_in.txt",ios::out);
        outfile<<table.state_number+table.output_number<<endl;
        int k=table.input_number+table.state_number;    //k is the variable numbers
        string str;
        int series_k3[8]={0,1,3,2,4,5,7,6}; //due to the property of 3-kmap
        int series_k4[16]={0,1,3,2,4,5,7,6,12,13,15,14,8,9,11,10};//due to the property of 4-kmap
        for(int i=0;i<table.state_number;i++){  //state data
            outfile<<k<<endl;
            switch(k){
                case 2:
                    for(int j=0;j<4;j++){
                         outfile<<table.state_voltage[ i ].at( j )<<" ";    //in order
                    }
                    break;
                case 3:
                    for(int j=0;j<8;j++){
                        outfile<<table.state_voltage[ i ].at( series_k3[ j ] )<<" ";
                    }
                    break;
                case 4:
                    for(int j=0;j<16;j++){
                        outfile<<table.state_voltage[ i ].at( series_k4[ j ] )<<" ";
                    }
                    break;
            }
            outfile<<endl;
        }

        for(int i=0;i<table.output_number;i++){ //output data
            outfile<<k<<endl;
            switch(k){
                case 2:
                    for(int j=0;j<4;j++){
                         outfile<<table.output_voltage[ i ].at( j )<<" ";//in order
                    }
                    break;
                case 3:
                    for(int j=0;j<8;j++){
                        outfile<<table.output_voltage[ i ].at( series_k3[ j ] )<<" ";
                    }
                    break;
                case 4:
                     for(int j=0;j<16;j++){
                        outfile<<table.output_voltage[ i ].at( series_k4[ j ] )<<" ";
                    }
                    break;
            }
            outfile<<endl;
        }
}

void circuit_file(class state table,char * filename){
    ifstream infile("kmap_out.txt",ios::in);
    ofstream outfile(filename,ios::out);
    int and_gate=0,or_gate=0;
    vector<bool>inverter;
    int inverter_number=0;
    vector<vector<vector <int>>> group;     // the first level is type of output variable; the second one is the number of rows; the last one is the number of columns
    vector<vector <int>>member;     //used to fulfill "group"
    vector<int>line;    //used to fulfill "member"
    string str;     //used to fulfill "line"
    bool bol=0;
    infile>>str;    //use string to ensure the completeness of receiver
    for(int i=0;i<table.state_number+table.output_number;i++){
        bol=0;
        while(1){
            for(int i=0;i<table.input_number+table.state_number;i++){
                infile>>str;
                if(str.size()!=1 || infile.eof()){  //end
                    bol=1;
                    break;
                }
                line.push_back(stoi(str));
            }
            if(bol==1)  break;
            member.push_back(line);
            line.clear();
        }
        group.push_back(member);
        member.clear();
    }
    bol=0;
    for(int k=0;k< table.input_number+table.state_number;k++){  //to test which variable use inverter
        for(int i=0;i<group.size();i++){
            for(int j=0;j<group[ i ].size();j++){
                if(group[ i ][ j ][ k ]==0)    bol=1;
            }
        }
        if(bol==1){
             inverter.push_back(1);
             inverter_number++;
        }
        else inverter.push_back(0);
        bol=0;
    }
    vector<int>node_order;  //store the combination of nodes
    int counter=0;  //how many variable
    int record=0;   //the numbers
    for(int i=0;i<group.size();i++){
        if(group[ i ].size()>1) or_gate++;  //the number of or gates
        for(int j=0;j<group[ i ].size();j++){
            for(int k=0;k<group[ i ][ j ].size();k++){
                if(group[ i ][ j ][ k ]!=2) counter++;
            }
            if(counter!=1){
                    for(int k=0;k<group[ i ][ j ].size();k++){
                        record+=group[ i ][ j ][ k ]*pow(10,group[ i ][ j ].size()-1-k);
                    }
                    if(node_order.size()==0)    node_order.push_back(record);
                    bol=0;
                    for(int k=0;k<node_order.size();k++){
                        if(record==node_order[ k ]){    //sentinel guard
                            bol=1;
                            break;
                        }
                     }
                     if(bol==0) node_order.push_back(record);
                     record=0;
            }
            counter=0;
        }
    }
    outfile<<"# "<<table.input_number<<" input";
    if(table.input_number>1)    outfile<<"s"<<endl;
    else outfile<<endl;
    outfile<<"# "<<table.output_number<< " output";
    if(table.output_number>1)    outfile<<"s"<<endl;
    else outfile<<endl;
    outfile<<"# "<<inverter_number<<" inverter";
     if(inverter_number>1)    outfile<<"s"<<endl;
    else outfile<<endl;
    outfile<<"# "<<table.state_number<<" flip-flop";
     if(table.state_number>1)    outfile<<"s"<<endl;
    else outfile<<endl;
    outfile<<"# "<<node_order.size()+or_gate<<" gate";
    if((node_order.size()+or_gate)>1)    outfile<<"s";
    outfile<<" ("<<node_order.size()<<" AND";
    if(node_order.size()>1) outfile<<"s";
    outfile<<" + "<<or_gate<<" OR";
    if(or_gate>1)   outfile<<"s";
    outfile<<")"<<endl<<endl;

    if(table.input_number!=0){  //inputs
        for(int i=0;i<table.input_number;i++){
            outfile<<"INPUT("<<table.input_name[ i ]<<")"<<endl;
        }
    }
    if(table.output_number!=0){ //outputs
        for(int i=0;i<table.output_number;i++){
            outfile<<"OUTPUT("<<table.output_name[ i ]<<")"<<endl;
        }
    }
    outfile<<endl;
    vector<char>name;
    for(int i=0;i<table.state_number;i++){
        name.push_back(table.state_name[ i ]);
    }
    for(int i=0;i<table.input_number;i++){
        name.push_back(table.input_name[ i ]);
    }
    for(int i=0;i<inverter.size();i++){ //inverters
        if(inverter[ i ]==1){
            outfile<<"!"<<name[ i ]<<" = NOT("<<name[ i ]<<")"<<endl;
        }
    }

    for(int i=0;i<node_order.size();i++){   //nodes
        outfile<<"N"<<i+1<<" = AND(";
        bol=0;
        for(int j=table.input_number+table.state_number-1;j>=0;j--){
            int code=(node_order[ i ]%power(10,j+1))/power(10,j );  //examine the corresponding numbers
            if(code==0){
                if(bol==1)  outfile<<",";
                bol=1;
                outfile<<"!"<<name[ table.input_number+table.state_number-1-j ];
            }
            else if(code==1){
                if(bol==1)  outfile<<",";
                bol=1;
                outfile<<name[ table.input_number+table.state_number-1-j ];
            }
        }
        outfile<<")"<<endl;
    }
    for(int i=0;i<table.state_number;i++){
        if(group[ i ].size()==1){
            outfile<<"D"<<table.state_name[ i ]<<" = ";
            int record=0;
            for(int k=0;k<group[ i ][ 0 ].size();k++){
                record+=group[ i ][ 0 ][ k ]*pow(10,group[ i ][ 0 ].size()-1-k);    //create numbers
            }
            bol=0;
            for(int k=0;k<node_order.size();k++){
                if(record==node_order[ k ]){
                        outfile<<"N"<<k+1;
                        bol=1;
                }
            }
            if(bol==0){
                for(int k=0;k<table.input_number+table.state_number;k++){
                    if(group[ i ][ 0 ][ k ]==1)  outfile<<name[ k ];
                    else if(group[ i ][ 0 ][ k ]==0)  outfile<<"!"<<name[ k ];
                }
            }

            outfile<<endl;
        }
        else{
                outfile<<"D"<<table.state_name[ i ]<<" = OR(";
                for(int j=0;j<group[ i ].size();j++){
                    int record=0;
                    for(int k=0;k<group[ i ][ j ].size();k++){
                        record+=group[ i ][ j ][ k ]*pow(10,group[ i ][ j ].size()-1-k);    //create numbers
                    }
                    if(j!=0)    outfile<<",";
                    bol=0;
                    for(int k=0;k<node_order.size();k++){
                        if(record==node_order[ k ]){        //find if corresponding
                            outfile<<"N"<<k+1;
                            bol=1;
                        }
                    }
                    if(bol==0){
                        for(int k=0;k<table.input_number+table.state_number;k++){   //create one
                            if(group[ i ][ j ][ k ]==1)  outfile<<name[ k ];
                            else if(group[ i ][ j ][ k ]==0)  outfile<<"!"<<name[ k ];
                        }
                    }
                }
                outfile<<")"<<endl;
        }
    }

    for(int i=0;i<table.state_number;i++){      //the flipflops
        outfile<<table.state_name[ i ]<<" = D(D"<<table.state_name[ i ]<<",CLK)"<<endl;
    }


      for(int i=0;i<table.output_number;i++){   //the outputs
        outfile<<table.output_name[ i ]<<" = ";
        if(group[ i+table.state_number ].size()==1){    //if it's made up of only one nodes
            int record=0;
            for(int k=0;k<group[ i+table.state_number ][ 0 ].size();k++){
                record+=group[ i+table.state_number ][ 0 ][ k ]*pow(10,group[ i+table.state_number ][ 0 ].size()-1-k);  //create numbers
            }
            bol=0;
            for(int k=0;k<node_order.size();k++){
                if(record==node_order[ k ]){    //find if corresponding
                        outfile<<"N"<<k+1;
                        bol=1;
                }
            }
            if(bol==0){ //if no nodes of the type
                for(int k=0;k<table.input_number+table.state_number;k++){
                    if(group[ i+table.state_number ][ 0 ][ k ]==1)  outfile<<name[ k ];
                    else if(group[ i+table.state_number ][ 0 ][ k ]==0)  outfile<<"!"<<name[ k ];
                }
            }
            outfile<<endl;
        }
        else{       //made up of many nodes
                outfile<<"OR(";
                for(int j=0;j<group[ i+table.state_number ].size();j++){
                    int record=0;
                    for(int k=0;k<group[ i+table.state_number ][ j ].size();k++){
                        record+=group[ i+table.state_number ][ j ][ k ]*pow(10,group[ i+table.state_number ][ j ].size()-1-k);//same logic as before
                    }
                    if(j!=0)    outfile<<",";
                    bol=0;
                    for(int k=0;k<node_order.size();k++){//same logic as before
                        if(record==node_order[ k ]){
                            outfile<<"N"<<k+1;
                            bol=1;
                        }
                    }
                    if(bol==0){ //same logic as before
                        for(int k=0;k<table.input_number+table.state_number;k++){
                            if(group[ i+table.state_number ][ j ][ k ]==1)  outfile<<name[ k ];
                            else if(group[ i+table.state_number ][ j ][ k ]==0)  outfile<<"!"<<name[ k ];
                        }
                    }
                }
                outfile<<")"<<endl;
        }
    }

    outfile<<"#END"<<endl;
}

int power(int a,int b){     //different from pow, the return type is <int>
        int total=1;
        for(int i=0;i<b;i++){
            total*=a;
        }
        if(b==0)    return 1;
        else return total;
}

void top()      //k map
{
    ifstream file ("kmap_in.txt",ios::in);
    int Case;
    int variables;
    int counter=1;

    file >> Case ;

    ofstream out ("kmap_out.txt",ios::out);

    while( file && Case>0 )
    {
        //cout << "#" << counter << endl ;
        out << "#" << counter << endl ;
        counter++;
        int judge=0;
        int c=0;
        int output[4]={2,2,2,2};
        int kmap[6][6]={};
        file >> variables ;
        if(variables==2)
        {
            for(int i=1;i<3;i++)
            {
                for(int k=1;k<3;k++)
                {
                    int value ;
                    file >> value ;
                    kmap[i][k]=value;
                    if(k==1)
                        kmap[i][k+2]=kmap[i][k];
                    if(k==2)
                        kmap[i][k-2]=kmap[i][k];
                    if(i==1)
                        kmap[i+2][k]=kmap[i][k];
                    if(i==2)
                        kmap[i-2][k]=kmap[i][k];
                }
            }

        // read inputs

            for(int i=1;i<3;i++)
            {
                for(int k=1;k<3;k++)
                {
                    if(kmap[i][k]==0)
                    {
                        c=1;
                        judge=1;
                        break;
                    }
                }
                if(c==1)
                    break;
            }
            if(judge==0)
            {
                for(int i=1;i<3;i++)
                {
                    for(int k=1;k<3;k++)
                        kmap[i][k]++;
                }
                for(int i=0 ;i<2;i++)
                {
                    out << output[i] << " " ;
                    //cout << output[i] << " " ;
                }
                out << endl;
                //cout<<endl;
                //out << endl;
                for(int i=0 ;i<2;i++)
                    output[i]=2;
            }

    // if all elements are 1

            for(int a=1;a<3;a++)
            {
                for(int b=1;b<3;b++)
                {
                    if(kmap[a][b]==1)
                    {
                        if(kmap[a-1][b]!=0)
                        {
                            if((a-1)==0)
                                kmap[2][b]++;

                            kmap[a-1][b]++;
                            kmap[a][b]++;

                            if(b==1)
                            output[1]=0;
                            if(b==2)
                            output[1]=1;

                            for(int i=0 ;i<2;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<2;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b-1]!=0)
                        {
                            if((b-1)==0)
                                kmap[a][2]++;

                            kmap[a][b-1]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;

                            for(int i=0 ;i<2;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<2;i++)
                                output[i]=2;
                        }

                // enclose 2 elements

                        else
                        {
                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;
                            if(b==1)
                                output[1]=0;
                            if(b==2)
                                output[1]=1;

                            for(int i=0 ;i<2;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<2;i++)
                                output[i]=2;
                        }

                // no elements can be enclosed
                    }
                }
            }
            judge=0;
            c=0;
        } // end if 2 variables


        if(variables==3)
        {
            for(int i=1;i<3;i++)
            {
                for(int k=1;k<5;k++)
                {
                    int value ;
                    file >> value ;
                    kmap[i][k]=value;
                    if(k==1)
                        kmap[i][k+4]=kmap[i][k];
                    if(k==4)
                        kmap[i][k-4]=kmap[i][k];
                    if(i==1)
                        kmap[i+2][k]=kmap[i][k];
                    if(i==2)
                        kmap[i-2][k]=kmap[i][k];
                }
            }

        // read inputs

            for(int i=1;i<3;i++)
            {
                for(int k=1;k<5;k++)
                {
                    if(kmap[i][k]==0)
                    {
                        c=1;
                        judge=1;
                        break;
                    }
                }
                if(c==1)
                    break;
            }
            if(judge==0)
            {
                for(int i=1;i<3;i++)
                {
                    for(int k=1;k<5;k++)
                        kmap[i][k]++;
                }
                for(int i=0 ;i<3;i++)
                {
                    out << output[i] << " " ;
                    //cout << output[i] << " " ;
                }
                out << endl;
                //cout<<endl;
                //out << endl;
                for(int i=0 ;i<3;i++)
                    output[i]=2;
            }

    // if all elements are 1

            for(int a=1;a<3;a++)
            {
                for(int b=1;b<5;b++)
                {
                    if(kmap[a][b]==1)
                    {
                        if(kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0&&b==1)
                        {
                            kmap[a][b]++;
                            kmap[a][b+1]++;
                            kmap[a][b+2]++;
                            kmap[a][b+3]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // enclose 4 straight elements

                        else if(kmap[a-1][b]!=0 && kmap[a][b-1]!=0 && kmap[a-1][b-1]!=0)
                        {
                            if((a-1)==0)
                                kmap[2][b]++;
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b]++;
                            kmap[a-1][b]++;
                            kmap[a][b-1]++;
                            kmap[a-1][b-1]++;

                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[1]=0;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[1]=1;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0 && kmap[a][b+1]!=0 && kmap[a+1][b+1]!=0)
                        {
                            if((a+1)==3)
                                kmap[1][b]++;
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b]++;
                            kmap[a+1][b]++;
                            kmap[a][b+1]++;
                            kmap[a+1][b+1]++;

                            if(b==1)
                                output[1]=0;
                            if(b==2)
                                output[2]=1;
                            if(b==3)
                                output[1]=1;
                            if(b==4)
                                output[2]=0;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // enclose 4 square elements

                        else if(kmap[a-1][b]!=0)
                        {

                            if((a-1)==0)
                                kmap[2][b]++;

                            kmap[a-1][b]++;
                            kmap[a][b]++;

                            if(b==1)
                            {
                                output[1]=0;
                                output[2]=0;
                            }
                            if(b==2)
                            {
                                output[1]=0;
                                output[2]=1;
                            }
                            if(b==3)
                            {
                                output[1]=1;
                                output[2]=1;
                            }
                            if(b==4)
                            {
                                output[1]=1;
                                output[2]=0;
                            }

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b-1]!=0)
                        {
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b-1]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;
                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[1]=0;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[1]=1;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b+1]!=0)
                        {

                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b+1]++;
                            kmap[a][b]++;

                            if(a==1)
                            output[0]=0;
                            if(a==2)
                            output[0]=1;
                            if(b==1)
                                output[1]=0;
                            if(b==2)
                                output[2]=1;
                            if(b==3)
                                output[1]=1;
                            if(b==4)
                                output[2]=0;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // enclose 2 elements

                        else
                        {

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;
                            if(b==1)
                            {
                                output[1]=0;
                                output[2]=0;
                            }
                            if(b==2)
                            {
                                output[1]=0;
                                output[2]=1;
                            }
                            if(b==3)
                            {
                                output[1]=1;
                                output[2]=1;
                            }
                            if(b==4)
                            {
                                output[1]=1;
                                output[2]=0;
                            }

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // no elements can be enclosed

                    }
                }
            }
            judge=0;
            c=0;
        }// end if 3 variables

        if(variables==4)
        {
            for(int i=1;i<5;i++)
            {
                for(int k=1;k<5;k++)
                {
                    int value ;
                    file >> value ;
                    kmap[i][k]=value;
                    if(k==1)
                        kmap[i][k+4]=kmap[i][k];
                    if(k==4)
                        kmap[i][k-4]=kmap[i][k];
                    if(i==1)
                        kmap[i+4][k]=kmap[i][k];
                    if(i==4)
                        kmap[i-4][k]=kmap[i][k];
                }
            }
         // read inputs

         for(int i=1;i<5;i++)
            {
                for(int k=1;k<5;k++)
                {
                    if(kmap[i][k]==0)
                    {
                        c=1;
                        judge=1;
                        break;
                    }
                }
                if(c==1)
                    break;
            }
            if(judge==0)
            {
                for(int i=1;i<5;i++)
                {
                    for(int k=1;k<5;k++)
                        kmap[i][k]++;
                }
                for(int i=0 ;i<4;i++)
                {
                    out << output[i] << " " ;
                    //cout << output[i] << " " ;
                }
                out << endl;
                //cout<<endl;
                //out << endl;
                for(int i=0 ;i<3;i++)
                output[i]=2;
            }

    // if all elements are 1

            for(int a=1;a<5;a++)
            {
                for(int b=1;b<5;b++)
                {
                    if(kmap[a][b]==1)
                    {
                        if((b==1&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b+1]!=0&&kmap[a-1][b+2]!=0&&kmap[a-1][b+3]!=0)||
                           (b==2&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b-1]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b+1]!=0&&kmap[a-1][b+2]!=0&&kmap[a-1][b-1]!=0)||
                           (b==3&&kmap[a][b+1]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b+1]!=0&&kmap[a-1][b-1]!=0&&kmap[a-1][b-2]!=0)||
                           (b==4&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a][b-3]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b-1]!=0&&kmap[a-1][b-2]!=0&&kmap[a-1][b-3]!=0))
                        {
                            if((a-1)==0)
                            {
                                kmap[4][1]++;
                                kmap[4][2]++;
                                kmap[4][3]++;
                                kmap[4][4]++;
                            }

                            kmap[a][1]++;
                            kmap[a][2]++;
                            kmap[a][3]++;
                            kmap[a][4]++;
                            kmap[a-1][1]++;
                            kmap[a-1][2]++;
                            kmap[a-1][3]++;
                            kmap[a-1][4]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((b==1&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b+1]!=0&&kmap[a+1][b+2]!=0&&kmap[a+1][b+3]!=0)||
                                (b==2&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b+1]!=0&&kmap[a+1][b+2]!=0&&kmap[a+1][b-1]!=0)||
                                (b==3&&kmap[a][b+1]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b+1]!=0&&kmap[a+1][b-1]!=0&&kmap[a+1][b-2]!=0)||
                                (b==4&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a][b-3]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b-1]!=0&&kmap[a+1][b-2]!=0&&kmap[a+1][b-3]!=0))
                        {
                            if((a+1)==5)
                            {
                                kmap[1][1]++;
                                kmap[1][2]++;
                                kmap[1][3]++;
                                kmap[1][4]++;
                            }

                            kmap[a][1]++;
                            kmap[a][2]++;
                            kmap[a][3]++;
                            kmap[a][4]++;
                            kmap[a+1][1]++;
                            kmap[a+1][2]++;
                            kmap[a+1][3]++;
                            kmap[a+1][4]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((a==1&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a+3][b]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b-1]!=0&&kmap[a+2][b-1]!=0&&kmap[a+3][b-1]!=0)||
                                (a==2&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a-1][b]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b-1]!=0&&kmap[a+2][b-1]!=0&&kmap[a-1][b-1]!=0)||
                                (a==3&&kmap[a+1][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b-1]!=0&&kmap[a-1][b-1]!=0&&kmap[a-2][b-1]!=0)||
                                (a==4&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a-3][b]!=0&&kmap[a][b-1]!=0&&kmap[a-1][b-1]!=0&&kmap[a-2][b-1]!=0&&kmap[a-3][b-1]!=0))
                        {
                            if((b-1)==0)
                            {
                                kmap[1][4]++;
                                kmap[2][4]++;
                                kmap[3][4]++;
                                kmap[4][4]++;
                            }

                            kmap[1][b]++;
                            kmap[2][b]++;
                            kmap[3][b]++;
                            kmap[4][b]++;
                            kmap[1][b-1]++;
                            kmap[2][b-1]++;
                            kmap[3][b-1]++;
                            kmap[4][b-1]++;

                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((a==1&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a+3][b]!=0&&kmap[a][b+1]!=0&&kmap[a+1][b+1]!=0&&kmap[a+2][b+1]!=0&&kmap[a+3][b+1]!=0)||
                                (a==2&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a-1][b]!=0&&kmap[a][b+1]!=0&&kmap[a+1][b+1]!=0&&kmap[a+2][b+1]!=0&&kmap[a-1][b+1]!=0)||
                                (a==3&&kmap[a+1][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a][b+1]!=0&&kmap[a+1][b+1]!=0&&kmap[a-1][b+1]!=0&&kmap[a-2][b+1]!=0)||
                                (a==4&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a-3][b]!=0&&kmap[a][b+1]!=0&&kmap[a-1][b+1]!=0&&kmap[a-2][b+1]!=0&&kmap[a-3][b+1]!=0))
                        {
                            if((b+1)==5)
                            {
                                kmap[1][1]++;
                                kmap[2][1]++;
                                kmap[3][1]++;
                                kmap[4][1]++;
                            }

                            kmap[1][b]++;
                            kmap[2][b]++;
                            kmap[3][b]++;
                            kmap[4][b]++;
                            kmap[1][b+1]++;
                            kmap[2][b+1]++;
                            kmap[3][b+1]++;
                            kmap[4][b+1]++;

                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[3]=1;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 8 rectangular elements


                        else if((a==1&&b==1&&kmap[1][4]!=0&&kmap[4][1]!=0&&kmap[4][4]!=0)||
                                (a==1&&b==4&&kmap[1][1]!=0&&kmap[4][1]!=0&&kmap[4][4]!=0)||
                                (a==4&&b==1&&kmap[1][4]!=0&&kmap[1][1]!=0&&kmap[4][4]!=0)||
                                (a==4&&b==4&&kmap[1][4]!=0&&kmap[4][1]!=0&&kmap[1][1]!=0))
                        {
                            kmap[1][1]++;
                            kmap[1][4]++;
                            kmap[4][1]++;
                            kmap[4][4]++;
                            output[1]=0;
                            output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 4 corner elements

                        else if((b==1&&kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0)||
                                (b==2&&kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b-1]!=0)||
                                (b==3&&kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0)||
                                (b==4&&kmap[a][b]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a][b-3]!=0))
                        {
                            kmap[a][1]++;
                            kmap[a][2]++;
                            kmap[a][3]++;
                            kmap[a][4]++;

                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((a==1&&kmap[a][b]!=0&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a+3][b]!=0)||
                                (a==2&&kmap[a][b]!=0&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a-1][b]!=0)||
                                (a==3&&kmap[a][b]!=0&&kmap[a+1][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0)||
                                (a==4&&kmap[a][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a-3][b]!=0))
                        {
                            kmap[1][b]++;
                            kmap[2][b]++;
                            kmap[3][b]++;
                            kmap[4][b]++;

                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }
                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 4 straight elements

                        else if(kmap[a-1][b]!=0 && kmap[a][b-1]!=0 && kmap[a-1][b-1]!=0)
                        {
                            if((a-1)==0)
                                kmap[4][b]++;
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b]++;
                            kmap[a-1][b]++;
                            kmap[a][b-1]++;
                            kmap[a-1][b-1]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;
                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0 && kmap[a][b-1]!=0 && kmap[a+1][b-1]!=0)
                        {
                            if((a+1)==5)
                                kmap[1][b]++;
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b]++;
                            kmap[a+1][b]++;
                            kmap[a][b-1]++;
                            kmap[a+1][b-1]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;
                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0 && kmap[a][b+1]!=0 && kmap[a+1][b+1]!=0)
                        {
                            if((a+1)==5)
                                kmap[1][b]++;
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b]++;
                            kmap[a+1][b]++;
                            kmap[a][b+1]++;
                            kmap[a+1][b+1]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;
                            if(b==1)
                            output[2]=0;
                            if(b==2)
                            output[3]=1;
                            if(b==3)
                            output[2]=1;
                            if(b==4)
                            output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a-1][b]!=0 && kmap[a][b+1]!=0 && kmap[a-1][b+1]!=0)
                        {
                            if((a-1)==0)
                                kmap[4][b]++;
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b]++;
                            kmap[a-1][b]++;
                            kmap[a][b+1]++;
                            kmap[a-1][b+1]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;
                            if(b==1)
                            output[2]=0;
                            if(b==2)
                            output[3]=1;
                            if(b==3)
                            output[2]=1;
                            if(b==4)
                            output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 4 square elements

                        else if(kmap[a-1][b]!=0)
                        {

                            if((a-1)==0)
                                kmap[4][b]++;

                            kmap[a-1][b]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;
                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0)
                        {
                            if((a+1)==0)
                                kmap[1][b]++;

                            kmap[a+1][b]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;
                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b-1]!=0)
                        {
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b-1]++;
                            kmap[a][b]++;

                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }
                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b+1]!=0)
                        {
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b+1]++;
                            kmap[a][b]++;

                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }
                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[3]=1;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 2 elements

                        else
                        {
                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }
                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // no elements can be enclosed

                    }
                }
            }
            judge=0;
            c=0;
        } // end if 4 variables
        Case--;
    }

}
