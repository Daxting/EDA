#include <iostream>
#include<fstream>
#include<vector>
#include<string>
#include<iomanip>
#include<math.h>
using namespace std;
struct stage{    //the struct is designed to store the data of each stage
    int stage_number;
    vector<string>resistance_series;
    vector<string>resistance_parallel;
    vector<double>resistance_series_value;
    vector<char>resistance_series_unit;
    vector<double>resistance_parallel_value;
    vector<char>resistance_parallel_unit;
};
string demo(double,char );  //to use a prefix to help print the value more simply
int input_selection();     //to let users choose function they want
double resistance_transformer(double,char);     //transform all the resistance into the unit of 'x'
void calculation (vector<struct stage>,vector<string>,double);      //function 2
void swap_stage (vector<struct stage>&,vector<string>&);        //function 3
void merge_stage (vector<struct stage>&,vector<string>&);       //function 4
void output_file(vector<struct stage>,vector<string>,double);       //function 5
void input_file(vector<struct stage>&, double &);          //function 1
void assistant(vector<struct stage>, double ,vector<double>&,vector<double>&,vector<string>&,vector<double>&);  //function 2 &function 5 assistant

int main()
{
    cout<<"*** Welcome to MySpice ***"<<endl;
    int input;
    double voltage=-1;      //-1 is default to execute fool-proof
    vector<struct stage> circuit;   //store data
    vector<string> nodes;   //store the names of nodes
    while(1){       //sentinel-guard loop
       input=input_selection();     //user input selection
       if(voltage==-1&&(input!=1 && input!=6) ){    //fool-proof
            cout<<"There is no circuit."<<endl;
            continue;
       }
        switch(input){
        case 1 :
            circuit.clear();    //ensure the vector clear
            input_file(circuit,voltage);    //function to input the data
            for(int i=0;i<=circuit.size();i++){
                if(i==0){
                        nodes.push_back("vdd");
                        continue;
                }
                string name="n";
                name+=std::to_string( i);
                nodes.push_back(name);      //input the names of nodes
            }
            break;
        case 2 :
            calculation(circuit,nodes,voltage);
            break;
        case 3 :
            swap_stage(circuit,nodes);
            break;
        case 4 :
            merge_stage(circuit,nodes);
            break;
        case 5 :
            output_file(circuit,nodes,voltage);
            break;
        case 6 :
            cout<<"Bye~"<<endl;
            return 0;
            break;
        }
    }
}

