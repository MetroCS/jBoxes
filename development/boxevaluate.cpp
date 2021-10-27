// evaluate.cpp:  this file holds just the one box::evaluate method

  // evalute/execute myself, depending on my kind
  //  (called by viewer::step, has access to box innards)
  //    must:       return status = 0 if fails somehow
  //                set source, value, type
  //                figure out correct aspect
  //                note boxes for inspection
  //
  //   a few boxes can be evaluated either as a location or
  //   a value---do the next step to the value if getvalue==1
  //
  //   some kinds use getvalue to provide info, like newop
  //   uses 0,1,2 to tell expected dimensionality of guy to build,
  //   since already figured out by viewer::step
  //
  //   if there is a user program error, return special status
  //   code < 0 leading to correct error message when get back
  //   to viewer::step
  //
  // purely for efficiency, pass in the universe box as u
  // so can get to list of classes, heap, stack, console with
  // little effort

  // newer feature:  return me if okay, return the bad box
  //   if find one---it might be inside of me, more accurate
  //   error reporting if can show it
	//
	//  new scheme:  whenever things happen in evaluation process
	//               that should be toured, put them in
	//
	//   sets ilist,cclist,ni to list of boxes and colors to be toured,
	//    whoever calls evaluate needs to transfer these into
	//     the tour

//------------------------------------------------------------------
// jb2006a:  use myRand() for random and an externally visible
//           data storage
long currentRNGValue = 1;

int myRand()
{
  // apply Schrage formula to currentRNGValue to get next one
  long x = currentRNGValue;  // just for convenience

  long a=16807, b15=32768, b16=65536, p=2147483647;

  long xhi = x/b16;
  long xalo = (x-xhi*b16)*a;
  long leftlo = xalo/b16;
  long fhi = xhi*a + leftlo;
  long k = fhi/b15;

  x = (((xalo-leftlo*b16)-p)+(fhi-k*b15)*b16) + k;

  if( x < 0 )
    x += p;

  currentRNGValue = x;

  return currentRNGValue % 100000000;
}

