namespace Lox;

public abstract record Expr
{
    public interface IVisitor<R>
    {
        R VisitTernaryExpr(Ternary expr);
        R VisitBinaryExpr(Binary expr);
        R VisitCallExpr(Call expr);
        R VisitGetExpr(Get expr);
        R VisitGroupingExpr(Grouping expr);
        R VisitLiteralExpr(Literal expr);
        R VisitSetExpr(Set expr);
        R VisitThisExpr(This expr);
        R VisitUnaryExpr(Unary expr);
        R VisitAssignExpr(Assign expr);
        R VisitLogicalExpr(Logical expr);
        R VisitVariableExpr(Variable expr);
        R VisitLambdaExpr(Lambda expr);
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

    public record Call(Expr Callee, Token Paren, List<Expr> Arguments) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitCallExpr(this);
        }
    }

    public record Get(Expr Object, Token Name) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitGetExpr(this);
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

    public record Set(Expr Object, Token Name, Expr Value) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitSetExpr(this);
        }
    }

    public record This(Token Keyword) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitThisExpr(this);
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

    public record Logical(Expr Left, Token Operator, Expr Right) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitLogicalExpr(this);
        }
    }

    public record Variable(Token Name) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitVariableExpr(this);
        }
    }

    public record Lambda(List<Token> Params, List<Stmt> Body) : Expr
    {
        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitLambdaExpr(this);
        }
    }

    public abstract R Accept<R>(IVisitor<R> visitor);
}
