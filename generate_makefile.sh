#!/bin/sh

cd $1

toplevel_dirs=xyz
interfaces=`find $toplevel_dirs -name "*.interface.yaml"`

for i in ${interfaces};
do
    iface_path=`dirname $i`/`basename $i .interface.yaml`
    iface=`echo $iface_path | sed 's/\//./g'`
    cat <<MAKEFILE

${i%.interface.yaml}/server.cpp: ${i} ${i%.interface.yaml}/server.hpp
	@mkdir -p \`dirname \$@\`
	\$(SDBUSPLUSPLUS) -r \$(srcdir) interface server-cpp ${iface} > \$@

${i%.interface.yaml}/server.hpp: ${i}
	@mkdir -p \`dirname \$@\`
	\$(SDBUSPLUSPLUS) -r \$(srcdir) interface server-header ${iface} > \$@

MAKEFILE

done

toplevel_dirs=tools/example/xyz
errors=`find $toplevel_dirs -name "*.errors.yaml"`

for e in ${errors};
do
    iface_path=`dirname $e`/`basename $e .errors.yaml`
    iface=`echo $iface_path | sed 's/\//./g'`
    cat <<MAKEFILE

${e%.errors.yaml}/error.hpp: ${e}
	@mkdir -p \`dirname \$@\`
	\$(SDBUSPLUSPLUS) -r \$(srcdir) error exception-header ${iface} > \$@

MAKEFILE

done

echo "elog-errors.hpp: \\"
for e in ${errors};
do
    echo "  ${e%.errors.yaml}/error.hpp\\"
done

cat << MAKEFILE

MAKEFILE
