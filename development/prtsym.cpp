// prtsym.cpp:  contains just printsymbol and its constants

// Note:  buildsymbolinfo hardcodes the
//        physical symbol widths as determined by examining
//        the drawing code below

// define various constants that help to define the font

float top = 12;
float halfdotsize = 1;

// paths that make up fragments of symbols

float abodyshape[][2] = {{6,4.2},{3.6,4.8},{1.2,3.6},{0,2.4},
{1.2,0.6},{3.0,0},{5.4,0.9}, {6.0,2.4}};

float atopshape[][2] = {{1.2,7.2},{3,7.8},{4.2,7.2},{6,6},{6,0}};

float cshape[][2] = {{6,0.8},{4,0.2},{2,0.2},{1.0,0.90},
{0.2,2}, {0,4}, {0.2,5.4},{1.0,6.6}, {2,7.4},
{4,7.8}, {6,7.0}};

float eshape[][2] = {{6,0.8},{4,0.2},{2,0.2},{1.0,0.90},
{0.2,2}, {0,4}, {0.2,5.4},{1.0,6.6}, {2,7.4},
{4,7.8}, {5.0,7.4}, {5.8,6.4}, {6,5.2}, {6,4}, {0,4} };

float bshape[][2] = {{0,0.8},{2,0.2},{4,0.2},{5.0,0.90},
{5.8,2}, {6,4}, {5.8,5.4},{5.0,6.6}, {4,7.4},
{2,7.8}, {0,7.0}};

float gtailshape[][2] = {{6,0},{5.8,-1.6},
{5.4,-2.8}, {4.6,-3.8}, {3.6,-4}, {2.4,-4}, {1.4,-3.8},
{0.6,-2.8} };

float jtailshape[][2] = {{3.6,0},{3.4,-1.6},
{3.0,-2.8}, {2.2,-3.8}, {1.2,-4}, {0,-3.8} };

float fshape[][2] = {{0,0},{0.4,0.8},{1.0,1.6},{2,3.0},
{4,3.0}};

float humpshape[][2] = {{0,6.50},{1.8,7.6},{2.8,7.6}, {4.2,7.0},
{4.6,6.4}, {4.8,6}};

float oshape[][2]= {
  {0,4},{0.10,5.0},{.4,5.70},{1.0,6.6},{1.7,7.30},{2.4,7.8}, {3.6,8}, 
  {4.8,7.8}, {5.70,7.30}, {6.4,6.6}, {7.0 ,5.70}, {7.30 ,5.0}, {7.4,4},
  {7.30,3.0},{7.0,2.30},{6.4,1.4},{5.70,.70},{4.8,.2},
  {3.6,0},{2.4,.2},  {1.7,.70}, {1,1.4},{.4,2.30}, {0.10,3.0} };

float sshape[][2] = {{0,.8},{1.2,.2},{2.6,0},{4.2,.2},{5.4,1.6},
  {5.4,2.4},{4.6,3.4},{3.0,4},{1.4,4.6},{.4,5.4},{.2,6.4},
  {.8,7.4},{2,7.8},{3.0,8},{4.4,7.8},{5.2,7.4}};

float ttailshape[][2] = {{1.8,1.0},{2.2,.4},{3.0,.2},{3.8,0}};

float ushape[][2] = {{0,7.8},{0,2},{.2,1.0},{.8,.2},{1.4,0},{3.0,0},
{3.6,.2},{4.8,1.0} };

// graph paper units:
//  0,0  20,0  25,1  30,3  35,6  40,10  41,13, 41,18, 40,21,
//    36,26   30,29  27,30  0,30
//   (multiply by .2 to get screen units)
float Pshape[][2] = {{0,0},{4,0},{5,.2},{6,.6},{7,1.2},{8,2},
{8.2,2.6},{8.2,3.6},{8,4.2},{7.2,5.2},{6,5.8},{5.4,6.0},{0,6.0}};

float Cshape[][2] = {{8.6,11.6},{7.4,12.2},{5.8,12.4},{5,12.4},
{3.8,12},{2.4,11.2},{1.2,10},{0.6,9},{0,7.6},{0,6.4},
{0,5.2},{0.4,4},{1,2.4},{2,1.4},{3.2,0.6},{4,0.2},{5,0},{6.4,0},
{7.8,0.6},{9,1.2}};

float Dshape[][2] = {{0,0},{0,11.6},{4.8,11.6},{6.2,11.2},
{7.4,10.6},{8.2,9.6},{8.8,8.8},{9.4,7.2},{9.4,4.8},{9.2,3.6},
{8.8,2.8},{8,1.6},{6.6,.6},{5.6,.2},{4.8,0} };

