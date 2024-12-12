#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <bitset>
#include <cstdint> 
#include<math.h>

using namespace std;

 vector<string> mipsRegisters = {
        "$zero", "$at", "$v0", "$v1", // 0-3
        "$a0", "$a1", "$a2", "$a3",   // 4-7
        "$t0", "$t1", "$t2", "$t3",   // 8-11
        "$t4", "$t5", "$t6", "$t7",   // 12-15
        "$s0", "$s1", "$s2", "$s3",   // 16-19
        "$s4", "$s5", "$s6", "$s7",   // 20-23
        "$t8", "$t9", "$k0", "$k1",   // 24-27
        "$gp", "$sp", "$fp", "$ra"    // 28-31
    };

// Function to convert a 16-bit binary string to decimal
int binaryToDecimal(const string& binaryStr) {
    int decimalValue = 0;

    // Check if it's a negative number (two's complement)
    bool isNegative = (binaryStr[0] == '1');

    // If the number is negative, use two's complement rules
    if (isNegative) {
        // Invert all bits (flip 1s to 0s and 0s to 1s)
        string invertedBinaryStr = binaryStr;
        for (int i = 0; i < invertedBinaryStr.length(); i++) {
            invertedBinaryStr[i] = (invertedBinaryStr[i] == '1') ? '0' : '1';
        }

        // Convert the inverted binary to decimal
        for (int i = 0; i < invertedBinaryStr.length(); i++) {
            if (invertedBinaryStr[i] == '1') {
                decimalValue += pow(2, invertedBinaryStr.length() - 1 - i);
            }
        }

        // Apply two's complement by adding 1 and then negating the value
        decimalValue = -(decimalValue + 1);
    } else {
        // Convert the positive binary to decimal
        for (int i = 0; i < binaryStr.length(); i++) {
            if (binaryStr[i] == '1') {
                decimalValue += pow(2, binaryStr.length() - 1 - i);
            }
        }
    }

    return decimalValue;
}

string binaryToHex(const string& binaryStr) {
    // Convert the binary string into an integer
    unsigned long decimal = bitset<64>(binaryStr).to_ulong();

    // Convert the decimal number to hexadecimal
    stringstream ss;
    ss << hex << decimal;
    return ss.str();
}

uintptr_t getAddressFromHexAndBinary(const string& hexPart, const string& binaryPart) {
    // Convert the binary part to hexadecimal
    string binaryHex = binaryToHex(binaryPart);

    // Concatenate the hexadecimal part and the binary-turned-hexadecimal part
    string fullAddressStr = hexPart + binaryHex;

    // Convert the concatenated string back to an address
    uintptr_t address;
    stringstream ss;
    ss << hex << fullAddressStr;
    ss >> address;

    return address;
}

void execute_R_Type(string &instruction,vector<int> &register_values,int &program_counter){
    //add
    if(instruction.substr(26,6)=="100000"){
        int rs = binaryToDecimal(instruction.substr(6,5));
        int rt = binaryToDecimal(instruction.substr(11,5));
        int rd = binaryToDecimal(instruction.substr(16,5));

        register_values[rd] = register_values[rs]+register_values[rt];
        program_counter++;
    }
    //sub
    else if(instruction.substr(26,6)=="100010"){
        int rs = binaryToDecimal(instruction.substr(6,5));
        int rt = binaryToDecimal(instruction.substr(11,5));
        int rd = binaryToDecimal(instruction.substr(16,5));

        register_values[rd] = register_values[rs]-register_values[rt];
        program_counter++;
    }
    //and
    else if(instruction.substr(26,6)=="100100"){
        int rs = binaryToDecimal(instruction.substr(6,5));
        int rt = binaryToDecimal(instruction.substr(11,5));
        int rd = binaryToDecimal(instruction.substr(16,5));

        register_values[rd] = register_values[rs] & register_values[rt];
        program_counter++;
    }
    //or
    else if(instruction.substr(26,6)=="100101"){
        int rs = binaryToDecimal(instruction.substr(6,5));
        int rt = binaryToDecimal(instruction.substr(11,5));
        int rd = binaryToDecimal(instruction.substr(16,5));

        register_values[rd] = register_values[rs] | register_values[rt];
        program_counter++;
    }
    //slt
    else if(instruction.substr(26,6)=="101010"){
        int rs = binaryToDecimal(instruction.substr(6,5));
        int rt = binaryToDecimal(instruction.substr(11,5));
        int rd = binaryToDecimal(instruction.substr(16,5));

        register_values[rd] = register_values[rs]<register_values[rt] ? 1 : 0;
        program_counter++;
    }
}

