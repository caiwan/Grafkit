"""
Recurse search and disables all the shader compilations in solution files
"""

import os
import re
import sys
import inspect
import fnmatch
    
def get_vcxproj(cwd):
    matches = []
    for root, dirnames, filenames in os.walk(cwd):
        for filename in fnmatch.filter(filenames, '*.vcxproj'):
            matches.append(os.path.join(root, filename))
            
    return matches;
       
if __name__ == "__main__":
    print("Remove all shader compilations from solutions")
    
    if len(sys.argv) != 2:
        print("Search root dir was not specified.")
        print("usage:")
        print("sln_disable_shader_compil.py <search_folder>")
        sys.exit(0)

    in_folder = sys.argv[1]
    
    files = get_vcxproj(in_folder)
    
    # <FXCompile Include="C:\work\ir-demos\ir-f17\assets\shaders\flat.hlsl" />
    # <None Include="C:\work\ir-demos\ir-f17\assets\sonic_attack.mp3" />
    fxcompie_re = re.compile("<(FXCompile)\\s(Include=\"(.*)\")\\s\\/>")
        
    for file in files:
        try:
        
            # --- read and replace stuff 
            res = []
            with open(file, 'r') as f:
                for line in f.readlines():
                    rline = line
                    match = fxcompie_re.search(line)
                    if match:
                        name = match.group(3)
                        rline = "<None Include=\"{0}\"/>\r\n".format(name)
                    res.append(rline)

            if not res:
                raise RuntimeError("gereral fuckup occured")
                
            # --- write everything back
            with open(file, 'w') as f:
                f.writelines(res)
        
            print("{0} has writen".format(file))
        except RuntimeError as e:
            print("{0} has fucked up".format(file))

            