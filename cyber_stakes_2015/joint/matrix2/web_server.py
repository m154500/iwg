from flask import Flask, render_template, request, redirect, make_response, g, session
import sqlite3
import random
import os
import string
import urlparse
from hashlib import sha512,sha1

app = Flask(__name__)
rand = os.urandom
allowed = string.ascii_letters + string.digits + " !.,?@()*[]:;\t"

@app.teardown_appcontext
def close_connection(exception):
  if getattr(g, '_database', None) is not None:
    db = g._database
    db.commit()
    db.close()

def get_db():
  if getattr(g, '_database', None) is None:
    g._database = sqlite3.connect('users.db')
    g._database.text_factory = str
  return g._database

def mac(username,admin):
  hstr = '||'.join(map(str,[username,admin,rand]))
  return '||'.join([username,admin,sha512(hstr).hexdigest()])

def chkmac(cookie):
  if not cookie:
    return False,'',0
  mac = cookie[-128:]
  valid = (sha512(str(cookie[:-128]) + str(rand)).hexdigest() == mac)
  username,admin = cookie.split('||')[:2]
  return valid,username,admin=='yes'

def do_login(user, admin):
  resp = make_response(redirect('/message'))
  resp.set_cookie('user',mac(user,'yes' if admin else 'no'))
  return resp

def logingate(request,success,args=None):
  if args == None:
    args = []
  valid,username,_ = chkmac(request.cookies.get('user'))
  if valid:
    return success(*args)
  else:
    return login()

@app.route('/')
def index():
  return logingate(request,msgpage)

@app.route('/login', methods=['POST','GET'])
def login():
  dat = urlparse.parse_qs(request.get_data())
  if ('username' in dat) and ('password' in dat):
    user = dat['username'][0]
    password = dat['password'][0]
  else:
    return render_template('login.html')

  if len(user) > 512:
    return render_template('error.html',msg= \
      " If I were you, I would hope we don't meet again."),403

  register = request.form.get('register','')
  c = get_db().cursor()
  if register:
    if (user == 'admin') or (user == '*'):
      return render_template('error.html', msg = \
      "You don't get it. ~~~ I built this place. ~~~~" + \
      "Down here I make the rules"), 403
    try:
      query = 'INSERT OR FAIL INTO users (name,password,admin) VALUES (?,"%s",0)'%sha1(password).digest()
      c.execute(query,[user])
    except sqlite3.IntegrityError:
      return render_template('error.html',msg= \
        " Now there's more than one of him?<br><br>~~~~"+ \
        "A lot more.<br><br>~~~~"+ \
        "But how? ~~~ How can that be possible?"),403
    return do_login(user,0)

  if user == 'admin':
    if password == open("./key").read():
      return do_login('admin',True)

  statement = 'SELECT admin,password FROM users WHERE name = ? and password = "%s"'%sha1(password).digest()
  result = c.execute(statement,[user]).fetchall()
  if len(result):
    return do_login(user,result[0])
  else:
    statement = "SELECT count(*) FROM users WHERE name = ?"
    result = c.execute(statement,[user]).fetchone()[0]
    if result < 1:
      return render_template('error.html', msg = \
      (" Do not try and login as '%s'.~~~~ That's impossible.<br>~~~~ Instead only try to "+ \
      "realize the truth.<br><br>~~~~What truth?<br><br>~~~~There is no such user '%s'")%\
      (user,user)), 403
    else:
      return render_template('error.html', msg = \
      " What are you waiting for?~~~ You're faster than this.~~~<br>"+ \
      "Don't ~~~think~~~ you are, ~~~know~~~ you are.~~~~~~ Come on.<br>"+ \
      "Stop trying to log in and log in."), 403

@app.route('/logout', methods=['GET'])
def logout():
  resp = make_response(redirect('/'))
  resp.set_cookie('user','')
  return resp

@app.route('/message', methods=['POST','GET'])
def msgpage():
  return logingate(request,msgs,[request])

def msgs(request):
  c = get_db().cursor()
  _,me,_ = chkmac(request.cookies.get('user'))
  dst = request.form.get('user','')
  msg = request.form.get('msg','')
  if msg and dst:
    msg = ''.join(filter(lambda x:x in allowed,msg))[:150]
    dst = ''.join(filter(lambda x:x in allowed,dst))
    query = 'INSERT INTO messages (%s,%s,%s) VALUES (?,?,?)'% \
     ('src','dst','msg')
    c.execute(query,[me,dst,msg])

  query = "SELECT src,msg FROM messages WHERE dst=? or dst='*'"
  results = c.execute(query,[me]).fetchall()

  return render_template('msgs.html',msgs = [dict(user=row[0], msg=row[1]) \
   for row in results])

@app.route('/admin', methods=['POST','GET'])
def adminpage():
  return logingate(request,admin,[request])

def admin(request):
  _,me,admin = chkmac(request.cookies.get('user'))
  if not admin:
    return render_template('error.html', msg = \
      " Choice is an illusion created between those with power and those without"), 403

  c = get_db().cursor()

  if request.args.get('pws',''):
    query = 'SELECT name,password from users'
    results = c.execute(query).fetchall()

    return render_template('msgs.html',msgs = [dict(user="%s"%(row[0]), \
      msg=row[1].encode("hex")) for row in results])
  else:
    if request.args.get('wipe',''):
      c = get_db().cursor();
      c.execute('DELETE FROM messages')
      c.execute('DELETE FROM users')
    if request.args.get('reset','') or request.args.get('wipe',''):
      try:
        query = 'INSERT INTO users (%s,%s) VALUES (?,?)'% \
         ('name','password')
        c.execute(query,['flag','XXXX NO LOGIN XXXX'])
      except:
        pass
      query = 'INSERT INTO messages (%s,%s,%s) VALUES (?,?,?)'% \
       ('src','dst','msg')
      c.execute(query,['admin','admin',open("./key").read()])

    query = 'SELECT src,dst,msg from messages'
    results = c.execute(query).fetchall()

    return render_template('msgs.html',msgs = [dict(user="%s->%s"%(row[0],row[1]), \
      msg=row[2]) for row in results])

if __name__ == '__main__':
  app.run(debug = True, host='0.0.0.0', port=8007)