void execute_addi(string &instruction,vector<int> &register_values,int &program_counter){
    //addi
    int rs = binaryToDecimal(instruction.substr(6,5));
    int rt = binaryToDecimal(instruction.substr(11,5));
    int constant = binaryToDecimal(instruction.substr(16,16));

    register_values[rt] = register_values[rs]+constant;
    program_counter++;
}

void execute_beq(string &instruction,vector<int> &register_values,int &program_counter){
    //beq
    int rs = binaryToDecimal(instruction.substr(6,5));
    int rt = binaryToDecimal(instruction.substr(11,5));
    int constant = binaryToDecimal(instruction.substr(16,16));

    program_counter = register_values[rs]==register_values[rt] ? constant : program_counter+1;
}

void execute_lw(string &instruction,vector<int> &register_values,int &program_counter,vector<pair<string,int>> &data_memory,string &baseAddress){
    //lw
    int rs = binaryToDecimal(instruction.substr(11,5));
    string constant = instruction.substr(16,16);

    uintptr_t address = getAddressFromHexAndBinary(baseAddress, constant);
    // Cast the address to an int pointer and dereference to get the value
    int* ptr = reinterpret_cast<int*>(address);
    register_values[rs] = *ptr;

    program_counter++;
}

void execute_sw(string &instruction,vector<int> &register_values,int &program_counter,vector<pair<string,int>> &data_memory,string &baseAddress){
    //sw
    int rs = binaryToDecimal(instruction.substr(11,5));
    string constant = instruction.substr(16,16);

    uintptr_t address = getAddressFromHexAndBinary(baseAddress, constant);
    // Cast the address to an int pointer and dereference to get the value
    int* ptr = reinterpret_cast<int*>(address);
    *ptr = register_values[rs];
    
    program_counter++;
}

void execute_j(string &instruction,vector<int> &register_values,int &program_counter){
    //j
    int constant = binaryToDecimal(instruction.substr(6,26));

    program_counter = constant;
}

void simulate(vector<pair<string,int>> &data_memory,string &baseAddress){
    ifstream input_file;
    input_file.open("./machine_code.txt");

    if(!input_file.is_open()){
        cout<<"Unable to open input file."<<endl;
        return;
    }

    vector<int> register_values(32,0);
    vector<string> instructions;
    string line;
    while(getline(input_file,line)){
        instructions.push_back(line);
    }

    int program_counter=0;

    while(program_counter<instructions.size() && program_counter>=0){
        string instruction = instructions[program_counter];
        int temp = program_counter;
        
        //R-type instruction
        if(instruction.substr(0,6)=="000000"){
            execute_R_Type(instruction,register_values,program_counter);
            
        }
        //I-type instruction:addi
        else if(instruction.substr(0,6)=="001000"){
            execute_addi(instruction,register_values,program_counter);
        }
        //I-type instruction:beq
        else if(instruction.substr(0,6)=="000100"){
            execute_beq(instruction,register_values,program_counter);
        }
        else if(instruction.substr(0,6)=="100011"){
            execute_lw(instruction,register_values,program_counter,data_memory,baseAddress);
        }
        else if(instruction.substr(0,6)=="101011"){
            execute_sw(instruction,register_values,program_counter,data_memory,baseAddress);
        }
        //J-type instruction
        else if(instruction.substr(0,6)=="000010"){
            execute_j(instruction,register_values,program_counter);
        }

        cout<<"After executing instruction "<<instruction<<" on line number "<<temp+1<<" values in register are: "<<endl;
        for(int i=0;i<32;i++){
            cout<<mipsRegisters[i]<<": "<<register_values[i]<<"  ";
        }
        cout<<endl<<"Values in data memory are as follows..."<<endl;
        for(int i=0;i<data_memory.size();i++){
            cout<<data_memory[i].first<<": "<<data_memory[i].second<<"  ";
        }
        cout<<endl<<endl;
    }

    
    input_file.close();
}