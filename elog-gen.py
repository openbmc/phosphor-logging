from mako.template import Template
from optparse import OptionParser
import yaml
import sys
import os

class elog_desc:
    def __init__(self,name,msg,meta):
        print "test"

def remove_me():
    mytemplate = Template(filename='elog-gen-template')

    errors = {}
    errors[0] = 'file_not_found'
    errors[1] = 'scom_fail'

    # Use a list to force the order
    meta = list()
    meta.append(['errnum','file_path','file_name'])
    meta.append(['address','chip'])

    error_msg = {}
    error_msg[errors[0]] = 'A required file was not found'
    error_msg[errors[1]] = 'A scom operation has failed'

    meta_data = {}
    meta_data['errnum'] = {}
    meta_data['errnum']['STR'] = "ERRNO=%d"
    meta_data['errnum']['STR_SHORT'] = 'ERRNO'
    meta_data['errnum']['TYPE'] = 'int'

    meta_data['file_path'] = {}
    meta_data['file_path']['STR'] = "FILE_PATH=%s"
    meta_data['file_path']['STR_SHORT'] = 'FILE_PATH'
    meta_data['file_path']['TYPE'] = 'const char *'

    meta_data['file_name'] = {}
    meta_data['file_name']['STR'] = "FILE_NAME=%s"
    meta_data['file_name']['STR_SHORT'] = 'FILE_NAME'
    meta_data['file_name']['TYPE'] = 'const char *'

    meta_data['address'] = {}
    meta_data['address']['STR'] = "ADDRESS=%s"
    meta_data['address']['STR_SHORT'] = 'ADDRESS'
    meta_data['address']['TYPE'] = 'const char *'

    meta_data['chip'] = {}
    meta_data['chip']['STR'] = "CHIP=%s"
    meta_data['chip']['STR_SHORT'] = 'CHIP'
    meta_data['chip']['TYPE'] = 'const char *'

    print(mytemplate.render(errors=errors,error_msg=error_msg,
                            meta=meta,meta_data=meta_data))


def gen_elog_hpp(i_elog_yaml, i_output_hpp):

    #print yaml.dump(yaml.load(open(i_elog_yaml)))
    #print yaml.load(open(i_elog_yaml))

    errors = dict()
    error_msg = dict()
    error_lvl = dict()
    meta = list()
    meta_data = dict()

    err_count = 0;
    ifile = yaml.safe_load(open(i_elog_yaml))
    for i in ifile['SW'].keys():
        #errors.append(i)
        errors[err_count] = i
        prop = ifile['SW'][i]
        error_msg[i] = prop['msg']
        error_lvl[i] = prop['level']
        tmp_meta=[]
        for j in prop['meta']:
            str_short = j['str'].split('=')[0]
            tmp_meta.append(str_short)
            meta_data[str_short] = {}
            meta_data[str_short]['str'] = j['str']
            meta_data[str_short]['str_short'] = str_short
            meta_data[str_short]['type'] = j['type']
        #meta.append([','.join(tmp_meta)])
        meta.append(tmp_meta)
        err_count+=1

    # Debug
    #for i in errors:
    #    print "ERROR: " + errors[i]
    #    print " MSG:  " + error_msg[errors[i]]
    #    print " LVL:  " + error_lvl[errors[i]]
    #    print " META: "
    #    print meta[i]

    mytemplate = Template(filename='elog-gen-template')
    print(mytemplate.render(errors=errors,error_msg=error_msg,
                            error_lvl=error_lvl,meta=meta,
                            meta_data=meta_data))


def main(i_args):
    parser = OptionParser()

    parser.add_option("-e","--elog",dest="elog_yaml",default="elog.yaml",
                      help="input error yaml file to parse");

    parser.add_option("-o","--output",dest="output_hpp", default="elog-gen.hpp",
                      help="output hpp to generate, elog-gen.hpp is default");

    (options, args) = parser.parse_args(i_args)

    #print options.elog_yaml
    #print options.output_hpp

    if (not (os.path.isfile(options.elog_yaml))):
        print "Can not find input yaml file " + options.elog_yaml
        exit(1);

    gen_elog_hpp(options.elog_yaml,options.output_hpp)
    #remove_me()

# Only run if it's a script
if __name__ == '__main__':
    main(sys.argv[1:])