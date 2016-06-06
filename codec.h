//
//  main
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

struct decode_huf_ver {
    char letter;
    decode_huf_ver *leftson, *rightson;
    decode_huf_ver* myself;
    bool used = false;
};

struct codec_state
{
    unsigned long long records;
    std::vector<decode_huf_ver*> destroy;
    decode_huf_ver root;
    std::vector<std::string> dictionary;
    std::vector<std::pair<char,std::string>> dict_mem;

};



void build_dict(ver const &, std::string, std::vector<std::string> &);

std::string dec_to_bin(int);

char pack_byte(bool bits[7]);





void learn(codec_state &, bool, std::string &);

void create_dict(codec_state &);


void encode(codec_state &, bool, bool, std::string &, std::string &);

void decode(codec_state &, std::string &, bool, std::string &);
