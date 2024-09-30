using System.Runtime.CompilerServices;

namespace Lox;

class Scanner
{
    private readonly string source;
    private readonly List<Token> tokens = [];

    private int start = 0;
    private int current = 0;
    private int line = 1;

    Scanner(string source)
    {
        this.source = source;
    }

    private async Task<List<Token>> ScanTokens()
    {
        while (!IsAtEnd())
        {
            start = current;
            await ScanToken();
        }

        tokens.Add(new Token(TokenType.EOF, string.Empty, null, line));
        return tokens;
    }

    private async Task ScanToken()
    {
        switch (Advance())
        {
            case '(':
                AddToken(TokenType.LEFT_PAREN);
                break;

            case ')':
                AddToken(TokenType.RIGHT_PAREN);
                break;

            case '{':
                AddToken(TokenType.LEFT_BRACE);
                break;

            case '}':
                AddToken(TokenType.RIGHT_BRACE);
                break;

            case ',':
                AddToken(TokenType.COMMA);
                break;

            case '.':
                AddToken(TokenType.DOT);
                break;

            case '-':
                AddToken(TokenType.MINUS);
                break;

            case '+':
                AddToken(TokenType.PLUS);
                break;

            case ';':
                AddToken(TokenType.SEMICOLON);
                break;

            case '*':
                AddToken(TokenType.STAR);
                break;

            case '!':
                AddToken(Match('=') ? TokenType.BANG_EQUAL : TokenType.BANG);
                break;

            case '=':
                AddToken(Match('=') ? TokenType.EQUAL_EQUAL : TokenType.EQUAL);
                break;

            case '<':
                AddToken(Match('=') ? TokenType.LESS_EQUAL : TokenType.LESS);
                break;

            case '>':
                AddToken(Match('=') ? TokenType.GREATER_EQUAL : TokenType.GREATER);
                break;

            default:
                await Lox.ErrorAsync(line, "Unexpected character.");
                break;
        }
    }

    // Advance and return true if the next character from source equals expected.
    // Return false otherwise.
    private bool Match(char expected)
    {
        if (IsAtEnd() || source[current] != expected)
        {
            return false;
        }
        else
        {
            current++;
            return true;
        }
    }

    private bool IsAtEnd()
    {
        return current >= source.Length;
    }

    private char Advance()
    {
        return source[current++];
    }

    private void AddToken(TokenType type)
    {
        AddToken(type, null);
    }

    private void AddToken(TokenType type, object? literal)
    {
        var text = source[start..current];
        tokens.Add(new Token(type, text, literal, line));
    }
}