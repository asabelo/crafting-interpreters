
#include <fstream>
#include <streambuf>

#include "chunk.hpp"
#include "common.hpp"
#include "debug.hpp"
#include "vm.hpp"

int repl(lox::vm&);

int run_file(lox::vm&, const std::string&);

int main(int argc, char* argv[])
{
    lox::chunk chunk{};
    lox::vm vm{ chunk };

    if (argc == 1)
    {
        return repl(vm);
    }
    else if (argc == 2)
    {
        return run_file(vm, argv[1]);
    }
    else
    {
        std::cerr << "Usage: clox [path]\n";

        return 64;
    }
}

static int repl(lox::vm& vm)
{
    std::string line;

    while (true)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, line))
        {
            std::cout << '\n';
            break;
        }

        vm.interpret(line);
    }

    return 0;
}

static int run_file(lox::vm& vm, const std::string& path)
{
    std::ifstream file_stream{ path };

    std::string source{ std::istreambuf_iterator<char>{ file_stream }, {} };

    auto result = vm.interpret(source);

    if (result == lox::interpret_result::INTERPRET_COMPILE_ERROR) return 65;
    if (result == lox::interpret_result::INTERPRET_RUNTIME_ERROR) return 70;

    return 0;
}
