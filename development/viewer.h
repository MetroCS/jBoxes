// this class is implemented in the file boxes.cpp so can
// use various globals in that file

class viewer 
{
public:
  // construct an empty viewer
  viewer();
		
  // initialize viewer to given settings, given in
  // scale=1 units, same as window units
  void init( int idcode );

  // set pixel size to (w,h), screen offset to (ulx,uly),
  // adjust conceptual size, in between rescale if says to
	//   8/9/2000:  if I'm portsviewer, set my backx, backy once
	//              know my scaled height
	//  jb19:  if doing author scaling, and this is a main viewer,
	//         set scale to match author window size
  void setSizeLocScale( int rescale,
            float w, float h, float ulx, float uly,
           float xs, float ys );

  // initialize viewer to given settings, given in
  // scale=1 units, same as window units
  void resetSize( float w, float h );

  void setOnScreen( char state );

  int isOnScreen();

  box* whoFocus();

  box* whoExec();

  int getId();

  // jb26:
  void getSize( float& vw, float& vh );

  void multScale( float f );

  void divScale( float f );

  // given window point (px,py), determine if is in my view area,
  // and if so, return 1 and (vx,vy) as scaled viewer coords of the
  // point,  if not found, return 0
  int locateLocation( float px, float py, float& vx, float& vy );

  mystring* getPercentageOffsets( float px, float py );

  // figure my focusx,y such that the given
  // newfocusbox will be centered in my view area
  void centerFocus( box* newfocusbox );

  // set my focusbox to the new one
  // and compute my backbox from it,
  // given a limitbox (if NULL, ignore)
  
  // "style" tells how to position over newfocus
  //     == 1 --> center the focus
  //     == 2 --> try to not move the focus 
  //     == 3 --> try to put the center of the apparent focus
  //              at the mouseupx, mouseupy point
	//     == 4 --> try to not move, even if means the focus is
	//               largely out of sight
  
  void setFocus( int style,
    box* newfocusbox, box* limitbox );

  // given (mx,my) as location in my view area,
  // set my focus, backbox, and all, based on moving the focus
  // to the part of box reality at that location, in terms of
  // my current backbox

	// 10/7/2000:  newly return in mx,my (turned in & params) the
	//             offset of the focus in the new focus box
	// 10/8/2000:  now use mouseoffx,y, so no changes here
  box* setFocusToLocation( float& mx, float& my, int& needredraw );

  // change offset of view area over focus by requested
  // fractions of full view width,height, then refigure
  // backbox
  void shift( float dx, float dy );

  // given translation vector (dx,dy) in pixels, translate
  // focus by that vector, transformed by scaling, and refigure
  // backbox
  void translate( float dx, float dy );

  // draw my portion of box reality in my viewport
  // using focus methodology, or in some special way depending
  // on my id
  void display();

  // set eb to execution box, I start going as the execution
	//  viewer, and return 1, else is error, and 0 is returned
  int startExecution( box* eb );

  // position myself tidily over the box of unique kind specialbox
  void fixOver( int specialbox );
 
  // manage a step of execution, from execbox
  //
  //  states are:
  //               initialstate
  //               waittoactstate
  //               waittoproducevaluestate
  //               waittoproducelocationstate
  //
  // new inspection list approach:  status is expunged---will
  //  pause when is something to look at
  void step();

  // need "box to draw"  (btd) to put in bounding box
  void startDrawToTeX( box* btd );

  void finishDrawToTeX();

  // jb25:  need to see where mouse is without committing
  box* findTargetBox( float mx, float my, char& whichpart, int& index );
  box* addNotebox( mystring* locstring );

  // jb26:  obtain backbox info
  box* getBackboxInfo( float& bx, float& by );

// viewer data
private:

  int tid;  // automatically generated tracing id

  int id;    // code number so I know what sort of viewer I am

  float pixelwidth, pixelheight;  // size of viewing region in
                                  // screen units
  float ulcx, ulcy;     // position of upper left corner in window
                        // in window units

  float width, height;  // size of viewing region in 
                        // scaled units

  float xscale, yscale; // display scaling

  // interactive stuff

  box* focusbox;        // pointer to my focus
  float focusx, focusy; // coordinates of ulc of apparent focus  
                         // in view area

  box* backbox;        // pointer to my current backbox
  float backx, backy;  // coordinates of ulc of backbox in
                       // view area, in scaled units

  // crucial for location of mouse clicks---have to know whether
  // various viewers that overlap screen regions are on-screen
  char onscreen;       // 'y' means on-screen, 'n' means not

  // execution stuff

  // will use this, maybe, if end up doing multi-viewers---idea
  // is to cut off display outside limit box, for clarity
  box* limitbox;  // the box I am restricted to
  float limx, limy;   // coordinates of ulc of limitbox in
                      // view area, in scaled units

  box* execbox;   // the box being executed, or just executed

  box* reqbox;    // the box that just asked another box to execute

};
