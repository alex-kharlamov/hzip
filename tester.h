#include "codec.h"

std::string load_file(std::string &, bool , codec_state &);

void load_dict(std::string &, codec_state &);

void encode_save(std::string &, std::string &);
void save_dict (std::string &, codec_state &);

std::string read_encoded(std::string &);

void decode_save(std::string &, std::string &);

void tester(std::string &, bool, bool );