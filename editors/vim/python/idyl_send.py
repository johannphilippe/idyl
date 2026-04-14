#!/usr/bin/env python3
"""idyl_send.py — send an OSC /idyl/eval message to a running idyl instance.

Usage:
    echo '<idyl code>' | python3 idyl_send.py [host [port]]

Defaults: host=127.0.0.1  port=9000
Code is read from stdin so that multiline text and special characters do not
require shell quoting.
"""
import sys
import socket


def osc_str(b: bytes) -> bytes:
    """Null-terminate a byte string and pad to the next 4-byte boundary."""
    b = b + b'\x00'
    return b + b'\x00' * ((-len(b)) % 4)


def main() -> None:
    host = sys.argv[1] if len(sys.argv) > 1 else '127.0.0.1'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 9000

    code = sys.stdin.buffer.read()

    # OSC message: address + type-tag string + one string argument
    msg = osc_str(b'/idyl/eval') + osc_str(b',s') + osc_str(code)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.sendto(msg, (host, port))
    finally:
        sock.close()


if __name__ == '__main__':
    main()
