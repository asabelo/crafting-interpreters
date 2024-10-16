namespace Lox;

public abstract record Stmt
{
    public interface IVisitor<R>
    {
        R VisitBlockStmt(Block stmt);
        R VisitExpressionStmt(Expression stmt);
        R VisitPrintStmt(Print stmt);
        R VisitVarStmt(Var stmt);
        R VisitIfStmt(If stmt);
        R VisitWhileStmt(While stmt);
    }

    public record Block(List<Stmt> Statements) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitBlockStmt(this);
        }
    }

    public record Expression(Expr InnerExpression) : Stmt
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitExpressionStmt(this);
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

    public abstract R Accept<R>(IVisitor<R> visitor);
}
