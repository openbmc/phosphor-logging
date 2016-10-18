#!/usr/bin/env python

r"""
This script will parse the input error log yaml file and generate
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

def gen_elog_hpp(i_elog_yaml, i_output_hpp):
    r"""
    Read the input yaml file, grab the relevant data and call the mako
    template to generate the header file.

    Description of arguments:
    i_elog_yaml                  yaml file describing the error logs
    i_output_hpp                 header file to output the generated code to
    """

    # Input parameters to mako template
    errors = dict()     # Main error codes
    error_msg = dict()  # Error msg that corresponds to error code
    error_lvl = dict()  # Error code log level (debug, info, error, ...)
    meta = list()       # The meta data names associated (ERRNO, FILE_NAME, ...)
    meta_data = dict()  # The meta data info (type, format)

    # see elog.yaml for reference
    ifile = yaml.safe_load(open(i_elog_yaml))
    err_count = 0;
    for i in ifile['SW'].keys():
        # Grab the main error
        errors[err_count] = i
        # Grab it's sub-items
        prop = ifile['SW'][i]
        error_msg[i] = prop['msg']
        error_lvl[i] = prop['level']
        tmp_meta=[]
        # grab all the meta data fields and info
        for j in prop['meta']:
            str_short = j['str'].split('=')[0]
            tmp_meta.append(str_short)
            meta_data[str_short] = {}
            meta_data[str_short]['str'] = j['str']
            meta_data[str_short]['str_short'] = str_short
            meta_data[str_short]['type'] = j['type']
        meta.append(tmp_meta)
        err_count+=1

    # Debug
    #for i in errors:
    #    print "ERROR: " + errors[i]
    #    print " MSG:  " + error_msg[errors[i]]
    #    print " LVL:  " + error_lvl[errors[i]]
    #    print " META: "
    #    print meta[i]

    # Load the mako template and call it with the required data
    mytemplate = Template(filename='elog-gen-template.mako.hpp')
    f = open(i_output_hpp,'w')
    f.write(mytemplate.render(errors=errors,error_msg=error_msg,
                            error_lvl=error_lvl,meta=meta,
                            meta_data=meta_data))
    f.close()


def main(i_args):
    parser = OptionParser()

    parser.add_option("-e","--elog",dest="elog_yaml",default="elog.yaml",
                      help="input error yaml file to parse");

    parser.add_option("-o","--output",dest="output_hpp", default="elog-gen.hpp",
                      help="output hpp to generate, elog-gen.hpp is default");

    (options, args) = parser.parse_args(i_args)

    if (not (os.path.isfile(options.elog_yaml))):
        print "Can not find input yaml file " + options.elog_yaml
        exit(1);

    gen_elog_hpp(options.elog_yaml,options.output_hpp)

# Only run if it's a script
if __name__ == '__main__':
    main(sys.argv[1:])