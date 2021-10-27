
#include <stdio.h>
#include "mystring.h"
#include "portstype.h"

void errorexit( char message[] );

    // construct new fresh ports/files
    portstype::portstype()
		{
		  int k;
			for( k=0; k<numports; k++ )
			{
			  statuses[k] = 0;
				files[k] = NULL;
			}

		}	

    //   whichPort == 1..4, adjust to array index 0..3
	  int portstype::getStatus( int whichPort )
		{
		  return statuses[whichPort-1];
		}

    // attempt to connect the specified port with iochoice to
		//    filestring, return whether or not succeeded
		//   8/12/2000:  if filestring is empty, return false
		//    without attempting open --- raises an assertion
		//    failure to do fopen( "" ... ), apparently
    int portstype::attemptOpen( int whichPort, mystring* iochoice,
		                        mystring* filestring )
		{
		   whichPort--;  // make use 0..3 instead of conceptual 1..4

       if( filestring->equals( "" ) )
			 {
			   return 0;
			 }

       // convert filestring to usual string
			 char* filename = filestring->charCopy();

       if( iochoice->equals("input") )
			 {// try to open for input
         files[whichPort] = fopen( filename, "rb" );

				 delete filename;  // to save storage space

				 if( files[whichPort] == NULL )
				 {
				   return 0; // open failed
				 }
				 else
				 {
				   statuses[whichPort] = readingPortStatus;
					                 // connected for input now
				   return 1;  // open succeeded			   
				 }
			 }
			 else if( iochoice->equals("output") )
			 {// try to open for output
         files[whichPort] = fopen( filename, "wb" );

				 delete filename;  // to save storage space

				 if( files[whichPort] == NULL )
				   return 0; // open failed
				 else
				 {
				   statuses[whichPort] = writingPortStatus;
				   return 1;  // open succeeded			   
				 }
			 }
			 else
			   errorexit("illegal iochoice in ports::attemptOpen");
			 return 0; // (to satisfy compiler on all paths)
		}

    // close all open files 
    void portstype::closeAll()
		{
		  int k;
			for( k=0; k<numports; k++ )
			  if( statuses[k] != unusedPortStatus )
				{
				  statuses[k] = unusedPortStatus;
				  fclose( files[k] );
					files[k] = NULL;
				}
		}

		FILE* portstype::getFile( int whichPort )
		{
		  return files[ whichPort-1 ];
		}

    void portstype::noteEmpty( int whichPort )
		{
		  statuses[ whichPort-1 ] = emptyPortStatus;
    }

    void portstype::closePort( int whichPort )
		{
		  fclose( files[ whichPort-1 ] );
			files[ whichPort-1 ] = NULL;
			statuses[ whichPort-1 ] = 0;
		}
