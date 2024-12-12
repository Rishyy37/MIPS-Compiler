#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<unordered_map>
#include<bitset>

#include "./simulator.cpp"

using namespace std;

vector<pair<string,int>> data_memory;
string baseAddress="";

// Opcodes for different instruction types
unordered_map<string, string> opcodes = {
    {"add", "000000"}, {"sub", "000000"}, {"and", "000000"}, {"or", "000000"}, {"slt", "000000"},
    {"lw", "100011"}, {"sw", "101011"},{"beq", "000100"}, {"addi", "001000"}, {"j", "000010"}
};

// Function codes for R-type instructions
unordered_map<string, string> funct_codes = {
    {"add", "100000"}, {"sub", "100010"}, {"and", "100100"}, {"or", "100101"}, {"slt", "101010"}
};

// register mapping
unordered_map<string, string> registers = {
    {"$zero", "00000"},{"$at","00001"},{"$v0","00010"},{"$v1","00011"},{"$a0", "00100"},{"$a1", "00101"},{"$a2", "00110"},{"$a3", "00111"},{"$t0", "01000"}, {"$t1", "01001"}, {"$t2", "01010"}, {"$t3", "01011"},{"$t4", "01100"},{"$t5", "01101"},{"$t6", "01110"},{"$t7", "01111"},
    {"$s0", "10000"}, {"$s1", "10001"}, {"$s2", "10010"}, {"$s3", "10011"}, {"$s4", "10100"}, {"$s5", "10101"}, {"$s6", "10110"}, {"$s7", "10111"},{"$t8", "11000"},{"$t9", "11001"},{"$gp", "11100"},{"$sp", "11101"},{"$fp", "11110"},{"$ra","11111"}
};

unordered_map<string, int> labelMap;

//remove spaces from both ends.
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

vector<string> makeWords(string line){

    vector<string> words;
    int index = 0;
    string s = "";

    while(index < line.size()){

        if(line[index] == ' ' || line[index] == ','){

            if(s.length() == 0){
                index++;
            }
            else{
                words.push_back(s);
                s = "";
                index++;
            }
        }
        else{
            s += line[index];
            index++;
        }
    }
    
    if(s.length() != 0){
        words.push_back(s);
    }

    return words;
}

string intToBinary16(int num) {
    bitset<16> binary(num);
    return binary.to_string();
}

string intToBinary26(int num) {
    bitset<26> binary(num);
    return binary.to_string();
}

string hexToBinary(const string &hexStr) {
    stringstream ss;
    ss << hexStr;
    unsigned int hexValue;
    ss >> hex >> hexValue;

    // Convert the hex value to binary (16 bits for 4 hex digits)
    bitset<16> binary(hexValue);
    return binary.to_string();
}

// Function to convert the last four hexadecimal digits of a memory address to binary
string convertAddressToBinary(int* address) {
    // Get the memory address
    uintptr_t addr = reinterpret_cast<uintptr_t>(address);
    
    // Convert the address to hexadecimal
    stringstream ss;
    ss << hex << addr;
    string hexAddress = ss.str();

    // Extract the last 4 digits of the hexadecimal address
    string lastFourHex = hexAddress.substr(hexAddress.length() - 4);

    // Convert the last four hex digits to binary
    return hexToBinary(lastFourHex);
}

//function to extract all hexadecimal numbers except last four hexadecimal numbers from address. 
string extractRemainingHexadecimal(int* ptr) {
    uintptr_t address = reinterpret_cast<uintptr_t>(ptr); // Convert pointer to uintptr_t
    stringstream ss;
    ss << hex << address;
    string hexAddress = ss.str();

    // Store the remaining hexadecimal digits (excluding the last 4)
    return hexAddress.substr(0, hexAddress.length() - 4);
}

vector<string> parseLabels(ifstream &input_file){
    string line;
    int line_number=-1;
    vector<string> instructions;
    while(getline(input_file,line)){
        line = trim(line);
        if (line.empty()){
            continue;
        }
        //line start with .text
        else if(line.rfind(".text",0)==0){
            line_number=0; //0 based indexing
        }
        else if(line_number!=-1){
            //position of :
            int colonPos = line.find(':');
            if (colonPos != -1) {
                //extracting label (procedure)name
                string label = trim(line.substr(0, colonPos));
                labelMap[label] = line_number;
                if(trim(line.substr(colonPos + 1)).size()>0){
                    instructions.push_back(trim(line.substr(colonPos + 1)));
                    line_number++;
                }
            }
            else{
                instructions.push_back(line);
                line_number++;
            }
        }
    }
    return instructions;
}

void parseDataSection(ifstream &input_file){
    string line;
    bool start=false;
    while(getline(input_file,line)){
        //remove extra spaces from start and end
        line = trim(line);

        if(line.empty()){
            continue;
        }
        //line start with .data
        else if(line.rfind(".data",0)==0){
            start=true;
        }
        //line start with .text
        else if(line.rfind(".text",0)==0){
            return;
        }
        else if(start){
            //extracting var_name and value
            stringstream MakeWords(line);
            string var_name,type,value;
            MakeWords>>var_name>>type>>value;

            //removing semi colon and converting to int
            var_name.pop_back();
            int int_value = stoi(value);

            if(type==".word"){
                data_memory.push_back({var_name,int_value});
            }
        }
    }
}

