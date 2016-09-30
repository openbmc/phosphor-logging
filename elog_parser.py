#!/usr/bin/python -u

import uuid
import yaml

with open('elog.yaml') as f:
    ifile = yaml.safe_load(f)

with open('elog-gen.hpp', 'w') as ofile:
    ofile.write('namespace phosphor\n{\n\n')
    ofile.write('namespace logging\n{\n\n')

    ofile.write('typedef struct\n{\n')
    ofile.write('    constexpr auto md_name;\n')
    ofile.write('    constexpr auto md_entry;\n')
    ofile.write('} md;\n\n')

    for i in ifile['SW'].keys():
        ofile.write('struct ' + i +'\n{\n')
        ofile.write('    ' +
                    'constexpr auto exc_msg = "org.freedesktop.' +
                    i +'";\n')
        prop = ifile['SW'][i]
        ofile.write('    constexpr auto msg = "' + prop['msg'] + '";\n')
        ofile.write('    constexpr auto msgid = ' + str(uuid.uuid4()) + ';\n')
        ofile.write('    level L = level::' + prop['level'] + ';\n')
        ofile.write('    std::vector<std::string> META_LIST {')
        for j in prop['meta_i']:
            ofile.write('"' + j + '", ')
        for j in prop['meta_s']:
            ofile.write('"' + j + '", ')
        ofile.write('};\n')
        ofile.write('};\n\n')

        for j in prop['meta_i']:
            ofile.write('md ' + j + ' {"' + j + '", "' + j + '=%d"};\n')
        for j in prop['meta_s']:
            ofile.write('md ' + j + ' {"' + j + '", "' + j + '=%s"};\n')
        ofile.write('\n')

    ofile.write('} // namespace logging\n\n')
    ofile.write('} // namespace phosphor\n')
