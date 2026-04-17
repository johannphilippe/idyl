#!/usr/bin/env ruby
# osc_receiver.rb — OSC test receiver for idyl
#
# Listens on a UDP port for raw OSC messages using plain sockets
# (no gem dependencies), prints each received message, then exits
# after the expected count or a timeout.
#
# Usage:
#   ruby osc_receiver.rb [port] [expected_count] [timeout_sec]
#
# Defaults: port=9000  expected_count=8  timeout_sec=10

require 'socket'

port           = (ARGV[0] || 9000).to_i
expected_count = (ARGV[1] || 8).to_i
timeout_sec    = (ARGV[2] || 10).to_f

received = []

sock = UDPSocket.new
sock.bind('0.0.0.0', port)

puts "osc_receiver: listening on port #{port}, expecting #{expected_count} message(s)…"
$stdout.flush

deadline = Time.now + timeout_sec

# ----- Minimal OSC parsing helpers -----

def read_osc_string(data, pos)
  term = data.index("\x00", pos)
  return nil, pos unless term
  s = data[pos...term]
  # advance past null + pad to 4-byte boundary
  pos = term + 1
  pos += (4 - pos % 4) % 4
  [s, pos]
end

def read_osc_int32(data, pos)
  return nil, pos if pos + 4 > data.size
  val = data[pos, 4].unpack1('N')
  # convert unsigned to signed
  val -= (1 << 32) if val >= (1 << 31)
  [val, pos + 4]
end

def read_osc_float32(data, pos)
  return nil, pos if pos + 4 > data.size
  val = data[pos, 4].unpack1('g')
  [val, pos + 4]
end

def parse_osc_message(data)
  address, pos = read_osc_string(data, 0)
  return nil unless address&.start_with?('/')

  tags, pos = read_osc_string(data, pos)
  return { address: address, args: [] } unless tags&.start_with?(',')

  args = []
  tags[1..].each_char do |tag|
    case tag
    when 'i'
      val, pos = read_osc_int32(data, pos)
      args << val
    when 'f'
      val, pos = read_osc_float32(data, pos)
      args << val.round(6)
    when 's'
      val, pos = read_osc_string(data, pos)
      args << "\"#{val}\""
    when 'T'
      args << true
    when 'F'
      args << false
    when 'N'
      args << nil
    end
  end
  { address: address, args: args }
end

# ----- Main receive loop -----

loop do
  remaining = deadline - Time.now
  break if remaining <= 0

  ready = IO.select([sock], nil, nil, [remaining, 0.1].min)

  if ready
    data, _addr = sock.recvfrom_nonblock(65536)
    msg = parse_osc_message(data)
    if msg
      line = "#{msg[:address]} #{msg[:args].map(&:inspect).join(', ')}"
      received << line
      puts line
      $stdout.flush
    end
  end

  break if received.size >= expected_count
end

sock.close

puts "---"
puts "osc_receiver: received #{received.size}/#{expected_count} message(s)"

if received.size >= expected_count
  puts "PASS"
  exit 0
else
  puts "FAIL"
  exit 1
end
