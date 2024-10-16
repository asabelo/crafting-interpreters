using System.ComponentModel;
using static Lox.TokenType;

namespace Lox;

public class Parser
{
    private class ParseError : Exception {}

    private readonly List<Token> tokens;
    private int current = 0;

    public Parser(List<Token> tokens)
    {
        this.tokens = tokens;
    }

    public async Task<List<Stmt>> ParseAsync()
    {
        var statements = new List<Stmt>();
        
        while (!IsAtEnd())
        {
            var declaration = await DeclarationAsync();

            if (declaration is not null)
            {
                statements.Add(declaration);
            }
        }

        return statements;
    }

    private async Task<Stmt?> DeclarationAsync()
    {
        try
        {
            if (Match(VAR)) return await VarDeclarationAsync();

            return await StatementAsync();
        } 
        catch (ParseError)
        {
            Synchronize();

            return null;
        }
    }

    private async Task<Stmt> StatementAsync()
    {
        if (Match(IF))
        {
            return await IfStatementAsync();
        }
        if (Match(PRINT))
        {
            return await PrintStatementAsync();
        }
        else if (Match(LEFT_BRACE))
        {
            return new Stmt.Block(await BlockAsync());
        }

        return await ExpressionStatementAsync();
    }

    private async Task<List<Stmt>> BlockAsync()
    {
        List<Stmt> statements = [];

        while (!Check(RIGHT_BRACE) && !IsAtEnd())
        {
            var declaration = await DeclarationAsync();

            if (declaration is not null)
            {
                statements.Add(declaration);
            }
        }

        await ConsumeAsync(RIGHT_BRACE, "Expect '}' after block.");

        return statements;
    }

    private async Task<Stmt> IfStatementAsync()
    {
        await ConsumeAsync(LEFT_PAREN, "Expect '(' after 'if'.");

        var condition = await ExpressionAsync();

        await ConsumeAsync(RIGHT_PAREN, "Expect ')' after if condition.");

        var thenBranch = await StatementAsync();
        var elseBranch = Match(ELSE) ? await StatementAsync() : null;

        return new Stmt.If(condition, thenBranch, elseBranch);
    }

    private async Task<Stmt> PrintStatementAsync()
    {
        var value = await ExpressionAsync();

        await ConsumeAsync(SEMICOLON, "Expect ';' after value.");

        return new Stmt.Print(value);
    }

    private async Task<Stmt> VarDeclarationAsync()
    {
        var name = await ConsumeAsync(IDENTIFIER, "Expect variable name.");

        Expr? initializer = null;

        if (Match(EQUAL)) 
        {
            initializer = await ExpressionAsync();
        }

        await ConsumeAsync(SEMICOLON, "Expect ';' after variable declaration.");

        return new Stmt.Var(name, initializer);
    }

    private async Task<Stmt> ExpressionStatementAsync()
    {
        var expression = await ExpressionAsync();

        await ConsumeAsync(SEMICOLON, "Expect ';' after expression.");

        return new Stmt.Expression(expression);
    }

    private async Task<Expr> ExpressionAsync()
    {
        return await AssignmentAsync();
    }

    private async Task<Expr> AssignmentAsync()
    {
        var expr = await EqualityAsync();

        if (Match(EQUAL))
        {
            var equals = Previous();
            var value = await AssignmentAsync();

            if (expr is Expr.Variable varExpr)
            {
                var name = varExpr.name;

                return new Expr.Assign(name, value);
            }

            await ErrorAsync(equals, "Invalid assignment target.");
        }

        return expr;
    }

    private async Task<Expr> EqualityAsync()
    {
        Expr expr = await ComparisonAsync();

        while (Match(BANG_EQUAL, EQUAL_EQUAL))
        {
            Token @operator = Previous();
            Expr right = await ComparisonAsync();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private async Task<Expr> ComparisonAsync()
    {
        Expr expr = await TermAsync();

        while (Match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
        {
            Token @operator = Previous();
            Expr right = await TermAsync();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private async Task<Expr> TermAsync()
    {
        Expr expr = await FactorAsync();

        while (Match(MINUS, PLUS))
        {
            Token @operator = Previous();
            Expr right = await FactorAsync();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private async Task<Expr> FactorAsync()
    {
        Expr expr = await UnaryAsync();

        while (Match(SLASH, STAR))
        {
            Token @operator = Previous();
            Expr right = await UnaryAsync();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private async Task<Expr> UnaryAsync()
    {
        if (Match(BANG, MINUS))
        {
            Token @operator = Previous();
            Expr right = await UnaryAsync();
            return new Expr.Unary(@operator, right);
        }

        return await PrimaryAsync();
    }

    private async Task<Expr> PrimaryAsync()
    {
        if (Match(FALSE))
        {
            return new Expr.Literal(false);
        }
        else if (Match(TRUE))
        {
            return new Expr.Literal(true);
        }
        else if (Match(NIL))
        {
            return new Expr.Literal(null);
        }
        else if (Match(NUMBER, STRING))
        {
            return new Expr.Literal(Previous().Literal);
        }
        if (Match(IDENTIFIER))
        {
            return new Expr.Variable(Previous());
        }
        else if (Match(LEFT_PAREN))
        {
            Expr expr = await ExpressionAsync();
            await ConsumeAsync(RIGHT_PAREN, "Expect ')' after expression.");
            return new Expr.Grouping(expr);
        }

        throw await ErrorAsync(Peek(), "Expect expression.");
    }

    private bool Match(params TokenType[] types)
    {
        foreach (var type in types)
        {
            if (Check(type))
            {
                Advance();
                return true;
            }
        }

        return false;
    }

    private async Task<Token> ConsumeAsync(TokenType type, string message)
    {
        if (Check(type)) return Advance();

        throw await ErrorAsync(Peek(), message);
    }

    private bool Check(TokenType type)
    {
        if (IsAtEnd()) return false;

        return Peek().Type == type;
    }

    private Token Advance()
    {
        if (!IsAtEnd()) current++;
        return Previous();
    }

    private bool IsAtEnd()
    {
        return Peek().Type == EOF;
    }

    private Token Peek() 
    {
        return tokens[current];
    }

    private Token Previous() 
    {
        return tokens[current - 1];
    }

    private static async Task<ParseError> ErrorAsync(Token token, string message)
    {
        await Lox.ErrorAsync(token, message);

        return new ParseError();
    }

    private void Synchronize()
    {
        Advance();

        while (!IsAtEnd())
        {
            if (Previous().Type == SEMICOLON) return;

            switch (Peek().Type)
            {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
            }

            Advance();
        }
    }
}
