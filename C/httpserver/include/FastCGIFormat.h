#ifndef FASTCGIFORMAT_H_
#define FASTCGIFORMAT_H_

//Define the version of FastCGI
#define FCGI_VERSION             1

//Define type value of FCGI_Header
#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

//Define Common header of Request
typedef struct {
    uchar version;
    uchar type;
    uchar requestIdB1;
    uchar requestIdB0;
    uchar contentLengthB1;
    uchar contentLengthB0;
    uchar paddingLength;
    uchar reserved;
} FCGI_Header;

//Define role value of FCGI_BeginRequestBody
#define FCGI_RESPONDER  1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER     3

//Define body of begin Request,this data is used send to FastCGI
typedef struct {
    uchar roleB1;
    uchar roleB0;
    uchar flags;
    uchar reserved[5];
} FCGI_BeginRequestBody;
typedef struct {
    FCGI_Header header;
    FCGI_BeginRequestBody body;
} FCGI_BeginRequestRecord;


//Define protocolStatus value of FCGI_EndRequestBody
#define FCGI_REQUEST_COMPLETE 0
#define FCGI_CANT_MPX_CONN    1
#define FCGI_OVERLOADED       2
#define FCGI_UNKNOWN_ROLE     3

//Define body of end Request,this data is used received from FastCGI
typedef struct {
    uchar appStatusB3;
    uchar appStatusB2;
    uchar appStatusB1;
    uchar appStatusB0;
    uchar protocolStatus;
    uchar reserved[3];
} FCGI_EndRequestBody;
typedef struct {
    FCGI_Header header;
    FCGI_EndRequestBody body;
} FCGI_EndRequestRecord;


#endif /* FASTCGIFORMAT_H_ */
