namespace Lox;

using Env = System.Environment;

public static class Lox 
{
    private static bool hadError = false;
    private static bool hadRuntimeError = false;

    private static readonly Interpreter interpreter = new(); 

    public static async Task Main(string[] args)
    {
        switch (args.Length)
        {
            case > 1:
                await Console.Out.WriteLineAsync("Usage: c#lox [script]");
                Env.Exit(64);
                break;

            case 1:
                await RunFileAsync(args[0]);
                break;

            default:
                await RunPromptAsync();
                break;
        }
    }

    // Read and execute source file 
    private static async Task RunFileAsync(string filePath)
    {
        var code = await File.ReadAllTextAsync(filePath);

        await RunAsync(code, fromPrompt: false);

        if (hadError)
        {
            Env.Exit(65);
        }
        else if (hadRuntimeError)
        {
            Env.Exit(70);
        }
    }

    // Start a REPL
    private static async Task RunPromptAsync()
    {
        var keepGoing = true;

        while (keepGoing)
        {
            await Console.Out.WriteAsync("> ");

            var code = await Console.In.ReadLineAsync();

            if (keepGoing = code is not null)
            {
                await RunAsync(code!, fromPrompt: true);

                hadError = false;
            }
        }
    }

    private static async Task RunAsync(string code, bool fromPrompt)
    {
        var scanner = new Scanner(code);
        var tokens = await scanner.ScanTokensAsync();

        var parser = new Parser(tokens, fromPrompt);
        var statements = await parser.ParseAsync();

        if (hadError || statements is null) return;

        var resolver = new Resolver(interpreter);
        resolver.Resolve(statements);

        if (hadError) return;

        await interpreter.Interpret(statements);
    }

    public static async Task ErrorAsync(int line, string message)
    {
        await ReportAsync(line, string.Empty, message);
    }

    // Helper function for error reporting
    private static async Task ReportAsync(int line, string where, string message)
    {
        await Console.Error.WriteLineAsync($"[line {line}] Error{where}: {message}");
        hadError = true;
    }

    public static async Task ErrorAsync(Token token, string message)
    {
        if (token.Type == TokenType.EOF)
        {
            await ReportAsync(token.Line, " at end", message);
        }
        else
        {
            await ReportAsync(token.Line, $" at '{token.Lexeme}'", message);
        }
    }

    // hehe
    public static void Error(Token token, string message)
    {
        if (token.Type == TokenType.EOF)
        {
            Report(token.Line, " at end", message);
        }
        else
        {
            Report(token.Line, $" at '{token.Lexeme}'", message);
        }
    }

    private static void Report(int line, string where, string message)
    {
        Console.Error.WriteLine($"[line {line}] Error{where}: {message}");
        hadError = true;
    }

    public static async Task RuntimeErrorAsync(RuntimeError error)
    {
        await Console.Error.WriteLineAsync($"{error.Message}\n[line {error.Token.Line}]");

        hadRuntimeError = true;
    }
}
