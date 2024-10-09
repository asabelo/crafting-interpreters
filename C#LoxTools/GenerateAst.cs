namespace Tool;

public class GenerateAst
{
    public static async Task Main(string[] args)
    {
        if (args.Length != 1)
        {
            await Console.Error.WriteLineAsync("Usage: generate_ast <output directory>");
            Environment.Exit(64);
        }

        string outputDir = args[0];

        await DefineAst
        (
            outputDir,
            "Expr",
            [
                "Binary   : Expr left, Token @operator, Expr right",
                "Grouping : Expr expression",
                "Literal  : Object value",
                "Unary    : Token @operator, Expr right"
            ]
        );
    }

    private static async Task DefineAst(string outputDir, string baseName, List<string> types)
    {
        string path = Path.ChangeExtension(Path.Combine(outputDir, baseName), "cs");

        using var writer = new StreamWriter(path);

        await writer.WriteAsync
        (
            $$"""
            namespace Lox;

            public abstract class {{baseName}}
            {
            """
        );

        foreach (var type in types)
        {
            var (className, fields) = type.Split(':', StringSplitOptions.TrimEntries) switch { var a => (a[0], a[1]) };
            
            await DefineType(writer, baseName, className, fields);
        }

        await writer.WriteAsync
        (
            "};\n"
        );
    }

    private static async Task DefineType(StreamWriter writer, string baseName, string className, string fieldList)
    {
        await writer.WriteAsync
        (
            $$"""

                public class {{className}} : {{baseName}} 
                {
                    public {{className}}({{fieldList}})
                    {
            """
        );

        var fields = fieldList.Split(", ");

        foreach (var field in fields)
        {
            var name = field.Split(' ')[1];
            await writer.WriteAsync
            (
                $"\n\t\t\tthis.{name} = {name};"
            );
        }

        await writer.WriteAsync
        (
            "\n\t\t}\n"
        );

        foreach (var field in fields)
        {
            await writer.WriteAsync
            (
                $"\n\t\tpublic readonly {field};"
            );
        }

        await writer.WriteAsync
        (
            "\n\t}\n"
        );
    }
}
