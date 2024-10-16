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
            if (value is Void)
            {
                throw new RuntimeError(name, $"Uninitialized variable '{name.Lexeme}'.");
            }

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
}
