
namespace Lox;

public class Lambda(Expr.Lambda lambda, Environment closure) : ICallable
{
    private readonly Expr.Lambda lambda = lambda;
    private readonly Environment closure = closure;

    public int Arity() => lambda.Params.Count;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var environment = new Environment(closure);

        for (int i = 0; i < lambda.Params.Count; ++i)
        {
            environment.Define(lambda.Params[i].Lexeme, arguments[i]);
        }

        try
        {
            interpreter.ExecuteBlock(lambda.Body, environment);
        }
        catch (ReturnStmtException ret)
        {
            return ret.Value;
        }

        return null;
    }

    public override string ToString() => "<lambda>";
}
