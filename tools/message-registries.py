#!/usr/bin/env python3

# This script is used to generate message registry files from JSON data.
# It fetches the latest message registry JSON files from a specified URL,
# processes them, and generates corresponding .hpp files in the designated output directory.
# The script also ensures that the generated files are formatted using clang-format,
# and it updates the meson.build file to include the newly generated header files.

import os
import json
import requests
import re
from bs4 import BeautifulSoup
import subprocess

# URL with JSON files
url = "https://redfish.dmtf.org/registries/"

# Directory to save .hpp files
script_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.join(script_dir, '..')

output_dir = os.path.join(root_dir, "lib/include/phosphor-logging/message-registries/")
local_dir = os.path.join(root_dir, "gen/message-registries/")
build_path = os.path.join(output_dir, 'meson.build')

os.makedirs(output_dir, exist_ok=True)
os.makedirs(local_dir, exist_ok=True)

# Get the list of files from the website
response = requests.get(url)
soup = BeautifulSoup(response.text, 'html.parser')
# Find all links to JSON files
json_links = [link['href'] for link in soup.find_all('a', href=True) if link['href'].endswith('.json')]

def format_file_with_clang_format(file_path, clang_format_config_path):
    try:
        # Run clang-format with the specified configuration file and target file
        subprocess.run(
            ['clang-format', '-i', '-style=file', '-assume-filename=' + file_path, file_path],
            check=True
        )
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running clang-format: {e}")
    except FileNotFoundError:
        print("clang-format not found. Ensure it is installed and available in PATH.")

# Function to convert JSON to .hpp file
def json_to_hpp(json_data, output_file, new_version=""):
    with open(output_file, 'w') as f:
        f.write("#pragma once\n")
        f.write('#include "../message-registries.hpp"\n\n')
        f.write("namespace redfish\n{\n")
        f.write("namespace registries\n{\n")
        f.write(f"namespace {json_data['Id'].replace('.', '_')}\n{{\n\n")

        for key, value in json_data['Messages'].items():
            f.write('struct ')
            if new_version:
                f.write(f'[[deprecated ("Use {new_version}")]]\n')
            elif 'Deprecated' in value:
                f.write(f'[[deprecated ("{value['Deprecated']}")]]\n')

            f.write(f'{key} :\n')
            param_types = []

            if 'ParamTypes' in value:
                for param_type in value['ParamTypes']:
                    if param_type == "number":
                        ctype = "double"
                    else:
                        ctype = "std::string"
                    param_types.append(ctype)
                f.write(f'    public RedfishMessage<{", ".join(param_types)}>\n{{\n')
            else:
                f.write('    public RedfishMessage<>\n{\n')
            f.write('    template <typename... Args>\n')
            f.write(f'    {key}(Args... args) :\n')
            f.write('        RedfishMessage(\n')
            f.write(f'            "{json_data["Id"]}.{key}",\n')
            f.write(f'            "{value["Description"]}",\n')
            if 'LongDescription' in value:
                f.write(f'            "{value["LongDescription"]}",\n')
            else:
                f.write(f'            "{value["Description"]}",\n')
            f.write(f'            "{value["Message"]}",\n')
            if 'MessageSeverity' in value:
                f.write(f'            "{value["MessageSeverity"]}",\n')
            elif 'Severity' in value:
                f.write(f'            "{value["Severity"]}",\n')
            else:
                f.write('            "OK",\n')
            if 'NumberOfArgs' in value:
                f.write(f'            {value["NumberOfArgs"]},\n')
            else:
                f.write('            0,\n')
            if 'ParamTypes' in value:
                f.write(f"            {{ {', '.join(f'\"{item}\"' for item in value['ParamTypes'])} }},\n")
            else:
                f.write("            {},\n")
            if 'ArgDescriptions' in value:
                f.write(f"            {{ {', '.join(f'\"{item}\"' for item in value['ArgDescriptions'])} }},\n")
            else:
                f.write("            {},\n")
            if 'ArgLongDescriptions' in value:
                f.write(f"            {{ {', '.join(f'\"{item}\"' for item in value['ArgLongDescriptions'])} }},\n")
            else:
                f.write("            {},\n")
            f.write(f'            "{value["Resolution"]}",\n')
            f.write('            std::make_tuple(std::forward<Args>(args)...))\n')
            f.write('    {}\n')
            f.write('};\n\n')

        f.write(f"}}; // namespace {json_data["Id"].replace(".", "_")}\n")
        f.write("}; // namespace registries\n")
        f.write("}; // namespace redfish\n")
    format_file_with_clang_format(output_file, '../.clang-format')

# Regular expression to match the desired pattern
pattern = re.compile(r'^(.+)\.(\d+\.\d+\.\d+)\.json$')

# Find all links to JSON files and filter them using the pattern
json_links = [link['href'] for link in soup.find_all('a', href=True) if pattern.match(link['href'])]

latest_files = {}

for file in json_links:
    match = pattern.match(file)
    if match:
        name, version = match.groups()
        version_tuple = tuple(map(int, version.split('.')))
        if name not in latest_files or version_tuple > latest_files[name][1]:
            latest_files[name] = (file, version_tuple)

hpp_file_list=[]
for json_link in json_links:
    json_local = local_dir + json_link;
    if not os.path.isfile(json_local):
        json_url = url + json_link
        json_response = requests.get(json_url)
        if json_response.status_code == 200:
            json_data = json_response.json()

            with open(json_local, 'wb') as f:
                f.write(json_response.content)

    if os.path.isfile(json_local):
        with open(json_local, 'r', encoding='utf-8') as file:
            json_data = json.load(file)
    else: continue
    hpp_file_name=f"{json_data['Id']}.hpp"
    hpp_file_list.append(hpp_file_name)
    hpp_file_path = os.path.join(output_dir, hpp_file_name)
    match = pattern.match(json_link)
    if match:
        name, version = match.groups()
        version_tuple = tuple(map(int, version.split('.')))
        if (name in latest_files) and (version_tuple < latest_files[name][1]):
            namespace=f'{name}_{"_".join(map(str, latest_files[name][1]))}'
            json_to_hpp(json_data, hpp_file_path, namespace)
        else:
            json_to_hpp(json_data, hpp_file_path)
    print(f"Created file: {hpp_file_path}")

print("Generation meson.build")
with open(build_path, 'w') as f:
    f.write('install_headers(\n')
    for hpp_file in hpp_file_list:
        f.write(f"    '{hpp_file}',\n")
    f.write("    subdir: 'phosphor-logging/message-registries'\n")
    f.write(')\n')

print("Generation completed.")