#!/usr/bin/env python3
"""idyl_send.py — send an OSC message to a running idyl instance.

Usage:
    echo '<payload>' | python3 idyl_send.py [host [port [address]]]

Defaults:
    host    = 127.0.0.1
    port    = 9000
    address = /idyl/eval

The payload is read from stdin and sent as the first (string) OSC argument.
"""
import sys
import socket


def osc_str(b: bytes) -> bytes:
    """Null-terminate a byte string and pad to the next 4-byte boundary."""
    b = b + b'\x00'
    return b + b'\x00' * ((-len(b)) % 4)


def main() -> None:
    host    = sys.argv[1] if len(sys.argv) > 1 else '127.0.0.1'
    port    = int(sys.argv[2]) if len(sys.argv) > 2 else 9000
    address = sys.argv[3] if len(sys.argv) > 3 else '/idyl/eval'

    payload = sys.stdin.buffer.read()

    addr_bytes = address.encode('utf-8') if not isinstance(address, bytes) else address
    msg = osc_str(addr_bytes) + osc_str(b',s') + osc_str(payload)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.sendto(msg, (host, port))
    finally:
        sock.close()


if __name__ == '__main__':
    main()