float Jshape[][2] = {{0,0.95},{1,0.4},{2.2,0},{3.2,0},
{4,.2},{4.8,.6},{5.8,2},{6,2.8},{6,12}};

float Oshape[][2] = {{0,6.2},{0,7.2},{.8,9.2},{2,11},{3.2,12},
{5,12.2},{6.4,12.2},{8,11.4},{9,10.4},{10,9},{10.4,7.6},
{10.4,5.6},{10.2,4},{9,2},{8,1},{6.4,.2},{5.6,0},{5,0},
{3.6,.4},{2,1.4},{.8,3},{.2,4.2}};

float Sshape[][2] = {{0,1},{1,.4},{2.2,0},{4.2,0},{5,.4},
{5.8,1},{6.6,2},{6.8,3},{6.6,4},{6.2,4.6},{5,5.2},{2.6,6},
{1.2,6.6},{.4,7.4},{0,8.4},{.2,9.4},{1.4,10.6},{3,11.2},
{4.6,11},{6,10.4}};

float Ushape[][2] = {{0,12},{0,4},{.2,3},{.8,2},{1.4,1.2},
{2.8,.4},{3.8,0},{4.8,0},{5.8,.4},{7,1.2},{8,2.4},
{8.4,4},{8.4,12}};

float oneshape[][2] = {{0,9.2},{1.4,9.4},{3,10},{3,0}};

float twoshape[][2] = {{0,8},{1,9.4},{2,10},{3,10.2},
{4.6,9.6},{5.4,8.6},{5.8,7.4},{5.6,6},{4.6,4.6},{2,2.2},{0,0},{6,0}};

float threeshape[][2] = {{0,9},{1,10},{2,10.6},{3,10.8},{4,10.6},
{5,9.8},{5.4,9},{5,7.6},{4,6.2},{3.2,5.8},{2,5.65},{3.2,5.6},
{4.4,5.4},{5.4,4.8},{6,3.6},{6,2},{5.2,1},{4,.2},{3,0},{1.8,.2},
{0,1.67}};

float fourshape[][2] = {{7,3.4},{0,3.4},{4.4,10.6},{4.4,0}};

float fiveshape[][2] = {{5.2,10.2},{0,10.2},{0,4.8},{1.8,6.2},
{2.4,6.4},{3.4,6.4},{4.4,5.6},{4.8,4.8},{5,4},{5,2.8},{4.8,2},
{4.2,1},{3.8,.6},{3,.2},{2.2,0},{.6,.4},{0,.8}};

float sixshape[][2] = {{5.4,10.4},{4.2,10.8},{3,10.6},{2,10.2},
{1,9},{.4,7.8},{.2,7},{0,5.4},{0,4.2},{0,3.2},{.4,2},{1,1},{1.8,.4},
{3,0},{4,.2},{4.6,.6},{5.4,1.8},{5.6,2.6},{5.6,4},{5.4,5.2},{5,6.2},
{4,7},{3.2,7.1},{2.2,6.8},{1.2,6},{.6,5.2},{0,4.2}};

float sevenshape[][2] = {{0,10.4},{6.6,10.4},{5.8,9.2},{5,7.8},{4,6},
{3.4,4},{3.2,3},{3,0}};

float top8cycle[][2]= {{3,6},{2.2,6},{1.1,6.4},{.4,7},{0,8},{0,8.4},
{.4,9.4},{1,10},{2,10.4},{3,10.6},{4,10.4},{4.8,10},{5.4,9.4},
{5.6,8.6},{5.4,8},{5.2,7.2},{4.4,6.4},{4,6}};

float bot8cycle[][2] = {{3,6},{2,5.6},{1.2,5},{.6,4},{.4,3.2},
{.4,2.4},{.6,1.6},{1.2,.8},{2.4,.2},{3,0},{3.6,0},{4.4,.2},{5.2,.8},
{5.6,1.4},{6,3},{5.8,3.8},{5.4,4.6},{4.8,5.4},{4,6}};

float nineshape[][2] = {{.2,.6},{2,0},{2.6,0},{4,1},{4.8,2},{5.2,3},
{5.4,4},{5.6,6},{5.6,7},{5.4,7.4},{5.2,8.4},{4.6,9.8},{3.8,10.4},
{3,10.6},{2,10.6},{1.3,10.2},{.4,8.8},{0,7.6},{0,6.6},{.2,5.6},
{1,4.2},{2,3.6},{2.8,3.6},{4.2,4.4},{4.8,5},{5.2,6},{5.6,7.4}};

