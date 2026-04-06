#pragma once 

#include <iostream> 
#include <filesystem>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>

namespace idyl::utilities {
    extern std::string main_source_path;

    static std::string module_path()
    {
        char *home = getenv("HOME");
        if (home == nullptr) {
            std::cerr << "Error: Could not determine home directory from environment variable 'HOME'. \n";
            return "";
        }
        std::string path = std::string(home) + "/.idyl/modules/";

        // Check if directory exists, if not create it
        std::error_code ec;
        if (!std::filesystem::exists(path)) {
            if (!std::filesystem::create_directories(path, ec)) {
                std::cerr << "Error: Could not create modules directory at '" << path << "'. " << ec.message() << "\n";
                return "";  
            } else {
                std::cout << "Created modules directory at '" << path << "'.\n";
            }
        }
        return path;
    }

    // System-wide library directory (set at compile time via CMake)
    static std::string system_lib_path()
    {
#ifdef IDYL_SYSTEM_LIB_DIR
        return IDYL_SYSTEM_LIB_DIR;
#else
        return "/usr/local/share/idyl/lib/";
#endif
    }

    static std::string dllextension()
    {
        #ifdef _WIN32
            return ".dll";
        #elif __APPLE__
            return ".dylib";
        #else  // Linux - Unix like
            return ".so";
        #endif 
    }
 
    static bool get_module_path(std::string& module_name)
    {
        // Check that name is valid (identifier, no special chars, etc.)
        for (char c : module_name) {
            if (!std::isalnum(c) && c != '_' ) { 
                std::cerr << "Error: Invalid module name '" << module_name << "'. Module names must be valid identifiers.\n";
                return false;
            }
        }             

        // Then add correct extension (.so, .dll, .dylib) based on platform and construct path 
        std::string path = module_path() + module_name + dllextension(); 

        // Check if file exists at path
        if(!std::filesystem::exists(path))
        {
            std::cerr << "Error: Module file not found at path '" << path << "'.\n";
            return false;
        }

        module_name = path;
        return true;
    }
    
    static bool get_library_path(std::string& library_name)
    {
        // First, check that library exists in idyl source file current directory 
        // Then, check in idyl library directory 

        // Check that name is valid (identifier, no special chars, etc.)
        for (char c : library_name) {
            if (!std::isalnum(c) && c != '_' && c != '/' && c != '.' ) { 
                std::cerr << "Error: Invalid library name '" << library_name << "'. Library names must be valid file paths.\n";
                return "";  
            }
        }

        // Add extension if not already present 
        if(library_name.find(".idl") == std::string::npos)
        {
            library_name += ".idl";
        }

        // Check current directory first
        std::filesystem::path local_path = std::filesystem::current_path() / library_name;
        local_path = local_path.lexically_normal(); // Normalize to remove redundant ./ and ../ components

        if (std::filesystem::exists(local_path)) {
            library_name = local_path.string();
            return true;
        }
        else if(std::filesystem::exists(idyl::utilities::main_source_path + "/" + library_name)) {
            library_name = idyl::utilities::main_source_path + "/" + library_name;
            return true;
        } else {
            // Check in lib/ relative to source tree root (dev layout: <source_dir>/../../lib/)
            if (!main_source_path.empty()) {
                std::filesystem::path src_root = std::filesystem::path(main_source_path);
                // Walk up from source file dir until we find a lib/ folder with the file
                for (int depth = 0; depth < 5; ++depth) {
                    std::filesystem::path candidate = src_root / "lib" / library_name;
                    if (std::filesystem::exists(candidate)) {
                        library_name = candidate.lexically_normal().string();
                        return true;
                    }
                    auto parent = src_root.parent_path();
                    if (parent == src_root) break;  // reached filesystem root
                    src_root = parent;
                }
            }

            // Then check in user library directory (~/.idyl/modules/)
            std::filesystem::path lib_dir_path = module_path() + library_name;
            if (std::filesystem::exists(lib_dir_path)) {
                library_name = lib_dir_path.string();
                return true;
            }
            // Then check in system library directory (<prefix>/share/idyl/lib/)
            std::filesystem::path sys_path = system_lib_path() + library_name;
            if (std::filesystem::exists(sys_path)) {
                library_name = sys_path.string();
                return true;
            }
            std::cerr << "Error: Library file not found at '" << local_path.string()
                      << "', '" << lib_dir_path.string()
                      << "', or '" << sys_path.string() << "'.\n";
            return false;
        }
        return false; // Should never reach here
    }

    static std::vector<std::string> list_dll_symbols(const std::string& path)
    {
        std::vector<std::string> symbols;
        // Load the dynamic library and list its exported symbols. 
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error: Could not load module at '" << path << "': "
                 << dlerror() << "\n";
            return symbols;
        } 
        // All modules will have a common symbol "idyl_module_init" which we can use to verify that the library is a valid module and to retrieve its metadata (name, exported symbols, etc.)
        typedef const char** (*init_func_t)(int* count);
        init_func_t init_func = (init_func_t)dlsym(handle, "idyl_module_init");
        if(!init_func)
        {
            std::cerr << "Error: Module at '" << path << "' does not contain required 'idyl_module_init' symbol. Is this a valid idyl module?\n";
            dlclose(handle);
            return symbols;
        } 
        int symbol_count = 0;
        const char** exported_symbols = init_func(&symbol_count);
        for(int i = 0; i < symbol_count; ++i)
        {
            symbols.push_back(exported_symbols[i]);
        }

        dlclose(handle);
        return symbols;
    }
}