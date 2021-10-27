// implementation of the translator class


#include <stdio.h>    // for FILE in mystring.h
//#include <iostream>
#include "mystring.h"
#include "box.h"
#include "utility.h"    // for requesterrormessage(), at least
#include "constants.h"  // for javabox, at least

#include "translator.h"

// jboxes1d:
jbtoken* reptok;  // reporter token for comparative counts

// initialize the input stuff
int translator::init( box* javasource )
{
  if( javasource->whichKind() != javabox )
	  return -1;

  // jboxes1d:
//  reptok = new jbtoken();
/*
cout << "Start translate, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

  // go on to set up for translation
  victim = javasource;
	previous = NULL;  // at start, there is no previous token
	returned = NULL;  //    and a token has not been put back

  inputstream = javasource->whatName();
  inputindex = 0;  // reset input sequence to start

	return 0;
}

int translator::translate( box* v )
{
  
	// initialize the input from the javabox
  init( v );

  // determine from context of victim whether to call 
	//  process Expression or Statement

  box* firstbox;
	int errorcode;

	if( victim->isAction() ) 
	{// victim needs to be a statement to perform an action

   // note:  allow naked sequence of statements in an action box,
	 //        build it into a sequence box.
	 //   Also, analogously, allow an empty javabox

	 int count = 0; // number of statements processed so far
	 int done = 0;
   jbtoken* token;
	 box* cumbox;
	 box* tempbox;

	 do{
	   token = getNextToken();
 		 if( token->whatKind() == '$' )
		 {// have successfully processed entire inputstream
		   token->destroy();
			 done = 1;
		 }
		 else
		 {// might be another (or first) statement
		   putbackToken();
	   
		   firstbox = processStatement( errorcode );
/*
cout << "after processStatement, line 88, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/
			 		 
  		 if( errorcode != 0 )
  		  return errorcode; 
				
			 // determine how to install firstbox into cumbox
			 if( count == 0 )
			 {// have first, possibly only statement 
			   cumbox = firstbox;
			 }
			 else if( count == 1 )
			 {// had 1 statement, just got second
			   tempbox = cumbox;
				 cumbox = new box;
				 cumbox->build( seqbox );
				 cumbox->addInnerAtFront( tempbox );
				 cumbox->addInnerAtCursor( firstbox );
			 }
			 else
			 {// adding third, etc. statement
			   cumbox->addInnerAtCursor( firstbox );
			 }

			 count++;  // whichever way, have another

		 }// another statement
	 }while( !done );

   // now, if still here---no error---paste in cumbox,
	 // unless turned out to be empty
	 if( count > 0 )
     victim->replaceInnerAt( cumbox, 1 );
	
	 return 0;
  
	}// statement

	else
	{// victim needs to be an expression to produce a value
	  firstbox = processExpression( errorcode );
		if( errorcode != 0 )
		  return errorcode;  
		else
		{// initial segment of tokens is valid expression,
		 // check for anything after

		  jbtoken* token = getNextToken();
			if( token->whatKind() != '$' )
			{
			  token->destroy();
				firstbox->destroy();
			  return -4;
			}
		  token->destroy();

		  // put firstbox in as victim's first box
		  victim->replaceInnerAt( firstbox, 1 );

			return 0;
		}  
	}// expression

}// translate

// given status code, request error or positive report
//  (this guy just encapsulates the various error messages and
//    status codes)
void translator::processStatusCode( int code )
{
  if( code != 0 )
	{// adjust the victim's name's cursor to indicate,
	 // to the discerning user, where the error occurred
	  inputstream->setCursor( inputindex );
	 // also, just to tidy up resources, and for clarity,
	 // kill off first
	 box* newfirstbox = new box;
	 newfirstbox->build( emptybox );
	 victim->replaceInnerAt( newfirstbox, 1 );
	}

  if( code == 0 )
	{
	  messagecolorcode = successcolorcode;
	  requesterrormessage(
				"Translation succeeded", victim );
  }   
  else if( code == -1 )
	  requesterrormessage( 
		"This box is not a java box so translation is impossible", 
		      victim );
	else if( code == -2 )
	  requesterrormessage(
		"Hit part not implemented yet", victim );
	else if( code == -3 )
	  requesterrormessage(
		"Have extra token following a valid statement", victim );
	else if( code == -4 )
	  requesterrormessage(
		"Have extra token following a valid expression", victim );
	else if( code == -1001 )
	  requesterrormessage(
		"A token can't start with this symbol", victim );
	else if( code == -1002 )
	  requesterrormessage(
		"Never had */ to close the comment started with /*", victim );
	else if( code == -1003 )
	  requesterrormessage(
		"This is a Java keyword that is not used in JBoxes", victim );
	else if( code == -1004 )
	  requesterrormessage(
		"More than 9 digits before the decimal point", victim );
	else if( code == -1005 )
	  requesterrormessage(
		"More than 9 digits after the decimal point", victim );
	else if( code == -1006 )
	  requesterrormessage(
		"Must have integer exponent after e or E", victim );
	else if( code == -1007 )
	  requesterrormessage(
		"Exponent must be an integer", victim );
	else if( code == -1008 )
	  requesterrormessage(
		"More than 3 digits in exponent", victim );
	else if( code == -1009 )
	  requesterrormessage(
		"Must have a printable symbol after opening single quote", victim );
	else if( code == -1010 )
	  requesterrormessage(
		"There is no closing single quote", victim );
	else if( code == -1011 )
	  requesterrormessage(
		"Unknown escape sequence", victim );
	else if( code == -1012 )
	  requesterrormessage(
		"There is no closing single quote", victim );
	else if( code == -1013 )
	  requesterrormessage(
		"There is a non-printable symbol in string literal", victim );
	else if( code == -1014 )
	  requesterrormessage(
		"Illegal escape sequence in string literal", victim );
	else if( code == -2001 )
	  requesterrormessage(
		"Incorrect expression, have extra operand(s)",
		victim );
	else if( code == -2002 )
	  requesterrormessage(
		"Incorrect expression, compared inappropriate tokens",
		victim );
	else if( code == -2003 )
	  requesterrormessage(
		"Incorrect expression, an inappropriate token was encountered",
		victim );
	else if( code == -2004 )
	  requesterrormessage(
		"Incorrect expression, need 2 operands for binary operator",
		victim );
	else if( code == -2005 )
	  requesterrormessage(
		"Incorrect expression, need 1 operand for unary operator",
		victim );
	else if( code == -2006 )
	  requesterrormessage(
		"() is not a legal expression",
		victim );
	else if( code == -3001 )
	  requesterrormessage(
		"Expected member name after the period",
		victim );
	else if( code == -3002 )
	  requesterrormessage(
		"Must put commas between argument expressions",
		victim );
	else if( code == -3003 )
	  requesterrormessage(
		"Missing right bracket ] after array index expression",
		victim );
	else if( code == -3004 )
	  requesterrormessage(
		"Can't have three dimensional arrays in JBoxes",
		victim );
	else if( code == -4001 )
	  requesterrormessage(
		"Illegal first token for a statement",
		victim );
	else if( code == -4002 )
	  requesterrormessage(
		"Must specify a data location after ++ or --",
		victim );
	else if( code == -4003 )
	  requesterrormessage(
		"++ and -- statements must have semi-colon after the location",
		victim );
	else if( code == -4004 )
	  requesterrormessage(
		"Must have semi-colon after method call",
		victim );
	else if( code == -4005 )
	  requesterrormessage(
		"Expected an assignment operator",
		victim );
	else if( code == -4006 )
	  requesterrormessage(
		"Expected semi-colon at end of assignment statement",
		victim );
	else if( code == -4007 )
	  requesterrormessage(
		"Expected ( after keyword if or while",
		victim );
	else if( code == -4008 )
	  requesterrormessage(
		"Missing ) after test expression following keyword if or while",
		victim );
	else if( code == -4009 )
	  requesterrormessage(
		"Missing while at end of do-while statement",
		victim );
	else if( code == -4010 )
	  requesterrormessage(
		"Missing semi-colon at end of do-while statement",
		victim );
	else if( code == -4011 )
	  requesterrormessage(
		"Missing ( after keyword for",
		victim );
	else if( code == -4012 )
	  requesterrormessage(
		"Missing ) at end of     for( ; ; )",
		victim );
	else if( code == -4013 )
	  requesterrormessage(
		"Missing semi-colon after expression to be returned",
		victim );
	else if( code == -5001 )
	  requesterrormessage(
		"Initializer must start with an identifier",
		victim );
	else if( code == -5002 )
	  requesterrormessage(
		"Can't have a call box as location",
		victim );
	else if( code == -5003 )
	  requesterrormessage(
		"Missing = after location in initializer",
		victim );
	else if( code == -5004 )
	  requesterrormessage(
		"Initializer must end with comma or semi-colon",
		victim );
	else if( code == -6001 )
	  requesterrormessage(
		"Must have semi-colon between expression and incrementor",
		victim );
	else if( code == -6002 )
	  requesterrormessage(
		"Missing incrementor between comma and )",
		victim );
	else if( code == -6003 )
	  requesterrormessage(
		"Assignment operation must have += or -= or *= or /= or %=",
		victim );
	else if( code == -6004 )
	  requesterrormessage(
		"Must have comma or ) after incrementor",
		victim );
	else if( code == -6005 )
	  requesterrormessage(
		"Incrementor must start with identifier or ++ or --",
		victim );
	else if( code == -6006 )
	  requesterrormessage(
		"Location to increment must start with identifier",
		victim );
	else if( code == -6007 )
	  requesterrormessage(
		"Location to increment can not be a method call",
		victim );
	else if( code == -7001 )
	  requesterrormessage(
		"Must have a primitive type name or a class name after the new",
		victim );
	else if( code == -7002 )
	  requesterrormessage(
		"Missing ] after first dimension",
		victim );
	else if( code == -7003 )
	  requesterrormessage(
		"Missing ] after second dimension",
		victim );
	else if( code == -7004 )
	  requesterrormessage(
		"Must have 0, 1, or 2 dimension expressions after the type name",
		victim );
	else if( code == -7005 )
	  requesterrormessage(
		"Must have ) after new <type> (", victim );
	else if( code == -7006 )
	  requesterrormessage(
		"Must have ; after new <type> ()", victim );
	else
	  errorexit("translator::processStatusCode---unknown code");
}

// lexical analysis methods

// give the code for the next char, -1 if none,
// look first in the lookahead spot, if is -1, try the inputstream
    char translator::getNextInputSymbol()
		{
		  if( inputindex == inputstream->length() )
			  return -1;   // have reached end of stream
			else
			{
			  inputindex++; // move pointer to next symbol
				return inputstream->charAt( inputindex ); // send it off
			}
		}

// put back x in lookahead, must be empty because otherwise
// would have used it to get x
//   putback is very easy with mystring as source of input
void translator::putback( char x )
		{
		  if( x != -1 )
		  inputindex--;  // just move the pointer back a spot
			      // unless "putting back" the end of the inputstream
		}

int translator::letter( char x )
		{
		  return x=='_' || ('a'<=x && x<='z') || ('A'<=x && x<='Z');
		}

int translator::digit( char x )
		{
		  return '0'<=x && x<='9';
		}

// utility function to abbreviate error recording,
//  takes desired error code to record, gets symbol to append
//  to token and other info from class data
void translator::noteError( jbtoken* token, int code )
{
  token->append( inputstream->charAt( inputindex ) );
	token->setKind( 'e' );
	token->setError( code, inputindex );
}

// the finite state machine that does the lexical analysis
//   This guy always returns a token, returns special $ token
//   if out of raw input symbols, returns error token with
//   error code accompanying if there is a problem
jbtoken* translator::produceToken()
		{
		  int state = 1;  // start state
			jbtoken* token = new jbtoken();

      int predigits=0, postdigits=0, expdigits=0; // for counting digits
			                                           // in parts of float literal
      // all exits from loop are returns

      int code;
      char x;

			do{
			  code = unsignchar(getNextInputSymbol());

				// otherwise, have a char, process it
				x = (char) code; 

        if( state == 1 )
				{
				  if( x == -1 )
					{// at start of looking for a token have hit end of input sequence
					  token->setKind('$');
					  return token;
					}
				  else if( x==' ' || code==tabsymbolcode
					                 || x==13 ) 
					{
					  // leave state at 1---spinning to consume leading whitespace
					}
					else if( x=='/' )
					{// if need it later, put it in manually
            state = 2; 
					}
					else if( x=='(' || x==')' || x=='{' || x=='}' ||
					         x=='[' || x==']' || x==';' || x==',' )
					{// have a separator, store it and we're done
						token->append(x);
						token->setKind('s');
						return token;   // replaces state 7  
					}
					else if( x=='=' )
					{
					  token->append(x);
						state = 8;
					}
					else if( x=='<' )
					{
					  token->append(x);
						state = 10;
					}
					else if( x=='>' )
					{
					  token->append(x);
						state = 12;
					}
					else if( x=='!' )
					{
					  token->append(x);
						state = 14;
					}
					else if( x=='&' )
					{// have &, done
            token->setKind('o');
					  token->append(x);  // store the &
            return token;   // replaces state 16
					}
					else if( x=='|' )
					{// have |, done
					  token->setKind('o');
					  token->append(x);  // store the |
						return token;   // replaces state 17
					}
					else if( x=='+' )
					{
					  token->append(x);
						state = 18;
					}
					else if( x=='-' )
					{
					  token->append(x);
						state = 21;
					}
					else if( x=='*' )
					{
					  token->append(x);
						state = 23;
					}
					else if( x=='%' )
					{
					  token->append(x);
						state = 25;
					}
					else if( letter(x) )					  
					{// starting identifier, keyword, or certain literals
					  token->append(x);
						state = 27;
					}
					else if( digit(x) )
					{
					  token->append(x);
						predigits = 1;
						state = 28;
					}
					else if( x=='.' )
					{
					  token->append(x);
						state = 33;
					}
					else if( x=='\'' )
					{// don't need to save the ' as part of the char literal
					  state = 34;
					}
					else if( x=='"' )
					{// don't need to save the " as part of the string literal
					  state = 40;
					}
					else
					{// illegal symbol out of state 1, is an error
					  noteError( token, -1001 );
						return token;
					}
				}// end of state 1

				else if( state == 2 )
				{
				  if( x=='*' )
					{// have /* comment starting
					  state = 3;
					}
					else if( x=='/' )
					{// have // comment starting
					  state = 5;
					}
					else if( x=='=' )
					{// just consumed = after not storing /, done
					  token->append('/');
					  token->append(x); // store the =
						token->setKind('o');
						return token;        // replaces state 6
					}
					else
					{// turned out to just be a / operator
					  token->setKind( 'o' );
						token->append( '/' );  // never stored it
						putback( x );  
						return token;
					}
				}// end of state 2

        else if( state == 3 )
				{
				  if( x=='*' )
					{// maybe starting end of block comment
					  state = 4;
					}
					else if( x == -1 )
					{// error to hit end of input without matching */
					  noteError( token, -1002 );
						return token;
					}
					else
					{// x is inside a block comment, just ditch it
					  // stay in state 3
					}
				}// end of state 3

        else if( state == 4 )
				{
				  if( x=='/' )
					{// end of block comment
					  state = 1;
					}
					else
					{// turned out the * was not before /, continue block comment
					  state = 3;
					}
				}// end of state 4

        else if( state == 5 )
				{// in a line comment
				  if( x == -1 )
					{// input ends while in line comment
					  token->setKind('$');
					  return token;
					}
				  else if( x != 13 )
					{// not end of line (in jboxes terms) so toss it
					  // stay in state 5
					}
					else
					{// hit end of line, which is end of comment
					  state = 1;
					}
				}// end of state 5

        // never move to state 6
        // never move to state 7

        else if( state == 8 )
				{
				  if( x=='=' )
					{// have == operator
					  token->setKind('o');
						token->append(x);
						return token;         // instead of going to state 9
					}
					else
					{// have just = operator, putback the non-= guy that told us
					  token->setKind('o');
            putback(x);
						return token;
					}
				}// end of state 8

        // state 9 is never actually moved to, for efficiency

				else if( state == 10 )
				{// got here with <
				  if( x=='=' )
					{// have <= operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 11
					}
					else
					{// have just < operator, putback the non-= guy that told us
					  token->setKind('o');
            putback(x);
						return token;
					}
				}// end of state 10

        // state 11 never moved to

				else if( state == 12 )
				{// got here with >
				  if( x=='=' )
					{// have >= operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 13
					}
					else
					{// have just > operator, putback the non-= guy that told us
					  token->setKind('o');
            putback(x);
						return token;
					}
				}// end of state 12

        // state 13 never moved to

				else if( state == 14 )
				{// got here with !
				  if( x=='=' )
					{// have != operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 15
					}
					else
					{// have just ! operator, putback the non-= guy that told us
					  token->setKind('u'); // have unary ! operator
            putback(x);
						return token;
					}
				}// end of state 14

        // state 15 never moved to

        // state 16 never moved to from state 1
        // state 17 never moved to from state 1

				else if( state == 18 )
				{// got here with +
				  if( x=='=' )
					{// have += operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 19
					}
					else if( x=='+' )
					{// have ++ operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 20
					}
					else
					{// have just + operator, putback the non- =,+ guy that told us
					  token->setKind('o');
            putback(x);
						return token;
					}
				}// end of state 18

        // state 19 never moved to
        // state 20 never moved to

				else if( state == 21 )
				{// got here with -
				  if( x=='=' )
					{// have -= operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 22
					}
					else if( x=='-' )
					{// have -- operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 22.5 
					}
					else
					{// have just - operator, putback the non- =,- guy that told us
            putback(x);
            // use previous token to decide unary or binary
						if( previous==NULL || previous->preUnaryMinus() )					 
  					  token->setKind('u');
						else
						  token->setKind('o');
						return token;
					}
				}// end of state 21

        // state 22 never moved to
        // state 22.5 never moved to (and a good thing, since it was left out)

				else if( state == 23 )
				{// got here with *
				  if( x=='=' )
					{// have *= operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 24
					}
					else
					{// have just * operator, putback the non-= guy that told us
					  token->setKind('o');
            putback(x);
						return token;
					}
				}// end of state 23

        // state 24 never moved to

				else if( state == 25 )
				{// got here with %
				  if( x=='=' )
					{// have %= operator
					  token->setKind('o');
						token->append(x);
						return token;           // replaces state 26
					}
					else
					{// have just % operator, putback the non-= guy that told us
					  token->setKind('o');
            putback(x);
						return token;
					}
				}// end of state 25

        // state 26 never moved to

        else if( state == 27 )
				{// got here with a letter or _
				  if( letter(x) || digit(x) )
					{// stick on end, keep collecting
					  token->append(x);
						// stay in state 27
					}
					else
					{// see guy not in identifier, put it back, determine kind
					  putback(x);  // was a lookahead

            if( token->equals("true") || token->equals("false") )
						 token->setKind( 'B' );  // boolean literal
						else if( token->equals("do") ||
						         token->equals("else") ||
						         token->equals("for") ||
						         token->equals("if") ||
						         token->equals("new") ||
						         token->equals("return") ||
						         token->equals("while") ||
										 token->equals("boolean") ||
						         token->equals("char") ||
						         token->equals("float") ||
						         token->equals("int") ||
                     token->equals("string")
									 )
						  token->setKind( 'k' );  // keyword
						else if( token->equals("abstract") ||
						         token->equals("break") ||
						         token->equals("byte") ||
						         token->equals("case") ||
						         token->equals("catch") ||
						         token->equals("class") ||
						         token->equals("const") ||
						         token->equals("continue") ||
						         token->equals("default") ||
						         token->equals("double") ||
						         token->equals("extends") ||
						         token->equals("final") ||
						         token->equals("finally") ||
						         token->equals("goto") ||
						         token->equals("implements") ||
						         token->equals("import") ||
						         token->equals("instanceof") ||
						         token->equals("interface") ||
						         token->equals("long") ||
						         token->equals("native") ||
						         token->equals("package") ||
						         token->equals("private") ||
						         token->equals("protected") ||
						         token->equals("public") ||
						         token->equals("short") ||
						         token->equals("static") ||
						         token->equals("super") ||
						         token->equals("switch") ||
						         token->equals("synchronized") ||
						         token->equals("throw") ||
						         token->equals("throws") ||
						         token->equals("transient") ||
						         token->equals("try") ||
						         token->equals("void") ||
						         token->equals("volatile") 
						       )
							noteError( token, -1003 ); // java keyword, not jb
					  else
						  token->setKind( 'i' );  // identifer

						return token;
					}
				}// end of state 27

        else if( state == 28 )
				{// got here with a digit
				  if( digit(x) )
					{// got another digit---might be too many
					  predigits++;
					  token->append(x);
						// stay in state 28
						if( predigits > 9 )
						{// error --- int part too long
						  noteError( token, -1004 );
							return token;
						}
					}
					else if( x=='.' )
					{// got . after some digits
					  token->append(x);
						state = 29;
					}
					else if( x=='e' || x=='E' )
					{// got eE after some digits
					  token->append(x);
						state = 30;
					}
					else
					{// got lookahead guy showing is an int
					  putback(x); 
						token->setKind('I'); // int literal
						return token;
					}  
				}// end of state 28

        else if( state == 29 )
				{// got digits . digits
				  if( digit(x) )
					{// got another digit after the decimal
					  postdigits++;
					  token->append(x);
						// state stays at 29
						if( postdigits > 9 )
						{// have too many digits after decimal (enforce here even if
						 // jboxes doesn't!
						   noteError( token, -1005 );
							 return token;
						}
					}
					else if( x=='e' || x=='E' )
					{// got exp part
					  token->append(x);
						state = 30;
					}
					else
					{// realize have float literal
					  putback(x);
						token->setKind('F'); // float literal
						return token;
					}
				}// end of state 29

        else if( state == 30 )
				{// just got eE
				  if( x=='+' || x=='-' )
					{
					  token->append(x);
						state = 31;
					}
					else if( digit(x) )
					{
					  expdigits = 1;
					  token->append(x);
						state = 32;
					}
					else
					{// error---can only have +-digit after eE
					  noteError( token, -1006 );
						return token;
					}
				}// end of state 30

        else if( state == 31 )
				{
				  if( digit(x) )
					{
					  expdigits = 1;
						token->append(x);
						state = 32;
					}
					else
					{
					  noteError( token, -1007 );
						return token;
					}
				}// end of state 31

        else if( state == 32 )
				{
				  if( digit(x) )
					{// got another digit in exp part
					  expdigits++;
						token->append(x);
						// state stays at 32
						if( expdigits > 3 )
						{
						  noteError( token, -1008 );
							return token;
						}
					}
					else
					{// got a non-digit telling us float is over
					  putback(x);
						token->setKind('F');  // have a float literal
						return token;
					}
				}// end of state 32

        else if( state == 33 )
				{// saw a period in state 1, is start of float or separator
          if( digit(x) )
					{
					  postdigits = 1;
						token->append(x);
						state = 29;
					}
					else
					{// any other than digit means was a separator
					  putback(x);  // put back the lookahead
					  token->setKind('s'); 
						return token;
					}				  
				}// end of state 33

        else if( state == 34 )
				{// got a single quote 
				  if( x != '\\' && x!='\'' && (' '<=x && x<='~') )
					{// have single printable that's not a backslash
					  token->append(x);  // stick it in the string
						state = 35; 
					}
					else if( x == '\\' )
					{// have backslash
					  // don't store the \, will store the actual char
						state = 37;
					}
					else
					{// error to have non-printable after '
					  noteError( token, -1009 );
						return token;
					}
				}// end of state 34

        else if( state == 35 )
				{
				  if( x == '\'' )
					{// got single quote as desired, have char literal
					  token->setKind('C');
						return token;         // replaces state 36
					}
					else
					{// error---never closed the char literal
					  noteError( token, -1010 );
						return token;
					}
				}// end of state 35

        // state 36 is never moved to

				else if( state == 37 )
				{//got single quote, backslash, must have ' or " or n or backslash
          if( x=='\'' || x=='"' || x=='\\' )
					{
					  token->append(x);  // it's fine to put these guys in a string
						                   // like this---problem is with literals
						state = 38;
					}
					else if( x=='n' )
					{
					  token->append( (char) 13 );
						state = 38;
					}
					else
					{
					  noteError( token, -1011 );
					  return token;
					}
				  
				}// end of state 37

        else if( state == 38 )
				{
				  if( x == '\'' )
					{// got single quote as desired, have char literal with escape
					 // sequence
					  token->setKind('C');          						 
						return token;         // replaces state 39
					}
					else
					{// error---never closed the char literal
					  noteError( token, -1012 );
						return token;
					}
				}// end of state 38

        // state 39 is never moved to

				else if( state == 40 )
				{// got "
				  if( x=='"' )
					{// got closing "
					  token->setKind('S');  // have string literal
						return token;   // replaces state 41
					}
					else if( x=='\\' )
					{// got start of escape sequence
					  // drop the backslash
						state = 42;
					}
					else if( ' '<=x && x<='~' )
					{// got printable, just slap it on
					  token->append(x);
						// stay in state 40
					}
					else
					{// error to have non-printable in string literal
					  noteError( token, -1013 );
						return token;
					}
				}// end of state 40

        // state 41 is never moved to

				else if( state == 42 )
				{
				  if( x=='\'' || x=='\\' || x=='"' )
					{
					  token->append(x);
						state = 40;
					}
					else if( x=='n' )
					{
					  token->append( (char) 13 );
						state = 40;
					}
					else
					{// error---illegal escape sequence char
					  noteError( token, -1014 );
						return token;
					}
				}// end of state 42

        else
				{// unknown state, is fatal programming error
				  errorexit(
					"Unknown state encountered in Java translation");
				}

      }while( 1 );
		  
		}// translator::produceToken

