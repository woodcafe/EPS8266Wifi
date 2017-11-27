
#include <SoftwareSerial.h>
#include "MsgBuf.h"

SoftwareSerial mySerial(9, 8); // RX, TX

int connectionId;
MsgBuf mbuf = MsgBuf(64);
MsgBuf wbuf = MsgBuf(2);
char* buf = wbuf.getBuf();
int cp;

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println(F("****************************"));
  Serial.println(F("*** WIFI STARTED 2017.11 ***"));
  Serial.println(F("****************************"));
  sendData(F("AT+RST\r\n"), 3000, true);
  sendData(F("AT+GMR\r\n"), 5000, true);
  sendData(F("AT+CIFSR\r\n"), 1000, true);
  sendData(F("AT+CIPMUX=1\r\n"), 1000, true);        // configure for multiple connections
  sendData(F("AT+CIPSERVER=1,80\r\n"), 1000, true); // turn on server on port 80
}

double toFloat(char* buf, char*debug) {
  char dispBuf[32];
  String sfloat = String(&buf[1]);
  double vf = sfloat.toDouble();
  String x = String(vf);
  _printf("[%s] = <%s>", debug , dtostrf(vf, 10, 10, dispBuf));
  return vf;
}

void TCP_parse() {
  char buf[32];
  if ( wbuf.indexOf("+IPD,") > 0 ) {
    char* buf = wbuf.getBuf();
    char cid = buf[wbuf.indexOf("+IPD,", 0) + 5] - 48;
    _printf("*** ConId = %d ",  cid);
    connectionId = cid;
    if ( wbuf.indexOf("G1 ") > 0 ) {
      cp = wbuf.indexOf("G1");
      cp = wbuf.substring(buf, cp, " ");
      _printf("[code] = <%s> cp=%d", buf, cp);
      cp = wbuf.substring(buf, cp, " ");
      double x = toFloat(buf, "XVAL");
      cp = wbuf.substring(buf, cp, " ");
      double y = toFloat(buf, "YVAL");
      cp = wbuf.substring(buf, cp, " ");
      double z = toFloat(buf, "ZVAL");
      String resp = "G1 X" + String(x) + " Y" + String(y) + " Z" + String(z);
      espsend(resp);
    } else {
      TCP_close(cid);
    }
  }
}

int readAll() {
  int size = 0;
  int retryCnt = 0;
  wbuf.clear();
  while (true) {
    while (mySerial.available() && wbuf.available()) {
      char c = mySerial.read();  // read the next character
      wbuf.append(c);
      retryCnt = 5;
    }
    if ( wbuf.size()  > size || retryCnt > 0) {
      size = wbuf.size();
      retryCnt --;
    } else {
      break;
    }
  }
  return wbuf.size();
}

void loop() // run over and over
{
  int n  = readAll();
  if ( n > 0 ) {
    _printf("\n===\n%s\n===", wbuf.getBuf());
    HTTP_request_parse();
  }
}

// ************************************************************
// ** WIFI
// ************************************************************

void TCP_close(int conId) {
  String closeCommand = "AT+CIPCLOSE=";  // close the socket connection////esp command
  closeCommand += conId; // append connection id
  closeCommand += "\r\n";
  sendData(closeCommand, 3000, true);
}
void espsend(String d) {
  String cipSend = " AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 500, true);
  sendData(d, 500, true);
}

void sendData(String command, const int timeout, boolean debug) {
  mbuf.clear();
  mySerial.print(command);
  long int time = millis();
  while ( (time + timeout) > millis()) {
    while (mySerial.available()) {
      char c = mySerial.read();  // read the next character
      mbuf.append(c);
    }
    if ( mbuf.indexOf("OK") > 0 ) {
      break;
    }
  }
  _printf("CMDRSP %s [[[%s]]]", (char*)command.c_str(), mbuf.getBuf());
}

void _printf(char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char buf[vsnprintf(NULL, 0, fmt, va) + 1];
  vsprintf(buf, fmt, va);
  Serial.println(buf);
  va_end(va);
}

String HTTP_response(String msg) {
  String response =  F("HTTP/1.1 200 OK\r\n");
  response += F("Connection: keep-alive\r\n");
  response += F("Content-Type: text/html\r\n");
  response += F("Content-Length: ");
  response += String(msg.length()) + "\r\n\r\n";
  response +=  msg;
  return response;
}
