using System.Text;

namespace Lox;

public class AstPrinter : Expr.IVisitor<string>
{
    public string Print(Expr expr)
    {
        return expr.Accept(this);
    }

    public string VisitBinaryExpr(Expr.Binary expr)
    {
        return Parenthesize(expr.@operator.Lexeme, expr.left, expr.right);
    }

    public string VisitGroupingExpr(Expr.Grouping expr)
    {
        return Parenthesize("group", expr.expression);
    }

    public string VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.value?.ToString() ?? "nil";
    }

    public string VisitUnaryExpr(Expr.Unary expr)
    {
        return Parenthesize(expr.@operator.Lexeme, expr.right);
    }

    private string Parenthesize(string name, params Expr[] exprs)
    {
        var builder = new StringBuilder();

        builder.Append('(').Append(name);
        foreach (var expr in exprs)
        {
            builder.Append(' ').Append(expr.Accept(this));
        }
        builder.Append(')');

        return builder.ToString();
    }
}
