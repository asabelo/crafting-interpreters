
namespace Lox;

public class Function(Stmt.Function declaration) : ICallable
{
    private readonly Stmt.Function declaration = declaration;

    public int Arity() => declaration.Params.Count;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var environment = new Environment(interpreter.Globals);

        for (int i = 0; i < declaration.Params.Count; ++i)
        {
            environment.Define(declaration.Params[i].Lexeme, arguments[i]);
        }

        interpreter.ExecuteBlock(declaration.Body, environment);

        return null;
    }

    public override string ToString() => $"<fn {declaration.Name.Lexeme}>";
}
