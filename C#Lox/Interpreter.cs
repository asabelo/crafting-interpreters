namespace Lox;

public class Interpreter : Expr.IVisitor<object?>
{
    public object? VisitBinaryExpr(Expr.Binary expr)
    {
        var left = Evaluate(expr.left);
        var right = Evaluate(expr.right);

        return expr.@operator.Type switch
        {
            TokenType.MINUS => CheckNumber(expr.@operator, left) - CheckNumber(expr.@operator, right),
            TokenType.STAR  => CheckNumber(expr.@operator, left) * CheckNumber(expr.@operator, right),
            TokenType.SLASH => CheckNumber(expr.@operator, left) / CheckNumber(expr.@operator, right),
            TokenType.PLUS when left is double leftDouble && right is double rightDouble => leftDouble + rightDouble,
            TokenType.PLUS when left is string leftString && right is string rightString => leftString + rightString,
            TokenType.PLUS => throw new RuntimeError(expr.@operator, "Operands must be two numbers or two strings."),

            TokenType.GREATER       => CheckNumber(expr.@operator, left) >  CheckNumber(expr.@operator, right),
            TokenType.GREATER_EQUAL => CheckNumber(expr.@operator, left) >= CheckNumber(expr.@operator, right),
            TokenType.LESS          => CheckNumber(expr.@operator, left) <  CheckNumber(expr.@operator, right),
            TokenType.LESS_EQUAL    => CheckNumber(expr.@operator, left) <= CheckNumber(expr.@operator, right),

            TokenType.BANG_EQUAL    => !IsEqual(left, right),
            TokenType.EQUAL_EQUAL   => IsEqual(left, right)
        };
    }

    public object? VisitGroupingExpr(Expr.Grouping expr)
    {
        return Evaluate(expr.expression);
    }

    public object? VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.value;
    }

    public object? VisitUnaryExpr(Expr.Unary expr)
    {
        var right = Evaluate(expr.right);

        return expr.@operator.Type switch
        {
            TokenType.BANG => !IsTruthy(right),
            TokenType.MINUS => -CheckNumber(expr.@operator, right),
            _ => null
        };
    }

    private static double CheckNumber(Token @operator, object? operand)
    {
        if (operand is double number) return number;

        throw new RuntimeError(@operator, "Operands must be numbers.");
    }

    private static bool IsEqual(object? left, object? right)
    {
        return (left, right) switch
        {
            (null, null) => true,
            (null, _)    => false,
            _            => left.Equals(right)
        };
    }

    private static bool IsTruthy(object? expression)
    {
        return expression switch
        {
            null => false,
            bool b => b,
            _ => true
        };
    }

    private object? Evaluate(Expr expression)
    {
        return expression.Accept(this);
    }
}
