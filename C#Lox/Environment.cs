namespace Lox;

public class Environment(Environment? enclosing = null)
{
    public Environment? Enclosing { get; } = enclosing;
    private readonly Dictionary<string, object?> values = [];

    public object? Get(Token name)
    {
        if (values.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }

        if (Enclosing is not null)
        {
            return Enclosing.Get(name);
        }

        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }

    public void Assign(Token name, object? value)
    {
        if (values.ContainsKey(name.Lexeme))
        {
            values[name.Lexeme] = value;
            return;
        }

        if (Enclosing is not null)
        {
            Enclosing.Assign(name, value);
            return;
        }

        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }

    public void Define(string name, object? value)
    {
        values[name] = value;
    }

    private Environment Ancestor(int distance)
    {
        var environment = this;

        for (int i = 0; i < distance; ++i) 
        {
            environment = environment.Enclosing!;
        }

        return environment;
    }

    public object? GetAt(int distance, string name)
    {
        return Ancestor(distance).values[name];
    }

    public void AssignAt(int distance, Token name, object? value)
    {
        Ancestor(distance).values[name.Lexeme] = value;
    }
}
