namespace Lox;

public class Instance(Class klass)
{
    private readonly Class klass = klass;

    private readonly Dictionary<string, object?> fields = [];

    public object? Get(Token name)
    {
        if (fields.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }

        if (klass.FindMethod(name.Lexeme) is Function method)
        {
            return method;
        }
        
        throw new RuntimeError(name, $"Undefined property '{name.Lexeme}'.");
    }

    public void Set(Token name, object? value)
    {
        fields[name.Lexeme] = value;
    }

    public override string ToString() => $"<instance of {klass.Name}>";
}