void mySrand( int seed )
{
  currentRNGValue = seed;
}
//----------------------------- end of jb2006A additions----------

  box* box::evaluate( box* u, int getvalue, int& status,
	                  box* ilist[], int cclist[], int& ni )
  {

//    #include "evaluate.cpp"
	  // traceEnter( tid, "box:evaluate" );

    box* codebox;
    box* cimbox;
    box* curbox;
    box* prevbox;
    box* firstbox;
    box* secondbox;
    box* thirdbox;
    box* methbox;
    box* stkbox;
    box* newbox;
    box* pbox;
    box* gbox;
    
    // minor utility variables
        long iresult;     // place to put the answers
        long iresult2;
        double dresult;
// old:        string* sresult;
        char answer[100];
        char op;
        int code;
        int count;
        int k;
        int junk;

    // just in case:

    status = 1;  // unless flag an error later

    ni = 0;  // so only have to set this for ones that tour somebody

    if( kind == callbox )
    {// my inners are ready, I need to do a lot of actions, but
      // nothing too startling!

      status = 1;  // monitor whether changes

      firstbox = first;  // this is the method "location"

      if( (firstbox->name)->charAt( 1 ) == '_' )
      {// is a system call --- will do it right here and now!

        // for efficiency, categorize the bif right here
        //  Note:  in idbox case below, checked to make sure
        //         the name was legitimate, here we're just
        //         simplifying various cases 
        int bifcode;

        if( firstbox->name->equals("_moveTo") )
          bifcode = moveTocode;
        else if( firstbox->name->equals("_print") )
          bifcode = printcode;
        else if( firstbox->name->equals("_char") )
          bifcode = charcode;
				else if( firstbox->name->equals("_ascii") )
				  bifcode = asciicode;
        else if( firstbox->name->equals("_get") )
          bifcode = getcode;
        else if( firstbox->name->equals("_getLine") )
          bifcode = getLinecode;
        else if( firstbox->name->equals("_clear") )
          bifcode = clearcode;
        else if( firstbox->name->equals("_random") )
          bifcode = randomcode;
        else if( firstbox->name->equals("_time") )  // jb2006a:  add _time
          bifcode = timecode;
	    else if( firstbox->name->equals("_seed") )
		  bifcode = seedcode;
        else if( firstbox->name->equals("_abs") )
				  bifcode = abscode;
        else if( firstbox->name->equals("_sqrt") )
				  bifcode = sqrtcode;
        else if( firstbox->name->equals("_sin") )
				  bifcode = sincode;
        else if( firstbox->name->equals("_cos") )
				  bifcode = coscode;
				else if( firstbox->name->equals("_tan") )
				  bifcode = tancode;
				else if( firstbox->name->equals("_asin") )
				  bifcode = asincode;
				else if( firstbox->name->equals("_acos") )
				  bifcode = acoscode;
        else if( firstbox->name->equals("_atan") )
				  bifcode = atancode;
        else if( firstbox->name->equals("_exp") )
				  bifcode = expcode;
        else if( firstbox->name->equals("_log") )
				  bifcode = logcode;
        else if( firstbox->name->equals("_floor") )
				  bifcode = floorcode;
        else if( firstbox->name->equals("_ceil") )
				  bifcode = ceilcode;
        else if( firstbox->name->equals("_intToString") )
				  bifcode = intToStringcode;
        else if( firstbox->name->equals("_floatToString") )
				  bifcode = floatToStringcode;
        else if( firstbox->name->equals("_charToString") )
				  bifcode = charToStringcode;
        else if( firstbox->name->equals("_charsToString") )
				  bifcode = charsToStringcode;
				else if( firstbox->name->equals("_progChar") )
				  bifcode = progCharcode;
        else if( firstbox->name->equals("_isInt") )
				  bifcode = isIntcode;
        else if( firstbox->name->equals("_isFloat") )
				  bifcode = isFloatcode;
        else if( firstbox->name->equals("_toInt") )
				  bifcode = toIntcode;
        else if( firstbox->name->equals("_toFloat") )
				  bifcode = toFloatcode;
        else if( firstbox->name->equals("_length") )
				  bifcode = lengthcode;
        else if( firstbox->name->equals("_charAt") )
				  bifcode = charAtcode;
//        else if( firstbox->name->equals("_copy") )
//				  bifcode = copycode;
        else if( firstbox->name->equals("_open") )
				  bifcode = opencode;
        else if( firstbox->name->equals("_close") )
				  bifcode = closecode;
        else if( firstbox->name->equals("_fget") )
				  bifcode = fgetcode;
        else if( firstbox->name->equals("_fput") )
				  bifcode = fputcode;
        else if( firstbox->name->equals("_eof") )
				  bifcode = eofcode;
        else if( firstbox->name->equals("_whatRow") )
				  bifcode = whatRowcode;
        else if( firstbox->name->equals("_whatCol") )
				  bifcode = whatColcode;
        else if( firstbox->name->equals("_hideCursor") )
				  bifcode = hideCursorcode;
        else if( firstbox->name->equals("_showCursor") )
				  bifcode = showCursorcode;
        else if( firstbox->name->equals("_halt") )
				  bifcode = haltcode;
        else if( firstbox->name->equals("_null") )
				  bifcode = nullcode;
        else if( firstbox->name->equals("_this") )
				  bifcode = thiscode;
        else if( firstbox->name->equals("_destroy") )
				  bifcode = destroycode;
        else
        {// unknown system method, so exit
          errorexit("unknown bif detected in callbox of evaluate");
        }

        if( bifcode == moveTocode )
        {// guys that take exactly two int args
          
          // move the console cursor to specified row, column
          // (and later, any bifs that take exactly two int args)

          // make sure there are the right number of arguments
	        if( innerLength() != 3 )
          {
      	    status = need2argserror;
               // jb21: this is ridiculous:  status = divbyzeroerror;
              // jb21:  compiler warned line below silly --- fix it
              //return secondbox;
              return this;
          } 

      	  // grab my two inners
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg
          secondbox = firstbox->next;

          // detect bad operands:

					long fi, si;

          if( ! firstbox->haveIntValue( fi ) ) 
          {
            status = notintegererror;
        		// traceLeave();
            return firstbox;
          }

          if( ! secondbox->haveIntValue( si ) )
          {
            status = notintegererror;
        		// traceLeave();
            return secondbox;
          } 
        
          // have fi, si, go ahead and do the action
      	  if( ! chargrid->moveCursor( fi, si ) )
          { 
	          status = illegalcursorlocationerror;
		    		// traceLeave();
            return this;
          } 

          // strip consumed inners
          first->stripValue();
          firstbox->stripValue();
          secondbox->stripValue();

          status = 2;  // signal special system call case

          // here's where, depending on bifcode, we
					// take specific actions and possibly tour
          if( bifcode == moveTocode )
          {// guys with two int args that affect console
            needredraw = 1;  // set global so will redraw the
                           // world even if wouldn't otherwise

						// no box put on tour

          }
          			    
        	// traceLeave();
          return this;

        }// guys that take two int arguments

        else if( bifcode == charcode )
        {// guys that take exactly one int arg 
        
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

          // detect bad operand:
          if( ! firstbox->haveIntValue(fi) ) 
          {
            status = notintegererror;
        		// traceLeave();
            return firstbox;
          }
 
          // depending on bif, finish evaluation and do tour
					if( bifcode == charcode )
					{
            // have fi, go ahead and produce the value

            if( ! legalsymbolcode( fi ) )
            { 
	            status = nosuchcharerror;
		    		  // traceLeave();
              return this;
            } 

            // the value of the call becomes the symbol 
						// 7/31/2000:  problem with this way of storing char # 0
						if( fi > 0 )
						{// usual case
              char verytemp[2];
              verytemp[0] = fi;  verytemp[1] = '\0';
              value = storeCharString( value, verytemp );
						}
						else
						{// create string with just symbol 0 specially
              value = storeCharString( value, "a" );
							value->replaceCharAt( 1, '\0' );						  
						}

            type = storeCharString( type, "char" );

            // since produced value, maybe show it
            if( aspect == 'f' )  // full aspect means I need to show
            {                        // my newly created "value"
              setAspect( 'v' );
            }

            fixupReality(0);

            status = 3;// signal special system call case---with value

            // put the box on tour
						ilist[ni] = this;
						cclist[ni] = newvaluecolorcode;
						ni++;

					}// do action for charcode

          // strip consumed inner in all cases
          first->stripValue();
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// guys with one int argument

        else if( bifcode == asciicode )
        {// given a char, produce its ascii code
        
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          // detect bad operand:
          if( !(firstbox->type)->equals("char") )
					{
					  status = notcharerror;
						// traceLeave();
						return firstbox;
					}

          // finish evaluation and do tour

          char verytempstring[30]; // store result
          long fi;

          // firstbox's value is a single char, get it and
					// change to int
					fi = (int) (firstbox->value)->charAt( 1 );

				  sprintf( verytempstring, "%ld", fi );
					value = storeCharString( value, verytempstring );
          type = storeCharString( type, "int" );
        
          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;// signal special system call case---with value

          // put the box on tour
  				ilist[ni] = this;
 					cclist[ni] = newvaluecolorcode;
					ni++;

					// strip consumed inner in all cases
          first->stripValue();
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// ascii --- takes a char, produces its code

        else if( bifcode == printcode )
        {// allows any number of arguments, of any type

          //v13:  hey, "of any type" sounds bad!
					//   And I meant it---this is hold over from days of
					//     confusion about "literal" --- clearly only want
					//     to allow printing of primitive types!

          // loop to access all inners and print them
          curbox = first;
          curbox = curbox->next;  // first arg or NULL
          while( curbox != NULL )
          {

					  // v13:  check primitiveness of type before print
            if( (curbox->type == NULL)
						     ||
						     ! isprimtype( curbox->type ) )
	  				{
		  			  status = badprimvalueerror;
			  			// traceLeave();
				  		return curbox;
					  }

						chargrid->print( curbox->value );
            curbox = curbox->next;
          }
          // loop to strip all consumed inner values
          curbox = first;
					curbox->stripValue();  // was missing this!
          curbox = curbox->next;
          while( curbox != NULL )
          {
            curbox->stripValue();
            curbox = curbox->next;
          }

          status = 2;  // signal special system call case

          // depending on the bif, may need to redraw the universe
          // (so that change to console shows up)
          if( bifcode == printcode )
            needredraw = 1;

          // note:  no tour for print --- if running as should in
					//         split screen, is obvious what happened

          // traceLeave();
          return this;
        
        }// guys with any number of free string arguments

        else if( bifcode == getcode || bifcode == getLinecode )
        {// input guys

          // make sure there are the right number of arguments
	        if( innerLength() != 1 )
          {
      	    status = neednoargserror;
		      	// traceLeave();
            return this;
          } 

          // just signal back need to wait for input
          if( bifcode == getcode )
            status = 4;
          else 
          {
            inputlength = 0;  // global --- start with nothing
            status = 5;
          }

          // note:  no touring for these guys---is done later
					//        once inputstring is available

          // traceLeave();
          return this;

        }// input guys

        else if( bifcode == clearcode ||
				         bifcode == hideCursorcode ||
								 bifcode == showCursorcode
							 )
        {// no arguments, produces no value

          // make sure there are the right number of arguments
	        if( innerLength() != 1 )
          {
      	    status = neednoargserror;
		      	// traceLeave();
            return this;
          } 

          // do it!
					if( bifcode == clearcode )
            chargrid->clear();
					else if( bifcode == hideCursorcode )
					{
            chargrid->hideCursor();
					}
					else if( bifcode == showCursorcode )
					{
					  chargrid->showCursor();
					}

          status = 2; // no value code

          // need to strip the method name 
					first->stripValue();

          // no tour, like for print and moveTo

          // traceLeave();
          return this;

        }// no arguments, no value
				
				else if( bifcode == randomcode || 
                         bifcode == timecode ||   // jb2006a:  adding _time
				         bifcode == whatRowcode ||
								 bifcode == whatColcode 
				    )
        {// no args, produces a value

          // make sure there are the right number of arguments
	        if( innerLength() != 1 )
          {
      	    status = neednoargserror;
		     		// traceLeave();
            return this;
          } 

          char verytempstring[30]; // to hold result

          if( bifcode == randomcode )
					{// the value of the call becomes the next random number 
           // jb2006a: replace: sprintf( verytempstring, "%ld", rand() );
            sprintf( verytempstring, "%ld", myRand() );
					}
          else if( bifcode == timecode )
          {
               time_t when;
               int now = time(&when);
               now = now % 1000000000;
//               std::cout << now << std::endl;
               sprintf( verytempstring, "%ld", now );
          }
					else if( bifcode == whatRowcode )
					{
					  sprintf( verytempstring, "%ld", chargrid->whatRow() );
					}
					else if( bifcode == whatColcode )
					{ 
					  sprintf( verytempstring, "%ld", chargrid->whatCol() );       
					}

          value = storeCharString( value, verytempstring );

          type = storeCharString( type, "int" );

          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                       // my newly created "value"
            setAspect( 'v' );
          }
          fixupReality(0);
          // strip consumed inner
          first->stripValue();
          status = 3;  // signal special system call case---with value

					// put this box with its new value on tour
					ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;

          // traceLeave();
          return this;
        }// no args, gives int value

        else if( bifcode == seedcode )
        {// one integer argument in range, no value produced
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 2 args, must be 
					//  first:  integer, between 1 and 32767

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

				  if( !( firstbox->haveIntValue( fi ) ) )
					{// first arg is not an integer
					  status = notintegererror;
            // traceLeave();
						return firstbox;
					}

          if( fi < 1 || 32767 < fi )
					{
					  status = argoutofrangeerror;
						// traceLeave();
						return firstbox;
					}

          // have syntactically okay seed value fi,
					// go ahead and seed it 
			// jb2006a:  replace: srand( (unsigned) fi );
                      mySrand( (unsigned) fi );

          status = 2;  // system call with no value

					// strip consumed inners
					first->stripValue(); 
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// seed

        else if( bifcode == abscode ||
				         bifcode == sqrtcode ||
								 bifcode == sincode ||
								 bifcode == coscode ||
								 bifcode == atancode ||
								 bifcode == floorcode ||
								 bifcode == ceilcode ||
								 bifcode == expcode ||
								 bifcode == logcode ||
								 bifcode == tancode ||
								 bifcode == asincode ||
								 bifcode == acoscode
						  )
        {// guys that take exactly one numeric arg
        
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;
					double fd;

					long coerced;

          // detect bad operand:
          if( ! firstbox->haveFloatValue( fd ) &&
					    ! firstbox->haveIntValue( fi ) )
          {
            status = cantcoercetofloaterror;
        		// traceLeave();
            return firstbox;
          }

          // either way, convert value string to double fd 
          coerced = (firstbox->value)->toFloat( fd );

          // depending on bif, finish evaluation and do tour
					  //  have argument in fd, in each case produce answer
						//  in ansd or ansi, or exit with error code

          char verytempstring[30]; // store result
					 
					if( bifcode == abscode )
					{// no error possible here
            sprintf( verytempstring, "%.12g", fabs( fd ) );
            type = storeCharString( type, "float" );
 					}
					else if( bifcode == sqrtcode )
					{// must have >= 0 arg
					  if( fd >= 0 )
						{
						  sprintf( verytempstring, "%.12g", sqrt( fd ) );
              type = storeCharString( type, "float" );
						}
						else
						{
						  status = negativeargerror;
							// traceLeave();
							return firstbox;
						}
					}
					else if( bifcode == logcode )
					{// must have >= 0 arg
					  if( fd > 0 )
						{
						  sprintf( verytempstring, "%.12g", log( fd ) );
              type = storeCharString( type, "float" );
						}
						else
						{
						  status = argoutofrangeerror;
							// traceLeave();
							return firstbox;
						}
					}
					else if( bifcode == sincode )
					{// any arg is cool
					  sprintf( verytempstring, "%.12g", sin( fd ) );
	          type = storeCharString( type, "float" );
    			}
					else if( bifcode == coscode )
					{// any arg is cool
					  sprintf( verytempstring, "%.12g", cos( fd ) );
	          type = storeCharString( type, "float" );
    			}
					else if( bifcode == tancode )
					{// must have cos not too large
					  if( fabs( sin(fd) ) <= fabs( cos(fd) ) * 1e16 )
 					  {
						  sprintf( verytempstring, "%.12g", tan( fd ) );
              type = storeCharString( type, "float" );
						}
						else
						{
						  status = argoutofrangeerror;
							// traceLeave();
							return firstbox;
						}
					}
					else if( bifcode == asincode )
					{// must have arg in [-1,1]
					  if( -1<=fd && fd<=1 )
 					  {
						  sprintf( verytempstring, "%.12g", asin( fd ) );
              type = storeCharString( type, "float" );
						}
						else
						{
						  status = argoutofrangeerror;
							// traceLeave();
							return firstbox;
						}
					}
					else if( bifcode == acoscode )
					{// must have arg in [-1,1]
					  if( -1<=fd && fd<=1 )
 					  {
						  sprintf( verytempstring, "%.12g", acos( fd ) );
              type = storeCharString( type, "float" );
						}
						else
						{
						  status = argoutofrangeerror;
							// traceLeave();
							return firstbox;
						}
					}				
					else if( bifcode == atancode )
					{
					  sprintf( verytempstring, "%.12g", atan( fd ) );
		        type = storeCharString( type, "float" );
    			}
					else if( bifcode == expcode )
					{
					  if( fd <= 500 )
						{
						  sprintf( verytempstring, "%.12g", exp( fd ) );
              type = storeCharString( type, "float" );					
						}
						else
						{
						  status = argoutofrangeerror;
							// traceLeave();
							return firstbox;
						}
					}
					else if( bifcode == floorcode )
					{
					  sprintf( verytempstring, "%ld", (long) floor(fd) );
	          type = storeCharString( type, "int" );
					}
					else if( bifcode == ceilcode )
					{
					  sprintf( verytempstring, "%ld", (long) ceil(fd) );
	          type = storeCharString( type, "int" );
					}
					
          // actually store value and type for the call box
          value = storeCharString( value, verytempstring );

          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;// signal special system call case---with value

          // put the box on tour
  				ilist[ni] = this;
 					cclist[ni] = newvaluecolorcode;
					ni++;

					// strip consumed inner in all cases
          first->stripValue();
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// guys with one float argument, produce float or int value

//jer start new zone
        else if( bifcode == intToStringcode ||
                 bifcode == floatToStringcode ||
								 bifcode == charToStringcode ||
								 bifcode == isIntcode ||
								 bifcode == isFloatcode ||
								 bifcode == toIntcode ||
								 bifcode == toFloatcode ||
								 bifcode == lengthcode 
							 )

        {// guys that take exactly one string (except for toString)
           // v14:  huh?  this comment must be wrong --- actually,
           //   these are all guys that have one string
           //   argument and
           //   produce a value of various types
        
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          // detect bad operand:
          if( (firstbox->value == NULL ) ||  // v14:  catch having none
              (firstbox->type == NULL ) ||
              ((!(firstbox->type)->equals("string") )
							 &&
			         bifcode != intToStringcode  &&
							 bifcode != floatToStringcode &&
							 bifcode != charToStringcode)
			      ) 
          {
            status = notastringerror;
        		// traceLeave();
            return firstbox;
          }
 
          if( bifcode == intToStringcode &&
					    !(firstbox->type)->equals("int") )
					{
					  status = notintegererror;
						// traceLeave();
						return firstbox;
					}
          if( bifcode == floatToStringcode &&
					    !(firstbox->type)->equals("float") &&
							!(firstbox->type)->equals("int")       )
					{
					  status = notfloaterror;
						// traceLeave();
						return firstbox;
					}
          if( bifcode == charToStringcode &&
					    !(firstbox->type)->equals("char") )
					{
					  status = notcharerror;
						// traceLeave();
						return firstbox;
					}

          // depending on bif, finish evaluation and do tour

          char verytempstring[30]; // store result
          long fi;
					double fd;
										 
					if( bifcode == intToStringcode ||
					    bifcode == floatToStringcode ||
							bifcode == charToStringcode 
						)
					{
            storeValue( firstbox->value );
            type = storeCharString( type, "string" );
 					}
					else if( bifcode == isIntcode )
					{
					  if( (firstbox->value)->toInt( fi ) )
              value = storeCharString( value, "true" );
						else
						  value = storeCharString( value, "false" );
            type = storeCharString( type, "boolean" );
					}
					else if( bifcode == isFloatcode )
					{
					  if( (firstbox->value)->toFloat( fd ) )
              value = storeCharString( value, "true" );
						else
						  value = storeCharString( value, "false" );
            type = storeCharString( type, "boolean" );
					}
					else if( bifcode == toIntcode )
					{
					  if( (firstbox->value)->toInt( fi ) )
						{// legal int
  					  sprintf( verytempstring, "%ld", fi );
							value = storeCharString( value, verytempstring );
	            type = storeCharString( type, "int" );
						}
						else
						{
              status = notintegerformerror;
          		// traceLeave();
              return firstbox;
						}
					}
					else if( bifcode == toFloatcode )
					{
					  if( (firstbox->value)->toFloat( fd ) )
						{// legal float
  					  sprintf( verytempstring, "%.12g", fd );
							value = storeCharString( value, verytempstring );
	            type = storeCharString( type, "float" );
						}
						else
						{
              status = notfloatformerror;
          		// traceLeave();
              return firstbox;
						}
					}

					else if( bifcode == lengthcode )
					{
 					  sprintf( verytempstring, 
						    "%ld", (firstbox->value)->length() );
						value = storeCharString( value, verytempstring );
            type = storeCharString( type, "int" );
					}			
        
          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;// signal special system call case---with value

          // put the box on tour
  				ilist[ni] = this;
 					cclist[ni] = newvaluecolorcode;
					ni++;

					// strip consumed inner in all cases
          first->stripValue();
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// guys with one string argument, produce some value

        else if( bifcode == charsToStringcode )
        {// special guy that takes a char[ argument and an int arg
				  // telling how many to use and produces
				 // corresponding string value
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 3 )
          {
      	    status = need2argserror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my first inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg
					                           // which holds the gridbox ref

          // detect bad operand:
          if( !(firstbox->type)->equals("char[")  ) 
          {
            status = notachararrayerror;
        		// traceLeave();
            return firstbox;
          }
 
          long fi;

					secondbox = firstbox->next;

          // detect bad operand:
          if( ! secondbox->haveIntValue(fi) ) 
          {
            status = notintegererror;
        		// traceLeave();
            return secondbox;
          }

          // figure string value and do tour---after another arg check

          // get the gridbox 
          gbox = findInHeap( u, firstbox->value );

					if( fi < 0 || gbox->innerLength() < fi )
					{
					  status = argoutofrangeerror;
					  // traceLeave();
						return secondbox;
					}

          // scan gbox and pop fi chars into thisbox's value
				  value = storeCharString( value, "" ); // start empty
					int index = 0;  // index where just put char
					                // and count of # extracted so far
					curbox = gbox->first;
					while( index < fi && curbox != NULL )
					{
					  index++;
					  value->insertCharAt( index, 
						                  (curbox->value)->charAt( 1 ) );
					  curbox = curbox->next;
					}

          type = storeCharString( type, "string" );

          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;// signal special system call case---with value

          // put the box on tour
  				ilist[ni] = this;
 					cclist[ni] = newvaluecolorcode;
					ni++;

					// strip consumed inner in all cases
          first->stripValue();
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// charsToString

        else if( bifcode == progCharcode )
				{// special guy --- takes int and string

          // make sure there are the right number of arguments
	        if( innerLength() != 3 )
          {
      	    status = need2argserror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my inners
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

          // detect bad operand:
          if( !(firstbox->type)->equals("int") ||
					    !(firstbox->haveIntValue( fi ) )
						)
          {
            status = notintegererror;
        		// traceLeave();
            return firstbox;
          }

          if( fi < minProgCode || 255 < fi )
					{
					  status = progcharrangeerror;
						// traceLeave();
						return firstbox;
					}

          secondbox = firstbox->next;

          if( !(secondbox->type)->equals("string") )
          {
            status = notastringerror;
        		// traceLeave();
            return secondbox;
          }

          // now, scan secondbox->value, checking legality
          int legal = 1;  // innocent until proven guilty
          int k = 1;
					int n = (secondbox->value)->length();
          char com, x1, y1, x2, y2;

          while( legal && k <= n )
					{// see if stuff starting with command at index k is legal
    
            com = (secondbox->value)->charAt( k );
 
            // grab args depending on command
            if( (com=='L' || com=='R' || com=='F') &&
						     k+4 <= n 
							)
						{// guys with 4 args
						  x1 = base36ToInt((secondbox->value)->charAt( k+1 ));
						  y1 = base36ToInt((secondbox->value)->charAt( k+2 ));
						  x2 = base36ToInt((secondbox->value)->charAt( k+3 ));
						  y2 = base36ToInt((secondbox->value)->charAt( k+4 ));
							k += 5;
						}
						else
						{
						  std::cout << 
                            "Illegal command at index " << k << 
                               std::endl;
						  legal = 0;  
						}

            // check legality of args
            if( com == 'L' || com=='R' || com=='F' )
						{
              // 2/2001:  silly to allow -1, apparently!
              if( x1 < 0 || 16 < x1 || 
							    x2 < 0 || 16 < x2 || 
									y1 < 0 || 18 < y1 ||
									y2 < 0 || 18 < y2 
								)
							{
							  std::cout << 
                              "Coordinate for command at index " << k <<
						       " out of range" << std::endl;
								legal = 0;
							}

						}

					}// loop to scan secondbox->value for legality
										
          if( !legal )
					{
					  status = badprogcharstringerror;
						// traceLeave();
						return secondbox;
					}

          // transfer over to progChars global and finish up

          // build a standard C string 
          progChars[ fi - minProgCode ] = new char[ n+1 ];

          for( k=1; k<=n; k++ )
					  progChars[ fi-minProgCode][k-1] = 
					         (secondbox->value)->charAt(k);

					progChars[ fi-minProgCode][n] = '\0';

          // leave as usual 
          status = 2;  // system call with no value

					// strip consumed inners
					first->stripValue(); 
          firstbox->stripValue();
					secondbox->stripValue();

          // traceLeave();
          return this;

				}// progCharcode

        else if( bifcode == charAtcode )
        {// guys that take string and int
        
          // make sure there are the right number of arguments
	        if( innerLength() != 3 )
          {
      	    status = need2argserror;
		    		// traceLeave();
            return this;
          } 

      	  // grab my inners
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          // detect bad operand:
          if( !(firstbox->type)->equals("string") )
          {
            status = notastringerror;
        		// traceLeave();
            return firstbox;
          }

          long fi;

          secondbox = firstbox->next;
					if( ! secondbox->haveIntValue( fi ) )
					{
            status = notintegererror;
        		// traceLeave();
            return secondbox;
					}

          if( fi < 0 || fi >= (firstbox->value)->length() )
					{
					  status = badstringindexerror;
						// traceLeave();
						return secondbox;
					}

          // depending on bif, finish evaluation and do tour

          char verytempstring[30]; // store result

				  sprintf( verytempstring, 
  					    "%c", (firstbox->value)->charAt( fi+1 ) );
						value = storeCharString( value, verytempstring );
            type = storeCharString( type, "char" );
        
          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;// signal special system call case---with value

          // put the box on tour
  				ilist[ni] = this;
 					cclist[ni] = newvaluecolorcode;
					ni++;

					// strip consumed inner in all cases
          first->stripValue();
          firstbox->stripValue();
					secondbox->stripValue();

          // traceLeave();
          return this;

        }// guys with one string, one int, produce some value
				
//jer end new zone

//jer implementing the new file/port bifs
//    (do them separately, not too worried about code bloat
//      at this point---just copy and paste and modify,
//      instead of using lots of branching

        else if( bifcode == opencode )
        {// open( port#, input or output, file name )
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 4 )
          {
      	    status = openerrornot3args;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 3 args, must be 
					//  first:  integer, between 1 and maxports

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

					if( ! firstbox->haveIntValue( fi ) )
					{// first arg is not an integer
					  status = notintegererror;
            // traceLeave();
						return firstbox;
					}

          if( fi < 1 || numports < fi )
					{
					  status = portrangeerror;
						// traceLeave();
						return firstbox;
					}

          // have syntactically okay port number fi,
					// see if it is in use
					if( ports->getStatus( fi ) != unusedPortStatus )
					{// port is already busy
					  status = openerrorportbusy;
						// traceLeave();
						return firstbox;
					}

          // now make sure second arg is "input" or "output"
					secondbox = firstbox->next;

					if( !( (secondbox->type)->equals("string") ) ||
					    (!(secondbox->value)->equals("input") &&
					     !(secondbox->value)->equals("output") 
							)
						)
					{
					  status = openerroriochoice;
						// traceLeave();
						return secondbox;
					}

          // have port# fi, know "input" or "output", so
					// if third is string, all is okay
					// (the open may fail, but that's not an error)

					thirdbox = secondbox->next;
			          
          if( !(thirdbox->type)->equals("string")  ) 
          {
            status = notastringerror;
        		// traceLeave();
            return thirdbox;
          }

          // pass info to ports and find out result
					if( ports->attemptOpen( 
					           fi, secondbox->value, thirdbox->value ) )
					{// file was opened, set value to true, fix up the
					 // portbox, put things on tour

            // set value to true, type to boolean
						value = storeCharString( value, "true" );
						type = storeCharString( type, "boolean" );
					  
						// access the portbox
						box* pb = accessPort( u, fi );
										
					  // copy filename from argument to pb's type,
            pb->type = storeString( pb->type, thirdbox->value );
						// then prepend the desired <- or ->
						if( ports->getStatus( fi ) == readingPortStatus )
  					{
					    (pb->type)->insertCharAt( 1, '-' );
							(pb->type)->insertCharAt( 1, '<' );

							// grab buffer-full of chars from file for port fi,
							//  make value of pb
              pb->loadBuffer( fi );

						}
					  else // output == ->
            {
					    (pb->type)->insertCharAt( 1, '>' );
							(pb->type)->insertCharAt( 1, '-' );

							// make value of pb "", ready built to append to
							pb->value = storeCharString( pb->value, "" );

						}
						 					 
						// put me on tour, followed by pb
    				ilist[ni] = pb;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;
    				ilist[ni] = this;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;

					}// open succeeded, store value, type, tour
					else
					{// open failed, set value to false, put me on tour

						value = storeCharString( value, "false" );
						type = storeCharString( type, "boolean" );

						// put me on tour
    				ilist[ni] = this;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;
					  
					}// open failed

          // whether open succeeded or failed, do finishing up								         
						
          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;  // system call that returns with a value

					// strip consumed inners
					first->stripValue();  // the bif value
          firstbox->stripValue();
					secondbox->stripValue();
					thirdbox->stripValue();

          // traceLeave();
          return this;

        }// open file

        else if( bifcode == fgetcode )
        {// fget( port#)
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 1 arg, must be 
					//  first:  integer, between 1 and maxports

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

				  if( !( firstbox->haveIntValue( fi ) ) )
					{// first arg is not an integer
					  status = notintegererror;
            // traceLeave();
						return firstbox;
					}

          if( fi < 1 || numports < fi )
					{
					  status = portrangeerror;
						// traceLeave();
						return firstbox;
					}

          // have syntactically okay port number fi,
					// see if it is open for input

          if( (ports->getStatus( fi ) != readingPortStatus) &&
					    (ports->getStatus( fi ) != emptyPortStatus) )
					{
					  status = portnotreadingerror;
						// traceLeave();
						return firstbox;
					}

          box* pb = accessPort( u, fi );

					if(  (pb->value)->length() == 0 )
					{// port is empty
					  status = endoffileerror;
						// traceLeave();
						return firstbox;
					}

          // now believe there is a char to get, get it and
					// refill buffer if needed

					char ch = (pb->value)->charAt( 1 );

					(pb->value)->deleteCharAt( 1 );

					if( (pb->value)->length() == 0 )
					{// refill buffer for this port
					  pb->loadBuffer( fi );
					}

         // jb17:  problem with char # 0 from file, switch to
				 //         just setting the value to "" and then
				 //          adding ch
         // set value to the char, type to char
				 //char verytempstring[2];  old
				 //sprintf( verytempstring, "%c", ch );  old
 				 value = storeCharString( value, "" );
				 value->insertCharAt( 1, ch );
				 type = storeCharString( type, "char" );
					  								
 				 // put me on tour, followed by pb
    				ilist[ni] = pb;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;
    				ilist[ni] = this;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;

          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;  // system call that returns with a value

					// strip consumed inners
					first->stripValue();  // the bif value
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// fget

        else if( bifcode == eofcode )
        {// eof( port#)
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 1 arg, must be 
					//  first:  integer, between 1 and maxports

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

				  if( !( firstbox->haveIntValue( fi ) ) )
					{// first arg is not an integer
					  status = notintegererror;
            // traceLeave();
						return firstbox;
					}

          if( fi < 1 || numports < fi )
					{
					  status = portrangeerror;
						// traceLeave();
						return firstbox;
					}

          // have syntactically okay port number fi,
					// see if it is open for input

          if( (ports->getStatus( fi ) != readingPortStatus) &&
					    (ports->getStatus( fi ) != emptyPortStatus) )
					{
					  status = portnotreadingerror;
						// traceLeave();
						return firstbox;
					}

          box* pb = accessPort( u, fi );

          // have port ready, get value of function
					if(  (pb->value)->length() == 0 )
					{// port is empty so return true
					  value = storeCharString( value, "true" );
					}
					else
					{// return false --- stuff still available
					  value = storeCharString( value, "false" );
					}

					type = storeCharString( type, "boolean" );

 				 // put me on tour
    				ilist[ni] = this;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;

          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                        // my newly created "value"
            setAspect( 'v' );
          }

          fixupReality(0);

          status = 3;  // system call that returns with a value

					// strip consumed inners
					first->stripValue();  // the bif value
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// eof

        else if( bifcode == fputcode )
        {// fput( port#, char to put )
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 3 )
          {
      	    status = need2argserror;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 2 args, must be 
					//  first:  integer, between 1 and maxports

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

				  if( !( firstbox->haveIntValue( fi ) ) )
					{// first arg is not an integer
					  status = notintegererror;
            // traceLeave();
						return firstbox;
					}

          if( fi < 1 || numports < fi )
					{
					  status = portrangeerror;
						// traceLeave();
						return firstbox;
					}

          // have syntactically okay port number fi,
					// see if it is open for writing

          if( ports->getStatus( fi ) != writingPortStatus )
					{
					  status = portnotwritingerror;
						// traceLeave();
						return firstbox;
					}

          // make sure second argument is a char type
          secondbox = firstbox->next;

          if( !(secondbox->type)->equals("char") )
 					{// second arg must be char
				    status = notcharerror;
					  // traceLeave();
					  return secondbox;
				  }

          // args are good, go ahead and ship the char to the
					// port

          box* pb = accessPort( u, fi );

          if( (pb->value)->length() == buffersize )
					{// buffer is full, ship it out before inserting new char
					  pb->saveBuffer( fi );
					}

					// now insert new char
          (pb->value)->insertCharAt( (pb->value)->length()+1, 
					                           (secondbox->value)->charAt(1) );

 				 // put portbox on tour
    				ilist[ni] = pb;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;

          fixupReality(0);

          status = 2;  // system call with no value

					// strip consumed inners
					first->stripValue(); 
          firstbox->stripValue();
					secondbox->stripValue();

          // traceLeave();
          return this;

        }// fput

        else if( bifcode == closecode )
        {// close( port# )
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 2 args, must be 
					//  first:  integer, between 1 and maxports

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

          long fi;

				  if( !( firstbox->haveIntValue( fi ) ) )
					{// first arg is not an integer
					  status = notintegererror;
            // traceLeave();
						return firstbox;
					}

          if( fi < 1 || numports < fi )
					{
					  status = portrangeerror;
						// traceLeave();
						return firstbox;
					}

          // have syntactically okay port number fi,
					// see if it is open so makes sense to close

          if( ports->getStatus( fi ) == unusedPortStatus )
					{
					  status = portnotopenerror;
						// traceLeave();
						return firstbox;
					}
      
			    // arg is good, go ahead and close the port
					
          box* pb = accessPort( u, fi );

          if( ports->getStatus( fi ) == writingPortStatus )
					{// open for output --- save buffer first
					  pb->saveBuffer( fi );
					}
					else
					{// one of two input states --- just clear the buffer
					  pb->value = storeCharString( pb->value, "" );
					}

					// either way, reset type
					pb->type = storeCharString( pb->type, "--" );

          // and close the real port
          ports->closePort( fi );

 				 // put portbox on tour --- to show it's unhooked type --
    				ilist[ni] = pb;
 	  				cclist[ni] = newvaluecolorcode;
		  			ni++;

          fixupReality(0);

          status = 2;  // system call with no value

					// strip consumed inners
					first->stripValue(); 
          firstbox->stripValue();

          // traceLeave();
          return this;

        }// close

        else if( bifcode == haltcode )
				{
	        if( innerLength() != 1 )
          {
      	    status = neednoargserror;
		      	// traceLeave();
            return this;
          } 

          status = 6;  // signal halt request
			    // traceLeave();
					return this;
											  
				}

        // jb17:  _null, _this, _destroy
        else if( bifcode == nullcode || bifcode == thiscode )
				{// two bifs with no args, return a reference
          // make sure there are the right number of arguments

	        if( innerLength() != 1 )
          {
      	    status = neednoargserror;
		     		// traceLeave();
            return this;
          } 

          char verytempstring[30]; // to hold result

          // produce the values and types
          if( bifcode == nullcode )
					{// the value of the call becomes #0
            sprintf( verytempstring, "#0" );
            value = storeCharString( value, verytempstring );
            type = storeCharString( type, "" );
					}
					else if( bifcode == thiscode )
					{// value becomes reference to this instance

            methbox = findInstance();
						
						if( methbox == NULL )
						{// inappropriate use of _this
						  status = notininstancemethoderror;
							return this;
						}
											  
            storeValue( methbox->name );
            storeType( methbox->type );
					}

          // since produced value, maybe show it
          if( aspect == 'f' )  // full aspect means I need to show
          {                       // my newly created "value"
            setAspect( 'v' );
          }
          fixupReality(0);
          // strip consumed inner
          first->stripValue();
          status = 3;  // signal special system call case---with value

					// put this box with its new value on tour
					ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;

          // traceLeave();
          return this;
				}// null, this

				else if( bifcode == destroycode )
				{// one arg, no value
				         
          // make sure there are the right number of arguments
	        if( innerLength() != 2 )
          {
      	    status = need1argerror;
		    		// traceLeave();
            return this;
          } 

          // if still here, have 1 arg, must be
					// non-prim, is #n to be destroyed

      	  // grab my inner
          firstbox = first;
	        firstbox = firstbox->next; // is now the first arg

				  if( isprimtype( firstbox->type ) ||
					    (firstbox->value)->equals( "#0" ) 
						)
					{// first arg is for sure not a valid ref to a heap guy
					  status = notvalidheapaddresserror;
						return firstbox;
					}

          // look for it, error if not found 
					// (could happen if user did destroy(p); destroy(p);
					//   --- not setting invalid references to #0)
          secondbox = firstbox->findInHeap( u, 
					                firstbox->value );

          if( secondbox == NULL )
					{// didn't find the guy in the heap
					  status = notvalidheapaddresserror;
						return firstbox;
					}

          // secondbox is guy in heap to be destroyed
					secondbox->remove();

          // nobody goes on tour?

          fixupReality(0); // for the call, what about the poor
					                 // heap box?

          status = 2;  // system call with no value

					// strip consumed inners
					first->stripValue(); 
          firstbox->stripValue();

          return this;
          
				}// destroy
        else
          errorexit("bad system name in callbox");

      }// system call

      // going on with the more traditional call (didn't return
			// above on a bif or error

      methbox = firstbox->source;  // should be original methodbox

      if( methbox->kind != methodbox )
      {
        status = illegalmethodreferror;
        // traceLeave();
        return methbox;  // my first box, which is where the error is
      }
      else
      {// methbox is a methodbox
      
        int classOrInstance; // the kind of method being called
        secondbox = methbox->outer;
        if( secondbox->kind == classmethodsbox )
          classOrInstance = classmethodbox;
        else
          classOrInstance = instancemethodbox;

        //  make appropriate copy of methbox, 

        newbox = methbox->copyMethodbox( classOrInstance );

        // jb16:  new approach --- insert newbox into stack
				//         early, need to set up hooks first

        // now paste newbox into reality:

        // make sure the codebox of newbox points back to
        // me as the "return IP"
				// jb16:  did have utility curbox, changed to codebox
        codebox = newbox->accessInnerAt( 4 );  // v10:  was 3
        codebox->source = this;

        // make newbox's source be to either the classbox
        // the class method belongs to, or to the
        // instancebox it belongs to, and "type" needs to be
        // the heap address of the instance (class name already
        // set as the type in the classmethodbox by copyMethodbox)
        if( newbox->kind == instancemethodbox )
        {// calling an instance method
         // use "type" to find the correct instance
          curbox = findInHeap( u, firstbox->type );
          if( curbox == NULL )
            errorexit("evaluate callbox---didn't find instance?");
          newbox->source = curbox; // the instance box
          newbox->storeType( firstbox->type );
        }
        // else source was set to the classbox in copyMethodbox

        // access the stack
        stkbox = u->findStack();
        // stick newbox in the stack
        stkbox->addInnerAtFront( newbox );
        // tidy up reality from the new guy outward
        // instead of this:  newbox->fixupReality(0);
        // do from the parameter box outward:
        curbox = newbox->first;
//            curbox->fixupReality(1);

         // and now do the java translation (with accompanying
				 //  fixups of reality
  			status = newbox->translateJava();
				if( status < 0 )
				{// detected error in translation---error has already
				 // been requested, so set status to inform step()
				  status = erroralreadyrequested;
				  return this;
				}

        // jb16:  now go on to work with the parameters

        pbox = newbox->first; // params box of the new frame
        curbox = this;  // me, the callbox        
        count = curbox->innerLength() - 1;  // adjust for method ref
        if( pbox->innerLength() != count )
        {
          status = argsnotequalparamserror;
        	// traceLeave();
          return this; // I'm at fault
        }
        else
        {// counts match, go on to transfer them across

          // put params box on tour:
  				ilist[ni] = pbox; cclist[ni] = newvaluecolorcode; ni++;

          pbox = pbox->first;  // first parameter, or null if count==0
          curbox = first; 
          curbox = curbox->next;  // first argument, or null

          for( k=1; k<= count; k++ )
          {// copy value from curbox to pbox, and check type
            // compatibility

//v14:  ???

            pbox->storeValue( curbox->value );

            if( ! pbox->storageCompatible( curbox ) )
            {
              status = incompatargparamerror;
          		// traceLeave();
              return curbox;
                 // cool --- had pbox, which was focusing on a box
                 // that wasn't hooked into reality!
            }

            // make sure this individual parameter box has
            // correct info about itself
            pbox->figureSizeLocs(junk);

            // simulate that the value has actually moved over
            // by stripping from curbox
            curbox->stripValue();

            // move ahead  (okay to do even if status <0)
            pbox = pbox->next;
            curbox = curbox->next;


          }// args all matched params, good to finish

        }// counts match       
        
      }// have methodbox in first spot

      // for uniformity, strip the first box of its method ref
      firstbox->stripValue();

      // do actions to make this callbox ready to wait
		  //   and then return (to avoid standard final actions)
      fixupReality(0); // to adjust for stripped inners

      // dye this call box waitcolor, since is not on tour,
 	    // won't return to its natural color until we request it
		  // later when it gets its value back (or is just returned to)
      setDye( waitcolorcode );

      // traceLeave();
      return this;   // since status = 1, is irrelevant

    }// callbox

    else if( kind == returnbox )
    {// I simply need to copy my value/type to my codebox
		 // v10:  no, now I simply need to put the value/type to my
		 //       retvalbox, and also copy to codebox for convenience
     
      // v10: 
      codebox = findCodebox();  // this is really the code box
			box* retvalbox = codebox->outer;  // the enclosing method box
			retvalbox = retvalbox->first; // the inputs box
			retvalbox = retvalbox->next;  // the output box

      firstbox = first;

      // jb17:  if the output box has empty type, is okay for
			//        me to have empty inner
			if( firstbox->kind == emptybox &&
			    (retvalbox->type==NULL || (retvalbox->type)->equals("")
					)
			  )
			{// okay to have emptybox if output box has no type
			  status = 2; // special signal back to step
				            // doesn't seem to be needed, but okay
				return NULL;
			}

      if( firstbox->value == NULL ||
          firstbox->type == NULL )
      {
        status = mustreturnvalueerror;
        // traceLeave();
        return firstbox;
      }

      if( ! (firstbox->type)->equals( retvalbox->type ) )
			{// returning wrong type of value
        // jboxes1c:
        // unless returning int when type is float
        if( (firstbox->type)->equals( "int" ) &&
             (retvalbox->type)->equals( "float" )
          )
        {
          // don't flag error and leave
        }
        // jboxes1c:
        //   or value is #0 (in which case must have class name
        // as output type, set that type as type being returned
        else if( (firstbox->value)->equals( "#0" ) &&
                 !isprimtype( retvalbox->type )
               )
        {// change firstbox type to retval's
          firstbox->storeType( retvalbox->type );
        }
        else
        {// is an error otherwise
  			  status = returningwrongtypeerror;
  				// traceLeave();
	  			return retvalbox;
        }
			}

      // have type match, so go on---store value and type both places

      codebox->storeValue( firstbox->value );
      codebox->storeType( firstbox->type );
			retvalbox->storeValue( firstbox->value );
			retvalbox->storeType( firstbox->type );

/*  v10:  don't want to show value aspect of codebox 
          never really did, but it didn't show up with old scheme,
					the cimbox died without displaying

					I.e., this was a bug of sorts before, now fixing it!

      if( codebox->aspect == 'f' )
      {
        codebox->setAspect('v');
        codebox->fixupReality(0);
      }
*/

      status = 1;

      // strip my inner
      firstbox->stripValue();

      // put the retvalbox on tour
      status = 1;
      ilist[ni] = retvalbox; cclist[ni] = newvaluecolorcode; ni++;

    }// returnbox

    else if( literalkind(kind) )
    {// new scheme --- evaluation of literal box can fail,
		 //  produce "literal has incorrect format" error

      long itemp;
  		double ftemp;

      // transfer name to value
      storeValue( name );

      // see if new value has correct format, set type
			if( kind == intbox )
			{
        type = storeCharString( type, "int" );

			  if( ! haveIntValue( itemp ) )
				{
          status = badliteralformaterror;
      		// traceLeave();
          return this;
				}

			}
			else if( kind == floatbox )
			{
        type = storeCharString( type, "float" );

			  if( ! haveFloatValue( ftemp ) )
				{
          status = badliteralformaterror;
      		// traceLeave();
          return this;
				}
			}
			else if( kind == charbox )
			{
        type = storeCharString( type, "char" );

			  if( value->length() != 1 )
				{
          status = badliteralformaterror;
      		// traceLeave();
          return this;
				}
			}
			else if( kind == booleanbox )
			{
        type = storeCharString( type, "boolean" );

			  if( ! value->equals("true") &&
				    ! value->equals("false")
					)
				{
          status = badliteralformaterror;
      		// traceLeave();
          return this;
				}
			}
			else if( kind == stringbox )
			{
        // string can't have bad format      
				type = storeCharString( type, "string" );
			}
			else
			  errorexit("unknown primitive type in evaluate");

      status = 1;
      ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;
 
       // traceLeave();
//  this was short-cutting literalbox's uniform behavior like
//  other boxes!
//      return NULL;  // nothing can go wrong with a literalbox

    }// literalbox

    else if( kind == idbox )
    {// evaluate id box --- search for my name in lots of contexts,

      //  method names (as determined by where we find them,
      //  or by their starting with _), simply locate themselves,
      //  with source pointing to the exact box of the method.

      //  data names either locate, or go ahead and lookup value,
      //  depending on getvalue parameter

        source = NULL;
        status = 0;    // so can search in simpler sequential
                       // code.  Error will set negative, 
                       // when find will set to 1

        // first see if I am empty, hence error
        if( name == NULL || name->equals("") )
        {
          status = emptyidnameerror;
      		// traceLeave();
          return this;
        }

        // next see if must be a system method name, so can
        // stop looking:
        if( name->charAt( 1 ) == '_' )
        {// build my value and type and return 
          // value will be the name, type will be "system"
          // leave source at NULL
          // check for legal

          if( name->equals( "_moveTo" ) ||
              name->equals( "_print" ) ||
              name->equals( "_char" ) ||
							name->equals( "_ascii" ) ||
              name->equals( "_get" ) ||
              name->equals( "_getLine" ) ||
              name->equals( "_clear" ) ||
              name->equals( "_random" ) ||
              name->equals( "_time" ) ||   // jb2006a:  add _time method
							name->equals( "_seed" ) ||

              name->equals( "_abs" ) ||
              name->equals( "_sqrt" ) ||
              name->equals( "_sin" ) ||
              name->equals( "_cos" ) ||
              name->equals( "_atan" ) ||
							name->equals( "_tan" ) ||
							name->equals( "_asin" ) ||
							name->equals( "_acos" ) ||
							name->equals( "_exp" ) ||
							name->equals( "_log" ) ||
              name->equals( "_floor" ) ||
              name->equals( "_ceil" ) ||

              name->equals( "_intToString" ) ||
              name->equals( "_floatToString" ) ||
              name->equals( "_charToString" ) ||
							name->equals( "_charsToString" ) ||
              name->equals( "_isInt" ) ||
              name->equals( "_isFloat" ) ||
              name->equals( "_toInt" ) ||
              name->equals( "_toFloat" ) ||
              name->equals( "_length" ) ||
              name->equals( "_charAt" ) ||
              name->equals( "_copy" ) || 
              name->equals( "_open" ) ||
              name->equals( "_close" ) ||
              name->equals( "_fget" ) ||
              name->equals( "_fput" ) ||
							name->equals( "_eof" ) ||

							name->equals( "_whatRow" ) ||
							name->equals( "_whatCol" ) ||
							name->equals( "_hideCursor" ) ||
							name->equals( "_showCursor" ) ||
							name->equals( "_halt" ) ||

							name->equals( "_progChar" ) ||

							name->equals( "_null" ) ||
							name->equals( "_this" ) ||
							name->equals( "_destroy" )
            )
          {// legal bif, set value and type

            value = storeString( value, name );
            type = storeCharString( type, "system" );

            // do finishing up stuff, because want to return
            // so don't go on to do stuff below!
            status = 1;
            if( valuekind( kind ) &&
             aspect == 'f' )  // full aspect means I need to show
            {                       // my newly created "value"
              setAspect( 'v' );
            }
            fixupReality(0);

            ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;
								
         	  // traceLeave();
            return this; 
            
          }
          else
          {
            status = badbifnameerror;
            // traceLeave();
            return this;
          }

        }

        // get my bearings
        codebox = findCodebox();  // get to the codebox
        cimbox = codebox->outer;  // the cimbox
        prevbox = cimbox->first;   // the paramsbox
				curbox = prevbox->next;   //v10:  the output box
        curbox = curbox->next;    // the localsbox

        // look in locals 
        source = curbox->searchInner( name );
        if( source != NULL )
        {// found in locals, note this for later
          status = 1;
        }

        if( status == 0 )
        {// try for parameter
          source = prevbox->searchInner( name );
          if( source != NULL )
          {// found in paramsbox
            status = 2;
          }
        }// in parameter box?

        if( status == 0 )
        {// look in my instancebox if I'm an instancemethod
          if( cimbox->kind == instancemethodbox )
          {// have an extra internal "layer" to try 

					  // jb17:  had the line below:
            //firstbox = cimbox->source;  // the instancebox
						//  but change to search symbolically for firstbox
						//  as the instancebox in the heapbox by using the
						//  type of the cimbox, which is like #17,
						//  error:  no such instance if can't find

            firstbox = heap->searchInner( cimbox->type );            

						if( firstbox == NULL )
						{// oops --- instance method with no instance
						  status = nosuchinstanceerror;
							return this;
						}

            // now that have firstbox, maybe, as guy on heap,
						// go on as before:

            source = firstbox->searchInner( name );

            if( source != NULL )
            {// is a data member
              status = 3;
            }
            else
              secondbox = firstbox->source;  // is the classbox
          }
          else
            secondbox = cimbox->source;  // is the classbox
        }

        // secondbox is my classbox, search it

        // v10:  now, instead of using cimbox->source indirectly or
				//       directly to get to the class data, do a name search
				//       in static data for the class name, then for the
				//       individual data box
				//  Leave everything intact, though, in case being used where
				//   I don't realize it

        if( status == 0 )
        {// try the classdata box
				 // v10:  see if name is a class data member
				 //          class name is stored as my type
				 //          staticdata is a global box 

/*				  // old version of searching for class data member
          curbox = secondbox->first;
          source = curbox->searchInner( name );

          if( source != NULL )
          {// is a class data member
            status = 4;
          }
*/

          // v10:  new version
					  // first see if the class name is even in staticdata
						   // secondbox is the class box, either way did last
							 // search, so use its name as search target
						curbox = staticdata->searchInner( secondbox->name );

						if( curbox == NULL )
						  status = 0;  // emphatic---are no class data members
						else
						{// is at least one class data member, search for name
						  source = curbox->searchInner( name );

							if( source != NULL )
							  status = 4;   // found a class data member and made
								              // source point to it
							else
							  status = 0;
						}

        }

        if( status == 0 )
        {// try the class methods

          curbox = secondbox->first;  // is now the class data box
									  
          curbox = curbox->next; // now the class methods box
          source = curbox->searchInner( name );
          if( source != NULL )
          {// is a class method
            status = 5;
          }
        }
        
        if( status == 0 )
        {// try for instance method
           curbox = curbox->next;  // on instance datas
           curbox = curbox->next;  // now on instance methods
           source = curbox->searchInner( name );
           if( source != NULL )
           {// is an instance method
             status = 6;
           }
        }

        if( status == 0 )
        {// see if my name is in fact a class name (so I'm part of
          // reference to a class method or data)

          thirdbox = u->first;  // is a classbox if there are any
                                  // (which there are or we wouldn't
                                  // be here!
          while( thirdbox->kind == classbox &&
                   !( (thirdbox->name)->equals( name ) )  
                 )
            thirdbox = thirdbox->next;

          if( thirdbox->kind==classbox )
          {// stopped on firstbox because names matched
            status = 7;
            source = thirdbox;
          }                 
        }

        if( status == 0 )
        {// my name isn't anywhere!   
          status = idnotfounderror;
          // traceLeave();
          return this;
        }

        // okay, now have status and source,
        // need to gather my data and finish, building
        // in my value and type either the location I refer to,
        // or the value stored at the location I refer to

        // firstbox is my instancebox, if I have one
        // secondbox is my classbox
        //  thirdbox, if relevant, is the class box equal to my name

        if( status > 0 )
        {// no error detected yet---means found my name somewhere

          if( 1<=status && status<=4 )
          {// databox guys, makes sense to do either location or
            // value, all behave mostly the same

            if( getvalue )
            {// want value and type of databox I refer to

              if( source->valueNotInit() )
              {// source has empty value
                status = datanotiniterror;
              }
              else
              {// source has a value, type, copy them to me
               // (I originally didn't have copy, originals went
                //   away when deleted this guy)
                storeValue( source->value );
                storeType( source->type );

                status = 1;
                ilist[ni] = this; cclist[ni] = newvaluecolorcode; 
                ni++;
              }
            }
            else
            {// want location of databox I refer to,
              // put category, based on status, in type,
              // namely local, param, #whatever, classname,
              //  value is just my name
              storeValue( name );
              
              if( status==1 )
                type = storeCharString( type, "local" );
                  
              else if( status==2 )
                type = storeCharString( type, "param" );
              else if( status==3 )
              {// need instance name(heap address) as my "type"
                storeType( firstbox->name );
              }
              else if( status==4 )
              {// need class name as my "type"

                 storeType( secondbox->name );
              }
              else
                errorexit("unknown status in forming type");

              ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;
        
            }// location of databox

          }// databox guys

          // 5 and 6 actions were switched!
          else if( status==5 )
          {// class method, produce location
            storeValue( name );
            storeType( secondbox->name );

            ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;
          }
          else if( status==6 )
          {// instance methods, produce location 
            storeValue( name );
            storeType( firstbox->name );

            ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;
          }
          else if( status==7 )
          {// class name, box that uses it will have to
            // recognize and use it---it's nothing by itself,
            // will have to be used in a memberbox
            storeValue( name );
            type = storeCharString( type, "global" );

            ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;
          }
          else
            errorexit("illegal status code in idbox, evaluate");
        
        }// found it somewhere

    }// idbox

    else if( kind == memberbox )
    {
/* old:
          // release resources, if any
          if(value != NULL)
            value->destroy();
          if(type != NULL)
            type->destroy();
*/

      firstbox = first;  // guy that specifies the value, which is
                          // either class name or reference
                         // I'm supposed to work from

      // jb2006a:  put firstbox->source == NULL as additional clause,
      //   happens when the first box obtained by method call,
      //   seems to work because firstbox->source isn't used---search heap
      //   for box with correct reference number
      if( ((firstbox->source)==NULL) ||
          (firstbox->source)->kind == databox ||
          (firstbox->source)->kind == valuebox
        )
      {// value of firstbox is a heap address, source
        // points to the the box that contains that address,
        // need to find the instancebox,
        // then search out the guy matching my name in the
        // instancebox, or in the instance methods of the class
        // the instance belongs to

        // search for firstbox's value as a heapaddress

        secondbox = findInHeap( u, firstbox->value );


        if( secondbox == NULL )
        {
          status = notvalidheapaddresserror;
        }
        else
        {// have a good instancebox or valuebox

          // 1/4/00:  found logical error:  sure, secondbox is
					//  a box on the heap, but if it's a gridbox,
					//  searchInner crashes becauses its inners are
					//  nameless---so intercept this error
					
					if( secondbox->kind == gridbox )
					{// error exit
					  status = cantbearrayerror;
					  // traceLeave();
						return this;
					}
					        
          // secondbox is the instance box my firstbox specifies,
          // now use my name to look for the specific data member
          curbox = secondbox->searchInner( name );

          if( curbox != NULL )
          {// curbox is the data member of the instance box
            // proceed to build on me value/type
           // from curbox

            source = curbox;  // either way, point to the actual box
                              // in the instancebox

            if( getvalue )
            {// I get the value
              if( source->valueNotInit() )
              {// source has empty value
                status = datanotiniterror;
              }
              else
              {// source has a value, type, copy them to me
               // (I originally didn't have copy, originals went
               //   away when deleted this guy)
                storeValue( source->value );
								storeType( source->type );

                status = 1;
              }          
            }
            else
            {// I get the location which is the address of the
              // instancebox (firstbox's value) as the type,
              // my name as the value
              storeValue( name );
							storeType( firstbox->value );

              status = 1;
            }// fill in my location

          }// my name is legit as a data member of the instance
          else
          {// not a data member, try for instance method

            curbox = secondbox->source;  // the class this instance
                                         // came from
            curbox = curbox->accessInnerAt( 4 ); // instance methods

            curbox = curbox->searchInner( name );

            if( curbox == NULL )
            {// have finally decided it's hopeless
              status = nosuchinstancemembererror;
          		// traceLeave();
              return this;
            }
            else
            {// is an instance method, build location in me
              source = curbox;  // leave pointing to the methodbox
              storeValue( name );
              storeType( secondbox->name );
            }

            status = 1;

          }// instance method?

        }// have a good instancebox

      }// my firstbox is not a class name

      else
      {// only other legal possibility is class name,
        // must handle data or method member of class

        if( (firstbox->source)->kind != classbox )
        {// possibilities are databox and class name,
          // anything else is a method ref, I think, but whatever
          // it is, it makes no sense!
          status = canthavemethodreferror;
          // traceLeave();
          return firstbox;
        }

        // firstbox evaluated to a class name, search for
        // class data or class member, produce appropriate
        // location or value

        secondbox = firstbox->source;  // is the classbox
        secondbox = secondbox->first;     // is classdata box

        curbox = secondbox->searchInner( name );

        if( curbox != NULL )
        {// found class data member

            // jboxes1c:  on 7/25/2004, found problem, not using
            // static data box.  Change right here, after
            // successfully finding class data box, now
            // slip in code to change source to be the data box
            // in the class box in the static data box, just a
            // sneaky little detour or patch:
            //***********************************************
            
               // firstbox->value is the class name, first find
               // box in staticdata box with that name (can't
               // fail unless I'm really confused):
               curbox = 
                 staticdata->searchInner( firstbox->value );
               curbox = curbox->searchInner( name );
                 // now curbox is data box in class in static data
            //***********************************************

            source = curbox;  // point to the class data box

            if( getvalue )
            {// I get the value
              if( source->valueNotInit() )
              {// source has empty value
                status = datanotiniterror;
            		// traceLeave();
                return source;  // show the bad class data member
              }
              else
              {
                storeValue( source->value );
								storeType( source->type );
                status = 1;
              }          
            }
            else
            {// I get the location which is type=class name=
              // firstbox's value, value is my name
              storeValue( name );
							storeType( firstbox->value );
              status = 1;
            }// fill in my location

        }// found as a class data
        else
        {// search on in class methods

          secondbox = secondbox->next;  // classmethodsbox
          curbox = secondbox->searchInner( name );

          if( curbox != NULL )
          {// found class method member,
            // build location 
            source = curbox;  // point straight to the methodbox
            storeValue( name );
						storeType( firstbox->value );
            status = 1;
          }
          else
          {
            status = nosuchclassmembererror;
            // traceLeave();
            return this;
          }

        }

      }// maybe is class name

      // whatever the case, make sure firstbox loses its
      // value/type and switches to 'f' aspect if has been
      // showing 'v'
      firstbox->stripValue();

      // put me on the tour as having just received a value
      ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;

    }// memberbox

    else if( kind == arraybox || kind == array2dbox )
    {// 2 or 3 inners are ready, first must have evaluated
      // to a value that is a heap address of a grid box,
      // 2nd and 3rd, if is one, must have evaluated to
      // integers.  Do error checking and either produce
      // location which will be the heap address for the "type"
      // and the selection string like [3] or [3][5],
      // with source pointing directly to the valuebox, or
      // will produce value which is copy of whatever is in
      // the valuebox, with type being the type of the gridbox

      firstbox = first;  // points to a box that holds my array
      secondbox = firstbox->next;  // the first index
      thirdbox = secondbox->next;  // second index if in 2d case,
                                    // else null

      // jb18:  firstbox need not source to a gridbox, could
			//        refer to a simple data box
			//  noticed because of gbox vs. firstbox error below,
			//  but is a hole in the type system. Hole is firstbox could
			//  be a string type with value like #1, old code doesn't
			//  catch.  Need to check type of source, 
			mystring* tempac = (firstbox->source)->type;
			if( tempac->charAt( tempac->length() ) != '[' )
			{
			  status = notarraytypeerror;
				return firstbox;
			}

      // access the gridbox
      gbox = findInHeap( u, (firstbox->source)->value );
      if( gbox == NULL  )
      {
        status = notagridboxerror;
        // traceLeave();
        return firstbox;   // jb18: had gbox which was just silly
      }

      // get and check first index
      if( secondbox->value == NULL ||
              ! (secondbox->value)->toInt( iresult ) 
          )
      {
        status = argnotlegalinterror;
        // traceLeave();
        return secondbox;
      }
      if( iresult < 0 )
      {
        status = negsizerequestederror;
        // traceLeave();
        return secondbox;
      }

      if( kind == arraybox ) // not array2d
      {// 1D case
        iresult2 = 0;
      }
      else
      {// 2D case
        if( thirdbox->value == NULL ||
              ! (thirdbox->value)->toInt( iresult2 ) 
          )
        { 
          status = argnotlegalinterror;
        	// traceLeave();
          return thirdbox;
        }
        if( iresult2 < 0 )
        { 
          status = negsizerequestederror;
        	// traceLeave();
          return thirdbox;
        } 
      }

      // feed in iresult, iresult2 to gbox, see if get a component
      pbox = gbox->accessArrayElement( iresult, iresult2, status );

      if( status < 0 )       // is error, pass errcode on out
      {// adjust the badbox returned depending on errorcode
        if( status == nonemptysecondfor1derror ||
            status == colindextoolargeerror )
				{
        	// traceLeave();
          return thirdbox;
				}
        else
				{
					// traceLeave();
          return secondbox;  // row index too large
				}
      }
  
      // got the component in pbox, now fix up my location or value

      source = pbox;  // either way, point directly to the guy

      if( getvalue == 0 )
      {// do symbolic location --- my value is index/indices in
        // brackets, my type is the heap address==name of gbox

        type = storeString( type, gbox->name );

        if( kind==arraybox ) // v10:  had iresult2==0 --- oops!
          sprintf( answer, "[%d]", iresult );
        else
          sprintf( answer, "[%d][%d]", iresult, iresult2 );

        value = storeCharString( value, answer );

/* old: (new is above)
        sresult = new string( answer );
        value = storeString( value, sresult );
        sresult->destroy();
*/
        
      }
      else
      {// do value--- value is value of pbox, type is type of gbox
        value = storeString( value, pbox->value );
        type = storeString( type, gbox->type );
      }

      // strip inners once they've been consumed
      firstbox->stripValue();
      secondbox->stripValue();
      if( kind == array2dbox )   // v10:  had iresult2>0 --- oops when
			                           //   index actually is 0!
        thirdbox->stripValue();

      // put me on the tour as having just received a value
      ilist[ni] = this; cclist[ni] = newvaluecolorcode; ni++;

    }// array boxes

    else if( kind == opbox )
    {    
      // grab my two inners
      firstbox = first;
      secondbox = first->next;

      int op; // opcode for operator we'll be trying to do
      if( name->equals("==") )
        op=eqcode;
      else if( name->equals( "!=" ) )
        op = necode;
      else if( name->equals("<") )
        op=ltcode;
      else if( name->equals(">") )
        op=gtcode;
      else if( name->equals("<=") )
        op= lecode;
      else if( name->equals(">=") )
        op= gecode;
      else if( name->equals("+") )
        op= pluscode;
      else if( name->equals("-") )
        op= minuscode;
      else if( name->equals("*") )
        op= timescode;
      else if( name->equals("/") )
        op= divcode;
      else if( name->equals("%") )
        op= modcode;
      else if( name->equals("&") )
        op= andcode;
      else if( name->equals("|") )
        op= orcode;     
      else
        op=noopcode;  // no op

      // detect bad operator
      if( op==noopcode )
      {
        status = illegaloperatorerror;
        // traceLeave();
        return this;
      }

      // jb17:  hey, this is an error never noticed before---
			//        should be able to compare non-prims
      // Plus, there was a strange error---in second check,
			//  didn't change firstbox to secondbox.  Probably these
			//  checks are bogus, anyway.  So, I'm going to rework
			//  this area a lot --- I don't think the type and value
			//  can ever be null, anyway!

      // check more carefully in cases depending on operators,
      // do all the necessary work in each subcase

         // for convenience, come up with a single char code 
         // representing each operand type:
         // (works since checked above whether is a legit prim or ?

         // utility variables for this area
         char fc = (firstbox->type)->charAt( 1 );
         char sc = (secondbox->type)->charAt( 1 );
         double fd, sd, ansd;
         long fi, si, ansi;
         int coerced; 

      if( op==eqcode || op==necode )
      {// equality operators --- types must match exactly,
        // just do string equality,
        // or convert both to float and see if they're the
        // same float

				// jb17:  separate out prims from non-prims

				  // get first letters of types for easy compare
					//  unless strings are empty
					if( (firstbox->type)->length() > 0 )
            fc = (firstbox->type)->charAt( 1 );
					else
					  fc = '-';
					if( (secondbox->type)->length() > 0 )
            sc = (secondbox->type)->charAt( 1 );
					else
					  sc = '-';

        if( !isprimtype( firstbox->type ) ||
				    !isprimtype( secondbox->type )
					)
				{// one or both of the operands is non-prim
				 // Types must be identical or "" for compare to be 
				 // allowed
				  if( (firstbox->type)->equals( "" ) ||
					    (secondbox->type)->equals( "" ) ||
					    (firstbox->type)->equals( secondbox->type ) 
						)
					{// okay to compare --- must produce ansi as 0 or 1
					  if( (firstbox->value)->equals( secondbox->value ) )
						  ansi = 1;
						else
						  ansi = 0;
					}
					else
					{// error
            status = incomptypeserror;
            return this;
					}				    
				}// one or both non-prim
				else
				{// both ops prim

          if( (fc=='f' && coercibletofloat(sc) ) || 
              (sc=='f' && coercibletofloat(fc) )
            )
          {// compare as floats
            coerced = (firstbox->value)->toFloat( fd );
            if( !coerced )
            {
              status = cantcoercetofloaterror;
          		// traceLeave();
              return firstbox;
            }

            coerced = (secondbox->value)->toFloat( sd );
            if( !coerced )
            {
              status = cantcoercetofloaterror;
          		// traceLeave();
              return secondbox;
            }
            // have fd, sd, produce whether or not equal
            if( fd==sd )
              ansi = 1;
            else
              ansi = 0;
    
          }
          else if( fc==sc )  // v14:  removed '?' possibility
          {// identical types, not float, can do string compare
            if( (secondbox->value)->equals( firstbox->value ) )
              ansi = 1;
            else 
              ansi = 0;
          }
          else
          {// incompatible types for equality op
            status = incomptypeserror;
          	// traceLeave();
            return this;
          }
        }// both ops primitive

          // ansi tells whether equal, flip if doing != op
        if( op == necode )
          ansi = !ansi;

        // store the value and type in me:

          if( ansi )
					  value = storeCharString( value, "true" );
					else
					  value = storeCharString( value, "false" );

					type = storeCharString( type, "boolean" );

      }// equality ops

      if( op==ltcode || op==gtcode || 
               op==lecode || op==gecode )
      {// relational operators---only compare numerics,
        // strings, chars.  All produce ansi = -1, 0, 1,
        // process to a boolean at the end

        if( coercibletofloat(fc) && coercibletofloat(sc) &&
            (numeric(fc) || numeric(sc) ) 
          )
        {// both are ?,i,f, at least one is i or f,
          // so compare as numbers after obtaining their values
          coerced = (firstbox->value)->toFloat( fd );
          if( !coerced )
          {
            status = cantcoercetofloaterror;
        		// traceLeave();
            return firstbox;
          }
          coerced = (secondbox->value)->toFloat( sd );
          if( !coerced )
          {
            status = cantcoercetofloaterror;
        		// traceLeave();
            return secondbox;
          }
          // have fd, sd, produce ansi code
          if( fd==sd )
            ansi = 0;
          else if( fd < sd )
            ansi = -1;
          else
            ansi = 1;
        }// numeric comparison

        // v14:  removed '?' junk for strings, chars
        else if( fc=='s' && sc=='s' )
        {// compare two strings
          ansi = (firstbox->value)->compare( secondbox->value );
        }
        else if( fc=='c' && sc=='c' )
        {// compare chars
          // see if both args are length 1
          if( (firstbox->value)->length() != 1 )
          {
            status = illegaloperanderror;
        		// traceLeave();
            return firstbox;
          }
          if( (secondbox->value)->length() != 1 )
          {
            status = illegaloperanderror;
        		// traceLeave();
            return secondbox;
          }

          if( (firstbox->value)->charAt( 1 ) <
              (secondbox->value)->charAt( 1 ) )
            ansi = -1;
          else if( (firstbox->value)->charAt( 1 ) ==
              (secondbox->value)->charAt( 1 ) )
            ansi = 0;
          else
            ansi = 1;

        }
        else
        {
          status = incomptypeserror;
        	// traceLeave();
          return this;
        }

        // ansi tells relationship, using -1,0,1 where
        //  -1 means less than, 0 means equal, 1 means greater than,
        // depending on op, produce ansi at 0 or 1
        // properly
        if( op == lecode )
        {
          if( ansi <= 0 )
            ansi = 1;
          else
            ansi = 0;
        }
        else if( op == gecode )
        {
          if( ansi >= 0 )
            ansi = 1;
          else
            ansi = 0;
        }
        else if( op == ltcode )
        {
          if( ansi < 0 )
            ansi = 1;
          else
            ansi = 0;
        }
        else if( op == gtcode )
        {
          if( ansi > 0 )
            ansi = 1;
          else
            ansi = 0;
        }
     
        // store the value and type in me:

          if( ansi )
					  value = storeCharString( value, "true" );
					else
					  value = storeCharString( value, "false" );

					type = storeCharString( type, "boolean" );

      }// relational ops
      
      if( op==andcode || op==orcode )
      {// logical operators
        if( fc !='b' )
        {
          status = illegaloperanderror;
        	// traceLeave();
          return firstbox;
        }
        if( sc !='b' )
        {
          status = illegaloperanderror;
					// traceLeave();
          return secondbox;
        }

        if( !(firstbox->value)->equals("true") &&
            !(firstbox->value)->equals("false")   )
        {
          status = illegaloperanderror;
					// traceLeave();
          return firstbox;
        }
        if( !(secondbox->value)->equals("true") &&
            !(secondbox->value)->equals("false")   )
        {
          status = illegaloperanderror;
					// traceLeave();
          return secondbox;
        }
          
        // produce ansi code
        if( op == andcode )
        {
          if( (firstbox->value)->equals("true") &&
              (secondbox->value)->equals("true")
            )
            ansi = 1;
          else
            ansi = 0;
        }
        else if( op == orcode )
        {
          if( (firstbox->value)->equals("true") ||
              (secondbox->value)->equals("true")
            )
            ansi = 1;
          else
            ansi = 0;        
        }
     
        // store the value and type in me:

          if( ansi )
					  value = storeCharString( value, "true" );
					else
					  value = storeCharString( value, "false" );

					type = storeCharString( type, "boolean" );

      }// logical ops
      
      if( (op==pluscode && fc != 's' && sc != 's' ) || 
			    op==minuscode || op==timescode ||
          op==divcode || op==modcode
        )
      {// arithmetic operators:  both operands must be numeric,

        // first, determine simplest numeric type that the ? type
        // can represent, error if isn't either one

        // v14:  make sure fc and sc are numeric, else error
        if( ! numeric( fc ) )
        {
          status = illegaloperanderror;
 					// traceLeave();
          return firstbox;
        }
        if( ! numeric( sc ) )
        {
          status = illegaloperanderror;
 					// traceLeave();
          return secondbox;
        }
        
      // v14:  now first case is fc=='i'
        if( fc == 'i' )
        {
          coerced = (firstbox->value)->toInt( fi );
          fd = fi;  // in case need to do float
		  // jb27:  value of firstbox could be empty, is logical error, not crash
          if( !coerced )
		  {
            status = illegaloperanderror;
            return firstbox;
		  }
        }
        else if( fc == 'f' )
        {
          coerced = (firstbox->value)->toFloat( fd );
		  // jb27:  same as int right above
          if( !coerced )
		  {
            status = illegaloperanderror;
            return firstbox;
		  }
        }
        else
          errorexit("unknown fc in arithmetic op case");

        if( sc == '?' )
        {// coerce to int, or float, or error
          coerced = (secondbox->value)->toInt( si );
          if( coerced )
          {
            sc = 'i';  // promote ? to int
            sd = si;
          }
          else
          {// wasn't an int, maybe is float
            coerced = (secondbox->value)->toFloat( sd );
            if( coerced )
              sc = 'f';  // promote ? to float
            else
            {
              status = illegaloperanderror;
              return secondbox;
            }// second arg is not numeric
          }//  second wasn't an int
        }// second is ?
        else if( sc == 'i' )
        {
          coerced = (secondbox->value)->toInt( si );
          sd = si;
		  // jb27:  same as for firstbox above
          if( !coerced )
		  {
            status = illegaloperanderror;
            return secondbox;
		  }
        }
        else if( sc == 'f' )
        {
          coerced = (secondbox->value)->toFloat( sd );
		  // jb27:  same as int above
          if( !coerced )
		  {
            status = illegaloperanderror;
            return secondbox;
		  }
        }
        else
          errorexit("unknown sc in arithmetic op case");

        // if still here, fc, sc are 'i' or 'f' and have the values

        char resulttype;  // will be 'i' or 'f'

        if( fc=='i' && sc=='i' )
        {// both int, so produce int result
       
          // have fi and si, go ahead and do the operation
          //  2/7/2001:  do all in parallel in
          //            double, check answer is within
          //             range
          resulttype = 'i';
          if( op==pluscode )
          {
            ansi = fi + si;
            ansd = (double) fi + (double) si;
          }
          else if( op==minuscode )
          {
            ansi = fi - si;
            ansd = (double) fi - (double) si;
          }
          else if( op==timescode )
          {
            ansi = fi * si;
            ansd = (double) fi * (double) si;
          }
          else if( op==divcode || op==modcode )
          {
            if( si == 0 )
            {
              status = divbyzeroerror;
    					// traceLeave();
              return secondbox;
            }

            if( op==divcode )
              ansi = fi / si;
            else
              ansi = fi % si;

            ansd = ansi;  // for uniformity with the others that can
                          //  overflow
          }

          // check for int overflow:
          if( ansd < - 999999999 || ansd > 999999999 )
          {          
            status = intrangeerror;
  					// traceLeave();
            return this;
          }

        }// int
          
        else 
        {// at least one is 'f', so do float
         
          // have two good ops, do the action into ansd
          resulttype = 'f';
          if( op == pluscode )
            ansd = fd + sd;
          else if( op == minuscode )
            ansd = fd - sd;
          else if( op == timescode )
            ansd = fd * sd;
          else if( op == divcode )
          {
            if( sd == 0 )
            {
              status = divbyzeroerror;
    					// traceLeave();
              return secondbox;
            }
            ansd = fd / sd;
          }
          else if( op == modcode )
          {
            status = illegaloperatorerror;
    				// traceLeave();
            return this;
          }

        }

        // build value,type depending on resulttype
        // store the value and type in me:
        if( resulttype == 'i' )
          sprintf( answer, "%ld", ansi );
        else 
          sprintf( answer, "%.12g", ansd );

        value = storeCharString( value, answer );
        if( resulttype == 'i' )
          type = storeCharString( type, "int" );
        else
          type = storeCharString( type, "float" );
      
      }// arithmetic types

      if( op==pluscode && (fc == 's' || sc == 's' )  )
      {// string concatenation of string and whatever primitive

        // no error checking nor conversion required!

        type = storeCharString( type, "string" );

        // produce value as concatenation of the two values
        value = storeString( value, firstbox->value );
				value->append( secondbox->value );				
				      
      }// string concatenation

      // oops --- forgot to set status if get here, no error
      status = 1;

      // strip consumed inners
      firstbox->stripValue();
      secondbox->stripValue();

      // put me on tour to show my nice new value
      ilist[ni] = this;
			cclist[ni] = newvaluecolorcode; ni++;

    }// opbox

    else if( kind == unopbox )
    {    
      // grab my inner
      firstbox = first;
      
      int op; // opcode for operator we'll be trying to do
      if( name->equals("-") )
        op=oppcode;
      else if( name->equals( "!" ) )
        op = notcode;
      else
        op=noopcode;  // no op

      // detect bad operator
      if( op==noopcode )
      {
        status = illegaloperatorerror;
    		// traceLeave();
        return this;
      }

      // detect obviously bad operands:
      // v14:  type might not exist
      // jb23:  strip out the type equals "?" leftover
      if( (firstbox->type == NULL ) ||
          (firstbox->value == NULL ) ||
          !isprimtype( firstbox->type ) 
        ) 
      {
        status = illegaloperanderror;
    		// traceLeave();
        return firstbox;
      }

      // check more carefully in cases depending on operators,
      // do all the necessary work in each subcase

         // for convenience, come up with a single char code 
         // representing the operand type:
         char fc = (firstbox->type)->charAt( 1 );
         double fd, ansd;
         long fi, ansi;
         int coerced; 
      
      if( op==notcode )
      {// logical operator
        if( fc !='b' &&  fc != '?' )
        {
          status = illegaloperanderror;
    			// traceLeave();
          return firstbox;
        }

        if( !(firstbox->value)->equals("true") &&
            !(firstbox->value)->equals("false")   )
        {
          status = illegaloperanderror;
    			// traceLeave();
          return firstbox;
        }

        if( (firstbox->value)->equals("true") )
				  value = storeCharString( value, "false" );
				else
				  value = storeCharString( value, "true" );

				type = storeCharString( type, "boolean" );

      }// logical op
      
      if( op==oppcode )
      {// arithmetic operator

        char resulttype;  // will be 'i' or 'f'

        if( fc=='i' )
        {// produce int result
       
          coerced = (firstbox->value)->toInt( fi );
          if( !coerced )
          {
            status = illegaloperanderror;
    				// traceLeave();
            return firstbox;
          }

          // have fi, go ahead and do the operation
          resulttype = 'i';
          ansi = - fi;

        }// int
          
        else if( fc=='f' )
        {// has to be a float op

          coerced = (firstbox->value)->toFloat( fd );
          if( !coerced )
          {
            status = illegaloperanderror;
    				// traceLeave();
            return firstbox;
          }

          resulttype = 'f';
          ansd = - fd;

        }
        else
        {
          status = incomptypeserror;
    			// traceLeave();
          return this;
        }

        // build value,type depending on resulttype
        // store the value and type in me:
        if( resulttype == 'i' )
          sprintf( answer, "%ld", ansi );
        else 
          sprintf( answer, "%.12g", ansd );

        value = storeCharString( value, answer );

				if( resulttype == 'i' )
				  type = storeCharString( type, "int" );
				else
				  type = storeCharString( type, "float" );
					      
      }// arithmetic type

      ilist[ni] = this; 
			cclist[ni] = newvaluecolorcode;
			ni++;

      // strip inner since consumed
      firstbox->stripValue();

    }// unopbox

    else if( kind == assbox )
    {// two inners are ready, do the storage, after checking

      // grab my two inners
      firstbox = first;
      secondbox = first->next;

      char op; // will hold which operator we're doing
      if( name->equals("=") )
        op=' ';
      else if( name->equals("+=") )
        op='+';
      else if( name->equals("-=") )
        op='-';
      else if( name->equals("*=") )
        op='*';
      else if( name->equals("/=") )
        op='/';
      else if( name->equals("%=") )
        op='%';
      else
        op='n';  // no op

	  // 10/2/2000:  if firstbox->source is NULL, is error in
	  //              all cases of assbox, wasn't caught before
	  if( firstbox->source == NULL )
	  {
        status = badtargetlocationerror;
		// traceLeave();
   	    return firstbox;				  
	  }

      // branch on op to analyze operand types and do it
      if( op=='n' )
        status = illegalassoperror;
      else if( op==' ' )
      {// assignment

        // had it wrong---don't want to change firstbox, instead
        // want to ask if firstbox->source is compatible, rather
        // than firstbox, because firstbox->source is going to be
        // getting the value!
				status = (firstbox->source)->storageCompatible( secondbox );
        if( status == 1  )
        {// types match or float=int case
         // so just transfer the data

          (firstbox->source)->storeValue( (secondbox->value) );
          status = 1;
        }
				else if( status == -1 )
				{// firstbox is bad
          status = badtargetlocationerror;
					// traceLeave();
					return firstbox;				  
				}
        else // status == 0
          status = incomptypeserror;
      }
      else
      {// jumper---op is one of +-*/%, gonna have to do some
        // arithmetic

        status = 1;   // stays there unless error detected

        // grab the value strings involved, make sure they
        // are both non-empty
				    // note:  these are just "abbreviation pointers"

        // jboxes1c:  oops, when came back and allowed string
        // for these guys, forgot that for a string, makes
        // sense for either box to have empty value, so
        // changed to allow

        mystring* frst = (firstbox->source)->value;
        mystring* scond = secondbox->value;
        if( frst == NULL || 
            (frst->equals( "" ) &&
               !((firstbox->source)->type)->equals( "string" ) // jboxes1c
            )
          )
          status = cantjumpemptyerror;
        else if( scond == NULL || 
                 ( scond->equals( "" ) &&
                   !(secondbox->type)->equals("string")
                 )
               )
          status = datanotiniterror;

        long ifirst, isecond;
        double dfirst, dsecond;
        char tcase;  // will be 'i' or 'f' or 's' (jb23)

        // jb23:  modify so can do string += any primitive type

        // determine legality and prepare operands
        if( ((firstbox->source)->type)->equals("int") &&
            (secondbox->type)->equals("int")    
          ) 
        {// int int
          tcase = 'i';
          ((firstbox->source)->value)->toInt( ifirst );
          (secondbox->value)->toInt( isecond );
        }

        /* jb23:  can't have the secondbox with type "?", so comment out
        else if( ((firstbox->source)->type)->equals("int") &&
                 (secondbox->type)->equals("?")  &&
                 (secondbox->value)->toInt( isecond )                             
               )   
        {// int <? that converts to int>
          tcase = 'i';
          ((firstbox->source)->value)->toInt( ifirst );
        }
        */

        else if( ((firstbox->source)->type)->equals("float") &&
                 (  (secondbox->type)->equals("float") ||
                    (secondbox->type)->equals("int")
                 )
               )  
        {// float op= <float or int>
          tcase = 'f';
          ((firstbox->source)->value)->toFloat( dfirst );
          (secondbox->value)->toFloat( dsecond );
        }

        /* jb23:  comment out this, too, can't have type == "?"
        else if( ((firstbox->source)->type)->equals("float") &&
                 (secondbox->type)->equals("?")  &&
                 (secondbox->value)->toFloat( dsecond )                             
               )   
        {// float op= <? that converts to float>
          tcase = 'f';
          ((firstbox->source)->value)->toFloat( dfirst );
        }
        */

        else if( op=='+' &&
                 ((firstbox->source)->type)->equals("string") &&
                 (  (secondbox->type)->equals("float") ||
                    (secondbox->type)->equals("int") ||
                    (secondbox->type)->equals("char") ||
                    (secondbox->type)->equals("string")
                 )
               )  
        {// string += any prim except boolean
          tcase = 's';         
        }
        
        else
          tcase = 'n';

        // branch and do it

        // produce char array answer from sprintf
        if( tcase == 'n' )
          status = incomptypeserror;

        else if( tcase == 's' )
        {// store below
  
        }

        else if( tcase == 'i' )
        {// all 5 are legal, watch out for / % if isecond is 0
// 2/2001:  proctect against integer overflow by doing in parallel
//          in double, checking size
          if( op == '+' )
          {
            iresult = ifirst + isecond;
            dresult = (double) ifirst + (double) isecond;
          }
          else if( op == '-' )
          {
            iresult = ifirst - isecond;
            dresult = (double) ifirst - (double) isecond;
          }
          else if( op == '*' )
          {
            iresult = ifirst * isecond;
            dresult = (double) ifirst * (double) isecond;
          }
          else if( op == '/' )
          {
            if( isecond == 0 )
              status = divbyzeroerror;
            else
              iresult = ifirst / isecond;

            dresult = iresult; // can't overflow
          }
          else if( op == '%' )
          {
            if( isecond == 0 )
              status = divbyzeroerror;
            else
              iresult = ifirst % isecond;

            dresult = iresult; // can't overflow
          }
          else
            errorexit("bad i op");

          // 2/2001:  intercept overflow here:
          if( dresult < -999999999 || 999999999 < dresult )
          {
            status = intrangeerror;
					  // traceLeave();
					  return this;				  
          }

          sprintf( answer, "%ld", iresult );
        }
        else if( tcase == 'f' )
        {// % is illegal, watch / for 0 divisor
          if( op == '+' )
            dresult = dfirst + dsecond;
          else if( op == '-' )
            dresult = dfirst - dsecond;
          else if( op == '*' )
            dresult = dfirst * dsecond;
          else if( op == '/' )
          {
            if( dsecond == 0 )
              status = divbyzeroerror;
            else
              dresult = dfirst / dsecond;
          }
          else if( op == '%' )
          {
            status = nofloatmoderror;
          }
          else
            errorexit("bad f op");

          sprintf( answer, "%.12g", dresult );
        }
        else
          errorexit("bad case in jumper of assbox");

        if( status == 1 )
        {// store the result back in first->source

          // jb23:  depending on tcase, store result
          if( tcase != 's' )
          {
            (firstbox->source)->value = 
	  				   storeCharString( (firstbox->source)->value,
		  				                      answer );
          }
          else
          {// concat scond onto end of frst
            frst->append(scond );
          }

        }

      }// jumper

      // if still here, want to put firstbox->source on tour
      ilist[ni] = firstbox->source; 
			cclist[ni] = newvaluecolorcode; ni++;
			// but put me on tour at end, so stays there if don't want
			// to go flying off --- and focuses the eye on the cause
			// and effect
      ilist[ni] = this; 
			cclist[ni] = signalcolorcode; ni++;    

      // strip inners
      firstbox->stripValue();
      secondbox->stripValue();

    }// assbox

    else if( kind == growbox )
    {// my one inner has a location ready
      
      // grab my inner
      firstbox = first;

      if( name->equals("++") )
        op='+';
      else if( name->equals("--") )
        op='-';
      else
        op='n';  // no op

      // branch on op to analyze operand types and do it
      if( op=='n' )
        status = illegalgrowoperror;
      else 
      {// op okay, check type of location---must be int

        // as with assbox, fix firstbox->type to be not the
        // location type (which is the box the variable's in),
        // but its source's type

        if( (firstbox->source)->type == NULL ||
            (firstbox->source)->value == NULL )
        {
          status = argnotlegalinterror;
          return firstbox;
        }

        firstbox->storeType( (firstbox->source)->type );

        if( ! (firstbox->type)->equals("int") )
        {
          status = notintegererror;
        }
        else
        {// okay to do it---get the value, grow it, store it
          // back

          // convert the value my inner points to an integer
          code = ((firstbox->source)->value)->toInt( iresult );

          if( !code )
          {
            status = argnotlegalinterror;
    				// traceLeave();
            return firstbox;
          }

          // do the tiny work
          if( op == '+' )
            iresult++;
          else
            iresult--;

          // 2/2001:  could kick over into overflow!
          //          can stay with non-double, just checking 1 off
          if( iresult < -999999999 || 999999999 < iresult )
          {
            status = intrangeerror;
            // traceLeave();
            return this;
          }

          // store the result back
          sprintf( answer, "%ld", iresult );

          (firstbox->source)->value =
					  storeCharString( (firstbox->source)->value,
						                     answer );

          // later will probably want to destroy firstbox->value,
          status = 1;
        } // okay to do it
      }// op okay

      // put place where value was stored on tour
      ilist[ni] = firstbox->source; 
			cclist[ni] = newvaluecolorcode; ni++;
			// put me on tour, too:
      ilist[ni] = this; 
			cclist[ni] = signalcolorcode; ni++;    

      // strip inner
      firstbox->stripValue();

    }// growbox

    else if( kind == newopbox )
    {
      if( getvalue == 0 )
      {// both inners were found empty, build a scalar
        if( isprimtype( name ) )
        {// can't build a single primitive box in heap!
          status = triedtonewprimerror;
        }
        else
        {// search for name in the universe's list of
          // classes, either find or don't
          newbox = addInstanceToHeap( u, status );

					ilist[ni] = newbox; cclist[ni] = newobjectcolorcode;
					ni++;

					ilist[ni] = this; cclist[ni] = signalcolorcode;
					ni++;

        }

        // no inners to strip

      }// build scalar
      else if ( getvalue == 1 || getvalue == 2 )
      {// build a 1D or 2D grid

        firstbox = first;
        if( firstbox->value == NULL ||
              ! (firstbox->value)->toInt( iresult ) 
          )
        {
          status = argnotlegalinterror;
    			// traceLeave();
          return firstbox;
        }
        // if still here, have iresult
        if( iresult <= 0 )
        {
          status = negsizerequestederror;
    			// traceLeave();
          return firstbox;
        }

        iresult2 = 0;

        if( getvalue == 2 )
        {// do argument stuff for second box

          secondbox = firstbox->next;
          if( secondbox->value == NULL ||
               !(secondbox->value)->toInt( iresult2 )
            )
          {
            status = argnotlegalinterror;
    				// traceLeave();
            return secondbox;
          }
          if( iresult2 <= 0 )
          {
            status = negsizerequestederror;
    				// traceLeave();
            return secondbox;
          }
        }

        // if get here, have iresult and iresult2 (may be 0 if 1D)

        // Note:  big room here for crashing by requesting
        //        too large of an array

        newbox = addGridboxToHeap( u, (int) iresult, (int) iresult2, 
                                    status );

				ilist[ni] = newbox; cclist[ni] = newobjectcolorcode;
				ni++;

				ilist[ni] = this; cclist[ni] = signalcolorcode;
				ni++;

        if( status < 0 )
        {
    			// traceLeave();
          return this;   // failed to build grid
        }

        firstbox->stripValue();
        if( iresult2 > 0 )
          secondbox->stripValue();

      }// build 1D or 2D grid

      // strip values in two places above where cases are
      // easier to spot

    }// newopbox

    else 
      errorexit("illegal kind in box::evaluate");

    // uniformly manage my appearance after evaluation
    // (if got here, no error occurred)

    // turns out I need to fixupReality, whatever my kind,
    // either because I have stripped values of my inners,
    // or I have pasted on a new value myself

    if( valuekind( kind ) &&
           aspect == 'f' )  // full aspect means I need to show
    {                       // my newly created "value"
      setAspect( 'v' );
    }

    fixupReality(0);

    // traceLeave();
    return this;  // for any sloppy errors that set status < 0
                  // but didn't return a specific badbox --- they
                  // just want me, as the overall culprit!

  }// box::evaluate
