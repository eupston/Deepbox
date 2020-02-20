'''Scans the main application for all library dependencies pulls the dependencies into the framework path,
and repaths the corresponding dependencies in the framework path'''

import os
import subprocess
import shutil

main_app_path = r"/Volumes/Macintosh\ HD/Users/macuser/Library/Audio/Plug-Ins/Components/Deepbox.component/Contents/MacOS/Deepbox"
library_path = r"/Volumes/Macintosh\ HD/Users/macuser/Library/Audio/Plug-Ins/Components/Deepbox.component/Contents/Frameworks"
targeted_path = "/usr/local/opt/"
new_path = "@loader_path/../Frameworks"
# excluded_path = "/System/Library/Frameworks"
excluded_path = ""


def embed_libraries_main_app(main_app_path):
    output = subprocess.check_output("otool -L {}".format(main_app_path), shell=True)
    output_dependencies = str(output).split(r"\t")
    cleaned_lib_dependencies = []
    for lib in output_dependencies:
        full_lib_path = lib[:lib.find("(") - 1]
        if "b'" not in full_lib_path:
            cleaned_lib_dependencies.append(full_lib_path)
    main_app_path_contents = os.path.dirname(os.path.dirname(main_app_path))
    main_app_path_framework = os.path.join(main_app_path_contents, "Frameworks")
    if not os.path.exists(main_app_path_framework):
        os.system("mkdir {}".format(main_app_path_framework))
    libs_copied = []
    for lib in cleaned_lib_dependencies:
        lib_basename = os.path.basename(lib)
        if not os.path.exists(os.path.join(main_app_path_framework, lib_basename)):
            if os.path.splitext(lib)[-1]:
                shutil.copyfile(lib, main_app_path_framework.replace("\\", "") + "/" + lib_basename)
            else:
                shutil.copy(lib, main_app_path_framework.replace("\\", "") + "/" + lib_basename)
            libs_copied.append(os.path.join(main_app_path_framework, lib_basename))
    return libs_copied

def repath_dependencies_lib_main_app(main_app_path, excluded_path, new_path="@loader_path/../Frameworks"):
    output = subprocess.check_output("otool -L {}".format(main_app_path), shell=True)
    output_dependencies = str(output).split(r"\t")
    cleaned_lib_dependencies = []
    for lib in output_dependencies:
        full_lib_path = lib[:lib.find("(") - 1]
        if "b'" not in full_lib_path:
            cleaned_lib_dependencies.append(full_lib_path)
    for lib_path in cleaned_lib_dependencies:
        lib_basename = os.path.basename(lib_path)
        os.system("install_name_tool -change '{}' {} {}".format(lib_path, os.path.join(new_path, lib_basename), main_app_path))

def repath_dependencies_all_dylibs(library_path, excluded_path, new_path="@loader_path/../Frameworks"):
    library_path_cleaned = library_path.replace("\\","")
    # all_dylibs = [lib for lib in os.listdir(library_path_cleaned) if os.path.splitext(lib)[1] == ".dylib"]
    all_dylibs = [lib for lib in os.listdir(library_path_cleaned)]
    for lib in all_dylibs:
        dylib_path = os.path.join(library_path, lib)
        new_dependencies_found = repath_dependencies_single_dylib(library_path, dylib_path, excluded_path, new_path)
        if new_dependencies_found:
            print("new_dependencies_found", new_dependencies_found)

def repath_dependencies_single_dylib(base_library_path, dylib_path, excluded_path=False, new_path="@loader_path/../Frameworks"):
    output = subprocess.check_output('otool -L "{}"'.format(dylib_path.replace("\\","")), shell=True)
    output_dependencies = str(output).split(r"\t")
    cleaned_lib_dependencies = []
    for lib in output_dependencies:
        full_lib_path = lib[:lib.find("(") - 1]
        if "b'" not in full_lib_path:
            cleaned_lib_dependencies.append(full_lib_path)
    if excluded_path:
        targeted_libs = [lib for lib in cleaned_lib_dependencies if excluded_path not in lib]
    else:
        targeted_libs = cleaned_lib_dependencies
    new_dependencies_found = []
    for lib_path in targeted_libs:
        lib_basename = os.path.basename(lib_path)
        new_lib_path = os.path.join(base_library_path, lib_basename).replace("\\","")
        os.system("install_name_tool -change {} {} {}".format(lib_path, os.path.join(new_path, lib_basename), dylib_path))
        if not os.path.exists(new_lib_path):
            if os.path.splitext(lib_path)[-1]:
                print("-"*20 + "copying1...", lib_path)
                shutil.copyfile(lib_path, new_lib_path)
            else:
                print("-"*20 + "copying2...", lib_path)
                shutil.copy(lib_path, new_lib_path)
            new_dependencies_found.append(new_lib_path)
    if new_dependencies_found:
        for dylib in new_dependencies_found:
            repath_dependencies_single_dylib(base_library_path, dylib, excluded_path, new_path)
            return new_dependencies_found
    return False

if __name__ == "__main__":
    # embed_libraries_main_app(main_app_path)
    # repath_dependencies_all_dylibs(library_path, excluded_path, new_path)
    repath_dependencies_lib_main_app(main_app_path, excluded_path)