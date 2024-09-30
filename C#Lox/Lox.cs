namespace Lox;

public static class Lox 
{
    private static bool hadError = false;

    public static async Task Main(string[] args)
    {
        switch (args.Length)
        {
            case > 1:
                await Console.Out.WriteLineAsync("Usage: c#lox [script]");
                Environment.Exit(64);
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
        
        await RunAsync(code);

        if (hadError)
        {
            Environment.Exit(65);
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
                await RunAsync(code!);

                hadError = false;
            }
        }
    }

    private static async Task RunAsync(string code)
    {
        foreach (var token in code.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
        {
            await Console.Out.WriteLineAsync(token);
        }
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
}
