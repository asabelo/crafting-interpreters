namespace Lox;

public class Interpreter : Expr.IVisitor<object?>
{
    public async Task Interpret(Expr expression)
    {
        try
        {
            var value = Evaluate(expression);
            await Console.Out.WriteLineAsync(Stringify(value));
        }
        catch (RuntimeError error)
        {
            await Lox.RuntimeErrorAsync(error);
        }
    }

    public object? VisitTernaryExpr(Expr.Ternary expr)
    {
        var left = Evaluate(expr.Left);

        return IsTruthy(left) ? Evaluate(expr.Middle) : Evaluate(expr.Right);
    }

    public object? VisitBinaryExpr(Expr.Binary expr)
    {
        var left = Evaluate(expr.Left);
        var right = Evaluate(expr.Right);

        return expr.Operator.Type switch
        {
            TokenType.MINUS => CheckNumber(expr.Operator, left) - CheckNumber(expr.Operator, right),
            TokenType.STAR  => CheckNumber(expr.Operator, left) * CheckNumber(expr.Operator, right),
            TokenType.SLASH => (left, right) switch
            {
                (_, 0.0) => throw new RuntimeError(expr.Operator, "Division by zero."),
                _ => CheckNumber(expr.Operator, left) / CheckNumber(expr.Operator, right)
            },
            TokenType.PLUS => (left, right) switch
            {
                (double leftDouble, double rightDouble) => leftDouble + rightDouble,
                (string leftString, var    rightOther)  => leftString + Stringify(rightOther),
                (var    leftOther,  string rightString) => Stringify(leftOther) + rightString,
                _ => throw new RuntimeError(expr.Operator, "Operands must be two numbers or a string and an object.")
            },

            TokenType.GREATER       => CheckNumber(expr.Operator, left) >  CheckNumber(expr.Operator, right),
            TokenType.GREATER_EQUAL => CheckNumber(expr.Operator, left) >= CheckNumber(expr.Operator, right),
            TokenType.LESS          => CheckNumber(expr.Operator, left) <  CheckNumber(expr.Operator, right),
            TokenType.LESS_EQUAL    => CheckNumber(expr.Operator, left) <= CheckNumber(expr.Operator, right),

            TokenType.BANG_EQUAL    => !IsEqual(left, right),
            TokenType.EQUAL_EQUAL   => IsEqual(left, right),

            TokenType.COMMA         => right,

            _ => throw new RuntimeError(expr.Operator, "Unexpected error.")
        };
    }

    public object? VisitGroupingExpr(Expr.Grouping expr)
    {
        return Evaluate(expr.Expression);
    }

    public object? VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.Value;
    }

    public object? VisitUnaryExpr(Expr.Unary expr)
    {
        var right = Evaluate(expr.Expression);

        return expr.Operator.Type switch
        {
            TokenType.BANG => !IsTruthy(right),
            TokenType.MINUS => -CheckNumber(expr.Operator, right),
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

    private static string Stringify(object? obj)
    {
        return obj?.ToString() ?? "nil";
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
