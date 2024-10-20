namespace Lox;

public abstract record Stmt
{
    public interface IVisitor<R>
    {
        R VisitBlockStmt(Block stmt);
        R VisitClassStmt(Class stmt);
        R VisitExpressionStmt(Expression stmt);
        R VisitFunctionStmt(Function stmt);
        R VisitPrintStmt(Print stmt);
        R VisitVarStmt(Var stmt);
        R VisitReturnStmt(Return stmt);
        R VisitIfStmt(If stmt);
        R VisitWhileStmt(While stmt);
        R VisitBreakStmt(Break stmt);
    }

    public record Block(List<Stmt> Statements) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitBlockStmt(this);
        }
    }

    public record Class(Token Name, Expr.Variable? Superclass, List<Function> ClassMethods, List<Function> InstanceMethods) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitClassStmt(this);
        }
    }

    public record Expression(Expr InnerExpression) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitExpressionStmt(this);
        }
    }

    public record Function(Token Name, List<Token> Params, List<Stmt> Body) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitFunctionStmt(this);
        }
    }

    public record Print(Expr InnerExpression) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitPrintStmt(this);
        }
    }

    public record Var(Token Name, Expr? Initializer) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitVarStmt(this);
        }
    }

    public record Return(Token Keyword, Expr? Value) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitReturnStmt(this);
        }
    }

    public record If(Expr Condition, Stmt ThenBranch, Stmt? ElseBranch) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitIfStmt(this);
        }
    }

    public record While(Expr Condition, Stmt Body) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitWhileStmt(this);
        }
    }

    public record Break(Token Keyword) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitBreakStmt(this);
        }
    }

    public abstract R Accept<R>(IVisitor<R> visitor);
}
