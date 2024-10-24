
#include "chunk.hpp"
#include "common.hpp"
#include "debug.hpp"
#include "vm.hpp"

void repl(lox::vm&);

void run_file(lox::vm&, const char*);

char* read_file(const char*);

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
        std::fprintf(stderr, "Usage: clox [path]\n");
        std::exit(64);
    }

    return 0;
}

static void repl(lox::vm& vm)
{
    char line[1024];
    for (;;)
    {
        std::printf("> ");

        if (!std::fgets(line, sizeof(line), stdin)) {
            std::printf("\n");
            break;
        }

        vm.interpret(line);
    }
}

static void run_file(lox::vm& vm, const char* path)
{
    char* source = read_file(path);
    lox::interpret_result result = vm.interpret(source);
    std::free(source);

    if (result == lox::interpret_result::INTERPRET_COMPILE_ERROR) std::exit(65);
    if (result == lox::interpret_result::INTERPRET_RUNTIME_ERROR) std::exit(70);
}

static char* read_file(const char* path)
{
#pragma warning (disable : 4996)
    FILE* file = std::fopen(path, "rb");

    if (!file)
    {
        std::fprintf(stderr, "Could not open file \"%s\".\n", path);
        std::exit(74);
    }

    std::fseek(file, 0L, SEEK_END);
    std::size_t fileSize = ftell(file);
    std::rewind(file);

    char* buffer = (char*)std::malloc(fileSize + 1);
    if (!buffer)
    {
        std::fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        std::exit(74);
    }

    std::size_t bytesRead = std::fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) 
    {
        std::fprintf(stderr, "Could not read file \"%s\".\n", path);
        std::exit(74);
    }

    buffer[bytesRead] = '\0';

    std::fclose(file);
    return buffer;
}
