using static Lox.TokenType;

namespace Lox;

public class Parser(List<Token> tokens, bool fromPrompt)
{
    private class ParseError : Exception {}

    private List<Token> Tokens { get; } = tokens;

    private bool FromPrompt { get; } = fromPrompt;

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
            if (Check(FUN))
            {
                if (CheckNext(IDENTIFIER))
                {
                    await ConsumeAsync(FUN, string.Empty);
                    
                    return await FunctionAsync("function");
                }
                else
                {
                    return await ExpressionStatementAsync();
                }
            }
            
            if (Match(CLASS)) return await ClassDeclarationAsync();

            if (Match(VAR)) return await VarDeclarationAsync();

            return await StatementAsync();
        } 
        catch (ParseError)
        {
            Synchronize();

            return null;
        }
    }

    private async Task<Stmt> ClassDeclarationAsync()
    {
        var name = await ConsumeAsync(IDENTIFIER, "Expect class name.");

        Expr.Variable? superclass = null;
        if (Match(LESS)) 
        {
            await ConsumeAsync(IDENTIFIER, "Expect superclass name.");

            superclass = new Expr.Variable(Previous());
        }

        await ConsumeAsync(LEFT_BRACE, "Expect '{' before class body.");

        var classMethods = new List<Stmt.Function>();
        var instanceMethods = new List<Stmt.Function>();

        while (!Check(RIGHT_BRACE) && !IsAtEnd())
        {
            if (Match(CLASS))
            {
                classMethods.Add(await FunctionAsync("class method"));
            }
            else
            {
                instanceMethods.Add(await FunctionAsync("instance method"));
            }
        }

        await ConsumeAsync(RIGHT_BRACE, "Expect '}' after class body.");

        return new Stmt.Class(name, superclass, classMethods, instanceMethods);
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
        else if (Match(BREAK))
        {
            return await BreakStatementAsync(); 
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

    private async Task<Stmt> BreakStatementAsync()
    {
        var keyword = Previous();

        await ConsumeAsync(SEMICOLON, "Expect ';' after 'break'.");
        
        return new Stmt.Break(keyword);
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

        if (FromPrompt && !Check(SEMICOLON))
        {
            if (!IsAtEnd())
            {
                throw await ErrorAsync(Peek(), "Expect single expression.");
            }

            return new Stmt.Print(expression);
        }
        else
        {
            await ConsumeAsync(SEMICOLON, "Expect ';' after expression.");

            return new Stmt.Expression(expression);
        }
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
        return await CommaAsync();
    }

    private async Task<Expr> LeftAssocTwoOpsAsync(Func<Expr, Token, Expr, Expr> expression, Func<Task<Expr>> operand, params TokenType[] operators)
    {
        var left = await operand();

        while (Match(operators))
        {
            var op = Previous();
            var right = await operand();
            left = expression(left, op, right);
        }

        return left;
    }

    private Task<Expr> BinaryAsync(Func<Task<Expr>> operand, params TokenType[] operators) => LeftAssocTwoOpsAsync((l, o, r) => new Expr.Binary(l, o, r), operand, operators);

    private Task<Expr> LogicalAsync(Func<Task<Expr>> operand, params TokenType[] operators) => LeftAssocTwoOpsAsync((l, o, r) => new Expr.Logical(l, o, r), operand, operators);

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
                return new Expr.Assign(varExpr.Name, value);
            }
            else if (expr is Expr.Get getExpr)
            {
                return new Expr.Set(getExpr.Object, getExpr.Name, value);
            }

            throw await ErrorAsync(equals, "Invalid assignment target.");
        }

        return expr;
    }

    private async Task<Expr> TernaryAsync()
    {
        var left = await OrAsync();
        
        if (Match(QUESTION_MARK))
        {
            var leftOp = Previous();

            var middle = await OrAsync();

            var rightOp = await ConsumeAsync(COLON, "Expect ':'.");

            var right = await TernaryAsync();

            left = new Expr.Ternary(left, leftOp, middle, rightOp, right);
        }

        return left;
    }

    private Task<Expr> OrAsync() => LogicalAsync(AndAsync, OR);

    private Task<Expr> AndAsync() => LogicalAsync(EqualityAsync, AND);

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

        return await CallAsync();
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
            else if (Match(DOT))
            {
                var name = await ConsumeAsync(IDENTIFIER, "Expect property name after '.'.");

                expr = new Expr.Get(expr, name);
            }
            else
            {
                break;
            }
        }

        return expr;
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
        else if (Match(FUN))
        {
            return await LambdaAsync();
        }
        else if (Match(SUPER))
        {
            var keyword = Previous();
            await ConsumeAsync(DOT, "Expect '.' after 'super'.");
            var method = await ConsumeAsync(IDENTIFIER, "Expect superclass method name.");
            return new Expr.Super(keyword, method);
        }
        else if (Match(THIS))
        {
            return new Expr.This(Previous());
        }
        else if (Match(IDENTIFIER))
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

    private async Task<Expr> LambdaAsync()
    {
        await ConsumeAsync(LEFT_PAREN, $"Expect '(' after 'fun'.");

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
        await ConsumeAsync(LEFT_BRACE, $"Expect '{{' before lambda body.");

        var body = await BlockAsync();

        return new Expr.Lambda(parameters, body);
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

    private bool CheckNext(TokenType type)
    {
        if (IsAtEnd()) return false;

        return PeekNext().Type == type;
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

    private Token PeekNext()
    {
        if (current + 1 >= Tokens.Count)
        {
            return Peek();
        }
        else
        {
            return Tokens[current + 1];
        }
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
