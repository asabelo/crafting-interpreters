namespace Lox;

public abstract class Expr
{
    public class Binary : Expr 
    {
        public Binary(Expr left, Token @operator, Expr right)
        {
			this.left = left;
			this.@operator = @operator;
			this.right = right;
		}

		public readonly Expr left;
		public readonly Token @operator;
		public readonly Expr right;
	}

    public class Grouping : Expr 
    {
        public Grouping(Expr expression)
        {
			this.expression = expression;
		}

		public readonly Expr expression;
	}

    public class Literal : Expr 
    {
        public Literal(Object value)
        {
			this.value = value;
		}

		public readonly Object value;
	}

    public class Unary : Expr 
    {
        public Unary(Token @operator, Expr right)
        {
			this.@operator = @operator;
			this.right = right;
		}

		public readonly Token @operator;
		public readonly Expr right;
	}
};
