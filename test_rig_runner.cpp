#include "process.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include "inipp.h"

#define STRINGIFY(s) STRINGIFY_2(s)
#define STRINGIFY_2(s) #s

using namespace TinyProcessLib;

bool arg_is_mode(std::string str)
{
    return std::regex_match(str,
        std::regex("(-gui)|(-tokens)|(-tree)"));
}

int main(int argc, char* argv[])
{
    inipp::Ini<char> ini;
    std::ifstream is("test_rig.ini");
    ini.parse(is);

    auto& java_section = ini.sections["Java Args"];

    std::string java_executable;
    inipp::extract(java_section["java_executable"], java_executable);

    std::string classpath;
    inipp::extract(java_section["classpath"], classpath);

    std::string antlr_class;
    inipp::extract(java_section["antlr_class"], antlr_class);

    auto& antlr_section = ini.sections["ANTLR Args"];

    std::string grammar_name;
    inipp::extract(antlr_section["grammar_name"], grammar_name);

    std::string start_rule;
    inipp::extract(antlr_section["start_rule"], start_rule);

    std::string harness_mode;
    inipp::extract(antlr_section["harness_mode"], harness_mode);

    std::string input_file;
    inipp::extract(antlr_section["input_file"], input_file);

    // Command line arguments to this program can override ini
    for (int i=1; i<argc; i++)
    {
        if (arg_is_mode(argv[i]))
            harness_mode = argv[i];
        else
            input_file = argv[i];
    }

    std::vector<std::string> command_line =
    {
        java_executable,
        "-cp",
        classpath,
        antlr_class,
        grammar_name,
        start_rule,
        harness_mode,
        input_file
    };

    std::cout << "Test rig command line:\n\n";
    for (auto const& arg : command_line)
        std::cout << " " << arg;
    std::cout << std::endl << std::endl;

    Process process(
            command_line,
            "",
            [](const char *bytes, size_t n) {
                std::cout << std::string(bytes, n);
            },
            [](const char *bytes, size_t n) {
                std::cout << std::string(bytes, n);
                // Add a newline for prettier output on some platforms:
                if(bytes[n - 1] != '\n')
                    std::cout << std::endl;
            });
    auto exit_status = process.get_exit_status();
    std::cout << "Process returned: " << exit_status << " (" << (exit_status == 0 ? "success" : "failure") << ")" << std::endl;
    return 0;
}
