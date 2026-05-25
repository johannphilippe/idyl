#!/usr/bin/ruby

require_relative("rtmidi.rb")

$rtm = RtMidiInfo.new
$inp, $outp = $rtm.list_devices

puts "API List"
puts RtMidiInfo.api_list

puts "Input Devices : "
for i in 0 .. ($inp.length - 1) do 
    puts "#{i} - #{$inp[i]}"
end

puts 
puts "Output Devices : "
for i in 0 .. ($outp.length - 1) do 
    puts "#{i} - #{$outp[i]}"
end


