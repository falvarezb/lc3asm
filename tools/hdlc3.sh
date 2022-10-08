#!/usr/bin/env sh
# 
# hexdump content of lc3 binaries (similar to 'lc3objdump.c').
# 
# Each line begins with an offset in decimal format and shows the 16-bit integer corresponding to an 
# instruction as 2 hexadecimal bytes.
# 
# ./tools/hdlc3.sh test/testfiles/t1.obj
# 000000  30 00
# 000002  10 21
# 000004  f0 25
#
# See 'man hexdump' and https://www.suse.com/c/making-sense-hexdump/#Custom%20field%20widths for details
# about hexdump usage

if [ $# -lt 1 ]; then
    echo "filename must be specified"
    exit 1
fi

FILE=$1
hexdump -v -e '"%04_ad" 2/1 " %.2x" " \n"' "$FILE"