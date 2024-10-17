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
            if (Match(FUN)) return await FunctionAsync("function");

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
        if (Match(PRINT))
        {
            return await PrintStatementAsync();
        }
        else if (Match(LEFT_BRACE))
        {
            return new Stmt.Block(await BlockAsync());
        }
        else if (Match(IF))
        {
            return await IfStatementAsync();
        }
        else if (Match(WHILE))
        {
            return await WhileStatementAsync();
        }
        else if (Match(FOR))
        {
            return await ForStatementAsync();
        }
        else if (Match(RETURN))
        {
            return await ReturnStatementAsync();
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

    private async Task<Stmt> ReturnStatementAsync()
    {
        var keyword = Previous();

        var value = Check(SEMICOLON) ? null : await ExpressionAsync();

        await ConsumeAsync(SEMICOLON, "Expect ';' after return value.");

        return new Stmt.Return(keyword, value);
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

    private async Task<Stmt> WhileStatementAsync()
    {
        await ConsumeAsync(LEFT_PAREN, "Expect '(' after 'while'.");

        var condition = await ExpressionAsync();

        await ConsumeAsync(RIGHT_PAREN, "Expect ')' after condition.");

        var body = await StatementAsync();

        return new Stmt.While(condition, body);
    }

    private async Task<Stmt> ForStatementAsync()
    {
        await ConsumeAsync(LEFT_PAREN, "Expect '(' after 'for'.");

        Stmt? initializer;
        if (Match(SEMICOLON)) initializer = null;
        else if (Match(VAR))  initializer = await VarDeclarationAsync();
        else                  initializer = await ExpressionStatementAsync();
        
        Expr? condition = null;
        if (!Check(SEMICOLON)) condition = await ExpressionAsync();
        await ConsumeAsync(SEMICOLON, "Expect ';' after loop condition.");

        Expr? increment = null;
        if (!Check(RIGHT_PAREN)) increment = await ExpressionAsync();
        await ConsumeAsync(RIGHT_PAREN, "Expect ')' after for clauses.");

        Stmt body = await StatementAsync();

        if (increment is not null)
        {
            body = new Stmt.Block([body, new Stmt.Expression(increment)]);
        }

        body = new Stmt.While(condition ?? new Expr.Literal(true), body);

        if (initializer is not null)
        {
            body = new Stmt.Block([initializer, body]);
        }

        return body;
    }

    private async Task<Stmt> ExpressionStatementAsync()
    {
        var expression = await ExpressionAsync();

        await ConsumeAsync(SEMICOLON, "Expect ';' after expression.");

        return new Stmt.Expression(expression);
    }

    private async Task<Stmt.Function> FunctionAsync(string kind)
    {
        var name = await ConsumeAsync(IDENTIFIER, $"Expect {kind} name.");

        await ConsumeAsync(LEFT_PAREN, $"Expect '(' after {kind} name.");

        var parameters = new List<Token>();

        if (!Check(RIGHT_PAREN))
        {
            do
            {
                if (parameters.Count > 255)
                {
                    _ = await ErrorAsync(Peek(), "Can't have more than 255 parameters.");
                }

                parameters.Add(await ConsumeAsync(IDENTIFIER, "Expect paramter name."));
            }
            while (Match(COMMA));
        }

        await ConsumeAsync(RIGHT_PAREN, "Expect ')' after parameters.");
        await ConsumeAsync(LEFT_BRACE, $"Expect '{{' before {kind} body.");

        var body = await BlockAsync();

        return new Stmt.Function(name, parameters, body);
    }

    private async Task<Expr> ExpressionAsync()
    {
        return await AssignmentAsync();
    }

    private async Task<Expr> AssignmentAsync()
    {
        var expr = await OrAsync();

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

    private async Task<Expr> OrAsync()
    {
        var expr = await AndAsync();

        while (Match(OR))
        {
            var @operator = Previous();
            var right = await AndAsync();
            expr = new Expr.Logical(expr, @operator, right);
        }

        return expr;
    }

    private async Task<Expr> AndAsync()
    {
        var expr = await EqualityAsync();

        while (Match(AND))
        {
            var @operator = Previous();
            var right = await EqualityAsync();
            expr = new Expr.Logical(expr, @operator, right);
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

        return await CallAsync();
    }

    private async Task<Expr> FinishCall(Expr callee)
    {
        var arguments = new List<Expr>();

        if (!Check(RIGHT_PAREN))
        {
            do
            {
                if (arguments.Count >= 255)
                {
                    _ = await ErrorAsync(Peek(), "Can't have more than 255 arguments.");
                }

                arguments.Add(await ExpressionAsync());
            }
            while (Match(COMMA));
        }

        var paren = await ConsumeAsync(RIGHT_PAREN, "Expect ')' after arguments.");

        return new Expr.Call(callee, paren, arguments);
    }

    private async Task<Expr> CallAsync()
    {
        var expr = await PrimaryAsync();

        while (true)
        {
            if (Match(LEFT_PAREN))
            {
                expr = await FinishCall(expr);
            }
            else
            {
                break;
            }
        }

        return expr;
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
