namespace Lox;

public abstract record Expr
{
    public interface IVisitor<R>
    {
        R VisitTernaryExpr(Ternary expr);
        R VisitBinaryExpr(Binary expr);
        R VisitGroupingExpr(Grouping expr);
        R VisitLiteralExpr(Literal expr);
        R VisitUnaryExpr(Unary expr);
        R VisitAssignExpr(Assign expr);
        R VisitVariableExpr(Variable expr);
    }

    public record Ternary(Expr Left, Token LeftOperator, Expr Middle, Token RightOperator, Expr Right) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitTernaryExpr(this);
        }
    }

    public record Binary(Expr Left, Token Operator, Expr Right) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitBinaryExpr(this);
        }
    }

    public record Grouping(Expr Expression) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitGroupingExpr(this);
        }
    }

    public record Literal(object? Value) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitLiteralExpr(this);
        }
    }

    public record Unary(Token Operator, Expr Expression) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitUnaryExpr(this);
        }
    }

    public record Assign(Token Name, Expr Value) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitAssignExpr(this);
        }
    }

    public record Variable(Token Name) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitVariableExpr(this);
        }
    }

    public abstract R Accept<R>(IVisitor<R> visitor);
}