void output_file(vector<struct stage>circuit, vector<string>nodes, double voltage){
    cout<<"Please enter the name of the output file: ";
    string str;     //file name
    cin>>str;
    ofstream outfile(str,ios::out);
    cout<<"Exporting the file..."<<endl;
    vector<double>nodes_voltage;        //store the voltage of each nodes
    vector<double>stage_current;        //store the current of each stage
    vector<string>resistance_name;      //the name of each resistance
    vector<double>resistance_value;
    assistant(circuit,voltage,nodes_voltage,stage_current,resistance_name,resistance_value);    //divide the function because this part is also used by function 2
    outfile<<"***circuits***"<<endl;
    outfile<<"stage "<<circuit.size()<<endl<<endl;
    outfile<<"v1 vdd gnd "<<voltage<<"V"<<endl<<endl;
    int k=0;    //to ensure the resistance print in order
    for(int i=0;i<circuit.size();i++){
        for(int j=0;j<circuit[ i ].resistance_series.size();j++){
            outfile<<left<<setw(8)<<resistance_name[ k ];
            k++;
            if(j==0)    outfile<<left<<setw(8)<<nodes[ i ]; // the first one
            else{
                str="ns"+std::to_string( circuit[ i ].stage_number )+"_"+std::to_string( j );   //the middle one
                outfile<<left<<setw(8)<<str;
            }
            if(j==circuit[ i ].resistance_series.size()-1)    outfile<<left<<setw(8)<<nodes[ i+1 ]; //the last one
            else{
                str="ns"+std::to_string( circuit[ i ].stage_number )+"_"+std::to_string( j +1);
                outfile<<left<<setw(8)<<str;
            }
            outfile<<demo(resistance_transformer(circuit[ i ].resistance_series_value[ j ],circuit[ i ].resistance_series_unit[ j ]),' ')<<endl;
        }

        for(int j=0;j<circuit[ i ].resistance_parallel.size();j++){
            outfile<<left<<setw(8)<<resistance_name[ k ];
            k++;
            if(j==0)    outfile<<left<<setw(8)<<nodes[ i+1 ];   //the first one
            else{
                str="np"+std::to_string( circuit[ i ].stage_number )+"_"+std::to_string( j ); //the middle one
                outfile<<left<<setw(8)<<str;
            }
            if(j==circuit[ i ].resistance_parallel.size()-1)    outfile<<left<<setw(8)<<"gnd";      //the last one
            else{
                str="np"+std::to_string( circuit[ i ].stage_number )+"_"+std::to_string( j+1 );
                outfile<<left<<setw(8)<<str;
            }
            outfile<<demo( resistance_transformer(circuit[ i ].resistance_parallel_value[ j ],circuit[ i ].resistance_parallel_unit[ j ]),' ')<<endl;
        }
    }
    outfile<<endl<<"***calculation results***"<<endl;
    outfile<<"**************************************"<<endl;
    outfile<<"**"<<right<<setw(21)<<"<<voltage>>"<<setw(15)<<"**"<<endl;
    for(int i=0;i<nodes.size();i++)
        outfile<<setw(4)<<left<<"**"<<setw(12)<<nodes[ i ]<<setw(20)<<demo(nodes_voltage[ i ],'v')<<"**"<<endl;

    outfile<<"**"<<right<<setw(21)<<"<<current>>"<<setw(15)<<"**"<<endl;
    for(int i=0;i<resistance_name.size();i++)
        outfile<<setw(4)<<left<<"**"<<setw(12)<<resistance_name[ i ]<<setw(20)<<demo(resistance_value[ i ],'A')<<"**"<<endl;

    outfile<<left<<setw(36)<<"**"<<"**"<<endl;
    outfile<<setw(20)<<"**"<<setw(16)<<"myspice2021"<<"**"<<endl;
    outfile<<"**************************************"<<endl;
    cout<<"The output file successfully exported!"<<endl;
}

void assistant(vector<struct stage>circuit, double voltage,vector<double>&nodes_voltage,vector<double>&stage_current,vector<string>&resistance_name,vector<double>&resistance_value){
    vector<double>current_diff;
    nodes_voltage.push_back(voltage);
    while(nodes_voltage.size()<=circuit.size()+1){
        double part1=0, part2 =0, total_resistance=0;           //large amount of parameter is necessary because they are not in a <struct> vector
        int i;      //to record the nodes numbers
        for(i=circuit.size()-1;i>=nodes_voltage.size();i--){    //calculate the value based on the number it needs
            part1=part2=0;  //initialized
            for(int j=0;j<circuit[ i-1 ].resistance_parallel_value.size();j++)
                part1+=resistance_transformer(circuit[ i-1 ].resistance_parallel_value[ j ] , circuit[ i-1 ].resistance_parallel_unit[ j ]);    //the smaller stage
            for(int j=0;j<circuit[ i ].resistance_series_value.size();j++)
                part2+=resistance_transformer(circuit[  i ].resistance_series_value[ j ] , circuit[  i ].resistance_series_unit[ j ]);  //the bigger stage
            if(i==circuit.size()-1){
                for(int j=0;j<circuit[ i ].resistance_parallel_value.size();j++)
                    part2+=resistance_transformer(circuit[  i ].resistance_parallel_value[ j ] , circuit[  i ].resistance_parallel_unit[ j ]);  //the bigger stage
            }
            else{
                part2+= total_resistance;   //the bigger stage
            }
            total_resistance=part1*part2/(part1+part2);     //parallel resistance calculation
        }
        if(nodes_voltage.size()==circuit.size()){       //the last one
            for(int j=0;j<circuit[ circuit.size()-1 ].resistance_parallel_value.size();j++)
                    total_resistance+=resistance_transformer(circuit[ circuit.size()-1 ].resistance_parallel_value[ j ] , circuit[ circuit.size()-1 ].resistance_parallel_unit[ j ]);
        }
        double total=0;     //to record the resistance
        for(int j=0;j<circuit[ i ].resistance_series_value.size();j++){
                total+=resistance_transformer(circuit[ i ].resistance_series_value[ j ] , circuit[ i ].resistance_series_unit[ j ]);
        }
        double current=nodes_voltage[ nodes_voltage.size()-1 ]/(total+total_resistance);
        stage_current.push_back(current);
        double diff=(double)nodes_voltage[nodes_voltage.size()-1]-current*total;
        nodes_voltage.push_back(diff );         //to round the value to three after decimal point
    }

    for(int i=0;i<stage_current.size()-1;i++){
        current_diff.push_back(stage_current[ i ]-stage_current[ i+1 ]);
    }
    current_diff.push_back(stage_current[stage_current.size()-1]);

    for(int i=0;i<circuit.size();i++){
        for(int j=0;j<circuit[ i ].resistance_series_value.size();j++){
            resistance_name.push_back(circuit[ i ].resistance_series[ j ]);
            resistance_value.push_back(stage_current[ i ]);
        }
         for(int j=0;j<circuit[ i ].resistance_parallel_value.size();j++){
            resistance_name.push_back(circuit[ i ].resistance_parallel[ j ]);
            if(i==circuit.size()-1) resistance_value.push_back(stage_current[ i ]);
            else    resistance_value.push_back(current_diff[ i ]);
        }
    }
}