/* changed zerocycle 2/13/04
float zerocycle[][2] = {{0,6},{.2,7.4},{.8,9.2},{1.4,10.2},{2,10.6},
{3,10.8},{4,10.6},{4.8,9.8},{5.4,9},{5.6,8},{5.8,6},{5.8,3.4},
{5.4,2},{5,1},{4.4,.4},{3.8,0},{3,0},{2,.4},{1.4,1.2},{.8,2.2},
{.2,3.6},{0,5}};
*/
float zerocycle[][2] = {{0,6},{.2,7.4},{.8,9.2},{1.4,10.2},
{2,10.6},{3,10.8},
{4,10.6},{4.6,10.2},{5.2,9.2},{5.8,7.4},{6,6},
{6,4.8},{5.8,3.4},{5.2,1.6},{4.6,0.6},{4,0.2},
{3,0},
{2,0.2},{1.4,0.6},{0.8,1.6},{0.2,3.4},{0,4.8} };

float dollarshape[][2]={{0,.8},{1,.2},{1.6,0},{2.8,0},{4,.2},
{5,1},{5.6,2},{5.6,3},{5.4,4},{4.4,4.8},{2.8,5.4},{1.4,6},
{.4,7},{0,8},{.4,9.2},{1.4,10.3},{2,10.6},{2.6,10.8},{4,10.6},
{5.6,10}};

float ampersandshape[][2]={{10.2,.4},{8.8,0},{7.6,.2},{6.6,.6},
{5.8,1.4},{5,2.2},{3.4,4.2},{2.6,5.8},{2.2,6.6},{1.8,8},{2,9},
{2.2,10.4},{3,11.2},{4,11.4},{5,11},{5.4,10},{5.2,9},{4.6,7.8},
{2.6,5.8},{1.2,4.6},{.4,3.8},{0,2.4},{.4,1.2},{1.2,.4},{2.4,0},
{3.2,0},{4.4,.6},{5.8,1.4},{7.2,2.6},{8.2,4.2},{9.4,6.4}};

float rightparenshape[][2]={{0,-4},{1.6,-1.8},{2.6,0},{2.8,1},{3,4},
{2.8,5},{2.4,9},{1.8,10},{0,12.6}};

float leftparenshape[][2]={{3,-4},{1.4,-1.8},{.4,0},{.2,1},{0,4},
{.2,5},{.6,9},{1.2,10},{3,12.6}};

float atshape[][2]={{8.2,1.2},{6.8,.2},{5,0},{4,.2},{3.2,.4},
{1.6,1.6},{.8,2.6},{.2,4},{0,5.4},{0,6},{.4,7.6},{1,8.8},{2,9.8},
{3,10.6},{4,11},{5,11},{6,10.8},{7.2,10},{7.8,9},{8.2,8},{8.4,6.6},
{8.2,5.4},{7.8,4},{7.2,3},{6.6,2.6},{6,2.2},{5.4,2.2},{4.2,2.8},
{3.6,3.6},{3.2,4.6},{3.2,6},{3.4,7},{4,8},{5,8.8},{6,8.6},{7,8.4},
{7.8,7.4},{8.2,5.4}};

float leftquoteshape[][2]={{0,8},{0,9},{.4,10},{1,10.6}};

float rightquoteshape[][2]={{1,8},{1,9},{.6,10},{0,10.6}};

float percentcircleshape[][2]={{0,1.2},{.2,2.2},{.4,3},{1,3.8},
{1.4,4},{2,4},{2.4,3.8},{3,3.2},{3.4,2.2},{3.4,1.2},{3.2,0},
{2.8,-1},{2.2,-1.6},{1.4,-1.6},{.6,-1},{.2,0}};

float tildeshape[][2]={{0,2.2},{.4,4},{1,5},{2,5.6},{3,5.8},
{4,5.4},{5.6,4.2},{7.4,3},{8.2,2.8},{8.8,2.8},{9.6,3.4},{10.2,4},
{10.8,5},{10.8,6.2}};

float lbrackshape[][2]={{2.2,-4},{0,-4},{0,12},{2.2,12}};
float rbrackshape[][2]={{0,-4},{2.2,-4},{2.2,12},{0,12}};
float lbraceshape[][2]={{5.6,12.2},{4.4,12.2},{3.6,11.8},{2.8,11},
{2.6,10.4},{2.4,6},{2,5},{1,4.4},{0,4.2},{1,4},{2,3.4},{2.4,2.4},
{2.6,-2},{2.8,-2.6},{3.6,-3.4},{4.4,-3.8},{5.6,-3.8}};

