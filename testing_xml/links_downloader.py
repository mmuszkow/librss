import httplib
from urlparse import urlparse
import urllib
import socket
import sys
import threading
import time

fin = open('feeds.txt', 'r')
i = 1

def download_page(url, filename): # url is urlparse.url
	conn = httplib.HTTPConnection(url.netloc)
	conn.request('GET', url.path)
	resp = conn.getresponse()
		
	if resp.status == 200 or (resp.status >= 300 and resp.status < 400):
		fout = open(filename, "wb")
		fout.write(resp.read());
		fout.close()
	else:
		print "%s -> %s" % (fullurl, resp.status)
		
	conn.close()
	
class down_thread(threading.Thread):
	def __init__ (self,url,i):
		threading.Thread.__init__(self)
		self.url = url
		self.i = i
	def run(self):
		try:
			download_page(self.url, str(self.i) + ".xml")
		except socket.error:
			errno, errstr = sys.exc_info()[:2]
			print "%s -> %s" % (fullurl, errno)
			print errstr
		except KeyboardInterrupt:
			print "Keyb interrupt"
		except:
			print "%s -> Unknown exception"  % (fullurl)

for line in fin:
	while(threading.activeCount() > 50):
		time.sleep(1)

	try:
		(_,fullurl) = line.split('\t')
		(fullurl, _) = fullurl.split('\n') # important
		url = urlparse(fullurl)
		thrd = down_thread(url, i)
		i = i + 1
		thrd.start()
	except:
		print "Main thread unknown exception"

fin.close()
