#!/usr/bin/env python

r"""
This script will parse error log yaml file(s) and generate
a header file which will then be used by the error logging client and
server to collect and validate the error information generated by the
openbmc software components.

This code uses a mako template to provide the basic template of the header
file we're going to generate.  We then call it with information from the
yaml to generate the header file.
"""

from mako.template import Template
from optparse import OptionParser
import yaml
import sys
import os


def order_inherited_errors(i_errors, i_parents):
    # the ordered list of errors
    errors = list()
    has_inheritance = False
    for error in i_errors:
        if(i_parents[error] is not None):
            has_inheritance = True
            break

    if(has_inheritance):
        # Order the error codes list such that an error is never placed
        # before it's parent. This way generated code can ensure parent
        # definitions preceed child error definitions.
        while(len(errors) < len(i_errors)):
            for error in i_errors:
                if(error in errors):
                    # already ordererd
                    continue
                if((not i_parents[error]) or (i_parents[error] in errors)):
                    # parent present, or has no parent, either way this error
                    # can be added
                    errors.append(error)
    else:
        # no inherited errors
        errors = i_errors

    return errors


def check_error_inheritance(i_errors, i_parents):
    for error in i_errors:
        if(i_parents[error] and (i_parents[error] not in i_errors)):
            print (error + " inherits " + i_parents[error] +
                   " but the latter is not defined")
            return False
    return True


def get_error_yaml_files(i_yaml_dir, i_test_dir):
    yaml_files = []
    for root, dirs, files in os.walk(i_yaml_dir):
        for files in filter(lambda file: file.endswith('.errors.yaml'), files):
            yaml_files.append(os.path.join(root, files))
    for root, dirs, files in os.walk(i_test_dir):
        for files in filter(lambda file: file.endswith('.errors.yaml'), files):
            yaml_files.append(os.path.join(root, files))
    return yaml_files


def get_meta_yaml_file(i_error_yaml_file):
    # the meta data will be defined in file name where we replace
    # <Interface>.errors.yaml with <Interface>.metadata.yaml
    meta_yaml = i_error_yaml_file.replace("errors", "metadata")
    return meta_yaml


def get_cpp_type(i_type):
    typeMap = {
        'int16': 'int16_t',
        'int32': 'int32_t',
        'int64': 'int64_t',
        'uint16': 'uint16_t',
        'uint32': 'uint32_t',
        'uint64': 'uint64_t',
        'double': 'double',
        # const char* aids usage of constexpr
        'string': 'const char*'}

    return typeMap[i_type]


def gen_elog_hpp(i_yaml_dir, i_test_dir, i_output_hpp,
                 i_template_dir, i_elog_mako, i_error_namespace):
    r"""
    Read  yaml file(s) under input yaml dir, grab the relevant data and call
    the mako template to generate the output header file.

    Description of arguments:
    i_yaml_dir                  directory containing base error yaml files
    i_test_dir                  directory containing test error yaml files
    i_output_hpp                name of the to be generated output hpp
    i_template_dir              directory containing error mako templates
    i_elog_mako                 error mako template to render
    """

    # Input parameters to mako template
    errors = list()  # Main error codes
    error_msg = dict()  # Error msg that corresponds to error code
    error_lvl = dict()  # Error code log level (debug, info, error, ...)
    meta = dict()  # The meta data names associated (ERRNO, FILE_NAME, ...)
    meta_data = dict()  # The meta data info (type, format)
    parents = dict()

    error_yamls = get_error_yaml_files(i_yaml_dir, i_test_dir)

    for error_yaml in error_yamls:
        # Verify the error yaml file
        if (not (os.path.isfile(error_yaml))):
            print "Can not find input yaml file " + error_yaml
            exit(1)

        # Verify the metadata yaml file
        meta_yaml = get_meta_yaml_file(error_yaml)
        if (not (os.path.isfile(meta_yaml))):
            print "Can not find meta yaml file " + meta_yaml
            exit(1)

        # Verify the input mako file
        template_path = "/".join((i_template_dir, i_elog_mako))
        if (not (os.path.isfile(template_path))):
            print "Can not find input template file " + template_path
            exit(1)

        get_elog_data(error_yaml,
                      meta_yaml,
                      # 3rd arg is a tuple
                      (errors,
                       error_msg,
                       error_lvl,
                       meta,
                       meta_data,
                       parents))

    if(not check_error_inheritance(errors, parents)):
        print "Error - failed to validate error inheritance"
        exit(1)

    errors = order_inherited_errors(errors, parents)

    # Load the mako template and call it with the required data
    yaml_dir = i_yaml_dir.strip("./")
    yaml_dir = yaml_dir.strip("../")
    template = Template(filename=template_path)
    f = open(i_output_hpp, 'w')
    f.write(template.render(
            errors=errors, error_msg=error_msg,
            error_lvl=error_lvl, meta=meta,
            meta_data=meta_data, error_namespace=i_error_namespace,
            parents=parents))
    f.close()


