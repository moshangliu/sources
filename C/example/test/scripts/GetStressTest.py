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
        print 'Usage: python Search_Resource.py host port url'
        sys.exit(1)

    host = sys.argv[1]
    port = int(sys.argv[2])
    url = sys.argv[3]

    while True:
        conn = httplib.HTTPConnection(host, port)

        conn.connect()
        conn.putrequest('GET', url)
        conn.putheader('X_FORWARDED_FOR', '10.237.1.246, 16.237.1.246, 17.237.1.246')
        conn.endheaders()

        response = conn.getresponse()

        print "%s, %s, %s" %(response.version, response.status, response.reason)
        print response.getheaders()
        print response.read()

    conn.close()
    
