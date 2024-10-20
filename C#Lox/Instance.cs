namespace Lox;

public class Instance(Class? klass)
{
    protected readonly Class? metaklass = klass;

    private readonly Dictionary<string, object?> fields = [];

    public object? Get(Token name)
    {
        if (fields.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }

        if (metaklass?.FindMethod(name.Lexeme) is Function method)
        {
            return method.Bind(this);
        }

        if (metaklass is not null)
        {
            return metaklass.Get(name);
        }

        throw new RuntimeError(name, $"Undefined property '{name.Lexeme}'.");
    }

    public void Set(Token name, object? value)
    {
        fields[name.Lexeme] = value;
    }

    public override string ToString() => $"<instance of {metaklass?.Name ?? "Meta class"}>";
}
