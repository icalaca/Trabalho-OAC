#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <regex>
#define BASE 0x00400000

using namespace std;

ifstream file;
vector<string> asmCode;

// Declaracao de metodos
// ---------------------
int readFile(char *fname);
int labelCount = 0;
vector<pair<int, string>> labels;
int binToDec(string bin);
string decToBin(int dec);
string getOpCode(string op);
string getFuncCode(string op);
string getRegCode(string reg);
string extend(string s,char c);
string binToHex(string bin);
std::vector<std::string> split(std::string &s, std::string rgx_str);
string Itype(string operation);
// ---------------------

int readFile(char *fname) {
    string line;
    file.open(fname);
    if (!file.is_open()) {
        cout << "Error!" << endl;
        return 1;
    }
    while (getline(file, line)){
      if(line != "")
        asmCode.push_back(line);
    }
    return 0;
}

void putLabels(){
  if(!labels.empty()){
    for(int i = 0;i < labels.size();i++)
      asmCode[labels[i].first].insert(0,labels[i].second);
  }
}

void addLabel(int line, string label){
  labels.push_back(make_pair(line,label+":"));
  labelCount++;
}

string getOpCode(string op) {
    if (op == "xori")  return "001110";
    if (op == "andi")  return "001100";
    if (op == "lui")   return "001111";
    if (op == "beq")   return "000100";
    if (op == "j")     return "000010";
    if (op == "bne")   return "000101";
    if (op == "lw")    return "100011";
    if (op == "addiu") return "001001";
    if (op == "sltiu") return "001011";
    if (op == "sw")    return "101011";
    return "000000";
}

string getFuncCode(string op) {
    if (op == "subu") return "100011";
    if (op == "or")   return "100101";
    if (op == "slt")  return "101010";
    if (op == "addu") return "100001";
    if (op == "sll")  return "000000";
    if (op == "srl")  return "000010";
}

string getRegCode(string reg) {
    if (reg == "$zero") return "00000";
    if (reg == "$v0") return "00010";
    if (reg == "$v1") return "00011";
    if (reg == "$a0") return "00100";
    if (reg == "$a1") return "00101";
    if (reg == "$a2") return "00110";
    if (reg == "$a3") return "00111";
    if (reg == "$t0") return "01000";
    if (reg == "$t1") return "01001";
    if (reg == "$t2") return "01010";
    if (reg == "$t3") return "01011";
    if (reg == "$t4") return "01100";
    if (reg == "$t5") return "01101";
    if (reg == "$t6") return "01110";
    if (reg == "$t7") return "01111";
    if (reg == "$t8") return "11000";
    if (reg == "$t9") return "11001";
    if (reg == "$s0") return "10000";
    if (reg == "$s1") return "10001";
    if (reg == "$s2") return "10010";
    if (reg == "$s3") return "10011";
    if (reg == "$s4") return "10100";
    if (reg == "$s5") return "10101";
    if (reg == "$s6") return "10110";
    if (reg == "$s7") return "10111";
    if (reg == "$gp") return "11100";
    if (reg == "$sp") return "11101";
    if (reg == "$fp") return "11110";
    if (reg == "$ra") return "11111";
}

string getOpCodeSullivan(string op) {
    if (op == "001110") return "xori";
    if (op == "001100") return "andi";
    if (op == "001111") return "lui";
    if (op == "000100") return "beq";
    if (op == "000010") return "j";
    if (op == "000101") return "bne";
    if (op == "100011") return "lw";
    if (op == "001001") return "addiu";
    if (op == "001011") return "sltiu";
    if (op == "101011") return "sw";
    if (op == "000000") return "";
}

string getFuncCodeSullivan(string op) {
    if (op == "100011") return "subu";
    if (op == "100101") return "or";
    if (op == "101010") return "slt";
    if (op == "100001") return "addu";
    if (op == "000000") return "sll";
    if (op == "000010") return "srl";
}

string getRegCodeSullivan(string reg) {
    if (reg == "00000") return "$zero";
    if (reg == "00010") return "$v0";
    if (reg == "00011") return "$v1";
    if (reg == "00100") return "$a0";
    if (reg == "00101") return "$a1";
    if (reg == "00110") return "$a2";
    if (reg == "00111") return "$a3";
    if (reg == "01000") return "$t0";
    if (reg == "01001") return "$t1";
    if (reg == "01010") return "$t2";
    if (reg == "01011") return "$t3";
    if (reg == "01100") return "$t4";
    if (reg == "01101") return "$t5";
    if (reg == "01110") return "$t6";
    if (reg == "01111") return "$t7";
    if (reg == "11000") return "$t8";
    if (reg == "11001") return "$t9";
    if (reg == "10000") return "$s0";
    if (reg == "10001") return "$s1";
    if (reg == "10010") return "$s2";
    if (reg == "10011") return "$s3";
    if (reg == "10100") return "$s4";
    if (reg == "10101") return "$s5";
    if (reg == "10110") return "$s6";
    if (reg == "10111") return "$s7";
    if (reg == "11100") return "$gp";
    if (reg == "11101") return "$sp";
    if (reg == "11110") return "$fp";
    if (reg == "11111") return "$ra";
}

