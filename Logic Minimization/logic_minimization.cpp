#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string.h>
#include <stdio.h>
using namespace std;

void receiver(vector<string>&, string );        //this function is to store the data properly
void to_string_converter(vector<vector<string>>&,vector<string>);   //this function is to divide each minterm
int if_can_combine(string,string);      //this function is to determine whether the two can combine to one implicant
void find_implicant(vector<vector<string>>&,int);       //this function use resursion to find the prime iimplicant
void output(vector<string>, vector<char>,ofstream&);    //this function output the file
void find_the_key(vector<string>&);     //this function find the essential implicant
void decoder(vector<string>, vector<string>&,vector<vector<string>>&);  //this function show the must appearing minterm
void dash_eliminater(string, vector<string>&);      //this function transform the complicated implicant into its member implicants
bool correct(vector<vector<string>>, vector<string>, string);   //this function test whether the function is the one I need
string string_creator(int , int );      //this function is set to create the string that I need in correct
string run_to_death(int, bool [],int);      //its a string with length equaling to numbers of variables

int main()
{
    string filename;
    cin>>filename;
    ifstream infile(filename,ios::in);
    string outputfilename;
    outputfilename.assign(filename,0,5);          //create file name
    outputfilename+="out.eqn";
    ofstream outfile(outputfilename,ios::out);
    vector<char>name;
    char chr;
    string str;
    infile>>str>>str>>chr;
    while(chr!=';'){
        name.push_back(chr);
        infile>>chr;
    }
    infile.seekg(0);
    getline(infile,str);
    outfile<<str<<endl;
    getline(infile,str);
    outfile<<str<<endl;
    outfile<<"output = ";
    infile>>str>>str>>str;
    vector<vector<string>>implicant;    //this vector store all the prime implicant
    vector<string>start;
    receiver(start,str);
    to_string_converter(implicant,start);   //divide the string
    find_implicant(implicant,0);        //0 is the first layer
    vector<string>answer;
    for(int i=0;i<implicant.size();i++){
        for(int j=0;j<implicant[ i ].size();j++){
            answer.push_back(implicant[ i ][ j ]);
        }
    }
    find_the_key(answer);   //delete unnecessary implicant
   output(answer, name, outfile);   //output file
    return 0;
}

void receiver(vector<string>&start, string original){
    string member;
    char *str =new char[original.size()+1];
    strcpy(str,original.c_str());   //this transform c++string to cstring
    const char *delim=")+(";    //the deliminator
    char *token=strtok(str,delim);
    while( token != NULL ){
        member=token;
        start.push_back(member);
        token=strtok(NULL,delim);
    }
}

void to_string_converter(vector<vector<string>>&implicant,vector<string>start){
    vector<string>trash;
    implicant.push_back(trash);
    for(int i=0;i<start.size();i++){
        string member;
        char *str =new char[start[ i ].size()+1];       //transform the received data to a form that can be processed easily
        strcpy(str,start[ i ].c_str());
        const char *delim="*";
        char *token=strtok(str,delim);
        while( token != NULL ){
            string temp=token;
            if(temp.size()>1) member+="0";
            else member+="1";
            token=strtok(NULL,delim);
        }
        implicant[0].push_back(member);
    }
}

int if_can_combine(string str1, string str2){
    int difference=0;
    int location;
    for(int i=0;i<str1.size();i++){
        if(str1[ i ]!=str2[ i ]){       //this means they only have one place different
                difference++;
                location=i;
        }
    }
    if(difference!=1)   return -1;      //-1 means not success
    else return location;
}

