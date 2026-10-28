#!/usr/bin/env python3
"""mockclock.py — імітатор годинника для перегляду веб-інтерфейсу без заліза (EN: a clock emulator to preview the web UI
without hardware). Віддає web/build/index.html (спершу запустіть web/webui_patch.py), /content — значення з
tools/sim/mock-settings.json (заводські налаштування прошивки), /lang?code=xx — lang/xx.json, /lang-list; POST /post
лише друкує поля й пам'ятає їх до перезапуску. Запуск / run: python3 tools/sim/mockclock.py [--port 8791]"""
import json, os, sys, urllib.parse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
settings = json.load(open(os.path.join(ROOT, 'tools', 'sim', 'mock-settings.json'), encoding='utf-8'))


class H(BaseHTTPRequestHandler):
    def log_message(self, *a): pass

    def send(self, code, body, ctype='application/json'):
        b = body if isinstance(body, bytes) else body.encode('utf-8')
        self.send_response(code)
        self.send_header('Content-Type', ctype)
        self.send_header('Content-Length', str(len(b)))
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(b)

    def do_GET(self):
        u = urllib.parse.urlparse(self.path)
        q = urllib.parse.parse_qs(u.query, keep_blank_values=True)
        if u.path == '/content':
            return self.send(200, json.dumps({k: settings.get(k, '') for k in q}))
        if u.path == '/lang':
            code = (q.get('code') or ['uk'])[0]
            f = os.path.join(ROOT, 'lang', os.path.basename(code) + '.json')
            return self.send(200, open(f, 'rb').read()) if os.path.exists(f) else self.send(404, 'no lang', 'text/plain')
        if u.path == '/lang-list':
            return self.send(200, json.dumps([{'code': 'uk', 'id': 3, 'rev': 'b7656c40'}]))
        if u.path == '/lang-led-status':
            return self.send(200, json.dumps({'lang': 3, 'rev': '1c54556f'}))
        if u.path == '/log':
            return self.send(200, '[up+0s]               [BOOT]    Power-on reset\n(mockclock: no real event log)\n', 'text/plain; charset=utf-8')
        if u.path == '/hist':
            return self.send(200, '{"step":1800,"cap":48}')
        if '.' not in u.path:
            return self.send(200, open(os.path.join(ROOT, 'web', 'build', 'index.html'), 'rb').read(), 'text/html; charset=utf-8')
        return self.send(404, 'not found', 'text/plain')

    def do_POST(self):
        n = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(n)
        ct = self.headers.get('Content-Type', '')
        fields = {}
        if 'multipart/form-data' in ct:
            bnd = ct.split('boundary=')[1].encode()
            for part in body.split(b'--' + bnd):
                if b'name="' in part:
                    name = part.split(b'name="')[1].split(b'"')[0].decode()
                    fields[name] = part.split(b'\r\n\r\n', 1)[1].rsplit(b'\r\n', 1)[0].decode('utf-8', 'replace')
        settings.update(fields)
        print('POST', self.path, fields, flush=True)
        return self.send(200, '', 'text/plain')


port = int(sys.argv[sys.argv.index('--port') + 1]) if '--port' in sys.argv else 8791
print(f'mockclock: http://127.0.0.1:{port}/', flush=True)
ThreadingHTTPServer(('127.0.0.1', port), H).serve_forever()