// produce token and remember it
//  (a thin wrapper on produceToken to note previous and allow
//   putback)
jbtoken* translator::getNextToken()
{
  jbtoken* result;
	if( returned == NULL )
  	result = produceToken();
	else
	{// have a putback token, use it
	  result = returned;
		returned = NULL;  // actually consume it
	}
	previous = result; // update previous to be the guy just produced

	return result;
}

void translator::putbackToken()
{
  returned = previous;
}

// little utility --- assign arbitrary opcodes above 100
//  to tokens that aren't
//     binary ops but are comparable to ops, -1 to those that
//     should never be compared,
//   binary ops get 1--6 depending on operator precedence
//  Thus, every token gets an int code to speed up work in
//  precedence method
int getopcode( jbtoken* token )
{
  mystring* op = token->getValue();

  if( token->whatKind()=='o' )
	{// assign precedence level code to binary ops, not =,+=, etc.
	  if( op->equals( "|" ) )
		  return 1;
		else if( op->equals( "&" ) )
		  return 2;
		else if( op->equals( "==" ) || op->equals( "!=" ) )
		  return 3;
		else if( op->equals( "<" ) || op->equals( ">" ) ||
             op->equals( "<=" ) || op->equals( ">=" ) 
					 )
		  return 4;
		else if( op->equals( "+" ) || op->equals( "-" ) )
		  return 5;
		else if( op->equals( "*" ) || op->equals( "/" ) ||
             op->equals( "%" ) 
					 )
		  return 6;
    else
		  return -1;  
	}
	else if( token->whatKind()=='s' )
	{// only ( and ) get 0 as comparable at all
	  if( op->equals( "(" ) )
		  return 101;
		else if( op->equals( ")" ) )
		  return 102;
		else
		  return -1;
	}
	else if( token->whatKind()=='$' )
	{// end of stream token is comparable
	  return 103;
	}
	else if( token->whatKind()=='u' )
	{// so are - and !
	  if( op->equals( "-" ) )
		  return 104;
		else // "!"
		  return 105;
	}
	else  // all others are definitely not comparable
	  return -1;
}