void find_implicant(vector<vector<string>>& implicant, int layer){
    bool arr[implicant[layer].size()]={};       //use to found a series of binary code
    vector<string>next;
    bool bol=0;     //open for the logic if reaching the goal
    for(int i=0;i<implicant[ layer ].size();i++){
        for(int j=i;j<implicant[ layer ].size();j++){
            if(if_can_combine(implicant[layer][ i ],implicant[layer][ j ])!=-1){
                int location=if_can_combine(implicant[layer][ i ],implicant[layer][ j ]);
                arr[ i ]=arr[ j ]=1;
                string str=implicant[layer][ i ];
                str[ location]='-';     //which means zero and one
                next.push_back(str);
                bol=1;
            }
        }
    }
    if(next.size()!=0)  implicant.push_back(next);
    vector<string>current;
    for(int i=0;i<implicant[layer].size();i++){
        if(arr[ i ]==0){
            current.push_back(implicant[layer][ i ]);
        }
    }
    implicant[layer].clear();       //to store a new data

    for(int i=0;i<current.size();i++){
        implicant[layer].push_back(current[ i ]);
    }
    if(bol==1){
        find_implicant(implicant,layer+1);
    }
    if(bol==0){
        vector<string>temp;
        bool boool;
        for(int i=0;i<implicant[layer].size();i++){
            boool=0;
            string str=implicant[layer][ i ];
            for(int j=0;j<temp.size();j++){
                if(str==temp[ j ])  boool=1;
            }
            if(boool==0)    temp.push_back(str);
        }
        implicant.pop_back();
        implicant.push_back(temp);      //create new memory
    }
}

bool correct(vector<vector<string>>fire, vector<string>lamp, string str){
    vector<vector<string>>accumulation;
    for(int i=0;i<str.size();i++){
        if(str[ i ]=='1'){      //things i need
            accumulation.push_back( fire[ i ]);
        }
    }
    vector<string>horizontal;   //transform 2d into 1d
    for(int i=0;i<accumulation.size();i++){
        for(int j=0;j<accumulation[ i ].size();j++){
            horizontal.push_back(accumulation[ i ][ j ]);
        }
    }
    bool bol;
    for(int i=0;i<lamp.size();i++){ //lamp is unnecessary in fact
        bol=0;
        for(int j=0;j<horizontal.size();j++){
            if(lamp[ i ]==horizontal[ j ]){
                bol=1;
                break;
            }
        }
        if(bol==0) break;
    }
    if(bol==0)  return 0;
    else    return 1;
}

string run_to_death(int digit, bool arr[], int shortest){
    int number_of_1;
    string str;     //run out all the possible binary combination
    static bool bol=0;
    do{
        number_of_1=0;
        if(bol==1){
            int i=0;
            while(1){
                if(arr[ i ]==0){
                    arr[ i ]=1;
                    break;
                }
                if(arr[ i ]=1){
                    arr[ i ]=0;
                }
                i++;
            }
        }
        bol=1;
        for(int i=digit-1;i>=0;i--){
            str+=to_string(arr[ i ]);
            if(arr[ i ]==1) number_of_1++;
        }
        if(number_of_1==digit)  break;
    }while(number_of_1>shortest);
    return str;
}

void find_the_key(vector<string>& answer){
    vector<string>lamp;     //the goal
    vector<vector<string>>fire;     //the content of each implicant
    decoder(answer,lamp,fire);
    vector<vector<string>>all_possible;     //the first layer
    vector<vector<string>>least_size_possible;      //the second one
    vector<string>member;   //use as a temp
    bool token=0;
    bool arr[fire.size()]={};
    int shortest=1000000; //this number is meaningless, just a big number
    do{
        string str=run_to_death(fire.size(),arr, shortest);

        if(correct( fire, lamp, str)==1){
            int shortest=0;
            for(int i=0;i<str.size();i++){
                if(str[ i ]=='1'){
                    shortest++;
                }
            }
            for(int j=0;j<answer.size();j++){
                if(str[ j ]=='1'){
                    member.push_back(answer[ j ]);
                }
            }
            all_possible.push_back(member);
            member.clear();
        }
        token=1;
        for(int i=0;i<str.size();i++){
            if(str[ i ]=='0'){
                token=0;
                break;
            }
        }
    }while(token==0);
    int least_size=all_possible.at(0).size();
    for(int i=0;i<all_possible.size();i++){
        if(least_size>all_possible[ i ].size()){
            least_size=all_possible[ i ].size();
        }
    }
     for(int i=0;i<all_possible.size();i++){
        if(least_size==all_possible[ i ].size()){
            least_size_possible.push_back(all_possible[ i ]);
        }
    }
    vector<int>least;
    int numbers=100000000;      //this number is meaningless, just a big number
    for(int i=0;i<least_size_possible.size();i++){
        int counter=0;
        for(int j=0;j<least_size_possible[ i ].size();j++){
            for(int k=0;k<least_size_possible[ i ][ j ].size();k++){
                if(least_size_possible[ i ][ j ][ k ]=='-') counter++;
            }
        }
        least.push_back(counter);
        if(numbers>counter) numbers=counter;
    }


    int key;
    for(int i=0;i<least.size();i++){
        if(numbers==least[ i ]){
            key=i;
            break;
        }
    }
    answer.clear();
    for(int i=0;i<least_size_possible[ key ].size();i++){
        answer.push_back(least_size_possible[ key ][ i ]);
    }
}

