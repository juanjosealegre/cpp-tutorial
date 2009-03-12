#!/bin/sh -e

${ACLOCAL-aclocal} -I m4 $ACLOCAL_FLAGS
${AUTOCONF-autoconf}
${AUTOHEADER-autoheader}
${AUTOMAKE-automake} --add-missing --copy $AUTOMAKE_FLAGS

# why did autoreconf failed to include the m4 macros folder?
#autoreconf -i -f -I m4
./configure "$@"
