using System.Text;

namespace Lox;

public class AstPrinter : Expr.IVisitor<string>
{
    public string Print(Expr expr)
    {
        return expr.Accept(this);
    }

    public string VisitTernaryExpr(Expr.Ternary expr)
    {
        return Parenthesize($"{expr.LeftOperator.Lexeme}{expr.RightOperator.Lexeme}", expr.Left, expr.Middle, expr.Right);
    }

    public string VisitAssignExpr(Expr.Assign expr)
    {
        throw new NotImplementedException();
    }

    public string VisitBinaryExpr(Expr.Binary expr)
    {
        return Parenthesize(expr.Operator.Lexeme, expr.Left, expr.Right);
    }

    public string VisitCallExpr(Expr.Call expr)
    {
        throw new NotImplementedException();
    }

    public string VisitGroupingExpr(Expr.Grouping expr)
    {
        return Parenthesize("group", expr.Expression);
    }

    public string VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.Value?.ToString() ?? "nil";
    }

    public string VisitLogicalExpr(Expr.Logical expr)
    {
        return Parenthesize(expr.Operator.Lexeme, expr.Left, expr.Right);
    }

    public string VisitUnaryExpr(Expr.Unary expr)
    {
        return Parenthesize(expr.Operator.Lexeme, expr.Expression);
    }

    public string VisitVariableExpr(Expr.Variable expr)
    {
        throw new NotImplementedException();
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

    public string VisitLambdaExpr(Expr.Lambda expr)
    {
        throw new NotImplementedException();
    }
}
