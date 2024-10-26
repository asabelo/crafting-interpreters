
#include <fstream>
#include <streambuf>

#include "chunk.hpp"
#include "common.hpp"
#include "debug.hpp"
#include "vm.hpp"

void repl(lox::vm&);

void run_file(lox::vm&, const std::string&);

int main(int argc, char* argv[])
{
    auto vm = lox::vm{};

    if (argc == 1)
    {
        repl(vm);
    }
    else if (argc == 2)
    {
        run_file(vm, argv[1]);
    }
    else
    {
        std::cerr << "Usage: clox [path]\n";

        std::exit(64);
    }

    return 0;
}

static void repl(lox::vm& vm)
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
}

static void run_file(lox::vm& vm, const std::string& path)
{
    std::ifstream file_stream{ path };

    std::string source{ std::istreambuf_iterator<char>{ file_stream }, {} };

    auto result = vm.interpret(source);

    if (result == lox::interpret_result::INTERPRET_COMPILE_ERROR) std::exit(65);
    if (result == lox::interpret_result::INTERPRET_RUNTIME_ERROR) std::exit(70);
}
