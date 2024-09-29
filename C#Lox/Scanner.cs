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

    List<Token> ScanTokens()
    {
        while (!IsAtEnd())
        {
            start = current;
            ScanToken();
        }

        tokens.Add(new Token(TokenType.EOF, string.Empty, null, line));
        return tokens;
    }

    private void ScanToken()
    {
        char c = Advance();

        var tokenType = c switch
        {
            '(' => TokenType.LEFT_PAREN,
            ')' => TokenType.RIGHT_PAREN,
            '{' => TokenType.LEFT_BRACE,
            '}' => TokenType.RIGHT_BRACE,
            ',' => TokenType.COMMA,
            '.' => TokenType.DOT,
            '-' => TokenType.MINUS,
            '+' => TokenType.PLUS,
            ';' => TokenType.SEMICOLON,
            '*' => TokenType.STAR
        };

        AddToken(tokenType);
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