string parse_R_Type(string line){

    vector<string> tokens = makeWords(line);

    string operation = tokens[0];
    string rd = tokens[1];
    string rs = tokens[2];
    string rt = tokens[3];

    string op = opcodes[operation];
    string funct = funct_codes[operation];
    string rs_code = registers[rs];
    string rt_code = registers[rt];
    string rd_code = registers[rd];
    string shamt = "00000"; 

    return op +  rs_code +  rt_code +  rd_code +  shamt +  funct + "\n";
}

string parse_I_Type(string &line){
    if(line.rfind("addi",0)==0){

        vector<string> tokens = makeWords(line);

        string operation = tokens[0];
        string rt = tokens[1];
        string rs = tokens[2];
        string immediate = tokens[3];

        string op = opcodes[operation];
        string rt_code = registers[rt];
        string rs_code = registers[rs];
        string constant = intToBinary16(stoi(immediate));
        
        return op +  rs_code +  rt_code +  constant + "\n";
    }
    else if(line.rfind("beq",0)==0){

        vector<string> tokens = makeWords(line);

        string operation = tokens[0];
        string rs = tokens[1];
        string rt = tokens[2];
        string label = tokens[3];

        string op = opcodes[operation];
        string rs_code = registers[rs];
        string rt_code = registers[rt];
        string target = intToBinary16(labelMap[label]);

        return op +  rs_code +  rt_code +  target + "\n";
    }
    else if(line.rfind("lw",0)==0){

        vector<string> tokens = makeWords(line);

        string operation = tokens[0];
        string rs = tokens[1];
        string immediate = tokens[2];
        
        int index = -1;
        for(int i=0;i<data_memory.size();i++){
            if(immediate==data_memory[i].first){
                index=i;
                break;
            }
        }
        if(index==-1){
            cout<<"Variable does not exists in data memory"<<endl;
            exit(-1);
        }

        string op = opcodes[operation];
        string rs_code = registers[rs];
        string constant =  convertAddressToBinary(&data_memory[index].second);

        return op +  "00000" +  rs_code +  constant + "\n";
    }
    else if(line.rfind("sw",0)==0){

        vector<string> tokens = makeWords(line);

        string operation = tokens[0];
        string rs = tokens[1];
        string immediate = tokens[2];

        int index = -1;
        for(int i=0;i<data_memory.size();i++){
            if(immediate==data_memory[i].first){
                index=i;
                break;
            }
        }
        if(index==-1){
            cout<<"Variable does not exists in data memory"<<endl;
            exit(-1);
        }

        string op = opcodes[operation];
        string rs_code = registers[rs];
        string constant =  convertAddressToBinary(&data_memory[index].second);

        return op +  "00000" +  rs_code +  constant + "\n";
    }
    else{
        return "unidentified expression";
    }
}

string parse_J_Type(string &line){

    vector<string> tokens = makeWords(line);

    string operation = tokens[0];
    string label = tokens[1];

    string op = opcodes[operation];
    string target = intToBinary26(labelMap[label]);
    
    return op +  target + "\n";
}

int main(){
    ifstream input_file;
    input_file.open("demo.asm");

    if(!input_file.is_open()){
        cout<<"Unable to open input file"<<endl;
        return -1;
    }

    ofstream output_file;
    output_file.open("machine_code.txt");

    if(!output_file.is_open()){
        cout<<"Unable to open output file."<<endl;
        return -1;
    }

    //take care of .data section (data memory)
    parseDataSection(input_file);

    if(data_memory.size()>0){
        // cout<<&data_memory[0].second<<endl;
        baseAddress = extractRemainingHexadecimal(&data_memory[0].second);
    }
    // cout<<baseAddress;

    //moving back to start of file
    input_file.clear();
    input_file.seekg(0, ifstream::beg);

    //knowing where the labels are present.
    vector<string> instructions = parseLabels(input_file);

    // for(int i=0;i<data_memory.size();i++){
    //     cout<<data_memory[i].first<<" "<<data_memory[i].second<<" "<<&data_memory[i].second<<endl;;
    // }

    for(int i=0;i<instructions.size();i++){
        string line = instructions[i];

        if((line.rfind("add",0)==0 && line.rfind("addi",0)!=0)|| line.rfind("sub",0)==0 || line.rfind("and",0)==0 || line.rfind("or",0)==0 || line.rfind("slt",0)==0){
            
            output_file<<parse_R_Type(line);
        }
        else if(line.rfind("lw",0)==0 || line.rfind("sw",0)==0 || line.rfind("addi",0)==0 || line.rfind("beq",0)==0){

            output_file<<parse_I_Type(line);
        }
        else if(line.rfind("j",0)==0){

            output_file<<parse_J_Type(line);
        }
    }

    input_file.close();
    output_file.close();

    simulate(data_memory,baseAddress);

    return 0;
}