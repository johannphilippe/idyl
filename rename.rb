#!/usr/bin/env ruby

Dir.glob('**/*.idl').each do |old_name|
  new_name = old_name.sub(/\.idl$/, '.idyl')
  File.rename(old_name, new_name)
  puts "Renamed: #{old_name} -> #{new_name}"
end