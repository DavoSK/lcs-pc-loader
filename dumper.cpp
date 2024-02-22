static std::vector<std::string> cxxSymbols{};
                    std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::vector<std::string> getSymbolsFromELF(bool demangled) {
    const char* cmd = demangled ? "nm --dynamic --defined-only --extern-only --demangle -f just-symbols libc++.so" : "nm --dynamic --defined-only --extern-only -f just-symbols libc++.so";
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    std::istringstream origStream(result);
    std::vector<std::string> lineList;
    std::string curLine;
    while (std::getline(origStream, curLine))
    {
        if (!curLine.empty())
            lineList.push_back(curLine);
    }

    return lineList;
}


                    
                    static std::vector<std::string> cxxSymbolsDemanged{};
                    static void* cxx_lib = NULL;

                    if(cxxSymbols.empty()) {
                        cxxSymbols = getSymbolsFromELF(false);
                        cxxSymbolsDemanged= getSymbolsFromELF(true);

                        cxx_lib = dlopen("/home/david/lcs/libc++.so", RTLD_LAZY);
                        if(!cxx_lib) {
                            printf("unable to magic !\n");
                            exit(1);
                        }
                    }

                    // try magic    
                    int status;
                    char *demangled_name = abi::__cxa_demangle(symstr, 0, 0, &status);  
                    if(demangled_name) {
                        //we wanna replace ndk shits
                        if(strstr(demangled_name, "__ndk1")) {
                            std::string nameToReplace(demangled_name);
                            std::string toFind = replace_all(nameToReplace, "__ndk1", "__1");

                            for(size_t j = 0; j < cxxSymbols.size(); ++j) {
                                const auto& currentSymbol = cxxSymbolsDemanged[j];
                                if(toFind == currentSymbol) {
                                    addr = dlsym(cxx_lib, cxxSymbols[j].c_str());
                                    
                                    char temp[255] = {0};
                                    sprintf(temp, "{ \"%s\", \"%s\" },\n", symstr, cxxSymbols[j].c_str());
                                    ourMap.insert(temp);
                                    break;
                                    
                                }
                            }
                        }
                        free(demangled_name);
                    }


