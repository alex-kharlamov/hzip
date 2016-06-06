//
//  main
//  hzip
//
//  Created by Alexey Kharlamov on 01.02.16.
//  Copyright © 2016 Alexey Kharlamov. All rights reserved.
//

//std::vector<std::string> dictionary(258, "");

#include "shared.h"

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


codec_state load_file(std::string& inp_file, bool _uint) {

    std::vector<std::string> dictionary(258, "");
    unsigned long start_time = clock();

    unsigned long long records = 0;

    std::string buffer = "";
    std::ifstream file(inp_file);

    if (_uint){
        while (!file.eof()){
            std::string temp_str;
            uint32_t temp = 0;
            file.read(reinterpret_cast<char*>(&temp), sizeof(temp));
            temp_str.resize(temp);
            file.read(const_cast<char*>(temp_str.data()), temp);

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

    codec_state state;
    state.records = records;
    state.buffer = buffer;

    return state;
}


void save(std::vector<std::string> &dictionary, std::string &inp_file, codec_state &state){
    std::ofstream fout(inp_file + ".dict");
    unsigned long long counter = 0;
    
    for (int i = 0; i < dictionary.size(); ++i) {
        if (dictionary[i] != "") {
            counter++;
        }
    }
    
    
    fout << counter << std::endl;

    for (int i = 0; i < dictionary.size(); ++i) {
        if (dictionary[i] != "") {
            fout << char(i - 128) << " " << dictionary[i] << std::endl;
        }
    }
    
    fout.close();
}

std::vector<std::string> learn(codec_state &state, std::string &inp_file, bool tester){

    std::vector<std::string> dictionary(258, "");    
    std::vector<int> dict(256, 0);
    
    for (unsigned long long i = 0; i < state.buffer.length(); ++i) {
        dict[int(static_cast<unsigned char> (state.buffer[i]))] += 1;
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

    for (auto elem : destroy){
        delete elem;
    }


    save(dictionary, inp_file, state);

    return dictionary;

}

std::string encode(codec_state &state, bool _uint, bool tester, std::vector<std::string> &dictionary, std::string &inp_file) {
    unsigned long long buff_out_size = 11000000;
    unsigned long builddict = clock();
    
    std::string codeout = "";
    codeout.reserve(state.buffer.length() * 8);
    
    int j = -1;
    std::string buff = "";
    buff.reserve(state.buffer.length());
    bool bits[8];
    
    for (unsigned long long i = 0; i < state.buffer.length(); ++i) {
        //codeout << dictionary[int(buffer[i]) + 128];
        // попробуем без упаковки
        //std::cout << dictionary[int(buffer[i]) + 128] << " ";
        for (auto elem : dictionary[int(state.buffer[i]) + 128]) {
            
            j += 1;
            bits[j] = bool(elem - '0');
            
            
            if (j == 7) {
                buff += pack_byte(bits);
                if (buff.length() > buff_out_size) {
                    codeout += buff;
                    buff = "";
                }
                j = -1;
            }
            
            
        }
        
    }
    //delete[] buffer;
    
    
    codeout += buff;
    for (int i = 0; i < j + 1; ++i) {
        codeout += std::to_string(bits[i]);
    }
    codeout += std::to_string(j + 1);
    //std::cout << buff;
    
    

    if (tester) {
        std::cout << "encoding done in " << (clock() - builddict) / (double)CLOCKS_PER_SEC << std::endl;
        std::cout << std::setprecision(15) <<  "encode: records per second : " << state.records / ((clock() - builddict) / (double)CLOCKS_PER_SEC) << std::endl;
    }
                    
                    
    return codeout;
    
}
                    void encode_save(std::string &buffer, std::string &inp_file){
                        std::ofstream codeout(inp_file + ".coded");
                        codeout << buffer;
                        codeout.close();
                    }


dict_state load_dict(std::string &inp_file){
    std::vector<decode_huf_ver*> destroy;
    unsigned long long buff_out_size = 11000000;
    unsigned long start = clock();
    std::ifstream dicin(inp_file + ".dict");
    std::string n;
    std::getline(dicin, n);
    
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
    
    dict_state state;
    state.destroy = destroy;
    state.root = root;
    
    return state;
    //std::cout << "nya" << std::endl;
}


std::string read_encoded(std::string &inp_file){
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
    
    
    return buffer;
    
}




std::string decode(std::string &inp_file, bool tester, dict_state &state, std::string &buffer) {
    unsigned long long buff_out_size = 11000000;
    
    std::string out = "";
    std::string mem = "";
    mem.reserve(buffer.length());
    std::string buff = "";
    buff.reserve(buffer.length());
    //std::cout << buffer[length - 1] << std::endl;
    
    
    
    decode_huf_ver *temp = new decode_huf_ver;
    state.destroy.push_back(temp);
    temp = &state.root;
    unsigned long long j = 0;
    
    std::string dec_to_str_vec [256];
    
    for (int i = 0; i < 256; ++i) {
        std::bitset<8> bits = i;
        
        dec_to_str_vec[i] = bits.to_string();
    }
    
    unsigned long pure_decode = clock();
    
    for (unsigned long long i = 0; i < buffer.length() - buffer[buffer.length() - 1] + '0' - 1; ++i) {
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
                            out += buff;
                            buff = "";
                        }
                        temp = &state.root;
                        
                    }
                } else {
                    temp = temp->leftson;
                    
                    if (temp->used == true) {
                        buff += temp->letter;
                        if (buff.length() > buff_out_size ) {
                            out += buff;
                            buff = "";
                        }
                        temp = &state.root;
                        
                    }
                }
                
                j += 1;
            }
            
            j = 0;
            mem = "";
            
        }
    }
    
    for (unsigned long long i = buffer.length() - buffer[buffer.length() - 1] + '0' - 1; i < buffer.length(); ++i) {
        mem += buffer[i];
        
        
    }
    
    while (j < mem.length()) {
        if (mem[j] == '1') {
            temp = temp->rightson;
            if (temp->used == true) {
                buff += temp->letter;
                if (buff.length() > buff_out_size ) {
                    out += buff;
                    buff = "";
                }
                temp = &state.root;
                
            }
        } else {
            temp = temp->leftson;
            
            if (temp->used == true) {
                buff += temp->letter;
                if (buff.length() > buff_out_size ) {
                    out += buff;
                    buff = "";
                }
                temp = &state.root;
                
            }
        }
        
        j += 1;
    }
    
    unsigned long memorised = clock();
    if (tester){
        std::cout << "decode done in " << (memorised - pure_decode) / (double)CLOCKS_PER_SEC << std::endl;
    }
    /*
    if (tester) {
        std::cout << std::setprecision(10) <<  "decode: records per second " << std::stoi(records) / ((memorised - pure_decode) / (double)CLOCKS_PER_SEC) << std::endl;
    }*/

    unsigned long outbuff = clock();
    //std::cout << "outbufing " << (outbuff - memorised) / (double)CLOCKS_PER_SEC << std::endl;
    
    
    out += buff;
    
    
    //std::cout << "decoding done in  " << (clock() - start) / (double)CLOCKS_PER_SEC << std::endl;
    for (auto elem : state.destroy){
        delete elem;
    }
    
    return out;
}

void decode_save(std::string &inp_file, std::string &buffer){
    std::ofstream out(inp_file + ".output");
    out << buffer;
    out.close();
}


void tester(std::string &inp_file, bool _uint, bool test){

    codec_state state = load_file(inp_file, _uint);

    std::vector<std::string> dictionary = learn(state, inp_file, test);

    std::string buffer = encode(state, _uint, test, dictionary, inp_file);
    
    encode_save(buffer, inp_file);
    
    dict_state state_dict = load_dict(inp_file);
    
    buffer = read_encoded(inp_file);
    
    buffer = decode(inp_file, test, state_dict, buffer);
    
    decode_save(inp_file, buffer);
    

    std::fstream file(inp_file);
    double file_size = 0;
    file.seekg(0, std::ios::end);
    file_size = file.tellg();

    std::fstream file_encoded(inp_file + ".coded");
    double file_encoded_size = 0;
    file_encoded.seekg (0, std::ios::end);
    file_encoded_size = file_encoded.tellg();
    file_encoded.close();

    std::fstream file_dict(inp_file + ".dict");
    double file_dict_size = 0;
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