// return < > = or space depending on desired precedence between
//  the tokens first and second
//  Note that any two tokens can be formally 
//  compared, but only a few
//  will not produce <space> which means they really aren't
//  comparable
//
//    if sim, for "simulate end of token stream",
//    treat second same as $, so returns > unless
//       first is ( or $ which yield error
char precedence( jbtoken* first, jbtoken* second, int sim )
{
  int foc, soc;  // first op code, second op code
	foc = getopcode( first );

	if( sim )
	{// simulate second token being $
	  if( foc==101 || foc==103 )
		  return ' ';
		else
		  return '>';
	}

	soc = getopcode( second );

  if( foc==-1 || soc==-1 ) // either one is incomparable
	  return ' ';
	else
	{// both are comparable
	  if( foc<100 && soc<100 )
		{// both are binary ops, use their codes to produce
		 // desired precedence
		  if( foc < soc )
			  return '<';
			else if( foc > soc )
			  return '>';
			else
			  return '>';  // equal precedence, do this to make left-assoc
		}
		else if( foc < 100 )
		{// compare binary op to specials
		  if( soc == 101 ) // left paren
			  return '<';
			else if( soc == 102 ) // right paren
			  return '>';
			else if( soc == 103 ) // $
			  return '>';
			else if( soc == 104 ) // unary minus
			  return '<';
			else if( soc == 105 ) // logical not
			  return '<';
			else
			  return ' ';
		}
		else if( soc < 100 )
		{// compare special to binary op
		  if( foc == 101 ) // left paren
			  return '<';
			else if( foc == 102 ) // right paren
			  return '>';
			else if( foc == 103 ) // $
			  return '<';
			else if( foc == 104 ) // unary minus
			  return '>';
			else if( foc == 105 ) // logical not
			  return '>';
		  else
			  return ' ';
		}
		else
		{// both are specials
		  if( foc == 101 )
			{// ( vs. soc
			  if( soc == 102 )
				  return '=';  // the only time this happens!
				else if( soc == 103 )
				  return ' ';  // error to compare ( to $
				else 
				  return '<';  // ( is very low precedence
			}
			else if( foc == 102 )
			{// ) vs. soc
			  if( soc == 101 )
				  return ' ';  //  ) ? ( is error
				else
				  return '>';  // ) is very high precedence
			}
			else if( foc == 103 )
			{// $ vs soc
			  if( soc == 102 || soc == 103)
				  return ' ';   // error to ask  $ ? ) or $ ? $
				else
				  return '<';
			}
			else if( foc == 104 || foc == 105 )
			{// unary ? special
        if( soc == 101 )   // (
				  return '<'; 			  
				else if( soc == 102 ) // )
				  return '>';
				else if( soc == 103 ) // $
				  return '>';
				else  // unary vs. unary
				  return '<';  
			}
			else
			  return ' ';  // all other (if any) 
				             // special ? special is error
		}// both are specials

	}// both are comparable

}// precedence

