# coding: utf-8
import httplib
import urllib
import random
import string
import sys
import json
import types
import traceback
import time

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print 'Usage: python PostTest.py host port params'
        sys.exit(1)

    host = sys.argv[1]
    port = int(sys.argv[2])
    url = sys.argv[3]

    content = "POST_DATA_BEGIN\r\n\r\nPOST_DATA_MID\r\n\r\nPOST_DATA_END"

    conn = httplib.HTTPConnection(host, port)
    conn.connect()
    conn.putrequest('POST', url)
    conn.putheader('Content-Length', '%d' %(len(content)))
    conn.putheader('X_FORWARDED_FOR', '10.237.1.246, 16.237.1.246, 17.237.1.246')
    conn.endheaders()

    conn.send(content)
    response = conn.getresponse()

    print "%s, %s, %s" %(response.version, response.status, response.reason)
    print response.getheaders()
    print response.read()

    conn.close()
    
