//
//  main.cpp
//  hzip
//
//  Created by Alexey Kharlamov on 01.02.16.
//  Copyright © 2016 Alexey Kharlamov. All rights reserved.
//

#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <algorithm>
#include <bitset>
#include <iomanip>
#if 0
#include <experimental/string_view>
#endif


//std::vector<std::string> dictionary(258, "");

struct ver {
    char letter;
    const ver *leftson, *rightson;
    unsigned long long counter;
    bool operator() (ver const &a, ver const &b) {
        return a.counter > b.counter;
    }
    ver* myself;
    bool letterused = false;
};

void build_dict(ver const &v, std::string str, std::vector<std::string> &dictionary) {
    if (v.letterused) {
        dictionary[int(v.letter) + 128] = str;
    }
    if (v.leftson != nullptr) {
        build_dict(*v.leftson, str + "0", dictionary);
    }
    if (v.rightson != nullptr) {
        build_dict(*v.rightson, str + "1", dictionary);
    }
    
}

std::string dec_to_bin(int dec) {
    int i;
    int mod;
    long double_ = 0;
    
    for (i = 0; dec>0; i++) {
        
        mod = dec % 2;
        dec = (dec - mod) / 2;
        double_ += mod * pow((double)10, i);
    }
    
    std::string res = std::to_string(double_);
    
    if (res.length() < 7) {
        std::string temp = "";
        for (int i = res.length(); i < 7; ++i) {
            temp += "0";
        }
        res = temp + res;
    }
    return res;
    
}


