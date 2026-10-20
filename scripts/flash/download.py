#!/usr/bin/env python3
"""Serve a firmware ZIP once, then stop (Codespaces private forwarded port)."""
import argparse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import threading

parser = argparse.ArgumentParser()
parser.add_argument("archive", type=Path)
parser.add_argument("--port", type=int, default=8765)
parser.add_argument("--idle-timeout", type=int, default=120)
args = parser.parse_args()
archive = args.archive.resolve()
if not archive.is_file():
    parser.error(f"Archive missing: {archive}")
if args.idle_timeout < 1:
    parser.error("--idle-timeout must be positive")

done = threading.Event()


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/":
            # Download starts automatically. window.close() works only when the
            # browser considers the tab script-opened; otherwise show guidance.
            html = """<!doctype html><html lang="en"><head>
<meta charset="utf-8"><title>R.I.D.E. firmware download</title>
</head><body><p>Downloading R.I.D.E. firmware…</p>
<p id="status">If this tab stays open, you can close it after downloading.</p>
<script>
const link = document.createElement('a');
link.href = '/download';
link.download = '';
document.body.appendChild(link);
link.click();
setTimeout(() => { window.close(); }, 1500);
</script></body></html>"""
            payload = html.encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(payload)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(payload)
            return
        if self.path != "/download":
            self.send_error(404)
            return
        if done.is_set():
            self.send_error(410, "Firmware already downloaded")
            return
        try:
            self.send_response(200)
            self.send_header("Content-Type", "application/zip")
            self.send_header("Content-Disposition",
                             f'attachment; filename="{archive.name}"')
            self.send_header("Content-Length", str(archive.stat().st_size))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            with archive.open("rb") as fh:
                while chunk := fh.read(1024 * 1024):
                    self.wfile.write(chunk)
            self.wfile.flush()
        except (BrokenPipeError, ConnectionResetError, OSError) as exc:
            print(f"Download interrupted: {exc}", flush=True)
            return
        # The server knows it sent all bytes; only the browser can verify it
        # actually saved them. Shut down from another thread to avoid deadlock.
        if not done.is_set():
            done.set()
            print("Firmware transferred; stopping download server.", flush=True)
            threading.Thread(target=self.server.shutdown, daemon=True).start()


server = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
server.daemon_threads = True
timer = threading.Timer(args.idle_timeout, server.shutdown)
timer.daemon = True
timer.start()
try:
    print(f"Firmware download: http://localhost:{args.port}/", flush=True)
    print("Keep Codespaces port visibility Private.", flush=True)
    print(f"Server stops after transfer or {args.idle_timeout}s of inactivity.", flush=True)
    server.serve_forever(poll_interval=0.1)
finally:
    timer.cancel()
    server.server_close()
    if not done.is_set():
        print("Download server closed without a completed transfer.", flush=True)
