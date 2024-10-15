using static Lox.TokenType;

namespace Lox;

public class Parser(List<Token> tokens)
{
    private class ParseError : Exception {}

    private List<Token> Tokens { get; } = tokens;

    private int current = 0;

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
        return await CommaAsync();
    }

    private async Task<Expr> BinaryAsync(Func<Task<Expr>> operand, params TokenType[] operators)
    {
        var left = await operand();

        while (Match(operators))
        {
            var op = Previous();
            var right = await operand();
            left = new Expr.Binary(left, op, right);
        }

        return left;
    }

    private Task<Expr> CommaAsync() => BinaryAsync(AssignmentAsync, COMMA);

    private async Task<Expr> AssignmentAsync()
    {
        var expr = await TernaryAsync();

        if (Match(EQUAL))
        {
            var equals = Previous();
            var value = await AssignmentAsync();

            if (expr is Expr.Variable varExpr)
            {
                var name = varExpr.Name;

                return new Expr.Assign(name, value);
            }

            await ErrorAsync(equals, "Invalid assignment target.");
        }

        return expr;
    }

    private async Task<Expr> TernaryAsync()
    {
        var left = await EqualityAsync();
        
        if (Match(QUESTION_MARK))
        {
            var leftOp = Previous();

            var middle = await EqualityAsync();

            var rightOp = await ConsumeAsync(COLON, "Expect ':'.");

            var right = await TernaryAsync();

            left = new Expr.Ternary(left, leftOp, middle, rightOp, right);
        }

        return left;
    }

    private Task<Expr> EqualityAsync() => BinaryAsync(ComparisonAsync, BANG_EQUAL, EQUAL_EQUAL);

    private Task<Expr> ComparisonAsync() => BinaryAsync(TermAsync, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL);

    private Task<Expr> TermAsync() => BinaryAsync(FactorAsync, MINUS, PLUS);

    private Task<Expr> FactorAsync() => BinaryAsync(UnaryAsync, SLASH, STAR);

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
        return Tokens[current];
    }

    private Token Previous() 
    {
        return Tokens[current - 1];
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
