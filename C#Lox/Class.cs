
namespace Lox;

public class Class : Instance, ICallable
{
    private static readonly Class metametaklass = new();

    public string Name { get; }
    
    private readonly Dictionary<string, Function> methods;

    private Class() : base(klass: null)
    {
        Name = "Meta class";
        methods = [];
    }

    public Class(string name, Dictionary<string, Function> methods) : base(metametaklass)
    {
        Name = name;
        this.methods = methods;
    }

    public Class(string name, Dictionary<string, Function> methods, Class metaklass) : base(metaklass)
    {
        Name = name;
        this.methods = methods;
    }

    public Function? FindMethod(string name)
    {
        return methods.GetValueOrDefault(name);
    }

    public int Arity() => FindMethod("init")?.Arity() ?? 0;

    public object? Call(Interpreter interpreter, List<object?> arguments)
    {
        Instance instance;
        
        if (metaklass is null)
        {
            var name = (string)arguments[0]!;
            var methods = (Dictionary<string, Function>)arguments[1]!;

            instance = new Class(name, methods, metametaklass);
        }
        else if (metaklass == metametaklass)
        {
            var name = (string)arguments[0]!;
            var methods = (Dictionary<string, Function>)arguments[1]!;
            
            instance = new Class(name, methods, this);
        }
        else
        {
            instance = new Instance(this);
        }

        if (FindMethod("init") is Function initializer)
        {
            initializer.Bind(instance).Call(interpreter, arguments);
        }

        return instance;
    }

    public override string ToString() => $"<class {Name}>";
}
