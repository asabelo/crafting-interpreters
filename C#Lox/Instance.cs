namespace Lox;

public class Instance(Class klass)
{
    private readonly Class klass = klass;

    public override string ToString() => $"<instance of {klass.Name}>";
}
