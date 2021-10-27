// interface for the translator class

#include "jbtoken.h"

const int maxboxstack=20;  
const int maxtokenstack=20;

class translator
{
private:

  box* victim;           // the javabox that is being translated

	mystring* inputstream;  // victim's name, for convenient ref.
	int inputindex;        // index of symbol in inputstream
	                       // that was just processed

  jbtoken* previous;     // maintain the previous token seen, for
	                 // use in determining whether - is 
                         // unary or binary 
	jbtoken* returned;     // when a token is putback, it is put here
	                       // and is used as the next token

public:

  int init( box* javasource );
  // attempt to translate given victim v, producing its
	//  inner box as a box version of its java code,
	//  return status code indicating why failed, or succeeded
  int translate( box* v );
  void processStatusCode( int code );
  char getNextInputSymbol();
  void putback( char x );
  int letter( char x );
  int digit( char x );
  jbtoken* getNextToken();
	jbtoken* produceToken();
	void putbackToken();
	void noteError( jbtoken* token, int code );
  box* processLiteral( jbtoken* firsttoken, int& errorcode );
  box* processExpression( int& errorcode );
  box* processIdChain( jbtoken* firsttoken, int& errorcode );
  box* processStatement( int& errorcode );
  box* processNewExpression( int& errorcode );
	box* processForInit( int& errorcode );
	box* processForPost( int& errorcode );

};
