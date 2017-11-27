//YWROBOT
#ifndef MsgBuf_h
#define MsgBuf_h
#include <WString.h>

class MsgBuf {
  public:
    MsgBuf(  int step);
    void clear();
    bool available();
    int size();
    int move(int speed);
    int append(char c);
    int indexOf(char* findWord);
    int indexOf(char* findWord, int start);
    int length(char* findWord);
    int  substring(char* dbuf, int s, char* word);
    char * MsgBuf::getBuf( );

  private:
    void _printf(char *fmt, ...) ;
    bool MsgBuf::matchWord( char* pWord, int start  );
    char buf[512];
    int top;
    int capacity;
};

#endif
