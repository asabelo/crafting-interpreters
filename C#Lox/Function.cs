
namespace Lox;

public class Function(Stmt.Function declaration, Environment closure) : ICallable
{
    private readonly Stmt.Function declaration = declaration;
    private readonly Environment closure = closure;

    public int Arity() => declaration.Params.Count;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var environment = new Environment(closure);

        for (int i = 0; i < declaration.Params.Count; ++i)
        {
            environment.Define(declaration.Params[i].Lexeme, arguments[i]);
        }

        try
        {
            interpreter.ExecuteBlock(declaration.Body, environment);
        }
        catch (ReturnStmtException ret)
        {
            return ret.Value;
        }

        return null;
    }

    public override string ToString() => $"<function {declaration.Name.Lexeme}>";
}
