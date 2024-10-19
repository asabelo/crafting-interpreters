namespace Lox;

public class Environment
{
    private readonly Environment? enclosing;
    private readonly Dictionary<string, object?> values = [];

    public Environment(Environment? enclosing = null)
    {
        this.enclosing = enclosing;
    }

    public object? Get(Token name)
    {
        if (values.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }

        if (enclosing is not null)
        {
            return enclosing.Get(name);
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

        if (enclosing is not null)
        {
            enclosing.Assign(name, value);
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
            environment = environment.enclosing!;
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