char pack_byte(bool bits[7]) {
#if 0
    unsigned char result(0);
    for (unsigned i(8); i--;) {
        result <<= 1;
        result |= unsigned char(bits[i]);
    }
    return result;
#endif
    short pow[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    auto res = 0;
    for (int i = 0; i < 8; ++i) {
        res += int(bits[i]) * pow[i];
    }
    return res;
    
}

void load_file(std::string& s, std::istream& is) {
    s.erase();
    if (is.bad()) return;
    //
    // attempt to grow string buffer to match file size,
    // this doesn't always work...
    s.reserve(is.rdbuf()->in_avail());
    char c;
    while (is.get(c)) {
        // use logarithmic growth stategy, in case
        // in_avail (above) returned zero:
        if (s.capacity() == s.size())
            s.reserve(s.capacity() * 3);
        s.append(1, c);
    }
}

void encode_tester(std::string &inp_file){

	std::fstream file(inp_file);
    unsigned long long file_size = 0;
    file.seekg(0, std::ios::end);
    file_size = file.tellg();

    std::fstream file_encoded(inp_file + ".coded");
    unsigned long long file_encoded_size = 0;
    file_encoded.seekg (0, std::ios::end);
    file_encoded_size = file_encoded.tellg();
    file_encoded.close();

    std::fstream file_dict(inp_file + ".dict");
    unsigned long long file_dict_size = 0;
    file_dict.seekg (0, std::ios::end);
    file_dict_size = file_dict.tellg();
    file_dict.close();

    std::cout << "Size of file " << std::setprecision(6) << double(file_size) / 1024 / 1024 << " MiB" << std::endl;
    std::cout << "Size of archived file " << double(file_encoded_size) / 1024 / 1024 << " MiB" << std::endl;
    double ratio = (1 - (double((file_encoded_size + file_dict_size))/double(file_size))) * 100;
    std::cout << "Compression ratio is " <<  ratio << "%" << std::endl;
    

    std::fstream file_output(inp_file);
    unsigned long long file_output_size = 0;
    file_output.seekg(0, std::ios::end);
    file_output_size = file_output.tellg();


    if (file_output_size != file_size){
    	std::cout << "size of output doesn't correct!" << std::endl;
    } else {
    	unsigned long long counter = 0;

    	while (!file.eof())
	    {
	    	std::string inp, outp;
	        std::getline(file, inp);
	        std::getline(file_output, outp);

	        if (inp != outp){
	        	counter += 1;
	        }
	    }

	    std::cout<< "we have " << counter << "  mistakes" << std::endl;
	    if (counter == 0){
	    	std::cout << "All done!" << std::endl;
	    }
    }
    file.close();
    file_output.close();

}

void encode(std::string &inp_file, bool _uint, bool tester) {
    unsigned long long buff_out_size = 11000000;
    std::vector<std::string> dictionary(258, "");
    unsigned long start_time = clock();

    unsigned long long records = 0;

    std::string buffer = "";
    std::ifstream file(inp_file);

    if (_uint){
    	/*
    	ssize_t get(std::string& str) {
    		if (!_file.good()) {
      			return -1;
    		}
    		if (!_read_blocks) {
      			std::getline(_file, str);
    		} else {
      			uint32_t sz = 0;
      			_file.read(reinterpret_cast<char*>(&sz), sizeof(sz));
      			str.resize(sz);
      			_file.read(const_cast<char*>(str.data()), sz);
    		}
    		return str.size();
  		}
		*/
  		
  		while (!file.eof()){
	  		std::string temp_str;
	  		uint32_t sz = 0;
	      	file.read(reinterpret_cast<char*>(&sz), sizeof(sz));
	      	temp_str.resize(sz);
	      	file.read(const_cast<char*>(temp_str.data()), sz);

	      	buffer += temp_str + "\n";

	      	//std::cout << temp_str << std::endl;

	      	records += 1;

	    }

	    buffer.erase(buffer.length() - 1, 1);

    } else {
    	    
	    // get length of file:
	    file.seekg(0, file.end);
	    unsigned long long file_length = file.tellg();
	    file.seekg(0, file.beg);
	    /*
	    char * buffer = new char[length];
	    
	    // read data as a block:
	    file.read(buffer, length);
	     
	    */
	    
	    buffer.reserve(file_length);
	    
	    std::string s;
	    
	    while (!file.eof())
	    {
	        std::getline(file, s);
	        buffer += s + "\n";
	        records += 1;
	    }
	    buffer.erase(buffer.length() - 1, 1);
    }

    //file.close();


    //std::cout << buffer << std::endl;
    
    unsigned long long length = buffer.length();
        
    std::vector<int> dict(256, 0);
    
    for (unsigned long long i = 0; i < length; ++i) {
        dict[int(static_cast<unsigned char> (buffer[i]))] += 1;
    }
    
    std::priority_queue<ver, std::vector<ver>, ver> qu;
    
    std::vector<std::pair<int, char>> chardict;
    for (int i = 0; i < 256; ++i) {
        if (dict[i] > 0) {
            chardict.push_back(std::make_pair(dict[i], char(i)));
        }
    }
    
    std::sort(chardict.begin(), chardict.end());
    std::vector<ver*> destroy;
    
    for (int i = 0; i < chardict.size(); ++i) { //vertex init
        ver* temp = new ver;
        temp->myself = temp;
        temp->counter = chardict[i].first;
        temp->letter = chardict[i].second;
        temp->leftson = nullptr;
        temp->rightson = nullptr;
        temp->letterused = true;
        qu.push(*temp);
        destroy.push_back(temp);
        
    }
    
    unsigned long init = clock();
    
    while (qu.size() > 1) { //build tree
        const ver* left;
        const ver* right;
        left = qu.top().myself;
        qu.pop();
        right = qu.top().myself;
        qu.pop();
        ver* newver = new ver;
        newver->myself = newver;
        newver->leftson = left;
        newver->rightson = right;
        newver->counter = left->counter + right->counter;
        qu.push(*newver);
        destroy.push_back(newver);
    }
    
    
    build_dict(qu.top(), "", dictionary); //making dict
    
    unsigned long dic = clock();
    if (tester){
    	std::cout << "build model of dict " << (dic - init) / (double)CLOCKS_PER_SEC << std::endl;
    }
    std::ofstream fout(inp_file + ".dict");
    unsigned long long counter = 0;
    
    for (int i = 0; i < dictionary.size(); ++i) {
        if (dictionary[i] != "") {
            counter++;
        }
    }
    
    
    fout << counter << std::endl; //в 1 строке каждого словаря хранится кол-во слов в нем КОСТЫЛИ!!! ИСПРАВИТЬ!!!
    fout << records << std::endl;

    for (int i = 0; i < dictionary.size(); ++i) {
        if (dictionary[i] != "") {
            fout << char(i - 128) << " " << dictionary[i] << std::endl;
        }
    }
    
    fout.close();
    
    unsigned long builddict = clock();
    
    std::ofstream codeout(inp_file + ".coded");
    
    int j = -1;
    std::string buff = "";
    buff.reserve(length);
    bool bits[8];
    
    for (unsigned long long i = 0; i < length; ++i) {
        //codeout << dictionary[int(buffer[i]) + 128];
        // попробуем без упаковки
        //std::cout << dictionary[int(buffer[i]) + 128] << " ";
        for (auto elem : dictionary[int(buffer[i]) + 128]) {
            
            j += 1;
            bits[j] = bool(elem - '0');
            
            
            if (j == 7) {
                buff += pack_byte(bits);
                if (buff.length() > buff_out_size) {
                    codeout << buff;
                    buff = "";
                }
                j = -1;
            }
            
            
        }
        
    }
    //delete[] buffer;
    
    
    codeout << buff;
    for (int i = 0; i < j + 1; ++i) {
        codeout << bits[i];
    }
    codeout << j + 1;
    //std::cout << buff;
    
    codeout.close();

    if (tester) {
		std::cout << "encoding done in " << (clock() - builddict) / (double)CLOCKS_PER_SEC << std::endl;
		std::cout << std::setprecision(15) <<  "encode: records per second : " << records / ((clock() - builddict) / (double)CLOCKS_PER_SEC) << std::endl;
	}

    for (auto elem : destroy){
    	delete elem;
    }
    if (tester){
    	encode_tester(inp_file);
	}
    
	
}

struct decode_huf_ver {
    char letter;
    decode_huf_ver *leftson, *rightson;
    decode_huf_ver* myself;
    bool used = false;
};



void decode(std::string &inp_file, bool tester) {
	std::vector<decode_huf_ver*> destroy;
    unsigned long long buff_out_size = 11000000;
    unsigned long start = clock();
    std::ifstream dicin(inp_file + ".dict");
    std::string n;
    std::getline(dicin, n);
    std::string records;
    std::getline(dicin, records);
    //std::cout << std::stoi(n) << std::endl;
    std::map<std::string, char> dict;
    decode_huf_ver root;
    root.leftson = &root;
    root.rightson = &root;
    root.myself = &root;
    for (int i = 0; i < std::stoi(n); ++i) {
        std::string str;
        char let;
        let = dicin.get();
        std::getline(dicin, str);
        //std::cout << let << " " << str << std::endl;
        //let = str[0];
        //str.erase(0, 2);
        dict[str] = let;
        decode_huf_ver *temp = new decode_huf_ver;
        destroy.push_back(temp);
        temp = &root;
        for (unsigned long long j = 0; j < str.length(); ++j) {
            if (str[j] == '1') {
                
                if (temp->rightson != &root) {
                    temp = temp->rightson;
                    if (j == str.length() - 1) {
                        temp->letter = let;
                        temp->used = true;
                        break;
                    }
                } else {
                    decode_huf_ver *temp_temp = new decode_huf_ver;
                    destroy.push_back(temp_temp);
                    temp_temp->leftson = &root;
                    temp_temp->rightson = &root;
                    temp->rightson = temp_temp;
                    temp = temp->rightson;
                    if (j == str.length() - 1) {
                        temp->letter = let;
                        temp->used = true;
                        break;
                    }
                    
                }
            }
            if (str[j] == '0') {
                
                
                if (temp->leftson != &root) {
                    temp = temp->leftson;
                    if (j == str.length() - 1) {
                        temp->letter = let;
                        temp->used = true;
                        break;
                    }
                } else {
                    decode_huf_ver *temp_temp = new decode_huf_ver;
                    destroy.push_back(temp_temp);
                    temp_temp->leftson = &root;
                    temp_temp->rightson = &root;
                    temp->leftson = temp_temp;
                    temp = temp->leftson;
                    if (j == str.length() - 1) {
                        temp->letter = let;
                        temp->used = true;
                        break;
                    }
                    
                }
                
            }
        }

        
        //std::cout << let << " " << str << " " <<  str.length() <<  std::endl;
    }
    
    
    dicin.close();
    //std::cout << "nya" << std::endl;
    std::string temp_coded = ".coded";
    std::string inp_file_coded = inp_file + temp_coded;
    const char *fileName(inp_file_coded.c_str());
    std::ifstream file(fileName, std::ios::binary);
    //std::ifstream file("coded", std::ios::binary);
    std::string buffer;
    
    file.seekg(0, std::ios_base::end);
    std::ifstream::pos_type len = file.tellg();
    file.seekg(0);
    buffer.resize(len);
    file.read((char*)buffer.data(), len);
    unsigned long long length = len;
    
    unsigned long dict_coded = clock();
    //std::cout << "reading dict with coded file " << (dict_coded - start) / (double)CLOCKS_PER_SEC << std::endl;
    
    std::ofstream out(inp_file + ".output");
    std::string mem = "";
    mem.reserve(length);
    std::string buff = "";
    buff.reserve(length);
    //std::cout << buffer[length - 1] << std::endl;
    
    decode_huf_ver *temp = new decode_huf_ver;
    destroy.push_back(temp);
    temp = &root;
    unsigned long long j = 0;
    
    std::string dec_to_str_vec [256];
    
    for (int i = 0; i < 256; ++i) {
        std::bitset<8> bits = i;
        
        dec_to_str_vec[i] = bits.to_string();
    }
    
    unsigned long pure_decode = clock();

    for (unsigned long long i = 0; i < length - buffer[length - 1] + '0' - 1; ++i) {
        //std::cout << buffer[i] << " " << int(buffer[i]) << std::endl;
        //mem += dec_to_bin(int(buffer[i]));
        //std::bitset<7> bits = int(buffer[i]);
        
        //mem += bits.to_string<char, std::char_traits<char>, std::allocator<char> >();
        mem += dec_to_str_vec[(256 + buffer[i]) % 256];
        
        if (mem.length() > buff_out_size * 10) {
            while (j < mem.length()) {
                if (mem[j] == '1') {
                    temp = temp->rightson;
                    
                    if (temp->used == true) {
                        buff += temp->letter;
                        if (buff.length() > buff_out_size ) {
                            out << buff;
                            buff = "";
                        }
                        temp = &root;
                        
                    }
                } else {
                    temp = temp->leftson;
                    
                    if (temp->used == true) {
                        buff += temp->letter;
                        if (buff.length() > buff_out_size ) {
                            out << buff;
                            buff = "";
                        }
                        temp = &root;
                        
                    }
                }
                
                j += 1;
            }
        
        j = 0;
        mem = "";
        
        }
    }
    
    for (unsigned long long i = length - buffer[length - 1] + '0' - 1; i < length; ++i) {
        mem += buffer[i];
        
        
    }
    unsigned long memorised = clock();
    if (tester){
    	std::cout << "decode done in " << (memorised - pure_decode) / (double)CLOCKS_PER_SEC << std::endl;
	}
    if (tester) {
    	std::cout << std::setprecision(10) <<  "decode: records per second " << std::stoi(records) / ((memorised - pure_decode) / (double)CLOCKS_PER_SEC) << std::endl;
    }

    while (j < mem.length()) {
            if (mem[j] == '1') {
                    temp = temp->rightson;
                
                    if (temp->used == true) {
                        buff += temp->letter;
                        if (buff.length() > buff_out_size ) {
                            out << buff;
                            buff = "";
                        }
                        temp = &root;
                   
                }
            } else {
                    temp = temp->leftson;
                
                    if (temp->used == true) {
                        buff += temp->letter;
                        if (buff.length() > buff_out_size ) {
                            out << buff;
                            buff = "";
                        }
                        temp = &root;
                    
                }
            }
        
        j += 1;
    }
    
    unsigned long outbuff = clock();
    //std::cout << "outbufing " << (outbuff - memorised) / (double)CLOCKS_PER_SEC << std::endl;
    
    
    out << buff;
    
    out.close();
    //std::cout << "decoding done in  " << (clock() - start) / (double)CLOCKS_PER_SEC << std::endl;
    for (auto elem : destroy){
    	delete elem;
    }
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout << "Please add file name" << std::endl;
	}

	if (argc == 2) {
		std::string second_arg;
		second_arg = (const char*) argv[1];
		if (second_arg == "--help") {
			std::cout << "First parameter is name of the file to encode" << std::endl;
			std::cout << "Second parameter is type of input file:" << std::endl;
			std::cout << "-lines for \\n files" << std::endl;
			std::cout << "-uint for LE uint32_t files" << std::endl;
			std::cout << "If you will add only name of the file, by default will be used -lines" << std::endl;
			std::cout << "You can add -tester to show debug info" << std::endl;
		} else {
			bool _uint = false;
			bool tester = false;
			encode(second_arg, _uint, tester);
	    	decode(second_arg, tester);
    	}


	}  

	if (argc == 3) {
		std::string second_arg, third_arg;
		second_arg = (const char*) argv[1];
		third_arg = (const char*) argv[2];
		if (third_arg == "-lines"){
			bool _uint = false;
			bool tester = false;
			encode(second_arg, _uint, tester);
			decode(second_arg,tester);
		}
		if (third_arg == "-uint")
		{
			bool _uint = true;
			bool tester = false;
			encode(second_arg, _uint,tester);
			decode(second_arg, tester);
		}
		if (third_arg == "-tester"){
			bool tester = true;
			bool _uint = false;
			encode(second_arg, _uint, tester);
			decode(second_arg, tester);
		}
    }
    if (argc == 4){
    	std::string second_arg;
		second_arg = (const char*) argv[1];
		if (argv[2] == "-lines"){
			bool _uint = false;
			bool tester = true;
			encode(second_arg, _uint, tester);
			decode(second_arg, tester);
		}
		if (argv[2] == "-uint")
		{
			bool _uint = true;
			bool tester = true;
			encode(second_arg,_uint, tester);
			decode(second_arg, tester);
		}
    }
}