def get_elog_data(i_elog_yaml,
                  i_elog_meta_yaml,
                  o_elog_data):
    r"""
    Parse the error and metadata yaml files in order to pull out
    error metadata.

    Description of arguments:
    i_elog_yaml                 error yaml file
    i_elog_meta_yaml            metadata yaml file
    o_elog_data                 error metadata
    """
    errors, error_msg, error_lvl, meta, meta_data, parents = o_elog_data
    ifile = yaml.safe_load(open(i_elog_yaml))
    mfile = yaml.safe_load(open(i_elog_meta_yaml))
    for i in ifile:
        match = None
        # Find the corresponding meta data for this entry
        for m in mfile:
            if m['name'] == i['name']:
                match = m
                break
        if (match is None):
            print "Error - Did not find meta data for " + i['name']
            exit(1)
        # Grab the main error and it's info
        errors.append(i['name'])
        parent = None
        if('inherits' in i):
            # xyz.openbmc.Foo, we need Foo
            # Get 0th inherited error (current support - single inheritance)
            parent = i['inherits'][0].split(".").pop()
        parents[i['name']] = parent
        error_msg[i['name']] = i['description']
        error_lvl[i['name']] = match['level']
        tmp_meta = []
        # grab all the meta data fields and info
        for j in match['meta']:
            str_short = j['str'].split('=')[0]
            tmp_meta.append(str_short)
            meta_data[str_short] = {}
            meta_data[str_short]['str'] = j['str']
            meta_data[str_short]['str_short'] = str_short
            meta_data[str_short]['type'] = get_cpp_type(j['type'])
        meta[i['name']] = tmp_meta

    # Debug
    # for i in errors:
    #   print "ERROR: " + errors[i]
    #   print " MSG:  " + error_msg[errors[i]]
    #   print " LVL:  " + error_lvl[errors[i]]
    #   print " META: "
    #   print meta[i]


def main(i_args):
    parser = OptionParser()

    parser.add_option("-m", "--mako", dest="elog_mako",
                      default="elog-gen-template.mako.hpp",
                      help="input mako template file to use")

    parser.add_option("-o", "--output", dest="output_hpp",
                      default="elog-gen.hpp",
                      help="output hpp to generate, elog-gen.hpp is default")

    parser.add_option("-y", "--yamldir", dest="yamldir",
                      default="None",
                      help="Base directory of yaml files to process")

    parser.add_option("-u", "--testdir", dest="testdir",
                      default="./tools/example/",
                      help="Unit test directory of yaml files to process")

    parser.add_option("-t", "--templatedir", dest="templatedir",
                      default="phosphor-logging/templates/",
                      help="Base directory of files to process")

    parser.add_option("-n", "--namespace", dest="error_namespace",
                      default="example/xyz/openbmc_project/Example",
                      help="Error d-bus namespace")

    (options, args) = parser.parse_args(i_args)

    gen_elog_hpp(options.yamldir,
                 options.testdir,
                 options.output_hpp,
                 options.templatedir,
                 options.elog_mako,
                 options.error_namespace)

# Only run if it's a script
if __name__ == '__main__':
    main(sys.argv[1:])