float rbraceshape[][2]={{0,12.2},{1.2,12.2},{2,11.8},{2.8,11},
{3,10.4},{3.2,6},{3.6,5},{4.6,4.4},{5.6,4.2},{4.6,4},{3.6,3.4},
{3.2,2.4},{3,-2},{2.8,-2.6},{2,-3.4},{1.2,-3.8},{0,-3.8}};

float commashape[][2]={{0,.6},{.2,1.2},{.6,1.4},{1,1.5},{1.6,1.2},
{2,.6},{2,0},{1.8,-.8},{1.4,-1.6},{1,-2},{.4,-2.2},{0,-2.1},
{.6,-1.6},{.8,-1.2},{.2,0}};

float lqshape[][2]={{0,0},{.2,.8},{.6,1.6},{1,2},{1.6,2.2},{2,2.1},
{1.4,1.8},{1.2,1.2},{1.2,.6},{1.8,0},{2,-.6},{1.8,-1.2},{1.4,-1.4},
{1,-1.5},{.4,-1.2},{0,-.6}};

float dotshape[][2]={{0,1},{.4,1.6},{1,1.8},{1.6,1.6},{1.8,1},
{1.6,.4},{1,0},{.2,.2}};

float quoteshape[][2]={{0,10},{.4,10.4},{1,10.4},{1.4,10},{1.2,6},
{.8,5.6},{.4,5.6},{.2,6}};

float questionshape[][2]={{0,10.6},{1,11},{2,11.2},{3,11.2},
{4.4,10.6},{5,9.8},{5.1,9.2},{4.6,8},{3.4,6.4},{3,6},{2.6,5},{2.4,3}};

// utility symbol part drawing functions 

// given array of points, number of points, (x,y), draw
//  the path as a linestrip, offset from (x,y), returning
//  (lastx, lasty) as offset from the origin
void drawsympath( float path[][2], int n, float x, float y,
                float& lastx, float& lasty )
{
  int k;

  if( !texdraw )
  {// draw to screen 
    glBegin(GL_LINE_STRIP);
      for( k=0; k<n; k++ )
        glVertex3f( x+ path[k][0], y+ path[k][1], textz );
    glEnd();
  }
  else
  {// draw to texfile

    if( pictex )
		{
		  for( k=0; k<n-1; k++ )
			  pictexLine( x+path[k][0], y+path[k][1],
				            x+path[k+1][0], y+path[k+1][1] );
		}
		else
		{
      psStartPath( x+ path[0][0], y+ path[0][1] );
      for(k=1; k<n; k++)
        psLineTo( x+ path[k][0], y+ path[k][1] );
//    psEndPath( 0 );
      psStroke();
    }
  }

  lastx = path[n-1][0];  lasty = path[n-1][1];
}

// path holds vertices in a cycle, do a line loop
void drawsymcycle( float path[][2], int n, float x, float y )
{
  int k;

  if( !texdraw )
  {// draw to screen 
    glBegin(GL_LINE_LOOP);
      for( k=0; k<n; k++ )
        glVertex3f( x+ path[k][0], y+ path[k][1], textz );
    glEnd();
  }
  else
  {// draw to texfile
    
		if( pictex )
		{
		  for( k=0; k<n-1; k++ )
			  pictexLine( x+path[k][0], y+path[k][1],
				            x+path[k+1][0], y+path[k+1][1] );
  	  pictexLine( x+path[n-1][0], y+path[n-1][1],
			            x+path[0][0], y+path[0][1] );

		}
		else
		{
      psStartPath( x+ path[0][0], y+ path[0][1] );
      for(k=1; k<n; k++)
        psLineTo( x+ path[k][0], y+ path[k][1] );
      psEndPath( 0 );
		}

  }

}

void drawsymline( float x, float y, 
                   float x1, float y1, float x2, float y2 )
{
  if( !texdraw )
  {// draw to screen
    glBegin(GL_LINES);
      glVertex3f( x+ x1, y+ y1, textz );
      glVertex3f( x+ x2, y+ y2, textz );
    glEnd();
  }
  else
  {// draw to texfile

    if( pictex )
		{
		  pictexLine( x+x1, y+y1, x+x2, y+y2 );
		}
		else
		{
      psMoveTo( x+x1, y+y1 );
      psLineTo( x+x2, y+y2 );
      psStroke();
		}

  }
}

