#include <stdio.h>        // so mystring can have FILE
#include "mystring.h"
#include "MessageQueue.h"
#include <iostream>     // for debug output
#include <stdlib.h>       // for error exiting

MessageQueue::MessageQueue()
{
  front = 0;
  rear = 0;
  number = 0;
}

int MessageQueue::isEmpty()
{
  if(number==0)
    return 1;
  else
    return 0;
}

int MessageQueue::getKind()
{
  return kinds[front];
}

int MessageQueue::getCode()
{
  return codes[front];
}

mystring* MessageQueue::getLabel()
{
  return labels[front];
}

void MessageQueue::remove()
{
  if(number==0)
  {
    std::cout << "tried to remove from empty MessageQueue" 
              << std::endl;
    exit(1);
  }

  number--;
  labels[front]->destroy();  // return storage space
  front++;
  if(front >= maxmessagequeue)
    front=0;

}

void MessageQueue::removeAll()
{
  // safely remove each one, releasing memory
  while( ! isEmpty() )
    remove();

  // tidy up, just for debugging convenience:
  front = 0;  rear = 0;
}

void MessageQueue::showFront()
{
  char* s = labels[front]->charCopy();
  delete s;
}

void MessageQueue::insert( int k, int c, char* lab, int x, int y )
{
  if(number==maxmessagequeue)
  {
    std::cout << "tried to insert into full MessageQueue" 
             << std::endl;
    exit(1);
  }

  kinds[rear] = k;  codes[rear] = c;  
  labels[rear] = new mystring(lab);
  xs[rear] = x;  ys[rear] = y;

  number++;

  rear++;          
  if(rear >= maxmessagequeue)
    rear = 0;

}// insert
