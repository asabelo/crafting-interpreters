namespace Lox;

public abstract class Expr
{
    public interface IVisitor<R>
    {
        R VisitAssignExpr(Assign expr);

        R VisitBinaryExpr(Binary expr);

        R VisitCallExpr(Call expr);

        R VisitGetExpr(Get expr);

        R VisitGroupingExpr(Grouping expr);

        R VisitLiteralExpr(Literal expr);

        R VisitLogicalExpr(Logical expr);

        R VisitSetExpr(Set expr);

        R VisitThisExpr(This expr);

        R VisitUnaryExpr(Unary expr);

        R VisitVariableExpr(Variable expr);
    }

    public class Assign : Expr 
    {
        public Assign(Token name, Expr value)
        {
            this.name = name;
            this.value = value;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitAssignExpr(this);
        }

        public readonly Token name;
        public readonly Expr value;
    }

    public class Binary : Expr 
    {
        public Binary(Expr left, Token @operator, Expr right)
        {
            this.left = left;
            this.@operator = @operator;
            this.right = right;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitBinaryExpr(this);
        }

        public readonly Expr left;
        public readonly Token @operator;
        public readonly Expr right;
    }

    public class Call : Expr 
    {
        public Call(Expr Callee, Token Paren, List<Expr> Arguments)
        {
            this.Callee = Callee;
            this.Paren = Paren;
            this.Arguments = Arguments;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitCallExpr(this);
        }

        public readonly Expr Callee;
        public readonly Token Paren;
        public readonly List<Expr> Arguments;
    }

    public class Get : Expr 
    {
        public Get(Expr Object, Token Name)
        {
            this.Object = Object;
            this.Name = Name;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitGetExpr(this);
        }

        public readonly Expr Object;
        public readonly Token Name;
    }

    public class Grouping : Expr 
    {
        public Grouping(Expr expression)
        {
            this.expression = expression;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitGroupingExpr(this);
        }

        public readonly Expr expression;
    }

    public class Literal : Expr 
    {
        public Literal(object? value)
        {
            this.value = value;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitLiteralExpr(this);
        }

        public readonly object? value;
    }

    public class Logical : Expr 
    {
        public Logical(Expr left, Token @operator, Expr right)
        {
            this.left = left;
            this.@operator = @operator;
            this.right = right;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitLogicalExpr(this);
        }

        public readonly Expr left;
        public readonly Token @operator;
        public readonly Expr right;
    }

    public class Set : Expr 
    {
        public Set(Expr Object, Token Name, Expr Value)
        {
            this.Object = Object;
            this.Name = Name;
            this.Value = Value;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitSetExpr(this);
        }

        public readonly Expr Object;
        public readonly Token Name;
        public readonly Expr Value;
    }

    public class This : Expr 
    {
        public This(Token Keyword)
        {
            this.Keyword = Keyword;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitThisExpr(this);
        }

        public readonly Token Keyword;
    }

    public class Unary : Expr 
    {
        public Unary(Token @operator, Expr right)
        {
            this.@operator = @operator;
            this.right = right;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitUnaryExpr(this);
        }

        public readonly Token @operator;
        public readonly Expr right;
    }

    public class Variable : Expr 
    {
        public Variable(Token name)
        {
            this.name = name;
        }

        public override R Accept<R>(IVisitor<R> visitor)
        {
            return visitor.VisitVariableExpr(this);
        }

        public readonly Token name;
    }

    public abstract R Accept<R>(IVisitor<R> visitor);
};
