
#include "MsgBuf.h"

MsgBuf::MsgBuf(int size)
{
  top = 0;
  capacity = 512;
}

int MsgBuf::append(char c) {
  if ( top > capacity ) return top;
  buf[top++] = c;
  buf[top] = 0;
  return top;
}

bool MsgBuf::available( ) {
  if ( top < capacity  ) {
    return true;
  }
  return false;
}

void MsgBuf::clear( ) {
  top = 0;
  buf[top] = 0;
}

int MsgBuf::size( ) {
  return top;
}

int MsgBuf::length( char *pWord) {
  int j = 0;
  while ( pWord[j++] > 0 ) {};
  int size = j - 1;
  return size;
}

int MsgBuf::substring( char* dbuf, int start, char* word ) {
  int ep = indexOf(word, start);
  int cnt = 0;
  if (ep > 0 ) {
    for (int i = start; i < ep+1; i++ ) {
      dbuf[cnt++] = buf[i];
    }
  }
  dbuf[cnt] = 0;
  return  ep+1;
}

bool MsgBuf::matchWord( char* pWord, int start  ) {
  int j = 0;
  while ( pWord[j++] > 0 ) {};
  int size = j - 1;
  // ***************
  for ( int i = start; i < start + size ; i++ ) {
    if ( buf[i] == pWord[i - start] ) {
      continue;
    } else {
      return false;
    }
  }
  return true;
}

int MsgBuf::indexOf( char* pWord) {
  return indexOf(pWord, 0);
}

int MsgBuf::indexOf( char* pWord, int offset ) {
  for ( int i = offset; i < top; i++ ) {
    if ( buf[i] == pWord[0] && matchWord(pWord, i) ) {
      return i;
    }
  }
  return -1;
}

char * MsgBuf::getBuf( ) {
  return buf;
}





