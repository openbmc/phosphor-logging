#!/usr/bin/python -u

import uuid
import yaml

with open('elog.yaml') as f:
    ifile = yaml.safe_load(f)

with open('elog-gen.hpp', 'w') as ofile:
    ofile.write('namespace phosphor\n{\n\n')
    ofile.write('namespace logging\n{\n\n')

    # TBD

if __name__ == '__main__':
     print "TODO\n"
