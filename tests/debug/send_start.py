#!/usr/bin/env python3
"""Send OSC /idyl/start <name> to localhost:9000."""
import socket, struct, sys

def osc_string(s):
    b = s.encode() + b'\x00'
    b += b'\x00' * ((4 - len(b) % 4) % 4)
    return b

def send_cmd(address, name, port=9000):
    addr = osc_string(address)
    typetag = osc_string(',s')
    arg = osc_string(name)
    pkt = addr + typetag + arg
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(pkt, ('127.0.0.1', port))
    sock.close()

if __name__ == '__main__':
    # Usage: send_start.py <cmd> <name>
    # cmd: start | stop | pause | resume
    # e.g. send_start.py start kick  → /idyl/process/start  kick
    cmd  = sys.argv[1] if len(sys.argv) > 1 else 'start'
    name = sys.argv[2] if len(sys.argv) > 2 else 'kick'
    address = f'/idyl/process/{cmd}'
    send_cmd(address, name)
    print(f"Sent {address} {name}")
