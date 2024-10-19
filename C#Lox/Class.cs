
namespace Lox;

public class Class(string name) : ICallable
{
    public string Name { get; } = name;

    public int Arity() => 0;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        return new Instance(this);
    }

    public override string ToString() => $"<class {Name}>";
}
