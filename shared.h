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

void build_dict(ver const &, std::string, std::vector<std::string> &);

std::string dec_to_bin(int);

char pack_byte(bool bits[7]);

void load_file(std::string &, std::istream &);

void encode_tester(std::string &);

void encode(std::string &, bool, bool);


void decode(std::string &, bool);