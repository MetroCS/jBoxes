// header for MessageQueue

// message constants
const int MQ_REG_KEY = 1;
const int MQ_SPEC_KEY = 2;
const int MQ_ALT_KEY = 3;
const int MQ_SIM_REG_KEY = 4;
const int MQ_SIM_SPEC_KEY = 5;
const int MQ_CLICK = 6;

const int MQ_LEFT_BUTTON = 7;
const int MQ_RIGHT_BUTTON = 8;

const int MQ_REQUEST = 9;  // automatically generated request,
                           // not from an input event

const int MQ_START_EXEC = 10;
const int MQ_ERROR_MESSAGE = 11;
const int MQ_HAVE_KEY = 12;
const int MQ_HAVE_LINE = 13;
const int MQ_INPUT_KEY = 14;
const int MQ_INPUT_LINE = 15;
const int MQ_HALT = 16;
const int MQ_STEP = 17;
const int MQ_READ_ON = 18;

const int maxmessagequeue = 1000;

class MessageQueue
{
  public:

    // constructor
    MessageQueue();

    // methods
    int isEmpty();
    int getKind();
    int getCode();
    mystring* getLabel();
    void showFront();
    void remove();
    void insert( int k, int c, char* lab, int x, int y );
    void removeAll();
    
  private:

    int front;  // index in data of first item in queue
    int rear;   // index in data of next available slot for storing
    int number; // number of items in queue
    
    // parallel arrays holding all message info
    int kinds[maxmessagequeue];
    int codes[maxmessagequeue];
    mystring* labels[maxmessagequeue];
    int xs[maxmessagequeue];
    int ys[maxmessagequeue];

};