void decoder(vector<string> answer, vector<string>&lamp,vector<vector<string>>&fire){
    vector<string>combination;
    for(int i=0;i<answer.size();i++){
        dash_eliminater(answer[ i ],combination);
        fire.push_back(combination);        //create the content of each implicant
        combination.clear();
    }
    for(int i=0;i<fire.size();i++){
        for(int j=0;j<fire[ i ].size();j++){
            bool bol=0;
            for(int k=0;k<lamp.size();k++){
                if(lamp[ k ]==fire[ i ][ j ]){
                    bol=1;
                    break;
                }
            }
            if(bol==0){
                    lamp.push_back(fire[ i ][ j ]);
                }
        }
    }
}

void dash_eliminater(string str,vector<string>&combination){
    vector<int>location;
    for(int i=0;i<str.size();i++){
        if(str[ i ]=='-')   location.push_back( i );    //find all the possible combination
    }
    for(int i=0;i<pow(2,location.size());i++){
        string new_str=str;
        string substitution=string_creator( i, location.size());
        for(int j=0;j<location.size();j++){
            new_str[ location[ j ] ]=substitution[ j ];
        }
        combination.push_back(new_str);
    }
}

string string_creator(int number, int digit){
    string str;
    int total=0;
    int k=1;
    while(number!=0){
        total+=k*(number%2);
        k*=10;
        number/=2;
    }
    str=to_string(total);
    if(str.size()!=digit){
        int diff=digit-str.size();
        string str1;
        for(int i=0;i<diff;i++){        //to ensure it has enough numbers
            str1+="0";
        }
        str1+=str;
        return str1;
    }
    else return str;
}

void output(vector<string> logic, vector<char>name, ofstream &outfile){
    for(int i=0;i<logic.size();i++){
        if(i!=0) outfile<<"+";
        int if_parenthesis=0;
        for(int k=0;k<logic[ i ].size();k++){
            if(logic[ i ][ k ]=='0' || logic[ i ][k]=='1')    if_parenthesis++;     //if needing to output
        }
        if(if_parenthesis==1){
            for(int k=0;k<logic[ i ].size();k++){
                if(logic[ i ][ k ]=='0'){
                    outfile<<"!"<<name[ k ];
                    break;
                }
                else if(k==1){
                    outfile<<name[ k ];
                    break;
                }
            }
        }
        else{
            outfile<<"(";
            for(int j=0;j<logic[ i ].size();j++){
                if( j!=0 && logic[ i ][ j -1]!='-'){
                    if( j==logic[ i ].size()-1 &&logic[ i ][ j ]=='-')  break;
                    outfile<<"*";
                }
                if(logic[ i ][ j ]=='1'){
                    outfile<<name[ j ];
                }
                else if(logic[ i ][ j ]=='0'){
                    outfile<<"!"<<name[ j ];
                }
            }
            outfile<<")";
        }
    }
}
