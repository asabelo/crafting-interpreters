using System.Globalization;

namespace Lox;

public class Scanner
{
    private static readonly Dictionary<string, TokenType> keywords = new()
    {
        { "and",    TokenType.AND    },
        { "class",  TokenType.CLASS  },
        { "else",   TokenType.ELSE   },
        { "false",  TokenType.FALSE  },
        { "for",    TokenType.FOR    },
        { "fun",    TokenType.FUN    },
        { "if",     TokenType.IF     },
        { "nil",    TokenType.NIL    },
        { "or",     TokenType.OR     },
        { "print",  TokenType.PRINT  },
        { "return", TokenType.RETURN },
        { "super",  TokenType.SUPER  },
        { "this",   TokenType.THIS   },
        { "true",   TokenType.TRUE   },
        { "var",    TokenType.VAR    },
        { "while",  TokenType.WHILE  },
        { "break",  TokenType.BREAK  }
    };
    
    private readonly string source;
    private readonly List<Token> tokens = [];

    private int start = 0;
    private int current = 0;
    private int line = 1;

    public Scanner(string source)
    {
        this.source = source;
    }

    public async Task<List<Token>> ScanTokensAsync()
    {
        while (!IsAtEnd())
        {
            start = current;
            await ScanTokenAsync();
        }

        tokens.Add(new Token(TokenType.EOF, string.Empty, null, line));
        return tokens;
    }

    private async Task ScanTokenAsync()
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

            case '?':
                AddToken(TokenType.QUESTION_MARK);
                break;

            case ':':
                AddToken(TokenType.COLON);
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
                else if (Match('*'))
                {
                    while (Peek() != '*' && PeekNext() != '/')
                    {
                        if (Advance() == '\n')
                        {
                            line++;
                        }
                    }

                    // Discard '*' and '/'
                    _ = Advance();
                    _ = Advance();
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
                await ScanStringAsync();
                break;

            default:
                if (IsDigit(c))
                {
                    ScanNumber();
                }
                else if (IsAlpha(c))
                {
                    ScanIdentifier();
                }
                else
                {
                    await Lox.ErrorAsync(line, "Unexpected character.");
                }
                break;
        }
    }

    private void ScanIdentifier()
    {
        while (IsAlphanumeric(Peek()))
        {
            _ = Advance();
        }

        string text = source[start..current];
        
        if (!keywords.TryGetValue(text, out TokenType type))
        {
            type = TokenType.IDENTIFIER;
        }

        AddToken(type);
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

        var doubleLiteral = double.Parse(source[start..current], CultureInfo.InvariantCulture);

        AddToken(TokenType.NUMBER, doubleLiteral);
    }

    private static bool IsDigit(char c)
    {
        return c is >= '0' and <= '9';
    }

    private async Task ScanStringAsync()
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

    private char PeekNext()
    {
        if (current + 1 >= source.Length)
        {
            return '\0';
        }
        else
        {
            return source[current + 1];
        }
    }

    private static bool IsAlpha(char c)
    {
        return (c is >= 'a' and <= 'z')
            || (c is >= 'A' and <= 'Z')
            || c == '_';
    }

    private static bool IsAlphanumeric(char c)
    {
        return IsAlpha(c) || IsDigit(c);
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
