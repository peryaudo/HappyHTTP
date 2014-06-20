#include "happyhttp.h"
#include <cstdio>
#include <cstring>

#ifdef WIN32
#include <winsock2.h>
#endif // WIN32

int count=0;

void OnBegin( const happyhttp::Response* r, void* userdata )
{
  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  count = 0;
}

void OnData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
{
  fwrite( data,1,n, stdout );
  count += n;
}

void OnComplete( const happyhttp::Response* r, void* userdata )
{
  printf( "COMPLETE (%d bytes)\n", count );
}



happyhttp::Error Test1()
{
  puts("-----------------Test1------------------------" );
  // simple simple GET
  happyhttp::Connection conn( "scumways.com", 80 );
  conn.setcallbacks( OnBegin, OnData, OnComplete, 0 );

  if (happyhttp::Error err = conn.request( "GET", "/happyhttp/test.php", 0, 0,0 )) {
    return err;
  }

  while( conn.outstanding() ) {
    if (happyhttp::Error err = conn.pump()) {
      return err;
    }
  }

  return NULL;
}



happyhttp::Error Test2()
{
  puts("-----------------Test2------------------------" );
  // POST using high-level request interface

  const char* headers[] = 
  {
    "Connection", "close",
    "Content-type", "application/x-www-form-urlencoded",
    "Accept", "text/plain",
    0
  };

  const char* body = "answer=42&name=Bubba";
  
  happyhttp::Connection conn( "scumways.com", 80 );
  conn.setcallbacks( OnBegin, OnData, OnComplete, 0 );
  if (happyhttp::Error err = conn.request( "POST",
      "/happyhttp/test.php",
      headers,
      (const unsigned char*)body,
      strlen(body) )) {
    return err;
  }

  while( conn.outstanding() ) {
    if (happyhttp::Error err = conn.pump()) {
      return err;
    }
  }

  return NULL;
}

happyhttp::Error Test3()
{
  puts("-----------------Test3------------------------" );
  // POST example using lower-level interface

  const char* params = "answer=42&foo=bar";
  int l = strlen(params);

  happyhttp::Connection conn( "scumways.com", 80 );
  conn.setcallbacks( OnBegin, OnData, OnComplete, 0 );

  if (happyhttp::Error err = conn.putrequest( "POST", "/happyhttp/test.php" )) return err;
  if (happyhttp::Error err = conn.putheader( "Connection", "close" )) return err;
  if (happyhttp::Error err = conn.putheader( "Content-Length", l )) return err;
  if (happyhttp::Error err = conn.putheader( "Content-type", "application/x-www-form-urlencoded" )) return err;
  if (happyhttp::Error err = conn.putheader( "Accept", "text/plain" )) return err;
  if (happyhttp::Error err = conn.endheaders()) return err;
  if (happyhttp::Error err = conn.send( (const unsigned char*)params, l )) return err;

  while( conn.outstanding() ) {
    if (happyhttp::Error err = conn.pump()) return err;
  }
  return NULL;
}




int main( int argc, char* argv[] )
{
#ifdef WIN32
    WSAData wsaData;
    int code = WSAStartup(MAKEWORD(1, 1), &wsaData);
  if( code != 0 )
  {
    fprintf(stderr, "shite. %d\n",code);
    return 0;
  }
#endif //WIN32
  happyhttp::Error err = NULL;
  if ((err = Test1())) { }
  else if ((err = Test2())) { }
  else if ((err = Test3())) { }

  if (err) {
    fprintf(stderr, "Error: %s\n", err->c_str());
  }
  
#ifdef WIN32
    WSACleanup();
#endif // WIN32
  return 0;
}