void calculation (vector<struct stage>circuit,  vector<string>nodes, double voltage){
    cout<<"Calculating..."<<endl;
    vector<double>nodes_voltage;        //the voltage of each nodes
    vector<double>stage_current;        //the current of each stages
    vector<string>resistance_name;      //help resistance name print in order
    vector<double>resistance_value;     //help resistance value print in order
    assistant(circuit,voltage,nodes_voltage,stage_current,resistance_name,resistance_value);
    cout<<"The result is successfully calculated!"<<endl;
    cout<<"**************************************"<<endl;
    cout<<"**"<<right<<setw(21)<<"<<voltage>>"<<setw(15)<<"**"<<endl;
    for(int i=0;i<nodes.size();i++)
        cout<<setw(4)<<left<<"**"<<setw(12)<<nodes[ i ]<<setw(20)<<demo(nodes_voltage[ i ],'v')<<"**"<<endl;

    cout<<"**"<<right<<setw(21)<<"<<current>>"<<setw(15)<<"**"<<endl;
    for(int i=0;i<resistance_name.size();i++)
        cout<<setw(4)<<left<<"**"<<setw(12)<<resistance_name[ i ]<<setw(20)<<demo(resistance_value[ i ],'A')<<"**"<<endl;

    cout<<left<<setw(36)<<"**"<<"**"<<endl;
    cout<<setw(20)<<"**"<<setw(16)<<"myspice2021"<<"**"<<endl;
    cout<<"**************************************"<<endl;

}

string demo(double value,char kind){   //use two parameter, "kind" is the type of the variable, like v, A, NUM
     if(value>1000000000){
            string str=std::to_string(value/(double)1000000000);
            return str.substr(0,str.size()-3)+"G"+kind;     //to_string&substr function help print beautifully
    }
    if(value>1000000){
            string str=std::to_string(value/(double)1000000);
            return str.substr(0,str.size()-3)+"Meg"+kind;   //to_string&substr function help print beautifully
    }
   if(value>1000){
            string str=std::to_string(value/(double)1000);
            return str.substr(0,str.size()-3)+"K"+kind; //to_string&substr function help print beautifully
    }
    if(value<0.001){
            string str=std::to_string(value*(double)1000000);
            return str.substr(0,str.size()-3)+"u"+kind;     //to_string&substr function help print beautifully
    }
    if(value<1){
            string str=std::to_string(value*(double)1000);
            return str.substr(0,str.size()-3)+"m"+kind;     //to_string&substr function help print beautifully
    }
   else{
       string str=std::to_string(value);
       return str.substr(0,str.size()-3)+kind;      //to_string&substr function help print beautifully
   }
}

