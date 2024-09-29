bool hadError = false;

switch (args.Length)
{
    case > 1:
        Console.WriteLine("Usage: c#lox [script]");
        Environment.Exit(64);
        break;

    case 1:
        await RunFile(args[0]);
        break;

    default:
        RunPrompt();
        break;
}

// Read and execute source file 
async Task RunFile(string filePath)
{
    var code = await File.ReadAllTextAsync(filePath);
    
    Run(code);

    if (hadError)
    {
        Environment.Exit(65);
    }
}

// Start a REPL
void RunPrompt()
{
    var keepGoing = true;

    while (keepGoing)
    {
        Console.Write("> ");

        var code = Console.ReadLine();

        if (code is null)
        {
            keepGoing = false;
        }
        else
        {
            Run(code);
            
            hadError = false;
        }
    }
}

void Run(string code)
{
    foreach (var token in code.Split(' '))
    {
        Console.WriteLine(token);
    }
}

async Task Error(int line, string message)
{
    await Report(line, string.Empty, message);
}

// Helper function for error reporting
/*private*/ async Task Report(int line, string where, string message)
{
    await Console.Error.WriteLineAsync($"[line {line}] Error{where}: {message}");
    hadError = true;
}
