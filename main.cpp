#include "shared.h"

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
            encode(second_arg, _uint, tester);
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
        std::string second_arg, third_arg;
        third_arg = (const char*) argv[2];
        second_arg = (const char*) argv[1];
        if (third_arg == "-lines"){
            bool _uint = false;
            bool tester = true;
            encode(second_arg, _uint, tester);
            decode(second_arg, tester);
        }
        if (third_arg == "-uint")
        {
            bool _uint = true;
            bool tester = true;
            encode(second_arg,_uint, tester);
            decode(second_arg, tester);
        }
    }
}
