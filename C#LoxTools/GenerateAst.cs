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
                "Literal  : object? value",
                "Unary    : Token @operator, Expr right"
            ]
        );

        await DefineAst
        (
            outputDir,
            "Stmt",
            [
                "Expression : Expr expression",
                "Print      : Expr expression"
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

        await DefineVisitor(writer, baseName, types);

        // AST classes
        foreach (var type in types)
        {
            var (className, fields) = type.Split(':', StringSplitOptions.TrimEntries) switch { var a => (a[0], a[1]) };
            
            await DefineType(writer, baseName, className, fields);
        }

        // base accept() method
        await writer.WriteAsync("\n    public abstract R Accept<R>(IVisitor<R> visitor);\n");

        await writer.WriteAsync
        (
            "};\n"
        );
    }

    private static async Task DefineVisitor(StreamWriter writer, string baseName, List<string> types)
    {
        await writer.WriteAsync
        (
            """
            
                public interface IVisitor<R>
                {
            """
        );
        
        foreach (var type in types)
        {
            string typeName = type.Split(':', StringSplitOptions.TrimEntries)[0];

            await writer.WriteAsync
            (
                $"""

                        R Visit{typeName}{baseName}({typeName} {baseName.ToLower()});

                """
            );
        }

        await writer.WriteAsync
        (
            "    }\n"
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
                $"\n            this.{name} = {name};"
            );
        }

        await writer.WriteAsync
        (
            "\n        }\n"
        );

        await writer.WriteAsync
        (
            $$"""

                    public override R Accept<R>(IVisitor<R> visitor)
                    {
                        return visitor.Visit{{className}}{{baseName}}(this);
                    }

            """
        );

        foreach (var field in fields)
        {
            await writer.WriteAsync
            (
                $"\n        public readonly {field};"
            );
        }

        await writer.WriteAsync
        (
            "\n    }\n"
        );
    }
}
