using System.Globalization;

namespace Lox;

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
        return Evaluate(expr.Expression);
    }

    public object? VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.Value;
    }

    public object? VisitLogicalExpr(Expr.Logical expr)
    {
        var left = Evaluate(expr.Left);

        if (expr.Operator.Type == TokenType.OR)
        {
            if (IsTruthy(left)) return left;
        }
        else
        {
            if (!IsTruthy(left)) return left;
        }

        return Evaluate(expr.Right);
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

    public object? VisitVariableExpr(Expr.Variable expr)
    {
        return environment.Get(expr.Name);
    }

    public object? VisitAssignExpr(Expr.Assign expr)
    {
        var value = Evaluate(expr.Value);

        environment.Assign(expr.Name, value);

        return value;
    }

    public object? VisitLambdaExpr(Expr.Lambda expr)
    {
        return new Lambda(expr, environment);
    }

    public Void VisitIfStmt(Stmt.If stmt)
    {
        var condition = Evaluate(stmt.Condition);

        if (IsTruthy(condition))
        {
            Execute(stmt.ThenBranch);
        }
        else if (stmt.ElseBranch is not null)
        {
            Execute(stmt.ElseBranch);
        }

        return Void.Value;
    }

    public Void VisitExpressionStmt(Stmt.Expression stmt)
    {
        _ = Evaluate(stmt.InnerExpression);

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
        var value = Evaluate(stmt.InnerExpression);

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
        var value = stmt.Initializer is Expr e ? Evaluate(e) : Void.Value;

        environment.Define(stmt.Name.Lexeme, value);
        
        return Void.Value;
    }

    public Void VisitWhileStmt(Stmt.While stmt)
    {
        try
        {
            while (IsTruthy(Evaluate(stmt.Condition)))
            {
                Execute(stmt.Body);
            }
        }
        catch (BreakStmtException) { }

        return Void.Value;
    }

    public Void VisitBlockStmt(Stmt.Block stmt)
    {
        ExecuteBlock(stmt.Statements, new Environment(environment));

        return Void.Value;
    }

    public Void VisitBreakStmt(Stmt.Break stmt)
    {
        throw new BreakStmtException();
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
