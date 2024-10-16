namespace Lox;

public sealed class Void
{
    public static readonly Void Value = new();
    private Void() {}
}