int funcType(string op){
    if (op == "000000") return 1; // 1 = R
    if (op == "001110") return 2; // 2 = I
    if (op == "001100") return 2; // 2 = I
    if (op == "001111") return 2; // 2 = I
    if (op == "000100") return 2; // 2 = I
    if (op == "000010") return 3; // 3 = J
    if (op == "000101") return 2; // 2 = I
    if (op == "100011") return 2; // 2 = I
    if (op == "001001") return 2; // 2 = I
    if (op == "001011") return 2; // 2 = I
    if (op == "101011") return 2; // 2 = I
}



string sullivanI(string bin, int line){
    string op = getOpCodeSullivan(bin.substr(0,6));
    string rs = getRegCodeSullivan(bin.substr(6,5));
    string rt = getRegCodeSullivan(bin.substr(11,5));
    string imm = std::to_string(binToDec(bin.substr(16,16)));
      if((op == "sw") || (op == "lw")){
          return (op + " " + rt + ", " + imm + "(" + rs + ")");
      }
      if(op == "lui"){
          return (op + " " + rt + ", " + imm);
      }
      if((op == "bne")||(op == "beq")){
          string label = "LABEL_"+labelCount;
          int l = line + binToDec(imm);
          addLabel(l,label);
          return (op + " " + rt + ", " + rs + ", "+label);
      }
    return (op + " " + rt + ", " + rs + ", " + imm);
}

string sullivanJ(string bin){
  string op = getOpCodeSullivan(bin.substr(0,6));
  string label = "LABEL_"+labelCount;
  int addr = binToDec(bin.substr(6,26));
  int line = (addr-BASE)/4;
  addLabel(line,label);
  return (op+" "+label);
}

string sullivanR(string bin){
  string op = getOpCodeSullivan(bin.substr(0,6));
  string rs,rt,rd,shift,func;
  rs = getRegCodeSullivan(bin.substr(6,5));
  rt = getRegCodeSullivan(bin.substr(11,5));
  rd = getRegCodeSullivan(bin.substr(16,5));
  if(op.empty()){
    func = getFuncCodeSullivan(bin.substr(26,6));
    if((func == "sll") || (func == "srl")){   //opcode == 000000
      shift = to_string(binToDec(bin.substr(21,5)));
      return (func+" "+rd+", "+rt+", "+shift);
    }
    return (func+" "+rd+", "+rs+", "+", "+rt);
  }
  return (op+" "+rd+", "+rs+", "+", "+rt);
}

void sullivan(string bin, int line){
    string aux = bin.substr(0,6);
    string resp;
    if(funcType(aux) == 1){
        resp = sullivanR(bin);
    }
    if(funcType(aux) == 2){
        resp = sullivanI(bin, line);
    }
    if(funcType(aux) == 3){
        resp = sullivanJ(bin);
    }
}

string extend(string s,char c,int ext){
    int aux = ext - s.length();
    s.insert(0,aux,c);
    return s;
}

vector<string> split(string &s, string rgx_str) {
    vector<string> v;
    regex rgx(rgx_str);
    sregex_token_iterator end;
    sregex_token_iterator it(s.begin(), s.end(), rgx, -1);
    while (it != end) {
        if((*it).compare(""))
            v.push_back(*it);
        it++;
    }
    return v;
}

int binToDec(string bin) {
    bool baux = false;
    int result = 0;
    if(bin[0] == 1){
      for(int i = bin.size()-1;i >= 0;i--){
          if(baux == true)
              bin[i] = ((bin[i] == '0') ? '1' : '0');
          else if(bin[i] == '1')
              baux = true;
      }
    }
    for (int i = 0; i < bin.size(); i++) {
      if (bin[i] == 0x31)
        result += pow(2, bin.size() - i - 1);
    }
    return (baux) ? result*-1 : result;
}

string decToBin(int dec,int ext) {
    bool baux = false;
    string aux;
    string res;
    int rem;
    int decn = dec;
    if(decn < 0) decn *= -1;
    while (decn > 0) {
        rem = decn % 2;
        decn = decn / 2;
        aux += 0x30 + rem;
    }
    if(dec < 0){
        for(int i = 0;i < aux.size();i++){
            if(baux == true)
                aux[i] = ((aux[i] == '0') ? '1' : '0');
            else if(aux[i] == '1')
                baux = true;
        }
        res = extend(string(aux.rbegin(), aux.rend()), '1',ext);
    }else{
        res = extend(string(aux.rbegin(), aux.rend()), '0',ext);
    }
    return res;
}

