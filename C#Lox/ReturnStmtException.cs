namespace Lox;

public class ReturnStmtException(object? value) : Exception
{
    public object? Value { get; } = value;
}
