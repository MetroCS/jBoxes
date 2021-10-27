class jbtoken
{
  private:


    char kind;  // code for the kind of token
		            //  'e' for ERROR!!!!
		            //  'i' for identifier, 'k' for keyword, 
								//  's' for separator, 'o' for binary operator,
								//  'u' for unary operator,
								// 'I', 'F', 'B', 'C', 'S' for literals
								// whitespace and comments are eliminated at the lexical
								// analysis phase
								//  '$' for token indicating end of input
    mystring* value;  // the specific identifier, keyword, 
	                  // error message, literal value
		int error;  // code # for type of error encountered in 
		            // producing the 'e' kind of token
		int position; // index in input stream where error occurred

	public:

	jbtoken();
	void append( char x );
	void setKind( char x );
  void setError( int code, int where );
	int getError();
  char whatKind();
	mystring* getValue();
  int equals( char* s );
	int preUnaryMinus();
	void show();

  void destroy();

  int numberUsed();

};// end of jbtoken class
