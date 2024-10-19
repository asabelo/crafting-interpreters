
namespace Lox;

public class Function(Stmt.Function declaration, Environment closure, bool isInitializer) : ICallable
{
    private readonly Stmt.Function declaration = declaration;

    private readonly Environment closure = closure;

    private readonly bool isInitializer = isInitializer;

    public Function Bind(Instance instance)
    {
        var environment = new Environment(closure);

        environment.Define("this", instance);

        return new Function(declaration, environment, isInitializer);
    }

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
            if (isInitializer) return closure.GetAt(0, "this");

            return ret.Value;
        }

        if (isInitializer) return closure.GetAt(0, "this");

        return null;
    }

    public override string ToString() => $"<function {declaration.Name.Lexeme}>";
}