// ====================== parser methods ======================
//   all these guys take a first token, consume some more of
//     the input stream, possibly, and either produce a non-zero
//     failure code or return a translated firstbox
//     They also tidy up firsttoken once it is no longer needed,
//     just to preserve space

box* translator::processLiteral( jbtoken* firsttoken, 
                                  int& errorcode )
{
  // all we have to do is build the box, similar to what is
	// done when create one interactively
	box* firstbox;

  char tknd = firsttoken->whatKind();
	int bknd;

	if( tknd=='C') bknd = charbox;
	else if( tknd=='B' ) bknd = booleanbox;
	else if( tknd=='F' ) bknd = floatbox;
	else if( tknd=='I' ) bknd = intbox;
	else if( tknd=='S' ) bknd = stringbox;
	else
	  errorexit("unknown literal kind in translator::processLiteral");

  firstbox = new box;
	firstbox->build( bknd );
	firstbox->storeName( firsttoken->getValue() );

  errorcode = 0;

	firsttoken->destroy();

	return firstbox;
}

// firsttoken is an id token, go from there to build a
//  box correctly from that point on, allowing for [ . ( 
//  cases
box* translator::processIdChain( jbtoken* firsttoken, 
                                  int& errorcode )
{
/*
cout << "Start processIdChain, have " << reptok->numberUsed()
              << " tokens in existence" << endl;
*/

  errorcode = 0;

	// build the id box for this id token, will for sure be needed
  box* cumbox = new box;
	cumbox->build( idbox );
	cumbox->storeName( firsttoken->getValue() );
	  firsttoken->destroy();  // no longer needed --- have the name

	// loop to scan tokens until hit end of chain or error
	int done = 0;  // flag to exit loop either way

	box* curbox;   // utility boxes
	box* argbox;   
	box* indexbox;
	box* index2box;  

	jbtoken* token;        // utility token

	do{

    token = getNextToken();

		if( token->whatKind() == 's' &&
		    (token->getValue())->equals( "." ) )
		{// field member
		  token->destroy();  // toss the . token to restore memory
			token = getNextToken();  // get the field id
			if( token->whatKind() != 'i' )
			{// error --- had to be field identifier
			  errorcode = -3001; 
				done = 1;
			}
			else
			{// have field identifier, build the member box
	      curbox = cumbox;  // holds whatever we've built so far
				cumbox = new box;
      	cumbox->build( memberbox );
	      cumbox->storeName( token->getValue() );
      	  token->destroy(); 
				cumbox->replaceInnerAt( curbox, 1 );			          		  
			}
		}// member

		else if( token->whatKind() == 's' &&
		    (token->getValue())->equals( "[" ) )
		{// array or 2d array
		  // won't know until later whether building array or array2d
			//  so need to store indexbox and index2box and cumbox,
			//  then build it all properly

      // jb23:  messed up the previous, do this later
			// token->destroy();  // done with the [

      curbox = cumbox;  // now cumbox can be used for the final one

		  // after [, need index expression
			indexbox = processExpression( errorcode );

      // jb23:  can destroy token safely now that following expression
      //        has been processed
      token->destroy();

			if( errorcode != 0 )
			{// error in first index
			  done = 1;  // will pass through the errorcode
			}
			else
			{// have first index expression
			  token = getNextToken();
				if( token->whatKind() != 's' ||
				    !(token->getValue())->equals( "]" )
					)
				{// missing ]
				  token->destroy();
					indexbox->destroy();
				  errorcode = -3003;
					done = 1;
				}
				else
				{// have curbox [ indexbox ]
				  token->destroy();  

					token = getNextToken(); // look ahead for another [

					if( token->whatKind() != 's' ||
  				    !(token->getValue())->equals( "[" )
						)
					{// fine, just had the one dim. array, finish up
					  putbackToken();
					  // this was wrong, not done with it!  token->destroy();
    				cumbox = new box;
          	cumbox->build( arraybox );
    				cumbox->replaceInnerAt( curbox, 1 );
						cumbox->replaceInnerAt( indexbox, 2 );
					}
					else
					{// looking like an array2d
            // jb23:  same error, don't destroy it yet for previous
					  // token->destroy();  // was the initial [ of second index

      			index2box = processExpression( errorcode );

            // jb23:  can toss it now
            token->destroy();

   			    if( errorcode != 0 )
      			{// error in second index
						  indexbox->destroy(); // not using it after all
      			  done = 1;  // will pass through the errorcode
			      }
			      else
        		{// have second index expression
      			  token = getNextToken();
			      	if( token->whatKind() != 's' ||
				          !(token->getValue())->equals( "]" )
					      )
      				{// missing ]
			      	  token->destroy();
								indexbox->destroy(); // not using either index box
								index2box->destroy();
      				  errorcode = -3003;
			      		done = 1;
				      } 
      				else
			      	{// have curbox [ indexbox ][ index2box ]
      				  token->destroy();

			      		token = getNextToken(); // look ahead for another [

      					if( token->whatKind() != 's' ||
  				          !(token->getValue())->equals( "[" )
									)
					      {// fine, don't have 3 [] in a row, go ahead
								 // and build the array2d
					        putbackToken();
					        // this was wrong:  token->destroy();
          				cumbox = new box;
                	cumbox->build( array2dbox );
    				      cumbox->replaceInnerAt( curbox, 1 );
						      cumbox->replaceInnerAt( indexbox, 2 );
									cumbox->replaceInnerAt( index2box, 3 );
					      }// is array2d
					      else
					      {// 3 [] in a row
       					  token->destroy();
  								indexbox->destroy();// not using either index box
	  							index2box->destroy();
         				  errorcode = -3004;
	  		      		done = 1;		  
      					}// trying to be array3d					  
							}// have ] after second index
						}// have second index
					}// may be array2d
 				}// have ]  after first index
			}// have first index
		}// arrays

else if( token->whatKind() == 's' &&
         (token->getValue())->equals( "(" ) )
{// call

  // build the callbox in cumbox
  curbox = cumbox;  // holds whatever we've built so far
  cumbox = new box;
  cumbox->build( callbox );

  // jb23:  can't do this, messes up previous below
  // token->destroy();     // done with the (

  cumbox->replaceInnerAt( curbox, 1 ); // put in the method name
						
  // process 0 or more args, find matching )
  int argsdone = 0;
  int argcount = 0; // only allow

  jbtoken* oldtoken = token;  // jb23  will for sure toss (

  do {
    // have processed argcount args, decide what to do next
    // look ahead to see if is  ) , or expression

    // jb23:  remember who needs to be killed
    //   had this, but caused problems, be more careful about
    //   storing NULL in oldtoken, only changing to something
    //   else that for sure needs to be tossed
    //  oldtoken = token;

    token = getNextToken();
    
    // jb23:  now that previous has been used, can kill oldtoken
    //    revised version---only kill non-NULL
    if( oldtoken != NULL )
    {
      oldtoken->destroy();
      oldtoken = NULL;     // so can avoid inappropriate toss
    }
				
    if ( token->whatKind()=='s' && (token->getValue())->equals( ")" ))
    {// have hit ending )
     // just have to be happy, except for saving space
      // jb23:  leaving loop now, okay to delete this way
      token->destroy();
      argsdone = 1;
    }
    else if( ( argcount > 0 &&
               ( token->whatKind()=='s' &&
               (token->getValue())->equals( "," ) )
             ) || 
             argcount == 0
           )
    {// have hit , indicating another argument expression,
     // or getting first argument expression

      // jb2006a:  had just that token kind is 's', so when hit ( at
      //           start of an argument, with argcount == 0 (i.e., first
      //           argument starts with (, things are slightly wrong)
      //   Fix was to demand that token is actually a comma
      if( token->whatKind()=='s' && 
          (token->getValue())->equals( "," )   // jb2006a added
        )
      {
        // jb23:  can't destroy token until have used below,
        // so instead note in oldtoken
        oldtoken = token;
        // token->destroy(); // done with the ,
      }
      else
      {// jb23:  when put back the token, leaving oldtoken
        //        at NULL so won't cause problems
        putbackToken();
      }

      argbox = processExpression( errorcode );

      if( errorcode == 0 )
      {// got good expression, add it in
        cumbox->addInnerAtCursor( argbox );
      }
      else
      {
        done = 1;  // error exit from processExpr
      }

   }
   else
    {// rather strange error --- like f( a b )
       errorcode = -3002;
       done = 1;
       // jb23:  now that we're doing the oldtoken stuff, need
       //        to clean up on every exit out of the loop
      token->destroy();
    }

  }while( !argsdone && !done );

}// call

		else
		{// some other token---stops the chain, put it back
		 // so we consume exactly the id chain
      putbackToken();		  
		  done = 1;
		}
    	  
	}while( !done );

  if( errorcode != 0 )
	{
	  cumbox->destroy();  // holds all resources used locally
	  return NULL;
	}
	else
	  return cumbox;

}

// given kind and value of a token, return whether it 
//  is legal in an expression, which is:
//     literals, 'i', 'o', 'u', and some 's', namely ( and )
int legalInExpression( char knd, mystring* val )
{
  if( knd=='i' || 
	    knd=='B' || knd=='C' || knd=='F' || knd=='I' || knd=='S' ||
			knd=='o' || knd=='u' 
		)
		return 1;
	else if( knd=='s' &&
	         (val->equals( "(" ) || val->equals( ")" ) )
				 )
		return 1;
	else
	  return 0;
}

