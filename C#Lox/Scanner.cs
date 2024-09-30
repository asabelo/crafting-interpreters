using System.Runtime.CompilerServices;
using System.Security.Cryptography;

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
        char c = Advance();

        switch (c)
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

            case '/':
                if (Match('/'))
                {
                    while (Peek() != '\n' && !IsAtEnd())
                    {
                        _ = Advance();
                    }
                }
                else
                {
                    AddToken(TokenType.SLASH);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
                // Ignore
                break;

            case '\n':
                line++;
                break;

            case '"':
                await ScanString();
                break;

            default:
                if (IsDigit(c))
                {
                    ScanNumber();
                }
                else
                {
                    await Lox.ErrorAsync(line, "Unexpected character.");
                }
                break;
        }
    }

    private void ScanNumber()
    {
        while (IsDigit(Peek()))
        {
            _ = Advance();
        }

        // Look for a fractional part
        if (Peek() == '.' && IsDigit(PeekNext()))
        {
            _ = Advance(); // consume the '.'

            while (IsDigit(Peek()))
            {
                _ = Advance();
            }
        }

        var doubleLiteral = double.Parse(source[start..current]);

        AddToken(TokenType.NUMBER, doubleLiteral);
    }

    private static bool IsDigit(char c)
    {
        return c is >= '0' and <= '9';
    }

    private async Task ScanString()
    {
        while (Peek() != '"' && !IsAtEnd())
        {
            if (Peek() == '\n')
            {
                line++;
            }

            _ = Advance();
        }

        if (IsAtEnd())
        {
            await Lox.ErrorAsync(line, "Unterminated string.");
            return;
        }

        _ = Advance(); // Closing '"'

        var stringLiteral = source[(start + 1)..(current - 1)];

        AddToken(TokenType.STRING, stringLiteral);
    }

    private char Peek()
    {
        return IsAtEnd() ? '\0' : source[current];
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