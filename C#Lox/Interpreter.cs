using System.Globalization;

namespace Lox;

public sealed class Void
{
    public static readonly Void Value = new();
    private Void() {}
}

public class Interpreter : Expr.IVisitor<object?>, Stmt.IVisitor<Void>
{
    public readonly Environment Globals = new();
    private Environment environment;

    public Interpreter()
    {
        Globals.Define("clock", new ClockCallable());

        environment = Globals;
    }

    private class ClockCallable : ICallable
    {
        public int Arity() => 0;

        public object? Call(Interpreter interpreter, List<object?> arguments)
        {
            return TimeSpan.FromTicks(DateTime.Now.Ticks).TotalSeconds;
        }

        public override string ToString() => "<native fn>";
    }

    public async Task Interpret(List<Stmt> statements)
    {
        try
        {
            foreach (var statement in statements)
            {
                Execute(statement);
            }
        }
        catch (RuntimeError error)
        {
            await Lox.RuntimeErrorAsync(error);
        }
    }

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
            TokenType.EQUAL_EQUAL   => IsEqual(left, right),

            _ => throw new RuntimeError(expr.@operator, "Unexpected error.")
        };
    }

    public object? VisitCallExpr(Expr.Call expr)
    {
        var callee = Evaluate(expr.Callee);

        var arguments = new List<object?>();

        foreach (var argument in expr.Arguments)
        {
            arguments.Add(Evaluate(argument));
        }

        if (callee is not ICallable function)
        {
            throw new RuntimeError(expr.Paren, "Can only call functions and classes.");
        }

        if (arguments.Count is int argCount && function.Arity() is int arity && argCount != arity)
        {
            throw new RuntimeError(expr.Paren, $"Expected {arity} arguments but got {argCount}.");
        }

        return function.Call(this, arguments);
    }

    public object? VisitGroupingExpr(Expr.Grouping expr)
    {
        return Evaluate(expr.expression);
    }

    public object? VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.value;
    }

    public object? VisitLogicalExpr(Expr.Logical expr)
    {
        var left = Evaluate(expr.left);

        if (expr.@operator.Type == TokenType.OR)
        {
            if (IsTruthy(left)) return left;
        }
        else
        {
            if (!IsTruthy(left)) return left;
        }

        return Evaluate(expr.right);
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

    public object? VisitVariableExpr(Expr.Variable expr)
    {
        return environment.Get(expr.name);
    }

    public object? VisitAssignExpr(Expr.Assign expr)
    {
        var value = Evaluate(expr.value);

        environment.Assign(expr.name, value);

        return value;
    }

    public Void VisitIfStmt(Stmt.If stmt)
    {
        var condition = Evaluate(stmt.condition);

        if (IsTruthy(condition))
        {
            Execute(stmt.thenBranch);
        }
        else if (stmt.elseBranch is not null)
        {
            Execute(stmt.elseBranch);
        }

        return Void.Value;
    }

    public Void VisitExpressionStmt(Stmt.Expression stmt)
    {
        _ = Evaluate(stmt.expression);

        return Void.Value;
    }

    public Void VisitFunctionStmt(Stmt.Function stmt)
    {
        var function = new Function(stmt, environment);

        environment.Define(stmt.Name.Lexeme, function);

        return Void.Value;
    }

    public Void VisitPrintStmt(Stmt.Print stmt)
    {
        var value = Evaluate(stmt.expression);

        Console.WriteLine(Stringify(value));

        return Void.Value;
    }

    public Void VisitReturnStmt(Stmt.Return stmt)
    {
        var value = stmt.Value is null ? null : Evaluate(stmt.Value);

        throw new ReturnStmtException(value);
    }

    public Void VisitVarStmt(Stmt.Var stmt)
    {
        var value = stmt.initializer is Expr e ? Evaluate(e) : null;

        environment.Define(stmt.name.Lexeme, value);
        
        return Void.Value;
    }

    public Void VisitWhileStmt(Stmt.While stmt)
    {
        while (IsTruthy(Evaluate(stmt.condition)))
        {
            Execute(stmt.body);
        }

        return Void.Value;
    }

    public Void VisitBlockStmt(Stmt.Block stmt)
    {
        ExecuteBlock(stmt.statements, new Environment(environment));

        return Void.Value;
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
        if (obj is double number) return number.ToString(CultureInfo.InvariantCulture);

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

    private void Execute(Stmt statement)
    {
        statement.Accept(this);
    }

    public void ExecuteBlock(List<Stmt> statements, Environment environment)
    {
        var previousEnvironment = this.environment;

        try
        {
            this.environment = environment;

            foreach (var statement in statements)
            {
                Execute(statement);
            }
        }
        finally
        {
            this.environment = previousEnvironment;
        }
    }
}