int countOpLabel(string label){// conta operacoes ate uma label
  int acm = 0;
  for(int i = 0;i < asmCode.size();i++){
    if(asmCode[i].find(label+":") != string::npos)
      return acm;
    if(asmCode[i].find(":") == string::npos){
      acm++;
    }
  }
  return -1;
}

int countOpLine(int line){// conta operacoes ate uma linha
  int acm = 0;
  for(int i = 0;i <= line;i++){
    if(asmCode[i].find(":") == string::npos){
      acm++;
    }
  }
  return acm;
}

string binToHex(string bin){
    string resp;
    for(int i = 0;i <= bin.length()-4;i+=4){
        string aux(bin,i,4);
        int vrau = binToDec(aux);
        if(vrau > 9){
            vrau += 0x37;
        }else{
            vrau += 0x30;
        }
        resp += vrau;
    }
    return resp;
}

string hexToBin(string hex){
    string resp;
    for(int i = 0; i < hex.length(); i++){
        if(!isalpha(hex[i])){
            resp += decToBin(hex[i]-0x30,4);
        }else{
            resp += decToBin(hex[i] - 0x37,4);
        }
    }
    return resp;
}

string Itype(string operation, int line) {
    vector<string> v = split(operation, "(\\s|,)+"); //     \\s e espaco em branco       | e um or (ou)       , e uma virgula
    string opcode = getOpCode(v[0]);
    string rt = getRegCode(v[1]);
    string rs;
    string imm;
      if((v[0] == "sw") || (v[0] == "lw")){
        vector<string> aux = split(v[2],"(\\(|\\))+");
        rs = getRegCode(aux[1]);
        imm = decToBin(atoi(aux[0].c_str()),16);
      }else if(v[0] == "lui"){
        rs = decToBin(0,5);
        imm = decToBin(atoi(v[2].c_str()),16);
      }else if((v[0] == "bne")||(v[0] == "beq")){
        rs = getRegCode(v[1]);
        rt = getRegCode(v[2]);
        string label = v[3];
        imm = decToBin(countOpLabel(label) - countOpLine(line),16);
      }else{
        rs = getRegCode(v[2]);
        imm = decToBin(atoi(v[3].c_str()),16);
      }
      return (opcode+rs+rt+imm);
}

string Jtype(string operation,int line){
  vector<string> v = split(operation,"(\\s)+");
  string opcode = getOpCode(v[0]);
  string label = v[1];
  int addr = BASE + 4*countOpLabel(label);
  return (opcode+decToBin(addr,28)); //o endereco de j e shiftado << indo de 26 pra 28 bits
}

string Rtype(string operation, int line){
    vector<string> v = split(operation,"(\\s|,)+");
    string opcode = getOpCode(v[0]);
    string rs;
    string rd;
    string rt;
    string shift;
    string func;
    if((v[0] == "sll") || (v[0] == "srl")){
        rs = decToBin(0,5);
        rt = getRegCode(v[2]);
        rd = getRegCode(v[1]);
        shift = decToBin(atoi(v[3].c_str()),5);
        func = getFuncCode(v[0]);
    }else{
      rs = getRegCode(v[2]);
      rt = getRegCode(v[3]);
      rd = getRegCode(v[1]);
      shift = decToBin(0,5);
      if(getOpCode(v[0]) == "000000")
        func = getFuncCode(v[0]);
      else
        func = decToBin(0,6);
    }
    return (opcode + rs + rt + rd + shift + func);
}




int main(int argc, char **argv) {
  asmCode.push_back("main:");
  asmCode.push_back("op1");
  asmCode.push_back("op2");
  asmCode.push_back("subu $t0, $t1, $t2");
  asmCode.push_back("op4");
  asmCode.push_back("label1:");
  asmCode.push_back("op5");
  asmCode.push_back("op6");
  asmCode.push_back("op7");
  asmCode.push_back("op8");
  asmCode.push_back("op9");
  asmCode.push_back("op10");
  asmCode.push_back("op11");  //10 -
  asmCode.push_back("label2:");
  asmCode.push_back("op12");//-9
  asmCode.push_back("op13");
  asmCode.push_back("op14");
    //string s = "beq $t2,$t3,label2";
    string s = "sll $t0, $t1, 4";
    //binToHex(Itype(s,3));
    //Rtype(s,3);
    //cout << binToHex(Rtype(s,3)) << endl;
    cout << sullivanR(hexToBin("00094102")) << endl;
    //cout << binToDec("100000100011") << endl;
    return 0;
}
