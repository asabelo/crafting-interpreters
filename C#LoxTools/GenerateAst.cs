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

        await DefineAstAsync
        (
            outputDir,
            "Expr",
            [
                "Ternary  : Expr Left, Token LeftOperator, Expr Middle, Token RightOperator, Expr Right",
                "Binary   : Expr Left, Token Operator, Expr Right",
                "Grouping : Expr Expression",
                "Literal  : object? Value",
                "Unary    : Token Operator, Expr Expression",
                "Assign   : Token Name, Expr Value",
                "Logical  : Expr Left, Token Operator, Expr Right",
                "Variable : Token Name"
            ]
        );

        await DefineAstAsync
        (
            outputDir,
            "Stmt",
            [
                "Block      : List<Stmt> Statements",
                "Expression : Expr InnerExpression",
                "Print      : Expr InnerExpression",
                "Var        : Token Name, Expr? Initializer",
                "If         : Expr Condition, Stmt ThenBranch, Stmt? ElseBranch",
                "While      : Expr Condition, Stmt Body"
            ]
        );
    }

    private static async Task DefineAstAsync(string outputDir, string baseName, List<string> types)
    {
        string path = Path.ChangeExtension(Path.Combine(outputDir, baseName), "cs");

        using var writer = new StreamWriter(path);

        await writer.WriteLineAsync("namespace Lox;");
        await writer.WriteLineAsync();
        await writer.WriteLineAsync($"public abstract record {baseName}");
        await writer.WriteLineAsync("{");

        await DefineVisitorAsync(writer, baseName, types);

        // AST classes
        foreach (var type in types)
        {
            var (className, fields) = type.Split(':', StringSplitOptions.TrimEntries) switch { var a => (a[0], a[1]) };
            
            await DefineTypeAsync(writer, baseName, className, fields);
        }

        // base accept() method
        await writer.WriteLineAsync();
        await writer.WriteLineAsync("    public abstract R Accept<R>(IVisitor<R> visitor);");

        await writer.WriteLineAsync("}");
    }

    private static async Task DefineVisitorAsync(StreamWriter writer, string baseName, List<string> types)
    {
        await writer.WriteLineAsync("    public interface IVisitor<R>");
        await writer.WriteLineAsync("    {");
       
        foreach (var type in types)
        {
            string typeName = type.Split(':', StringSplitOptions.TrimEntries)[0];

            await writer.WriteLineAsync($"        R Visit{typeName}{baseName}({typeName} {baseName.ToLower()});");
        }

        await writer.WriteLineAsync("    }");
    }

    private static async Task DefineTypeAsync(StreamWriter writer, string baseName, string className, string fieldList)
    {
        var fields = fieldList.Split(", ");

        await writer.WriteLineAsync();
        await writer.WriteLineAsync($"    public record {className}({fieldList}) : {baseName}");
        await writer.WriteLineAsync( "    {");
        await writer.WriteLineAsync( "        public override R Accept<R>(IVisitor<R> visitor)");
        await writer.WriteLineAsync( "        {");
        await writer.WriteLineAsync($"            return visitor.Visit{className}{baseName}(this);");
        await writer.WriteLineAsync( "        }");
        await writer.WriteLineAsync( "    }");
    }
}