// only produce an error if the part being processed has error,
// as detected by stuff on the tokenstack not comparing well
// with others, huh?
box* translator::processExpression( int& errorcode )
{
  // every call to processExpression has its own auxiliary
	//  data structures---very unlikely to have very many
	//  pending calls --- would be like a[ b[ c[ k+1 ] ] ]

	// tokenstack keeps track of the operators and parens
  jbtoken* tokenstack[maxtokenstack];
	int toptokenstack = 0; // points to next available spot

	// boxstack stores the boxes as they are built, end up
	//  returning boxstack[0] or detecting error if boxstack
	//  has different from 1 boxes on it when done
  box* boxstack[maxboxstack];
	int topboxstack = 0;

  // initialize tokenstack to have single $ token, just so
	// can uniformly pop stack, etc.
	jbtoken* bottom = new jbtoken;
	bottom->setKind( '$' );
  tokenstack[0] = bottom;  toptokenstack++;

  int done = 0;

  jbtoken* token;
	jbtoken* top;
	jbtoken* prev;

	box* tempbox;

	int justSawLeftParen; // special case to eliminate ()
	int parenMatch = 0;  // monitor ( ) count so can detect
	                     //   unbalanced ) that is like end of
											 //   tokenstream
  int simEOTS = 0;     // set simulate end of token stream for
	                     // efficiency when first notice it

	do{

    token = getNextToken();

		// if token is ) with parenMatch == 0 or is a token
		//  that is not legal in an expression,
		//  we're at the simulated
		//  end of the token stream, use simEOTS to detect
		//  this at relevant spot
		if( simEOTS ||
		    (
				  ( parenMatch==0 && token->whatKind()=='s' &&
		           (token->getValue())->equals( ")" )
					)
				  ||
				  ! legalInExpression( token->whatKind(),
				          token->getValue() )
				)
			)
		{// simulate end of token stream
		  simEOTS = 1;
			putbackToken();  // this simulates being on end, keep
			                 // grabbing the same token repeatedly
		}

    // crazy () kludge! 
		//   and do matching work, too

		if( token->whatKind()=='s' &&
		    (token->getValue())->equals( ")" )
			)
		{// token is )
		  
			parenMatch--;  // adjust count for )

		  if( justSawLeftParen )
			{// ) immediately after ( is a special error
		    errorcode = -2006;
			  bottom->destroy();  // all others exit at end
			  return NULL;
			}
		}

		if( token->whatKind()=='s' &&
		    (token->getValue())->equals( "(" )
			)
		{// token is (
		  
			parenMatch++;  // adjust count for (

		  justSawLeftParen = 1;
		}
		else  // not a left paren
      justSawLeftParen = 0;

    if( token->whatKind()=='i' )
		{// have start of an identifier---process the chain
		  // before couldn't fail, but now processIdChain can, so
			// be more careful!
			tempbox = processIdChain( token, errorcode );
/*
cout << "After processIdChain in expression processing, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

			if( errorcode != 0 )
			{// error noticed in id chain, pass it on out
	      done = 1;
			}
			else
			{// got id chain successfully, push it on the stack
			  boxstack[ topboxstack ] = tempbox;
				topboxstack++;
			}
		}
		else if( token->whatKind()=='B' ||
		         token->whatKind()=='C' ||
		         token->whatKind()=='F' ||
		         token->whatKind()=='I' ||
		         token->whatKind()=='S' 
     			 )
    {// have a literal---process it, push the resulting box
      boxstack[ topboxstack ] = processLiteral( token, errorcode );		  
			  topboxstack++;
			if( errorcode != 0 )
			  errorexit("what??? in processExpression with literal");
		}

	  else
	  {// token that does not lead to a value
  
	    if( ( simEOTS ||
			      token->whatKind()=='$'
					)   // end of token stream
					&&
			    (tokenstack[ toptokenstack-1 ])->whatKind() == '$'
					    // token stack is empty
			  )
		  {// end of input stream, token stack is empty, return happy
		  //  ? unless box stack has more than one 
			  if( topboxstack == 1 )
			  {// success!
			    errorcode = 0;
				  done = 1;
			  }
			  else
			  {// error---unprocessed stuff left on token stack
				 // when input tokens run out
				  done = 1;
			    errorcode = -2001;
			  }
		  }
		  else
		  {// still have work to do

        // pop the token stack
  	    toptokenstack--;   top = tokenstack[ toptokenstack ];

        char prec = precedence( top, token, simEOTS );

    	  if( prec=='<' || prec=='=' )
		    {// top has lower or equal precedence to token 
			    // push the top back on the token stack
			    toptokenstack++;  // haven't taken top away really
				  // push the input token
				  tokenstack[ toptokenstack ] = token;  toptokenstack++;
			  }
			  else if( prec == '>' )
 			  {// top has higher precedence than token

          // jboxes1d:
          prev = NULL;

          do{
            // jboxes1d:  if prev refers to anyone, it's about to
            //            leak
            if( prev != NULL )
              prev->destroy();

           // note previous top of stack
	  		    prev = top;
					  // process the token just removed from token stack
					  if( prev->whatKind() == 'o' )
					  {// binary operator
					   // build opbox with top two, else error
						  if( topboxstack >= 2)
						  {// have at least 2 boxes to use
						   // pop the two operand boxes, build opbox, push it
                tempbox = new box;
							  tempbox->build( opbox );
							  tempbox->storeName( prev->getValue() ); // the op
							  tempbox->replaceInnerAt( boxstack[ topboxstack-2], 
							                           1 );
							  tempbox->replaceInnerAt( boxstack[ topboxstack-1], 
							                           2 );
							  boxstack[ topboxstack-2 ] = tempbox;
							  topboxstack--;
						  }
						  else
						  {
						    done = 1;
							  errorcode = -2004;
						  } 
					  }// prev is binary op
 					  else if( prev->whatKind() == 'u' )
					  {// unary operator
					   // build unopbox with top 1, else error
						  if( topboxstack >= 1)
						  {// have at least 1 box to use					   
						   // pop the operand box, build unopbox, push it
                tempbox = new box;
							  tempbox->build( unopbox );
							  tempbox->storeName( prev->getValue() ); // the op
							  tempbox->replaceInnerAt( boxstack[ topboxstack-1], 
							                           1 );
							  boxstack[ topboxstack-1 ] = tempbox;

						  }
						  else
						  {
						    done = 1;
							  errorcode = -2005;
						  } 
					  }// prev is unary op
 					  else if( (prev->whatKind() == 's') &&
					           ((prev->getValue())->equals("(") ||
                      (prev->getValue())->equals(")")
						 			   )
					        )
					  {// parens just get tossed when matched up
					      // intentionally left empty!
              // jboxes1d:  release the token that has been processed
					  }
					  else
					  {// all others lead to error
					    done = 1;
						  errorcode = -2003;
					  }

            // pop the next token
					  toptokenstack--; top = tokenstack[ toptokenstack ];

            // until top has lower precedence than prev
						//   don't simulate end of token stream here,
						//    because top and prev are actual tokens that
						//    got pushed earlier, not the simulated end token
						//    coming in from token stream
          }while( ! (precedence( top, prev, 0 ) == '<') );

          // jboxes1d:
          if( prev != NULL )
            prev->destroy();

          // push top back on token stack
				  tokenstack[ toptokenstack ] = top; toptokenstack++;
				  // put token back in input stream
          putbackToken();
					  // adjust parenMatch when put ) back that was already
						// counted
						if( token->whatKind()=='s' &&
						    (token->getValue())->equals( ")" )
							)
							parenMatch++;

		    }// top had higher precedence than the input token
			
			  else
  		  {// precedence between top and token ill-defined,
				 // is an error unless top is bottom of tokenstack,
				 // meaning all scanned tokens have been processed,
				 // and boxstack has only one guy on it ---
				 //  put token back to be processed by someone else

         // comment above is wrong --- if get here, it's an
				 //  error 
  			    errorcode = -2002;
	  			  done = 1;
 		    }
		
		  }// still have work to do

		}// non value token

  } while( ! done ); // loop to process all info

  // errorcode set above, just need to return the box
  if( errorcode == 0 )
	{// succeeded, only have to release bottom
	  bottom->destroy();
	  return boxstack[ 0 ];
	}
	else
	{// found error, release all resources on stacks
    int k;
		for( k=0; k<topboxstack; k++ )
		  boxstack[k]->destroy();
		for( k=0; k<toptokenstack; k++ )
		  tokenstack[k]->destroy();
	  return NULL;
	}
  
}// processExpression

box* translator::processStatement( int& errorcode )
{
  jbtoken* token = getNextToken();

/*
cout << "Start processStatement, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

	box* cumbox;  // accumulate final box in this
	box* sbox;    // utility to hold a statement
	box* idbox;
	box* rhsbox;
	box* ebox;
	box* s2box;
	box* initbox;
	box* postbox;

  errorcode = 0;

	int done = 0;

	if( token->whatKind()=='s' &&
	    (token->getValue())->equals( ";" ) 
		)
	{// done with empty statement, need to built emptybox
	  cumbox = new box;
		cumbox->build( emptybox );
		return cumbox;
	}

	else if( token->whatKind()=='s' &&
	    (token->getValue())->equals( "{" ) 
		)
	{// must be starting compound statement

    token->destroy();  // done with the {

	  // build cumbox as a sequence box
    cumbox = new box;
		cumbox->build( seqbox );

		// loop to process statements until error or hit }
    do {
		  token = getNextToken();

    	if( token->whatKind()=='s' &&
	       (token->getValue())->equals( "}" ) 
		    )
			{// yea, we're done!
			  token->destroy();  // tidy up the }
			  done = 1;
			}
			else
			{// hoping for a statement (or an error)
			  putbackToken();  // did a look ahead
				sbox = processStatement( errorcode );

				if( errorcode == 0 )
				{// successfully got a statement, insert it
				  cumbox->addInnerAtCursor( sbox );
				}
				else
				{// encountered error in trying to get a statement
				  cumbox->destroy();  // just didn't work out, tidy it
				  done = 1;
					// pass errorcode on out
				}
			}

		} while( !done );

    if( errorcode == 0 )
		  return cumbox;
		else
		  return NULL;

	}// { } case

	else if( token->whatKind()=='i'
      	 )
	{// must be starting call or assignment
    idbox = processIdChain( token, errorcode );
/*
cout << "After processIdChain starting call or assignment, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

		if( errorcode != 0 )
		{// error found in processing id chain
		  return NULL;  // pass out errorcode
		}
		else
		{// idbox is good idchain, is call or assignment
		  if( idbox->whichKind() == callbox )
			{// call statement
			  token = getNextToken();
				if( token->whatKind()=='s' &&
				    (token->getValue())->equals( ";" )
					)
				{// yea---have ; at end
          token->destroy();  // ; is consumed
					return idbox;
				}
				else
				{// error --- no ; after call
				  errorcode = -4004;
					token->destroy();
					idbox->destroy();
					return NULL;
				}
			}// call statement
			else
			{// ass statement---left hand side is not a call
        token = getNextToken();
				if( token->whatKind()=='o' &&
				    (
  				    (token->getValue())->equals( "=" ) ||
  				    (token->getValue())->equals( "+=" ) ||
  				    (token->getValue())->equals( "-=" ) ||
  				    (token->getValue())->equals( "*=" ) ||
  				    (token->getValue())->equals( "/=" ) ||
  				    (token->getValue())->equals( "%=" ) 
						)
					)
				{// next token was a good ass op
				  // build assbox with idbox and the ass op
				  cumbox = new box;
					cumbox->build( assbox );
					cumbox->replaceInnerAt( idbox, 1 );
					cumbox->storeName( token->getValue() );

          // jb23:  oops, this messed up the previous checking
          //        for unary minus, so don't do it,
          //        do it later
					//  token->destroy();

					// scan on to get the right hand side and ;

					// first see if rhs is a new expression
					rhsbox = processNewExpression( errorcode );
/*
cout << "after processNewExpression, line 2094, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

					if( errorcode != 0 )
					{
					  cumbox->destroy();
						return NULL;
					}

          // if didn't get a new expression, try for standard
					if( rhsbox == NULL )
  					rhsbox = processExpression( errorcode );

          // jb23:  destroy the token now that have processed the
          //        following expression
          token->destroy();

					if( errorcode != 0 )
					{// bad rhs expression found
					  cumbox->destroy();
						return NULL;
					}
					else
					{// have good rhs expression, see if next is ; 
					  token = getNextToken();

          	if( token->whatKind()=='s' &&
	              (token->getValue())->equals( ";" ) 
		          )
						{// yea!
						  token->destroy();  // consume the ;
							cumbox->replaceInnerAt( rhsbox, 2 );
							return cumbox;
						}
						else
						{// no ;
	            errorcode = -4006;
							token->destroy();
							cumbox->destroy();
							rhsbox->destroy();  // do separately---never inserted
							return NULL;
						}// no ;					
					}// good rhs
				}// good ass op
				else
				{// next token was not an ass op
				  token->destroy();  
					idbox->destroy();
				  errorcode = -4005;
					return NULL;
				}// not ass op
			}// ass statement
		}// call or assignment?
	}// starts with id

	else if( token->whatKind()=='o' &&
	         ( (token->getValue())->equals( "++" ) ||
				     (token->getValue())->equals( "--" )
           )
		)
	{// must be starting grow statement
	  // build the growbox with correct operator
	  cumbox = new box;
		cumbox->build( growbox );
		cumbox->storeName( token->getValue() );
		  token->destroy();  // done with the ++ or --

		// get the idchain
    token = getNextToken();

		if( token->whatKind() != 'i' )
		{// illegal idchain in growbox
		  errorcode = -4002;
      token->destroy();
			cumbox->destroy();
			return NULL;
		}
		else
		{// have id to start, get rest of chain
		  idbox = processIdChain( token, errorcode );
/*
cout << "After processIdChain for growing, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

      if( errorcode != 0 )
			{// error in trying to process the id chain
			  return NULL;  // pass errorcode on out
			}
			else 
			{// have idbox, but might not be correct sort
			  if( idbox->whichKind() == callbox )
				{
				  idbox->destroy();
				  errorcode = -4002;
					return NULL;
				}
				else
				{// idbox is good for the growbox, paste it in and finish
          cumbox->replaceInnerAt( idbox, 1 );
					// oops, next token might not be the required ;
					token = getNextToken();
					if( token->whatKind()=='s' &&
					    (token->getValue())->equals( ";" )
						)
					{// yea!
					  token->destroy();  // just needed to see the ;
						return cumbox;
					}
					else
					{
					  errorcode = -4003;
						cumbox->destroy();
						token->destroy();
						return NULL;
					}// have ; at end?
				}// have non-callbox
			}// have good idchain
		}// have id at start
	}// grow case

	else if( token->whatKind()=='k'
	          &&
					 ! (token->getValue())->equals( "new" )
					  &&
					 ! (token->getValue())->equals( "else" )
           // jb23:  rule out the five primitive type key words too
            &&
           ! (token->getValue())->equals("boolean")
            &&
           ! (token->getValue())->equals("char")
            &&
           ! (token->getValue())->equals("float")
            &&
           ! (token->getValue())->equals("int")
            &&
           ! (token->getValue())->equals("string")
      	 )
	{// must be starting if, while, do, for, return ---
	 //   these are the only allowed key words other than new,
	 //   which is special
	  
		if( (token->getValue())->equals( "if" ) )
		{// if statement
		  token->destroy();  // toss the "if"
			token = getNextToken();
			if( token->whatKind() != 's'
	          ||
					 ! (token->getValue())->equals( "(" )
				)
			{// error --- no ( after if
			  token->destroy();
			  errorcode = -4007;
				return NULL;
			}
			else
			{// have if(

        // jb23:  don't toss until use as previous
			  // token->destroy();  // toss the (

				ebox = processExpression( errorcode );

        // jb23:  now can toss it
        token->destroy();

				if( errorcode != 0 )
				{// bad expression
				  return NULL;
				}
				else
				{// have good expression
				  token = getNextToken();
    			if( token->whatKind() != 's'
	            ||
			  		 ! (token->getValue())->equals( ")" )
				    )
			    {// error --- no ) after E
					  token->destroy();
						ebox->destroy();
			      errorcode = -4008;
				    return NULL;
			    }
					else
					{// have if( E ), go ahead and build
					  token->destroy();

  				  cumbox = new box;
	  				cumbox->build( branchbox );

						// insert ebox in spot 1
						cumbox->addInnerAtFront( ebox );

						// now search on for the following statement
						sbox = processStatement( errorcode );
/*
cout << "after processStatement, line 2281, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

						if( errorcode != 0 )
						{// bad statement
						  cumbox->destroy();
							return NULL;
						}
						else
						{// have good statement after if(E)
						  // next token is else, or done --- can't be error!

              cumbox->addInnerAtCursor( sbox );

							token = getNextToken();

        			if( token->whatKind() != 'k'
	                ||
			      		  !(token->getValue())->equals( "else" )
				        )
							{// not else---so yea, we're simply done
							  putbackToken();  // looked ahead to see if was else					  
  							return cumbox;  // inner box 3 is left empty
							}
							else
							{// have if(E)S else
							  token->destroy();  // toss the else
								s2box = processStatement( errorcode );
/*
cout << "after processStatement, line 2310, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

                if( errorcode != 0 )
								{
								  cumbox->destroy();
									return NULL;
								}
								else
								{// have if(E)S1 else S2
								  cumbox->replaceInnerAt( s2box, 3 );
									return cumbox;
								}// have if(E)S1 else S2
							}// have if(E)S else
						}// have if(E)S
          }// have if( E )
				}// have if( E
			}// have if(
		}// if statement

		else if( (token->getValue())->equals( "while" ) )
		{// while statement
		  token->destroy();  // toss the "while"
			token = getNextToken();
			if( token->whatKind() != 's'
	          ||
					 ! (token->getValue())->equals( "(" )
				)
			{// error --- no ( after while
			  token->destroy();
			  errorcode = -4007;
				return NULL;
			}
			else
			{// have while(

        // jb23:  don't toss until use for previous
			  // token->destroy();  // toss the (

				ebox = processExpression( errorcode );

        // jb23:  now toss it
        token->destroy();

				if( errorcode != 0 )
				{// bad expression
				  return NULL;
				}
				else
				{// have good expression
				  token = getNextToken();
    			if( token->whatKind() != 's'
	            ||
			  		 ! (token->getValue())->equals( ")" )
				    )
			    {// error --- no ) after E
					  token->destroy();
						ebox->destroy();
			      errorcode = -4008;
				    return NULL;
			    }
					else
					{// have while( E ), go ahead and build
					  token->destroy();

  				  cumbox = new box;
	  				cumbox->build( whilebox );

						// paste ebox in spot 1
						cumbox->replaceInnerAt( ebox, 1 );

						// now search on for the following statement
						sbox = processStatement( errorcode );
/*
cout << "after processStatement, line 2384, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

						if( errorcode != 0 )
						{// bad statement
						  cumbox->destroy();
							return NULL;
						}
						else
						{// have good statement after while(E)
              cumbox->replaceInnerAt( sbox, 2 );
 							return cumbox;  // inner box 3 is left empty
						}// have while(E)S
          }// have while( E )
				}// have while( E
			}// have while(
		}// while statement

		else if( (token->getValue())->equals( "do" ) )
		{// do statement
		  token->destroy();  // toss the "do"

			sbox = processStatement( errorcode );
/*
cout << "after processStatement, line 2408, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

			if( errorcode != 0 )
			{
			  return NULL;
			}
			else
			{// have: do S
			  token = getNextToken();
   			if( token->whatKind() != 'k'
	            ||
 		  		  ! (token->getValue())->equals( "while" )
			    )
				{// missing while
				  token->destroy(); // toss the token
          sbox->destroy();
				  errorcode = -4009;
					return NULL;
				}
				else
				{// have do S while
    		  token->destroy();  // toss the "while"
		    	token = getNextToken();
    			if( token->whatKind() != 's'
	            ||
				  	  ! (token->getValue())->equals( "(" )
		    		)
			    {// error --- no ( after while
					  token->destroy();  // toss the whatever
						sbox->destroy();
			      errorcode = -4007;
				    return NULL;
    			}
		    	else
    			{// have: do S while(
    			  token->destroy();  // toss the (
		    		ebox = processExpression( errorcode );
				    if( errorcode != 0 )
    				{// bad expression
						  sbox->destroy();
		    		  return NULL;
     				}
		    		else
				    {// have good expression
    				  token = getNextToken();
        			if( token->whatKind() != 's'
	                ||
      		  		  ! (token->getValue())->equals( ")" )
				        )
			        {// error --- no ) after E
					      token->destroy();
								sbox->destroy();
    						ebox->destroy();
		    	      errorcode = -4008;
				        return NULL;
			        }
    					else
		     			{// have: do S while( E ), go ahead and build
							 //        (though if ; turns out missing, will toss)
    					  token->destroy();  // toss the )

      				  cumbox = new box;
	      				cumbox->build( dobox );

    						// paste sbox, ebox
    						cumbox->replaceInnerAt( sbox, 1 );
                cumbox->replaceInnerAt( ebox, 2 );

      				  token = getNextToken();
          			if( token->whatKind() != 's'
	                  ||
        		  		  ! (token->getValue())->equals( ";" )
				          )
								{
								  token->destroy();
									cumbox->destroy();
									errorcode = -4010;
									return NULL;
								}
								else
								{// yea!
    							return cumbox;  
								}// have:  do S while(E);
  						}// have: do S while(E)
            }// have: do S while( E 
				  }// have: do S while( 
			  }// have: do S while
			}// have do S
		}// do statement

		else if( (token->getValue())->equals( "for" ) )
		{// for statement

      token->destroy();  // toss the for
			token = getNextToken();  // get the (
 			if( token->whatKind() != 's'
          ||
	  		  ! (token->getValue())->equals( "(" )
      )
			{// missing ( after for
			  token->destroy();
				errorcode = -4011;
				return NULL;
			}

      token->destroy();  // toss the (

      // process 0 or more inits, up through ;
      initbox = processForInit( errorcode );
			if( errorcode != 0 )
			{
			  return NULL; // error in for init
			}      

			ebox = processExpression( errorcode );
			if( errorcode != 0 )
			{
			  initbox->destroy();
				return NULL;
			}

      // process ; followed by 0 or more incs, and the )
			postbox = processForPost( errorcode );
			if( errorcode != 0 )
			{
			  initbox->destroy();
				ebox->destroy();
				return NULL;
			}

/*
			token = getNextToken();  // get the )
 			if( token->whatKind() != 's'
          ||
	  		  ! (token->getValue())->equals( ")" )
      )
			{// missing ) after for posts
			  token->destroy();
				errorcode = -4012;
				return NULL;
			}

      token->destroy();  // toss the )
*/

			sbox = processStatement( errorcode );
/*
cout << "after processStatement, line 2556, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

      if( errorcode != 0 )
			{
			  initbox->destroy();
				ebox->destroy();
				postbox->destroy();
				return NULL;
			}

      // put it all together if still here!
			cumbox = new box;
			cumbox->build( forbox );
			cumbox->replaceInnerAt( initbox, 1 );
			cumbox->replaceInnerAt( ebox, 2 );
			cumbox->replaceInnerAt( sbox, 3 );
			cumbox->replaceInnerAt( postbox, 4 );

      return cumbox;

		}// for statement

		else if( (token->getValue())->equals( "return" ) )
		{// return statement

      // jb23:  destroy it later, after previous has been used
		  //  token->destroy();  // toss the return token

      // look ahead to see if is a new expression
			box* nbox = processNewExpression( errorcode );
/*
cout << "after processNewExpression, line 2596, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

      // jb23:  kill it now
      token->destroy();

			if( errorcode != 0 )
			{// inherited error
			  return NULL;
			}

			if( nbox != NULL )
			{// have a legal new expression, build the cumbox to return
  			cumbox = new box;
	  		cumbox->build( returnbox );
		  	cumbox->replaceInnerAt( nbox, 1 );
			}
			else
			{// not a new expression, go ahead with expression
        ebox = processExpression( errorcode );
	  		if( errorcode != 0 )
		  	{
				  return NULL;
			  }
  			cumbox = new box;
	  		cumbox->build( returnbox );
		  	cumbox->replaceInnerAt( ebox, 1 );
			}

      // either way, have cumbox, just need to check the ;

 			token = getNextToken();  // get the ;

 			if( token->whatKind() != 's'
          ||
	  		  ! (token->getValue())->equals( ";" )
        )
			{// missing ; after expression to return
			  token->destroy();
				cumbox->destroy();
				errorcode = -4013;
				return NULL;
			}
      // jboxes1d:  was leak here, not releasing the ; token if is good
      else
      {
        token->destroy();
      }

      return cumbox;

		}// return statement

    else
		{
		  errorexit(
			"Illegal key word found in translator::processStatement");
			return NULL;
		}

	}// if while do for return cases

	else
	{// illegal first token for statement
	  errorcode = -4001;
		return NULL;
	}
	 
}// processStatement

// process tokens after for( up through and including ;
//  with 0 or more statements that are assignments --- these
//  are the only stylistically good things to put here, right?
//     Produces either single box or seqbox
box* translator::processForInit( int& errorcode )
{
  jbtoken* token;
	int count = 0;  // number of initers inserted into cumbox
	box* cumbox = NULL;  // inited as signal not built yet
	box* idbox;
	box* rhsbox;
	box* initbox;
	box* sbox;

  // might just have a semi-colon at start
	token = getNextToken();

	if( token->whatKind() == 's'
      &&
      (token->getValue())->equals( ";" )
	  )
  {// empty initer, starts with ;
	  token->destroy();
		cumbox = new box;
		cumbox->build( emptybox );
		return cumbox;
	}
	else
	  putbackToken();

  // otherwise, expect 1 or more initers:

  do{ // process an initer or find error 

    token = getNextToken();

    if( token->whatKind() == 'i' )
		{// hit legit start of an initer

		  // get the rest of the id chain
		  idbox = processIdChain( token, errorcode );
/*
cout << "After processIdChain starting initer, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

      // might be a flat out bad id chain
			if( errorcode != 0 )
			{
			  if( cumbox != NULL )
				  cumbox->destroy();
				return NULL;
      }

			// have good id chain, make sure it's not a call
			if( idbox->whichKind() == callbox )
			{
			  if( cumbox != NULL )
				  cumbox->destroy();
				idbox->destroy();
				errorcode = -5002;
				return NULL;		 
			}

      // verify =
			token = getNextToken();

			if( token->whatKind() != 'o'
          ||
  	      ! (token->getValue())->equals( "=" )
				)
			{
			  token->destroy();
			  if( cumbox != NULL )
				  cumbox->destroy();
				idbox->destroy();
				errorcode = -5003;
				return NULL;		 
			}

			// have:   idchain =          now get rhs
		  token->destroy();
			rhsbox = processExpression( errorcode );

			if( errorcode != 0 )
			{
			  if( cumbox != NULL )
				  cumbox->destroy();
				idbox->destroy();
				return NULL;		 
			}

      // finally, verify , or ; next
			token = getNextToken();
			if( token->whatKind() != 's'
          ||
  	      ( ! (token->getValue())->equals( "," ) &&
					  ! (token->getValue())->equals( ";" )
					)
				)
			{
			  token->destroy();
			  if( cumbox != NULL )
				  cumbox->destroy();
				idbox->destroy();
				rhsbox->destroy();
				errorcode = -5004;
				return NULL;		 
			}

      // have:  idchain = rhs sep    build initbox, fasten in
			initbox = new box;
			initbox->build( assbox );

      // jboxes1d:  tiny leak fix 
      mystring* eqsign = new mystring( "=" );
			initbox->storeName( eqsign );
      eqsign->destroy();

			initbox->replaceInnerAt( idbox, 1 );
			initbox->replaceInnerAt( rhsbox, 2 );

      if( count == 0 )
			{// have just seen first initbox, make it cumbox, may be
			 // only
			  cumbox = initbox;
			}
			else if( count == 1 )
			{// have hit second initbox, switch to seqbox
        sbox = new box;
				sbox->build( seqbox );
				sbox->addInnerAtFront( cumbox );
				sbox->addInnerAtCursor( initbox );
				cumbox = sbox;			  
			}
			else
			{// add in third or more initer
			  cumbox->addInnerAtCursor( initbox );
			}

      count++;  

			if( (token->getValue())->equals( "," ) )
			{// ends with , 
			  token->destroy();
				// just spin around hoping to get next initer
			}
			else
			{// ends with ;
			  token->destroy();
				// yea!
				return cumbox;
			}
			
		}// hit id

		else
		{// illegal token 
		  token->destroy();  
			if( count > 0 )
			  cumbox->destroy(); 
      errorcode = -5001;
			return NULL;
		}// illegal token

	}while( 1 );  // will return with error or stuff; in loop body
}

// process from:  for( ... ; .... including initial ; and
//  terminal ) with sequence of 0 or more comma-separated
//  statements that are grows or op= type
//     Produces either single box or seqbox
box* translator::processForPost( int& errorcode )
{
  jbtoken* token;
  box* cumbox;
	box* idbox;
	box* rhsbox;
	box* incbox;
	box* sbox;
  mystring* opstring;

  token = getNextToken();
	
	if( token->whatKind() != 's'
      ||
      ! (token->getValue())->equals( ";" )
	)
  {
	  token->destroy();
	  errorcode = -6001;
		return NULL;
	}

  // process sequence of 0 or more incrementors separated
	// by commas, terminated by )

  int hadComma = 0;  // flag for whether ended incrementor with ,
	int count = 0;   // number of incrementors processed so far

  do{ // process an incer or find error or hit )

    token = getNextToken();

		if( token->whatKind() == 's'
        &&
	      (token->getValue())->equals( ")" ) 
			)
    {// at end, error if hadComma

		  token->destroy();  // toss the )

		  if( hadComma )
			{// error---have:   , )
				if( count > 0 )
				  cumbox->destroy();
				errorcode = -6002;
				return NULL;
			}

      if( count == 0 )
			{// never saw an incer, build emptybox now
			  cumbox = new box;
				cumbox->build( emptybox );
			}

			return cumbox;

		}// hit )

    else if( token->whatKind() == 'i' )
		{// hit legit start of an assop incrementor

		  // get the rest of the id chain
		  idbox = processIdChain( token, errorcode );
/*
cout << "After processIdChain starting assop in for loop, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

      // might be a flat out bad id chain
			if( errorcode != 0 )
			{
			  if( count > 0 )
				  cumbox->destroy();
				return NULL;
      }

			// have good id chain, make sure it's not a call
			if( idbox->whichKind() == callbox )
			{
			  if( count > 0 )
				  cumbox->destroy();
				idbox->destroy();
				errorcode = -5002;
				return NULL;		 
			}

      // verify incremental assop
			token = getNextToken();

			if( token->whatKind() != 'o'
          ||
  	      (
					  ! (token->getValue())->equals( "+=" ) &&
					  ! (token->getValue())->equals( "-=" ) &&
					  ! (token->getValue())->equals( "*=" ) &&
					  ! (token->getValue())->equals( "/=" ) &&
					  ! (token->getValue())->equals( "%=" ) 
					)
				)
			{// error --- don't have inc assop
			  token->destroy();
			  if( count > 0 )
				  cumbox->destroy();
				idbox->destroy();
				errorcode = -6003;
				return NULL;		 
			}

      // make copy of the assop
      opstring = (token->getValue())->copy();

			token->destroy();  // tidy up the assop copy

			// have:   idchain ?=          now get rhs
			rhsbox = processExpression( errorcode );

			if( errorcode != 0 )
			{
			  if( count > 0 )
				  cumbox->destroy();
				idbox->destroy();
				opstring->destroy();
				return NULL;		 
			}

      // finally, verify , or ) next
			token = getNextToken();
			if( token->whatKind() != 's'
          ||
  	      ( ! (token->getValue())->equals( "," ) &&
					  ! (token->getValue())->equals( ")" )
					)
				)
			{// didn't terminate incer correctly
			  token->destroy();
			  if( count > 0 )
				  cumbox->destroy();
				idbox->destroy();
				rhsbox->destroy();
				opstring->destroy();
				errorcode = -6004;
				return NULL;		 
			}

      // have:  idchain ?= rhs sep    build incbox, fasten in
			incbox = new box;
			incbox->build( assbox );
			incbox->storeName( opstring );
			  opstring->destroy();   // storeName makes a copy
			incbox->replaceInnerAt( idbox, 1 );
			incbox->replaceInnerAt( rhsbox, 2 );

      if( count == 0 )
			{// have just seen first incbox, make it cumbox, may be
			 // only
			  cumbox = incbox;
			}
			else if( count == 1 )
			{// have hit second incbox, switch to seqbox
        sbox = new box;
				sbox->build( seqbox );
				sbox->addInnerAtFront( cumbox );
				sbox->addInnerAtCursor( incbox );
				cumbox = sbox;			  
			}
			else
			{// add in third or more incer
			  cumbox->addInnerAtCursor( incbox );
			}

      count++;  

			if( (token->getValue())->equals( "," ) )
			{// ends with , 
			  token->destroy();
				hadComma = 1;    // note just saw a comma
			}
			else
			{// ends with )
			  putbackToken();  // will exit happy on next iteration
				hadComma = 0;   // didn't JUST see comma
			}
			
		}// hit id

    else if( token->whatKind() == 'o'
             &&
  	         ( (token->getValue())->equals( "++" ) ||
					     (token->getValue())->equals( "--" )
					   )
		       )
		{// ++ or --

      opstring = (token->getValue())->copy(); // save the op
			token->destroy();

      token = getNextToken();
			
      if( token->whatKind() != 'i' )
			{
			  token->destroy();
				if( count > 0 )
				  cumbox->destroy();
				opstring->destroy();
			  errorcode = -6006;
				return NULL;
			}
									
		  // get the rest of the id chain
		  idbox = processIdChain( token, errorcode );
/*
cout << "After processIdChain grow in for loop, have " << 
                 reptok->numberUsed()
              << " tokens in existence" << endl;
*/

      // might be a flat out bad id chain
			if( errorcode != 0 )
			{
			  if( count > 0 )
				  cumbox->destroy();
				opstring->destroy();
				return NULL;
      }

			// have good id chain, make sure it's not a call
			if( idbox->whichKind() == callbox )
			{
			  if( count > 0 )
				  cumbox->destroy();
				idbox->destroy();
				opstring->destroy();
				errorcode = -6007;
				return NULL;		 
			}

      // verify , or ) next
			token = getNextToken();

			if( token->whatKind() != 's'
          ||
  	      ( ! (token->getValue())->equals( "," ) &&
					  ! (token->getValue())->equals( ")" )
					)
				)
			{// didn't terminate incer correctly
			  token->destroy();
			  if( count > 0 )
				  cumbox->destroy();
				idbox->destroy();
				opstring->destroy();
				errorcode = -6004;
				return NULL;		 
			}

      // have:  ++ idchain sep    build incbox, fasten in
			incbox = new box;
			incbox->build( growbox );
			incbox->storeName( opstring );
			  opstring->destroy();   // storeName makes a copy
			incbox->replaceInnerAt( idbox, 1 );
			
      if( count == 0 )
			{// have just seen first incbox, make it cumbox, may be
			 // only
			  cumbox = incbox;
			}
			else if( count == 1 )
			{// have hit second incbox, switch to seqbox
        sbox = new box;
				sbox->build( seqbox );
				sbox->addInnerAtFront( cumbox );
				sbox->addInnerAtCursor( incbox );
				cumbox = sbox;			  
			}
			else
			{// add in third or more incer
			  cumbox->addInnerAtCursor( incbox );
			}

      count++;  

			if( (token->getValue())->equals( "," ) )
			{// ends with , 
			  token->destroy();
				hadComma = 1;    // note just saw a comma
			}
			else
			{// ends with )
			  putbackToken();  // will exit happy on next iteration
				hadComma = 0;
			}
       					  
		}// ++ or --

		else 
		{// illegal token 
		  token->destroy();  
			if( count > 0 )
			  cumbox->destroy(); 
      errorcode = -6005;
			return NULL;
		}// illegal token

	}while( 1 );  // will return with error or stuff; in loop body

}// processForPost

