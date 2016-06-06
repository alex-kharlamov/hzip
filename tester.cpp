#include "tester.h"


std::string load_file(std::string &inp_file, bool _uint, codec_state &state) {

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

    state.records = records;

    return buffer;
}

void load_dict(std::string &inp_file, codec_state &state){
    std::ifstream dicin(inp_file + ".dict");
    std::string n;
    std::getline(dicin, n);

    std::vector<std::pair<char,std::string>> dict_mem;

    for (int i = 0; i < std::stoi(n); ++i) {
        std::string str;
        char let;
        let = dicin.get();
        std::getline(dicin, str);
        dict_mem.push_back(std::make_pair(let, str));
    }
    dicin.close();
    state.dict_mem = dict_mem;
}

void encode_save(std::string &buffer, std::string &inp_file){
    std::ofstream codeout(inp_file + ".coded");
    codeout << buffer;
    codeout.close();
    buffer = "";
}

void save_dict (std::string &inp_file, codec_state &state){
    std::ofstream fout(inp_file + ".dict");
    unsigned long long counter = 0;
    
    for (int i = 0; i < state.dictionary.size(); ++i) {
        if (state.dictionary[i] != "") {
            counter++;
        }
    }
    
    
    fout << counter << std::endl;

    for (int i = 0; i < state.dictionary.size(); ++i) {
        if (state.dictionary[i] != "") {
            fout << char(i - 128) << " " << state.dictionary[i] << std::endl;
        }
    }
    
    fout.close();
}

std::string read_encoded(std::string &inp_file){
    std::string temp_coded = ".coded";
    std::string inp_file_coded = inp_file + temp_coded;
    const char *fileName(inp_file_coded.c_str());
    std::ifstream file(fileName, std::ios::binary);
    std::string buffer;
    file.seekg(0, std::ios_base::end);
    std::ifstream::pos_type len = file.tellg();
    file.seekg(0);
    buffer.resize(len);
    file.read((char*)buffer.data(), len);
    return buffer; 
}

void decode_save(std::string &inp_file, std::string &buffer){
    std::ofstream output(inp_file + ".output");
    output << buffer;
    output.close();
}


void tester(std::string &inp_file, bool _uint, bool test) {

    //encode start

    codec_state state;

    std::string in = load_file(inp_file, _uint, state);

    learn(state, test, in);

    save_dict(inp_file, state);

    std::string out;

    encode(state, _uint, test, in, out);
    
    encode_save(out, inp_file);
    out.shrink_to_fit();

    //encode end

    

    //decode start
    std::string buffer, decode_out;
    codec_state decode_state;

    load_dict(inp_file, decode_state);

    create_dict(decode_state);
    
    buffer = read_encoded(inp_file);
    
    decode(decode_state, buffer, test, decode_out);
    
    decode_save(inp_file, decode_out);

    //decode end


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