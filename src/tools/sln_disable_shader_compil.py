"""
Recurse search and disables all the shader compilations in solution files
"""

import os
import sys
import inspect
import argparse
import fnmatch

def get_args():
    parser = argparse.ArgumentParser()

    # get all script args
    _, all_arguments = parser.parse_known_args()
    double_dash_index = all_arguments.index('--')
    script_args = all_arguments[double_dash_index + 1:]

    # add parser rules
    parser.add_argument('-c', '--cwd', help="")
    parsed_script_args, _ = parser.parse_known_args(script_args)

    return parsed_script_args
    
def get_vcxproj(cwd):
    matches = []
    for root, dirnames, filenames in os.walk(cwd):
        for filename in fnmatch.filter(filenames, '*.vcxproj'):
            matches.append(os.path.join(root, filename))
            
    return matches;

def replace_fx(lines):
    res = []
    for line in lines:
        res.append(line)
       
if __name__ == "__main__":
    print("Remove all shader compilations from solutions")
    
    args = get_args()
    files = get_vcxproj(args.cwd)
    
    for file in files:
        try:
            res = []
            with open(infile, 'r') as f:
                res = replace_fx(f.readlines())
                
            if not res:
                raise RuntimeError("fuckup")
                
            with open(infile, 'w') as f:
                f.writelines(res)
        
            print("{0} has writen".format(file))
        except RuntimeError as e:
            print("{0} has fucked up".format(file))
    
    # <FXCompile Include="C:\work\ir-demos\ir-f17\assets\shaders\flat.hlsl" />
    # <None Include="C:\work\ir-demos\ir-f17\assets\sonic_attack.mp3" />
    