// this guy is special sort of expression, needs to be
//  allowed after return, after =, and in arguments in
//  process id chain
//  Just to keep it pure, must do like x = new int[3];
//   disallow (new int[3])
box* translator::processNewExpression( int& errorcode )
{
	jbtoken* token;

	token = getNextToken();

/*
cout << "Start processNewExpression, have " << 
                 reptok->numberUsed()
                 << " tokens in existence" << endl;
*/

  if( token->whatKind() != 'k'
      ||
      !(token->getValue())->equals( "new" )
		)
	{// lookahead didn't find new, restore it and quit
	  putbackToken();  // put back the non-new token
		return NULL;
	}

	// starts with a new, go on to produce new box or error

  token->destroy();  // toss the "new" 

	token = getNextToken();  // should be the type name

  // just for convenience
  char knd = token->whatKind();
	mystring* val = token->getValue();

	if( knd!='i'   // expecting a class name
	    &&
			(knd!='k' ||
			 (! val->equals("boolean") &&
			  ! val->equals("char") &&
			  !	val->equals("float") &&
				! val->equals("int") &&
				! val->equals("string")
			 )
			)
	  )
  {// can't be a type name
	  token->destroy();
    errorcode = -7001;
		return NULL;
	}

  // go ahead and optimistically start building the box
	box* nbox;
	box* indexbox;
	box* index2box;
	
	nbox = new box;
	nbox->build( newopbox );
	
	nbox->storeName( token->getValue() ); 

  token->destroy();   // jb2006a (comment):  toss the <new>

	token = getNextToken();

// jb2006a:  replace this code, which looks for just ;
//           by new version that looks for ();
/*
  if( token->whatKind() == 's' &&
	    (token->getValue())->equals(";") 
		)
	{// have just a single guy requested
	  putbackToken();
		return nbox;
	}
*/

  // jb2006a:  look for (, internally look for ) and then ; or report error
  if( token->whatKind() == 's' &&
        (token->getValue())->equals("(")
    )
  {// saw ( after new
    // now look for matching ) immediately:
    token->destroy();  // return the <(> token to heap
    token = getNextToken();
    if( token->whatKind() == 's' &&
        (token->getValue())->equals(")")
      )
    {// saw ) so now have new (
      token->destroy();  // release the <)> token
      token = getNextToken();  
      if( token->whatKind() == 's' &&
          (token->getValue())->equals(";") 
        )
	{// have seen the ; so happy
	  putbackToken();
          return nbox;
	}// have seen the ; so happy
        else
        {// error:  saw non semi-colon after new <type>()
          token->destroy();  
          errorcode = -7006;
          return NULL;
        }// error:  saw non semi-colon after new <type>()
    }// saw ) so now have new (
    else
    {// needed ), didn't get so is error
      token->destroy();
      errorcode = -7005;
      return NULL;
    }// needed ), didn't get so is error
  }// looking for ( after new
// jb2006a --- end of modified block

  else if( token->whatKind() == 's' &&
		    (token->getValue())->equals( "[" ) )
	{// [E] or [E][E]

		token->destroy();  // done with the [

	  // after [, need index expression
		indexbox = processExpression( errorcode );

		if( errorcode != 0 )
		{// error in first index
		  nbox->destroy();
		  return NULL;
		}

    // have first dimension, slap it in
    nbox->replaceInnerAt( indexbox, 1 );

	  token = getNextToken();
		
		if( token->whatKind() != 's' ||
		    !(token->getValue())->equals( "]" )
			)
		{// missing ]
		  token->destroy();
			nbox->destroy();
		  errorcode = -7002;
			return NULL;
		}

	  token->destroy();

		token = getNextToken(); // look ahead for another [

		if( token->whatKind() != 's' ||
		    !(token->getValue())->equals( "[" )
			)
		{// fine, just had the one dim, finish up
		  putbackToken();  // should be ; by the way
			return nbox;
		}

	  token->destroy();  // was the initial [ of second index

		index2box = processExpression( errorcode );

    if( errorcode != 0 )
		{// error in second index
		  nbox->destroy(); 
			return NULL;
    }

    nbox->replaceInnerAt( index2box, 2 );

	  token = getNextToken();

   	if( token->whatKind() != 's' ||
        !(token->getValue())->equals( "]" )
      )
 		{// missing ]
   	  token->destroy();
			nbox->destroy();
		  errorcode = -7003;
			return NULL;
    } 

    token->destroy();

    return nbox;

  }// [E] or [E][E]
	
	else
	{// other than ; or [ after type id, is error
    token->destroy();
		errorcode = -7004;
		return NULL;	  
	}
	  
}
