namespace Lox;

public class Resolver(Interpreter interpreter) : Stmt.IVisitor<Unit>, Expr.IVisitor<Unit>
{
    private readonly Interpreter interpreter = interpreter;

    private readonly Stack<Dictionary<string, bool>> scopes = new();

    private FunctionType currentFunction = FunctionType.NONE;

    private ClassType currentClass = ClassType.NONE;

    private enum FunctionType
    {
        NONE, FUNCTION, METHOD, INITIALIZER
    }

    private enum ClassType
    {
        NONE, CLASS, SUBCLASS
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

    private void ResolveFunction(Stmt.Function function, FunctionType type)
    {
        var enclosingFunction = currentFunction;
        currentFunction = type;

        BeginScope();

        foreach (var param in function.Params)
        {
            Declare(param);
            Define(param);
        }

        Resolve(function.Body);

        EndScope();

        currentFunction = enclosingFunction;
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

        Resolve(stmt.statements);

        EndScope();

        return Unit.Value;
    }

    public Unit VisitClassStmt(Stmt.Class stmt)
    {
        var enclosingClass = currentClass;
        currentClass = ClassType.CLASS;

        Declare(stmt.Name);
        Define(stmt.Name);

        if (stmt.Superclass is not null)
        {
            if (stmt.Name.Lexeme == stmt.Superclass.name.Lexeme)
            {
                Lox.Error(stmt.Superclass.name, "A class can't inherit from itself.");
            }

            currentClass = ClassType.SUBCLASS;
            Resolve(stmt.Superclass);
            
            BeginScope();
            scopes.Peek()["super"] = true;
        }

        BeginScope();
        scopes.Peek()["this"] = true;

        foreach (var method in stmt.Methods)
        {
            var declaration = method.Name.Lexeme == "init" ? FunctionType.INITIALIZER : FunctionType.METHOD;

            ResolveFunction(method, declaration);
        }

        EndScope();

        if (stmt.Superclass is not null)
        {
            EndScope();
        }

        currentClass = enclosingClass;

        return Unit.Value;
    }

    public Unit VisitVarStmt(Stmt.Var stmt)
    {
        Declare(stmt.name);

        if (stmt.initializer is not null)
        {
            Resolve(stmt.initializer);
        }

        Define(stmt.name);

        return Unit.Value;
    }

    public Unit VisitFunctionStmt(Stmt.Function stmt)
    {
        Declare(stmt.Name);
        Define(stmt.Name);

        ResolveFunction(stmt, FunctionType.FUNCTION);

        return Unit.Value;
    }

    public Unit VisitExpressionStmt(Stmt.Expression stmt)
    {
        Resolve(stmt.expression);

        return Unit.Value;
    }

    public Unit VisitIfStmt(Stmt.If stmt)
    {
        Resolve(stmt.condition);
        Resolve(stmt.thenBranch);
        if (stmt.elseBranch is not null) Resolve(stmt.elseBranch);

        return Unit.Value;
    }

    public Unit VisitPrintStmt(Stmt.Print stmt)
    {
        Resolve(stmt.expression);

        return Unit.Value;
    }

    public Unit VisitReturnStmt(Stmt.Return stmt)
    {
        if (currentFunction == FunctionType.NONE)
        {
            Lox.Error(stmt.Keyword, "Can't return from top-level code.");
        }

        if (stmt.Value is not null)
        {
            if (currentFunction == FunctionType.INITIALIZER)
            {
                Lox.Error(stmt.Keyword, "Can't return a value from an initializer.");
            }
            
            Resolve(stmt.Value);
        }

        return Unit.Value;
    }

    public Unit VisitWhileStmt(Stmt.While stmt)
    {
        Resolve(stmt.condition);
        Resolve(stmt.body);

        return Unit.Value;
    }

    #endregion Stmt.IVisitor


    #region Expr.IVisitor

    public Unit VisitVariableExpr(Expr.Variable expr)
    {
        if (scopes.TryPeek(out var scope) && scope.TryGetValue(expr.name.Lexeme, out var defined) && !defined)
        {
            Lox.Error(expr.name, "Can't read local variable in its own initializer.");
        }

        ResolveLocal(expr, expr.name);

        return Unit.Value;
    }

    public Unit VisitAssignExpr(Expr.Assign expr)
    {
        Resolve(expr.value);
        ResolveLocal(expr, expr.name);

        return Unit.Value;
    }

    public Unit VisitBinaryExpr(Expr.Binary expr)
    {
        Resolve(expr.left);
        Resolve(expr.right);

        return Unit.Value;
    }

    public Unit VisitCallExpr(Expr.Call expr)
    {
        Resolve(expr.Callee);
        expr.Arguments.ForEach(Resolve);

        return Unit.Value;
    }

    public Unit VisitGetExpr(Expr.Get expr)
    {
        Resolve(expr.Object);

        return Unit.Value;
    }

    public Unit VisitGroupingExpr(Expr.Grouping expr)
    {
        Resolve(expr.expression);

        return Unit.Value;
    }

    public Unit VisitLiteralExpr(Expr.Literal expr)
    {
        return Unit.Value;
    }

    public Unit VisitLogicalExpr(Expr.Logical expr)
    {
        Resolve(expr.left);
        Resolve(expr.right);

        return Unit.Value;
    }

    public Unit VisitSetExpr(Expr.Set expr)
    {
        Resolve(expr.Value);
        Resolve(expr.Object);

        return Unit.Value;
    }

    public Unit VisitSuperExpr(Expr.Super expr)
    {
        if (currentClass == ClassType.NONE)
        {
            Lox.Error(expr.Keyword, "Can't use 'super' outside a class.");
        }
        else if (currentClass != ClassType.SUBCLASS)
        {
            Lox.Error(expr.Keyword, "Can't use 'super' in a class with no superclass.");
        }

        ResolveLocal(expr, expr.Keyword);

        return Unit.Value;
    }

    public Unit VisitThisExpr(Expr.This expr)
    {
        if (currentClass == ClassType.NONE)
        {
            Lox.Error(expr.Keyword, "Can't use 'this' outside a class.");
        }
        else
        {
            ResolveLocal(expr, expr.Keyword);
        }

        return Unit.Value;
    }

    public Unit VisitUnaryExpr(Expr.Unary expr)
    {
        Resolve(expr.right);

        return Unit.Value;
    }

    #endregion Expr.IVisitor

}