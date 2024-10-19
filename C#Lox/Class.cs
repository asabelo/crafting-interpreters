
namespace Lox;

public class Class(string name, Dictionary<string, Function> methods) : ICallable
{
    public string Name { get; } = name;

    private readonly Dictionary<string, Function> methods = methods;

    public Function? FindMethod(string name)
    {
        return methods.GetValueOrDefault(name);
    }

    public int Arity() => FindMethod("init")?.Arity() ?? 0;
    
    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        var instance = new Instance(this);

        if (FindMethod("init") is Function initializer)
        {
            initializer.Bind(instance).Call(interpreter, arguments);
        }

        return instance;
    }

    public override string ToString() => $"<class {Name}>";
}
