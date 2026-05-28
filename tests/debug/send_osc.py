#!/usr/bin/env python3
"""Send a string as an OSC /idyl/eval message to localhost:9000."""
import socket, struct, sys, time

def osc_string(s):
    b = s.encode() + b'\x00'
    # pad to 4-byte boundary
    b += b'\x00' * ((4 - len(b) % 4) % 4)
    return b

def osc_packet(address, s):
    addr = osc_string(address)
    typetag = osc_string(',s')
    arg = osc_string(s)
    return addr + typetag + arg

def send(code, port=9000):
    pkt = osc_packet('/idyl/eval', code)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(pkt, ('127.0.0.1', port))
    sock.close()
    print(f"Sent {len(pkt)} bytes to port {port}")

if __name__ == '__main__':
    code = open(sys.argv[1]).read() if len(sys.argv) > 1 else sys.stdin.read()
    send(code)