void drawsymdot( float x, float y,
                  float x1, float y1 )
{
  if( !texdraw )
  {// draw to screen
    glBegin(GL_POLYGON);
      glVertex3f( x+x1-halfdotsize, y+y1+halfdotsize,
                      textz );
      glVertex3f( x+x1+halfdotsize, y+y1+halfdotsize,
                      textz );
      glVertex3f( x+x1+halfdotsize, y+y1-halfdotsize,
                      textz );
      glVertex3f( x+x1-halfdotsize, y+y1-halfdotsize,
                      textz );
      
    glEnd();
  }
  else
  {// draw to texfile

    if( pictex )
		{
		  pictexRect( x+x1-halfdotsize, y+y1-halfdotsize,
			            x+x1+halfdotsize, y+y1+halfdotsize, 'F' );
		}
		else
		{
      psStartPath( x+x1-halfdotsize, y+y1+halfdotsize );
      psLineTo( x+x1+halfdotsize, y+y1+halfdotsize );
      psLineTo( x+x1+halfdotsize, y+y1-halfdotsize );
      psLineTo( x+x1-halfdotsize, y+y1-halfdotsize);
      psEndPath( 1 );
		}

  }
}

// draw sym with its LOWER LEFT CORNER at (x,y)
void drawprintable( float x, float y, char sym )
{
  float lastx, lasty, lastx2;

  // just for more efficient branching (dumb!) but really
  // for organizing, arrange into groups
  if( 'a' <= sym && sym <= 'z' )
  {// lower case

    if(sym=='a')
    {
      drawsympath( abodyshape, 8, x, y,
                lastx, lasty );
      drawsympath( atopshape, 5, x, y, lastx, lasty );
    }
    else if(sym=='b')
    {
      drawsympath( bshape, 11, x, y, lastx, lasty );
      drawsymline( x, y, 0, 0, 0, top );
    }
    else if(sym=='c')
    {
      drawsympath( cshape, 11, x, y,
                lastx, lasty );
    }     
    else if(sym=='d')
    {
      drawsympath( cshape, 11, x, y, lastx, lasty );
      drawsymline( x, y, lastx, 0, lastx, top );
    }
    else if(sym=='e')
    {
      drawsympath( eshape, 15, x, y, lastx, lasty );
    }
    else if(sym=='f')
    {
      drawsympath( fshape, 5, x+1.8, y+9.6, lastx, lasty );
      drawsymline( x, y, 1.8, 0, 1.8, 9.6 );
      drawsymline( x, y, 0, 7.4, 4.2, 7.4 );
    }
    else if(sym=='g')
    {
      drawsympath( cshape, 11, x, y,
                lastx, lasty );
      drawsymline( x, y, lastx, 8, lastx, 0 );
      drawsympath( gtailshape, 8, x, y, lastx, lasty );
    }     
    else if(sym=='h')
    {
      drawsympath( humpshape, 6, x, y,
                lastx, lasty );
      drawsymline( x, y, lastx, lasty, lastx, 0 );
      drawsymline( x, y, 0, 0, 0, top );     
    }     
    else if(sym=='i')
    {
      drawsymline( x, y, halfdotsize, (2/3.0)*top, halfdotsize, 0 );
      drawsymdot( x, y, halfdotsize, top );
    }
    else if(sym=='j')
    {
      drawsymline( x, y, 3.6, 8, 3.6, 0 );
      drawsymdot( x, y, 3.6, top );
      drawsympath( jtailshape, 6, x, y, lastx, lasty );
    }
    else if(sym=='k')
    {
      drawsymline( x, y, 0, 0, 0, top );
      drawsymline( x, y, 0,  2.4, 6, 7.2 );
      drawsymline( x, y, 6,0, 2.4, 4.8 );
    }
    else if(sym=='l')
    {
      drawsymline( x, y, 0, 0, 0, top );
    }
    else if (sym=='m')
    {
      drawsymline( x, y, 0, 0, 0, 8 );
      drawsympath( humpshape, 6, x, y, lastx, lasty );
      drawsymline( x, y, lastx, lasty, lastx, 0 );
      drawsympath( humpshape, 6, x+lastx, y, lastx2, lasty );
      lastx += lastx2;
      drawsymline( x, y, lastx, lasty, lastx, 0 );
    }
    else if (sym=='n')
    {
      drawsymline( x, y, 0, 0, 0, 8 );
      drawsympath( humpshape, 6, x, y, lastx, lasty );
      drawsymline( x, y, lastx, lasty, lastx, 0 );
    }
    else if(sym=='o')
    {
      drawsymcycle( oshape, 24, x, y );
    }
    else if(sym=='p')
    {
      drawsympath( bshape, 11, x, y, lastx, lasty );
      drawsymline( x, y, 0, 8, 0, -4 );
    }
    else if(sym=='q')
    {
      drawsympath( cshape, 11, x, y, lastx, lasty );
      drawsymline( x, y, lastx, 8, lastx, -4 );
    }
    else if(sym=='r')
    {
      drawsymline( x, y, 0, 0, 0, 8 );
      drawsympath( humpshape, 6, x, y, lastx, lasty );
    }
    else if(sym=='s')
    {
      drawsympath( sshape, 16, x, y, lastx, lasty );
    }
    else if(sym=='t')
    {
      drawsymline( x, y, 1.8, 1.0, 1.8, 10 ); // vertical
      drawsymline( x, y, 0, 7.4, 4.2, 7.4 ); // cross bar
      drawsympath( ttailshape, 4, x, y, lastx, lasty );
    }
    else if(sym=='u')
    {
      drawsympath( ushape, 8, x, y, lastx, lasty );      
      drawsymline( x, y, 4.8, 7.8, 4.8,0);     
    }
    else if(sym=='v')
    {
      drawsymline( x, y, 0, 8, 4, 0 );
      drawsymline( x, y, 4, 0, 8, 8 );
    }
    else if(sym=='w')
    {
      drawsymline( x, y, 0, 8, 3.0, 0 );
      drawsymline( x, y, 3.0, 0, 6, 8 );
      drawsymline( x, y, 6, 8, 9.0, 0 );
      drawsymline( x, y, 9.0, 0, 12, 8 );
    }
    else if(sym=='x')
    {
      drawsymline( x, y, 0, 8, 8, 0 );
      drawsymline( x, y, 0, 0, 8, 8 );
    }
    else if(sym=='y')
    {
      drawsymline( x, y, 0, 8, 4, 0 );
      drawsymline( x, y, 2,-4, 8, 8 );
      drawsymline( x, y, 0, -4, 2, -4 );
    }
    else if(sym=='z')
    {
      drawsymline( x, y, 0, 8, 6.4, 8 );
      drawsymline( x, y, 6.4, 8, 0, 0 );
      drawsymline( x, y, 0,0, 6.4,0);
    }

  }// lower case

  else if( 'A' <= sym && sym <= 'Z' )
  {// upper case

    if(sym=='A')
    {
      drawsymline( x, y, 0, 0, 5.5, 12 );
      drawsymline( x, y, 5.5, 12, 11, 0 );
      drawsymline( x, y, 1.8, 4, 9.2, 4 );
    }
    else if(sym=='B')
    {
      drawsympath( Pshape, 13, x, y, lastx, lasty );      
      drawsympath( Pshape, 13, x, y+6, lastx, lasty );
      drawsymline( x, y, 0,0,0,12);     
    }
    else if(sym=='C')
    {
      drawsympath( Cshape, 20, x, y, lastx, lasty );      
    }
    else if(sym=='D')
    {
      drawsymcycle( Dshape, 15, x, y);      
    }
    else if(sym=='E')
    {
      drawsymline( x, y, 0, 0, 8, 0 );
      drawsymline( x, y, 0, 12, 8, 12 );
      drawsymline( x, y, 0, 6, 7.2, 6 );
      drawsymline( x, y, 0, 0, 0, 12 );
    }
    else if(sym=='F')
    {
      drawsymline( x, y, 0, 12, 8, 12 );
      drawsymline( x, y, 0, 6, 7.2, 6 );
      drawsymline( x, y, 0, 0, 0, 12 );
    }
    else if(sym=='G')
    {
      drawsympath( Cshape, 20, x, y, lastx, lasty );      
      drawsymline( x, y, 9,1, 9,5.4);
      drawsymline( x, y, 9,5.4,5.4,5.4);
    }
    else if(sym=='H')
    {
      drawsymline( x, y, 0, 0, 0, 12 );
      drawsymline( x, y, 9, 0, 9, 12 );
      drawsymline( x, y, 0, 6, 9, 6 );
      } 
    else if(sym=='I')
    {
      drawsymline( x, y, 3, 0, 3, 12 );
      drawsymline( x, y, 0, 0, 6, 0 );
      drawsymline( x, y, 0, 12, 6, 12 );
    } 
    else if(sym=='J')
    {
      drawsympath( Jshape, 9, x, y, lastx, lasty );      
    }
    else if(sym=='K')
    {
      drawsymline( x, y, 0, 0, 0, 12 );
      drawsymline( x, y, 0, 5, 9, 12 );
      drawsymline( x, y, 3, 7.5, 9, 0 );
    } 
    else if(sym=='L')
    {
      drawsymline( x, y, 0, 0, 7.5, 0 );
      drawsymline( x, y, 0, 0, 0, 12 );
    } 
    else if(sym=='M')
    {
      drawsymline( x, y, 0, 0, 0, 12 );
      drawsymline( x, y, 0, 12, 6, 0 );
      drawsymline( x, y, 6, 0, 12, 12 );
      drawsymline( x, y, 12, 12, 12, 0 );
    } 
    else if(sym=='N')
    {
      drawsymline( x, y, 0, 0, 0, 12 );
      drawsymline( x, y, 0, 12, 9, 0 );
      drawsymline( x, y, 9, 0, 9, 12 );
    } 
    else if(sym=='O')
    {
      drawsymcycle( Oshape, 22, x, y );
    }
    else if(sym=='P')
    {
      drawsympath( Pshape, 13, x, y+6, lastx, lasty );
      drawsymline( x, y, 0,0,0,12);     
    }
    else if(sym=='Q')
    {
      drawsymcycle( Oshape, 22, x, y );
      drawsymline( x, y, 5,3.6, 9.2,-1.8);     
    }
    else if(sym=='R')
    {
      drawsympath( Pshape, 13, x, y+6, lastx, lasty );
      drawsymline( x, y, 0,0,0,12);     
      drawsymline( x, y, 8.2, 0, 5, 6 );
    }
    else if(sym=='S')
    {
      drawsympath( Sshape, 20, x, y, lastx, lasty );
    }
    else if(sym=='T')
    {
      drawsymline( x, y, 0, 12, 10.5, 12 );
      drawsymline( x, y, 5.25, 0, 5.25, 12 );     
    } 
    else if(sym=='U')
    {
      drawsympath( Ushape, 13, x, y, lastx, lasty );
    }
    else if(sym=='V')
    {
      drawsymline( x, y, 0, 12, 5.5, 0 );
      drawsymline( x, y, 5.5, 0, 11, 12 );
    } 
    else if(sym=='W')
    {
      drawsymline( x, y, 0, 12, 4, 0 );
      drawsymline( x, y, 4,0, 8,12);
      drawsymline( x, y, 8,12, 12,0 );
      drawsymline( x, y, 12,0, 16,12 );
    } 
    else if(sym=='X')
    {
      drawsymline( x, y, 0, 0, 10, 12 );
      drawsymline( x, y, 0, 12, 10, 0 );
    } 
    else if(sym=='Y')
    {
      drawsymline( x, y, 0,12, 5.75,5 );
      drawsymline( x, y, 5.75,5, 5.75,0 );
      drawsymline( x, y, 5.75,5, 11.5,12 );
    } 
    else if(sym=='Z')
    {
      drawsymline( x, y, 0, 12, 11, 12 );
      drawsymline( x, y, 11,12, 0,0 );
      drawsymline( x, y, 0,0, 11, 0 );
    } 

  }
  else
  {// non-letter
    if(sym=='1')
    {
      drawsympath( oneshape, 4, x, y, lastx, lasty );  
      drawsymline( x, y, 0, 0, 6, 0 );
    }
    else if(sym=='2')
    {
      drawsympath( twoshape, 12, x, y, lastx, lasty );   
    }
    else if(sym=='3')
    {
      drawsympath( threeshape, 21, x, y, lastx, lasty );   
    }
    else if(sym=='4')
    {
      drawsympath( fourshape, 4, x, y, lastx, lasty );   
    }
    else if(sym=='5')
    {
      drawsympath( fiveshape, 17, x, y, lastx, lasty );   
    }
    else if(sym=='6')
    {
      drawsympath( sixshape, 27, x, y, lastx, lasty );   
    }
    else if(sym=='7')
    {
      drawsympath( sevenshape, 8, x, y, lastx, lasty );   
    }
    else if(sym=='8')
    {
      drawsymcycle( top8cycle, 18, x, y );   
      drawsymcycle( bot8cycle, 19, x, y );   
    }
    else if(sym=='9')
    {
      drawsympath( nineshape, 27, x, y, lastx, lasty );   
    }
    else if(sym=='0')
    {
      drawsymcycle( zerocycle, 22, x, y );   
    }
    else if(sym=='$')
    {
      drawsympath(  dollarshape, 20, x, y, lastx, lasty );
      drawsymline( x, y, 3, -1.4, 3, 12 );
    }
    else if(sym=='&')
    {
      drawsympath( ampersandshape,  31, x, y, lastx, lasty );
    }
    else if(sym=='(')
    {
      drawsympath( leftparenshape, 9, x, y, lastx, lasty );
    }
    else if(sym==')')
    {
      drawsympath( rightparenshape, 9, x, y, lastx, lasty );
    }
    else if(sym=='@')
    {
      drawsympath( atshape,  38 , x, y, lastx, lasty );
    }
    else if(sym=='`')
    {
      drawsymcycle( lqshape, 15, x, y+12 );
    }
    else if(sym== 39) 
    {
      drawsymcycle( commashape, 15, x, y+12 );
    }
    else if(sym=='*')
    {
      drawsymline( x,y, 4.33,0, 4.33,10 );
      drawsymline( x,y, 0,2.5, 8.66, 7.5 );
      drawsymline( x,y, 0,7.5, 8.66,2.5 );
    }     
    else if(sym=='#')
    {
      drawsymline( x,y, 0,2,11.8,2 );
      drawsymline( x,y, 0,5.4,11.8,5.4 );
      drawsymline( x,y, 2,-3.8,5.8,11 );
      drawsymline( x,y, 6,-3.8,9.8,11 );
    } 
    else if(sym=='!')
    {
      drawsymline( x, y, .9,4,.9,10.8);
      drawsymcycle( dotshape, 8, x, y );
    }
    else if(sym=='%')
    {
      drawsymcycle( percentcircleshape, 16, x,y+8 );
      drawsymcycle( percentcircleshape, 16, x+6.8, y+1.4 );
      drawsymline( x,y, 1.4,-.8,9.4,12.4);
    }
    else if(sym=='~')
    {
      drawsympath( tildeshape, 14, x,y, lastx, lasty );
    }
    else if(sym=='+')
    {
      drawsymline( x,y, 0,4.2,10.8,4.2 );
      drawsymline( x,y, 5.2,-1.2,5.2,9.8 );
    } 
    else if(sym=='=')
    {
      drawsymline( x,y, 0,2.2,10.8,2.2 );
      drawsymline( x,y, 0,5.4,10.8,5.4 );
    } 
    else if(sym== 92 )  // backslash
    {
      drawsymline( x,y, 0,12.4,6,-3.6 );
    } 
    else if(sym=='|')
    {
      drawsymline( x,y, 0,-3.6, 0,3 );
      drawsymline( x,y, 0,5.8, 0, 12.4 );
    } 
    else if(sym=='[')
    {
      drawsympath( lbrackshape, 4, x, y, lastx, lasty );
    }
    else if(sym==']')
    {
      drawsympath( rbrackshape, 4, x, y, lastx, lasty );
    }
    else if(sym=='{')
    {
      drawsympath( lbraceshape, 17, x, y, lastx, lasty );
    }
    else if(sym=='}')
    {
      drawsympath( rbraceshape, 17, x, y, lastx, lasty );
    }
    else if(sym=='_')
    {
      drawsymline( x,y, 0,0,6,0);
    } 
    else if(sym=='-')
    {
      drawsymline( x,y, 0,4,8,4);
    } 
    else if(sym== '.')
    {
      drawsymcycle( dotshape, 8, x, y );
    }
    else if(sym== ',')
    {
      drawsymcycle( commashape,  15, x, y );
    }
    else if(sym== ':')
    {
      drawsymcycle( dotshape, 8, x, y );
      drawsymcycle( dotshape, 8, x, y+6 );
    }
    else if(sym== ';')
    {
      drawsymcycle( commashape,  15, x, y );
      drawsymcycle( dotshape, 8, x, y+6 );
    }
    else if(sym=='?')
    {
      drawsympath( questionshape, 12, x, y, lastx, lasty );
      drawsymcycle( dotshape, 8, x+1.5, y );
    }
    else if(sym=='"')
    {
      drawsymcycle( quoteshape, 8, x, y );
      drawsymcycle( quoteshape, 8, x+3, y );
    }
    else if(sym=='/')
    {
      drawsymline( x, y, 0,-4,6,12);
    }
    else if(sym=='^')
    {
      drawsymline( x,y,0,9.8,2.3,12.3 );
      drawsymline( x,y,2.3,12,4.6,9.8 );
    } 
    else if(sym=='<')
    {
      drawsymline( x,y, 0,4.3,9.4,8.8 );
      drawsymline( x,y, 0,4.3,9.4,-.2 );
    } 
    else if(sym=='>')
    {
      drawsymline( x,y, 9.4,4.3,0,8.8 );
      drawsymline( x,y, 9.4,4.3,0,-.2);
    } 
    
  }
}
