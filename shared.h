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

struct codec_state
{
    std::string buffer;
    unsigned long long records;
};

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

struct dict_state{
    std::vector<decode_huf_ver*> destroy;
    decode_huf_ver root;
};

void decode_save(std::string &, std::string &);

void save(std::vector<std::string> &, std::string &, codec_state &);

void build_dict(ver const &, std::string, std::vector<std::string> &);

std::string dec_to_bin(int);

char pack_byte(bool bits[7]);

codec_state load_file(std::string& , bool );

void decode_save(std::string &, std::string &);

std::vector<std::string> learn(codec_state &, std::string &, bool );

void encode_save(std::string &, std::string &);

void tester(std::string &, bool, bool);

dict_state load_dict(std::string &);

std::string encode(codec_state &, bool, bool, std::vector<std::string> &, std::string &);

std::string read_encoded(std::string &);

std::string decode(std::string &, bool, dict_state &, std::string &);
