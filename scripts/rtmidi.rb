require "bundler/inline"

gemfile do
	source "https://rubygems.org"
	gem "ffi", require: true
    gem "os", require: true 
end

class IntPtr < FFI::Struct
    layout  :value, :int
end

module LibC
  extend FFI::Library
  ffi_lib FFI::Library::LIBC
  
  # memory allocators
  attach_function :malloc, [:size_t], :pointer
  attach_function :calloc, [:size_t], :pointer
  attach_function :valloc, [:size_t], :pointer
  attach_function :realloc, [:pointer, :size_t], :pointer
  attach_function :free, [:pointer], :void
  
  # memory movers
  attach_function :memcpy, [:pointer, :pointer, :size_t], :pointer
  attach_function :bcopy, [:pointer, :pointer, :size_t], :void

end # module LibC

module RtMidiFuncs
    extend FFI::Library
	ffi_lib "/usr/lib/x86_64-linux-gnu/librtmidi.so"

    enum :RtMidiAPI, [:RTMIDI_API_UNSPECIFIED, 0,   
        :RTMIDI_API_MACOSX_CORE,
        :RTMIDI_API_LINUX_ALSA,     
        :RTMIDI_API_UNIX_JACK, 
        :RTMIDI_API_WINDOWS_MM,     
        :RTMIDI_API_RTMIDI_DUMMY,   
        :RTMIDI_API_WEB_MIDI_API,  
        :RTMIDI_API_WINDOWS_UWP,    
        :RTMIDI_API_ANDROID,    
        :RTMIDI_API_NUM]

    attach_function :rtmidi_get_port_count,[:pointer],:int
    attach_function :rtmidi_get_port_name,[:pointer,:int,:strptr,:pointer],:int
    attach_function :rtmidi_in_create,[:int, :string, :int],:pointer
    attach_function :rtmidi_out_create,[:int, :string], :pointer
    attach_function :rtmidi_in_free,[:pointer], :void
    attach_function :rtmidi_out_free,[:pointer], :void
    attach_function :rtmidi_open_port, [:pointer, :int, :string], :void
    attach_function :rtmidi_in_get_message, [:pointer, :pointer, :pointer], :double
    attach_function :rtmidi_out_send_message, [:pointer, :strptr, :int], :int

    callback :RtMidiCallback,[:double, :pointer, :uint32, :pointer], :void
    attach_function :rtmidi_in_set_callback,[:pointer, :RtMidiCallback, :pointer],:void
    attach_function :rtmidi_in_cancel_callback,[:pointer],:void

    attach_function :rtmidi_in_ignore_types,[:pointer, :bool, :bool, :bool],:void
end

class RtMidiInfo
    def initialize()
        @inptr = RtMidiFuncs.rtmidi_in_create(:RTMIDI_API_LINUX_ALSA, "", 0)
        @outptr = RtMidiFuncs.rtmidi_out_create(:RTMIDI_API_LINUX_ALSA, "")
    end

    def list_devices()
        incnt = RtMidiFuncs.rtmidi_get_port_count(@inptr)
        inputs = Array.new
        for i in 0 .. (incnt-1) do
            nullptr = FFI::MemoryPointer.new(0)
            lenptr = FFI::MemoryPointer.new(:int, 1)
            res = RtMidiFuncs.rtmidi_get_port_name(@inptr, i, FFI::Pointer::NULL, lenptr)
            strptr = FFI::MemoryPointer.new(:uchar, lenptr.read_int)
            res = RtMidiFuncs.rtmidi_get_port_name(@inptr, i, strptr, lenptr)
            inputs.push(strptr.read_string)
        end

        outputs = Array.new
        outcnt = RtMidiFuncs.rtmidi_get_port_count(@outptr)
        for i in 0 .. (outcnt-1) do
            nullptr = FFI::MemoryPointer.new(0)
            lenptr = FFI::MemoryPointer.new(:int, 1)
            res = RtMidiFuncs.rtmidi_get_port_name(@outptr, i, FFI::Pointer::NULL, lenptr)
            strptr = FFI::MemoryPointer.new(:uchar, lenptr.read_int)
            res = RtMidiFuncs.rtmidi_get_port_name(@outptr, i, strptr, lenptr)
            outputs.push(strptr.read_string)
        end

        return inputs, outputs
    end


    def self.api_list()
        return """
        0 : RTMIDI_API_UNSPECIFIED 
        1 : RTMIDI_API_MACOSX_CORE
        2 : RTMIDI_API_LINUX_ALSA     
        3 : RTMIDI_API_UNIX_JACK 
        4 : RTMIDI_API_WINDOWS_MM     
        5 : RTMIDI_API_RTMIDI_DUMMY   
        6 : RTMIDI_API_WEB_MIDI_API  
        7 : RTMIDI_API_WINDOWS_UWP   
        8 : RTMIDI_API_ANDROID   
        9 : RTMIDI_API_NUM
        """
    end
end

class RtMidiIn
    def initialize(api_index, client_name) 
        @ptr = RtMidiFuncs.rtmidi_in_create(api_index, client_name, 0)
        @buf = FFI::MemoryPointer.new(:uchar, 8192)
        ObjectSpace.define_finalizer(self, proc {
            RtMidiFuncs.rtmidi_in_free(@ptr)
        })
        ignore_types(false, false, false)
    end

    def ignore_types(sysex, time, sense)
        RtMidiFuncs.rtmidi_in_ignore_types(@ptr, sysex, time, sense)
    end

    def open_port(port_nbr, port_name)
        RtMidiFuncs.rtmidi_open_port(@ptr, port_nbr, port_name)
    end

    ## Does not work, prefer callback instead
    def get_message()
        nullptr = FFI::MemoryPointer.new(0)
        lenptr = FFI::MemoryPointer.new(:int, 1)
        timestamp = RtMidiFuncs.rtmidi_in_get_message(@ptr, @buf, lenptr)
        return timestamp, @buf.read_array_of_uchar(lenptr.read_int)
    end

    def set_callback(cbk)
        @rb_cbk = cbk
        @c_cbk = FFI::Function.new(:void, [:double, :pointer, :uint32, :pointer]) do |stamp, msg, len, udata|
            @rb_cbk.call(stamp, msg.read_array_of_uchar(len), len)
        end
        nullptr = FFI::MemoryPointer.new(0)
        RtMidiFuncs.rtmidi_in_set_callback(@ptr, @c_cbk, nullptr)
    end

    def cancel_callback()
        RtMidiFuncs.rtmidi_in_cancel_callback(@ptr)
    end
end

class RtMidiOut 
    def initialize(api_index, client_name) 
        @ptr = RtMidiFuncs.rtmidi_out_create(api_index, client_name)
        ObjectSpace.define_finalizer(self, proc {
            RtMidiFuncs.rtmidi_out_free(@ptr)
        })
    end

    def open_port(port_nbr, port_name)
        RtMidiFuncs.rtmidi_open_port(@ptr, port_nbr, port_name)
    end

    def send_message(msg)
        len = msg.length
        buf = FFI::MemoryPointer.new(:uchar, len) 
        for i in 0 .. (len-1) do 
            buf.put(:uchar, i, msg[i].to_i)
        end
        return RtMidiFuncs.rtmidi_out_send_message(@ptr, buf, len)
    end
end