void swap_stage (vector<struct stage>&circuit, vector<string>&nodes){
    int num1,num2;
    while(1){
        cout<<"Please enter the first stage you want to swap: ";
        cin>>num1;
        if(num1>circuit.size()){        //fool-proof
            cout<<"The node doesn't exist. Please enter again."<<endl;
        }
        else    break;
    }
    while(1){
        cout<<"Please enter the second stage you want to swap: ";
        cin>>num2;
        if(num2>circuit.size()){        //fool-proof
            cout<<"The node doesn't exist. Please enter again."<<endl;
        }
        else    break;
    }
    if(num1>num2) swap(num1,num2);  //to ensure num1 is the smaller one
    stage temp=circuit.at(num1-1);  //switch the two stage
    circuit.at(num1-1)=circuit.at(num2-1);
    circuit.at(num2-1)=temp;
    string str=nodes[ num1];    //switch the two stage
    nodes[num1]=nodes[num2];
    nodes[num2]=str;

    cout<<circuit[ num2-1 ].resistance_series[ 0 ]<<" is changing node from "<<nodes[num1-1]<<" to "<<nodes[num2-1]<<" ..."<<endl;
    cout<<circuit[ num1-1 ].resistance_series[ 0 ]<<" is changing node from "<<nodes[num2]<<" to "<<nodes[num1-1]<<" ..."<<endl;
    if(num2!=circuit.size()){       //if num2 is the last stage in circuit, print two stage change will be enough
         cout<<circuit[ num2 ].resistance_series[ 0 ]<<" is changing node from "<<nodes[num1]<<" to "<<nodes[num2]<<" ..."<<endl;
    }
    cout<<"The stage is successfully swapped!"<<endl;
}

void merge_stage (vector<struct stage>&circuit,  vector<string>&nodes){
    int num1,num2;
     while(1){
        cout<<"Please enter the first stage you want to merge: ";
        cin>>num1;
        if(num1>circuit.size()){        //fool-proof
            cout<<"The node doesn't exist. Please enter again."<<endl;
        }
        else    break;
    }
    while(1){
        cout<<"Please enter the second stage you want to merge: ";
        cin>>num2;
        if(num2>circuit.size()){        //fool-proof
            cout<<"The node doesn't exist. Please enter again."<<endl;
        }
        else    break;
    }
    if(num1>num2)   swap(num1,num2);    //to ensure the stage are merged to the smaller one
    cout<<"Resistors are merged into RP" <<num1<<"."<<endl;
    double part1=0, part2=0, total_resistance=0;    //these variables are used to calculate total resistance of each condition
    for(int i=0;i<circuit[ num1-1].resistance_parallel_value.size();i++)
        part1+=resistance_transformer(circuit[ num1-1].resistance_parallel_value[ i ] , circuit[ num1-1].resistance_parallel_unit[ i ]);    //the small stage
    for(int i=0;i<circuit[ num2-1 ].resistance_series_value.size();i++)
        part2+=resistance_transformer(circuit[ num2-1 ].resistance_series_value[ i ] , circuit[ num2-1 ].resistance_series_unit[ i ]);      //the big stage
    for(int i=0;i<circuit[ num2-1 ].resistance_parallel_value.size();i++)
        part2+=resistance_transformer(circuit[  num2-1 ].resistance_parallel_value[ i ] , circuit[  num2-1 ].resistance_parallel_unit[ i ]);    //the big stage

    total_resistance=part1*part2/(part1+part2); //parallel resistance calculation
    cout<<"=> RP"<<num1<<" "<<nodes[ num1 ]<<" gnd "<<fixed<<setprecision(3)<<demo(total_resistance,' ')<<endl;
    cout<<"The stage is Successfully merged!"<<endl;
    circuit[num1-1].resistance_parallel.clear();        //clear the merged stage data
    circuit[num1-1].resistance_parallel_value.clear();      //clear the merged stage data
    circuit[num1-1].resistance_parallel_unit.clear();       //clear the merged stage data
    circuit[num1-1].resistance_parallel_value.push_back(total_resistance);      //the newly combined resistance
    circuit[num1-1].resistance_parallel_unit.push_back('x');    //because it has no prefix
    string str=std::to_string(num1);    // to_string function help transform the int type into string
    string str1="RP"+str.substr(0,str.size()-6);      //new resistance
    circuit[num1-1].resistance_parallel.push_back(str1);
    circuit.erase(circuit.begin()+num2-1);  //delete the specific vector space
    nodes.erase(nodes.begin()+num2);
}

