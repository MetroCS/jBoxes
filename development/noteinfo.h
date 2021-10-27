// header for noteinfo data structure
//   a noteinfo has derived information about a notebox

const int maxNotes = 16;  // max size of array of notes

class noteinfo
{
  public:

    // constructor
    noteinfo( box* abox );

    void update( box* unibox, 
                 float cvw, float cvh, float pvw, float pvh );
    void display();

    // utility methods for Lesson::prepareNotes, arrangment
    int isPinned();  
    float pin( float current, float glue, float vw, float vh );
    void unpin();
    float getSize();
    int getGlue();
    char getEdge();
    void adjustInnerBorder( float& ulx, float& uly,
                            float& lrx, float& lry );

    void convertArrowheadToViewCoords( float cx, float cy );

    char getViewer();

    box* getNote();
    void destroy();

  private:

    box* nb;  // the notebox itself

    float w, h;  // width and height of the displayed note
                 // in world coords
                 // (determined after typesetting)

    mystring* text;  // the typeset text

    float nx, ny;   // coordinates of ulc of note in coordinate system
                    // of the univbox (only determined when lesson
                    // figures out where it ought to go)

    char edge;      // edge on which to situate this note
    char filling;     // arrangement style for note

    float ax, ay;   // coordinates of the ulc of arrowhead,
                    // First in 
                    // universal coordinate system (determined purely
                    // from the location string in the notebox),
                    // later converted to view region coords

    char astyle;    // arrowhead style ('a' 'l' 'f' or 'e')
                    // or 'n' for no arrow

    float aw, ah;   // width and height of arrowhead frame, only used if
                    // astyle is appropriate (determined once know the
                    // arrowhead style is 'f' and what part is pointed
                    // to by the location string)

    int pinned;     // utility data to let each noteinfo remember 
                    // whether it has been pinned down

    char viewerId;   // tells which viewer this note is displayed in

};
