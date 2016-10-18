from mako.template import Template

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

#print meta_data['errnum']['TYPE']

print(mytemplate.render(errors=errors,error_msg=error_msg,
                        meta=meta,meta_data=meta_data))

