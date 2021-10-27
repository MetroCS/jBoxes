#include <stdio.h>
#include "mystring.h"
#include "jbtoken.h"  
#include <iostream>

int numTokens = 0;

    // default constructor
	jbtoken::jbtoken()
	{
	  kind = ' ';
	  value = new mystring("");
		error = 0;  // normally is no error
		position = -1;  // no position if no error

    numTokens++;
	}

	// append given char to self
	void jbtoken::append( char x )
	{
	  value->insertCharAt( value->length()+1, x );
	}

    // set kind to specified char
	void jbtoken::setKind( char x )
	{
	  kind = x;
	}

  void jbtoken::setError( int code, int where )
	{
	  error = code;
		position = where;
	}

  int jbtoken::getError()
	{
	  return error;
	}

  char jbtoken::whatKind()
	{
	  return kind;
	}

    // determine whether my value is equal to s
   int jbtoken::equals( char* s )
	{
      return value->equals( s );
	}

    // output the token to screen (for debugging)
	void jbtoken::show()
	{
	  int k;
	  std::cout << "<" << kind << ",";
	  for( k=1; k<=value->length(); k++ )
	    if( ' '<=value->charAt(k) && value->charAt(k)<='~' )
				  std::cout << value->charAt(k);
		else
	 	  std::cout << "[" << (int) value->charAt(k) << "]";
		std::cout << ">";
		if( error != 0 )
		  std::cout << " (error code = " << error << 
                      ", at pos: " <<
			           position << ")";

	}

  // return whether this token is the kind that precedes a
	//  unary minus
	//   (not being preUnaryMinus means is pre binary minus, or
	//    an error, but we make no attempt to notice errors here)
	int jbtoken::preUnaryMinus()
	{
	  if( (kind=='k' && value->equals("return") ) ||
		    (kind=='s' && (value->equals("(") || 
				               value->equals("[") ||
											 value->equals(",")
											)                       ) ||
				kind=='o'
		  )
			return 1;
		else
		  return 0;
	}

  // release resources used by this guy, especially the 
	//  value mystring
	void jbtoken::destroy()
	{
	  value->destroy();
		delete this;
    numTokens--;
	}

	mystring* jbtoken::getValue()
	{
	  return value;
	}

  int jbtoken::numberUsed()
  {
    return numTokens;
  }
