//
// 
//  hzip
//
//  Created by Alexey Kharlamov on 01.02.16.
//  Copyright © 2016 Alexey Kharlamov. All rights reserved.
//

#include "codec.h"

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


void learn(codec_state &state, bool tester, std::string &buffer){
    std::vector<std::string> dictionary(258, "");    
    std::vector<int> dict(256, 0);
    
    for (unsigned long long i = 0; i < buffer.length(); ++i) {
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

    for (auto elem : destroy){
        delete elem;
    }

    state.dictionary = dictionary;

}

void encode(codec_state &state, bool _uint, bool tester, std::string &buffer, std::string &codeout) {
    unsigned long long buff_out_size = 11000000;
    unsigned long builddict = clock();
    
    codeout.reserve(buffer.length() * 8);
    
    int j = -1;
    std::string buff = "";
    buff.reserve(buffer.length());
    bool bits[8];
    
    for (unsigned long long i = 0; i < buffer.length(); ++i) {
        //codeout << dictionary[int(buffer[i]) + 128];
        // попробуем без упаковки
        //std::cout << dictionary[int(buffer[i]) + 128] << " ";
        for (auto elem : state.dictionary[int(buffer[i]) + 128]) {
            
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
    
}





void decode(codec_state &state, std::string &buffer, bool tester, std::string &out) {
    unsigned long long buff_out_size = 11000000;
    
    std::string mem = "";
    mem.reserve(buffer.length());
    std::string buff = "";
    buff.reserve(buffer.length());
    
    
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
}
void create_dict(codec_state &state) {
    std::vector<decode_huf_ver*> destroy;
    unsigned long long buff_out_size = 11000000;
    unsigned long start = clock();
    
    std::map<std::string, char> dict;
    decode_huf_ver root;
    root.leftson = &root;
    root.rightson = &root;
    root.myself = &root;
    for (auto elem : state.dict_mem) {
        std::string str = elem.second;
        char let = elem.first;
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
        
        
    }
    
    state.destroy = destroy;
    state.root = root;
}