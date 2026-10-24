from pathlib import Path
import subprocess,sys,time,json,urllib.request
v=Path(__file__).resolve().parent;root=v.parents[2]
config=v/'pty_fixture.json'
if config.exists():config.unlink()
node='/home/user/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/bin/node'
fixture=subprocess.Popen([sys.executable,str(v/'pty_console_fixture.py')],cwd=root,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
try:
 for i in range(100):
  if fixture.poll() is not None:raise RuntimeError(fixture.stdout.read())
  if config.exists():
   data=json.loads(config.read_text())
   try:
    urllib.request.urlopen(data['url'],timeout=.5).close();break
   except OSError:pass
  time.sleep(.05)
 else:raise RuntimeError('Fixture startup timed out')
 subprocess.run([node,str(v/'browser_test.js')],cwd=root,check=True,timeout=120)
finally:
 fixture.terminate()
 try:fixture.wait(timeout=5)
 except subprocess.TimeoutExpired:fixture.kill();fixture.wait()
 print('PTY fixture stopped')
