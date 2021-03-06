import httplib
from urlparse import urlparse
import socket
import sys

fin = open('test_list_final.txt', 'r')

for line in fin:
    (_,fullurl) = line.split('\t')
    (fullurl, _) = fullurl.split('\n') # important
    url = urlparse(fullurl)
    try:
        conn = httplib.HTTPConnection(url.netloc)
        conn.request('GET', url.path)
        resp = conn.getresponse()
        
        if resp.status == 200 or (resp.status >= 300 and resp.status < 400):
            fout = open('checked_feeds.txt', 'a')
            fout.write(line)
            fout.close()
        else:
            print "%s -> %s" % (fullurl, resp.status)
        
        conn.close()
    except socket.error:
        errno, errstr = sys.exc_info()[:2]
        print "%s -> %s" % (fullurl, errno)
        print errstr
    except KeyboardInterrupt:
        break
    except:
        print "%s -> Unknown exception"  % (fullurl)

fin.close()

