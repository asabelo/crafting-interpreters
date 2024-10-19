namespace Lox;

public class Resolver(Interpreter interpreter) : Stmt.IVisitor<Unit>, Expr.IVisitor<Unit>
{
    private readonly Interpreter interpreter = interpreter;

    private readonly Stack<Dictionary<string, bool>> scopes = new();

    private CallableType currentCallable = CallableType.NONE;

    private enum CallableType
    {
        NONE, FUNCTION, LAMBDA
    }

#region Private helpers

    private void Resolve(Expr expression) => expression.Accept(this);

    private void Resolve(Stmt statement) => statement.Accept(this);

    private void ResolveLocal(Expr expr, Token name)
    {
        foreach (var (scope, index) in scopes.Select((s, i) => (s, i)))
        {
            if (scope.ContainsKey(name.Lexeme))
            {
                interpreter.Resolve(expr, index);
                return;
            }
        }
    }

    private void ResolveCallable(List<Token> parameters, List<Stmt> body, CallableType type)
    {
        var enclosingCallable = currentCallable;
        currentCallable = type;

        BeginScope();

        foreach (var parameter in parameters)
        {
            Declare(parameter);
            Define(parameter);
        }

        Resolve(body);

        EndScope();

        currentCallable = enclosingCallable;
    }

    private void BeginScope() => scopes.Push([]);

    private void EndScope() => scopes.Pop();

    private void Declare(Token name)
    {
        if (scopes.TryPeek(out var scope))
        {
            if (scope.ContainsKey(name.Lexeme))
            {
                Lox.Error(name, "Already a variable with this name in this scope.");
            }

            scope[name.Lexeme] = false;
        }
    }

    private void Define(Token name)
    {
        if (scopes.TryPeek(out var scope))
        {
            scope[name.Lexeme] = true;
        }
    }

#endregion Helpers


#region Public methods

    public void Resolve(List<Stmt> statements) => statements.ForEach(Resolve);

#endregion Public methods


#region Stmt.IVisitor

    public Unit VisitBlockStmt(Stmt.Block stmt)
    {
        BeginScope();

        Resolve(stmt.Statements);

        EndScope();

        return Unit.Value;
    }

    public Unit VisitVarStmt(Stmt.Var stmt)
    {
        Declare(stmt.Name);

        if (stmt.Initializer is not null)
        {
            Resolve(stmt.Initializer);
        }

        Define(stmt.Name);

        return Unit.Value;
    }

    public Unit VisitFunctionStmt(Stmt.Function stmt)
    {
        Declare(stmt.Name);
        Define(stmt.Name);

        ResolveCallable(stmt.Params, stmt.Body, CallableType.FUNCTION);

        return Unit.Value;
    }

    public Unit VisitExpressionStmt(Stmt.Expression stmt)
    {
        Resolve(stmt.InnerExpression);

        return Unit.Value;
    }

    public Unit VisitIfStmt(Stmt.If stmt)
    {
        Resolve(stmt.Condition);
        Resolve(stmt.ThenBranch);
        if (stmt.ElseBranch is not null) Resolve(stmt.ElseBranch);

        return Unit.Value;
    }

    public Unit VisitPrintStmt(Stmt.Print stmt)
    {
        Resolve(stmt.InnerExpression);

        return Unit.Value;
    }

    public Unit VisitReturnStmt(Stmt.Return stmt)
    {
        if (currentCallable == CallableType.NONE)
        {
            Lox.Error(stmt.Keyword, "Can't return from top-level code.");
        }

        if (stmt.Value is not null) Resolve(stmt.Value);

        return Unit.Value;
    }

    public Unit VisitWhileStmt(Stmt.While stmt)
    {
        Resolve(stmt.Condition);
        Resolve(stmt.Body);

        return Unit.Value;
    }

    public Unit VisitBreakStmt(Stmt.Break stmt)
    {
        return Unit.Value;
    }

    #endregion Stmt.IVisitor


    #region Expr.IVisitor

    public Unit VisitTernaryExpr(Expr.Ternary expr)
    {
        Resolve(expr.Left);
        Resolve(expr.Middle);
        Resolve(expr.Right);

        return Unit.Value;
    }

    public Unit VisitVariableExpr(Expr.Variable expr)
    {
        if (scopes.TryPeek(out var scope) && scope.TryGetValue(expr.Name.Lexeme, out var defined) && !defined)
        {
            Lox.Error(expr.Name, "Can't read local variable in its own initializer.");
        }

        ResolveLocal(expr, expr.Name);

        return Unit.Value;
    }

    public Unit VisitAssignExpr(Expr.Assign expr)
    {
        Resolve(expr.Value);
        ResolveLocal(expr, expr.Name);

        return Unit.Value;
    }

    public Unit VisitBinaryExpr(Expr.Binary expr)
    {
        Resolve(expr.Left);
        Resolve(expr.Right);

        return Unit.Value;
    }

    public Unit VisitCallExpr(Expr.Call expr)
    {
        Resolve(expr.Callee);
        expr.Arguments.ForEach(Resolve);

        return Unit.Value;
    }

    public Unit VisitGroupingExpr(Expr.Grouping expr)
    {
        Resolve(expr.Expression);

        return Unit.Value;
    }

    public Unit VisitLiteralExpr(Expr.Literal expr)
    {
        return Unit.Value;
    }

    public Unit VisitLogicalExpr(Expr.Logical expr)
    {
        Resolve(expr.Left);
        Resolve(expr.Right);

        return Unit.Value;
    }

    public Unit VisitUnaryExpr(Expr.Unary expr)
    {
        Resolve(expr.Expression);

        return Unit.Value;
    }

    public Unit VisitLambdaExpr(Expr.Lambda expr)
    {
        ResolveCallable(expr.Params, expr.Body, CallableType.LAMBDA);

        return Unit.Value;
    }

    #endregion Expr.IVisitor

}