double resistance_transformer(double value, char unit){     //to transform the value into a standard unit
    if(unit=='u')  return  value/1000000;
    if(unit=='m')  return  value/1000;
    if(unit=='K')  return  value*1000;
    if(unit=='M')  return value*1000000;
    if(unit=='G')  return  value*1000000000;
    else return value;
}

int input_selection(){
        int input;
        cout<<"=========================================="<<endl;
        cout<<"Please type 1 to input file"<<endl;
        cout<<"Please type 2 to calculate voltage and current"<<endl;
        cout<<"Please type 3 to swap stage"<<endl;
        cout<<"Please type 4 to merge stage"<<endl;
        cout<<"Please type 5 to output file"<<endl;
        cout<<"Please type 6 to exit"<<endl;
        cout<<"Enter your selection Here: ";
        cin>>input;
        cout<<"=========================================="<<endl;
        return input;
}

void input_file(vector<struct stage>&circuit, double &voltage){
        string str;
        double temp_double;
        char temp_char;
        cout<<"Please enter the name of the input file:";
        while(1){
            bool bol=1; //to avoid the stage has no resistance
            cin>>str;
            ifstream file(str, ios::in);
            if(file){
                cout<<"Loading the file..."<<endl;
                cout<<"The input file successfully loaded!"<<endl;
                string trash;
                int stage_numbers, memory;
                file>>trash>>stage_numbers>>trash>>trash>>trash>>voltage>>trash;    //trash are info that I don't need
                for(int i=0;i<stage_numbers;i++){
                    stage hello;    //hello is the space storing the data temporary
                    hello.stage_number=i+1;
                    while(1){
                        if(bol==1){
                                file>>trash;
                                bol=0;
                        }
                        if(static_cast<int>(trash[3])==95){         //to examine whether it toggle the resistance type
                            memory=static_cast<int>(trash[2])-48;
                        }
                        else{
                            memory=10*(static_cast<int>(trash[2])-48)+static_cast<int>(trash[3])-48;
                        }
                        if(memory!=i+1){
                            circuit.push_back(hello);       //put the data into vector
                            break;
                        }
                        else{
                            bol=1;
                            if(trash[1]=='S'){
                                hello.resistance_series.push_back(trash);       //data that will be used next
                                file>>trash>>trash>>temp_double;
                                hello.resistance_series_value.push_back(temp_double);   //data that will be used next
                                if(file.peek()!='\n') {
                                        file>>temp_char;
                                        if(temp_char=='M')  file>>trash;
                                        hello.resistance_series_unit.push_back(temp_char);  //data that will be used next
                                }
                                else hello.resistance_series_unit.push_back('x');   //to ensure the data stored in order
                            }
                            else if(trash[1]=='P'){
                                hello.resistance_parallel.push_back(trash);//data that will be used next
                                file>>trash>>trash>>temp_double;
                                hello.resistance_parallel_value.push_back(temp_double);//data that will be used next
                                if(file.peek()!='\n') {
                                        file>>temp_char;
                                        if(temp_char=='M')  file>>trash;
                                        hello.resistance_parallel_unit.push_back(temp_char);//data that will be used next
                                }
                                else hello.resistance_parallel_unit.push_back('x'); //to ensure the data stored in order
                            }
                        }
                    }
                }
                return ;    //return to main function
            }
            else{
                cout<<"The file is not found."<<endl;
                cout<<"Please enter the name of the input file again: ";
            }
        }
}
