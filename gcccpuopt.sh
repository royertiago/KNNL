#!/bin/sh

# Author:
#    http://www.pixelbeat.org/
# Notes:
#    This script is Linux specific
#    This script is x86 (32 bit) specific
#    It should work on any gcc >= 2.95 at least
#    It only returns CPU specific options. You probably also want -03 etc.
# Changes:
#    V0.1, 12 Mar 2003, Initial release
#    V0.2, 01 Jun 2005, Added support for 3.2>=gcc<=4.0
#    V0.3, 03 Jun 2005, Added support for pentium-m
#    V0.4, 03 Jun 2005, Fix silly bugs
#    V0.5, 07 Jun 2005, Clarify/Simplify confusing floating point expr usage
#                       Print warning when CPU only supported on a newer gcc

if [ "$1" = "--version" ]; then
    echo "0.5" && exit
fi

# This table shows when -march options were introduced into _official_ gcc releases.
# Note there are vendor deviations that complicate this.
# For e.g. redhat introduced the prescott option in 3.3-13.
#   gcc-2.95   = i386, i486, i586,pentium, i686,pentiumpro, k6
#   gcc-3.0   += athlon
#   gcc-3.1   += pentium-mmx, pentium2, pentium3, pentium4, k6-2, k6-3, athlon-{tbird, 4,xp,mp}
#   gcc-3.3   += winchip-c6, winchip2, c3
#   gcc-3.4.0 += k8,opteron,athlon64,athlon-fx, c3-2
#   gcc-3.4.1 += pentium-m, pentium3m, pentium4m, prescott, nocona

[ -z "$CC" ] && CC=gcc

try_march() {
    $CC -march=$1 -S -o /dev/null -xc /dev/null >/dev/null 2>&1
}

try_line() {
    skip=0
    for arch in $1; do
        if try_march $arch; then
            echo $arch
            return
        elif [ "$skip" == "0" ]; then
            skip=1
            echo "Warning: Newer versions of GCC better support your CPU with -march=$arch" >&2
        fi
    done
    return 1
}

IFS=":"
while read name value; do
    unset IFS
    name=`echo $name`
    value=`echo $value`
    IFS=":"
    if [ "$name" == "vendor_id" ]; then
        vendor_id="$value"
    elif [ "$name" == "cpu family" ]; then
        cpu_family="$value"
    elif [ "$name" == "model" ]; then
        cpu_model="$value"
    elif [ "$name" == "flags" ]; then
        flags="$value"
        break #flags last so break early
    fi
done < /proc/cpuinfo
unset IFS

if [ "$vendor_id" == "AuthenticAMD" ]; then
    if [ "$cpu_family" == "4" ]; then
        _CFLAGS="-march=i486"
    elif [ "$cpu_family" == "5" ]; then
        if [ "$cpu_model" -lt "4" ]; then
            _CFLAGS="-march=pentium"
        elif [ "$cpu_model" == "6" ] || [ "$cpu_model" == "7" ]; then
            _CFLAGS="-march=k6"
        elif [ "$cpu_model" == "8" ] || [ "$cpu_model" == "12" ]; then
            line="k6-2 k6"
        elif [ "$cpu_model" == "9" ] || [ "$cpu_model" == "13" ]; then
            line="k6-3 k6-2 k6"
        fi
    elif [ "$cpu_family" == "6" ]; then
        if [ "$cpu_model" -le "3" ]; then
            line="athlon k6-3 k6-2 k6"
        elif [ "$cpu_model" == "4" ]; then
            line="athlon-tbird athlon k6-3 k6-2 k6"
        elif [ "$cpu_model" -ge "6" ]; then #athlon-{4,xp,mp}
            line="athlon-4 athlon k6-3 k6-2 k6"
        fi
    elif [ "$cpu_family" == "15" ]; then #k8,opteron,athlon64,athlon-fx
        line="k8 athlon-4 athlon k6-3 k6-2 k6"
    fi
elif [ "$vendor_id" == "CentaurHauls" ]; then
    if [ "$cpu_family" == "5" ]; then
        if [ "$cpu_model" == "4" ]; then
            line="winchip-c6 pentium"
        elif [ "$cpu_model" == "8" ]; then
            line="winchip2 winchip-c6 pentium"
        elif [ "$cpu_model" -ge "9" ]; then
            line="winchip2 winchip-c6 pentium" #actually winchip3 but gcc doesn't support this currently
        fi
    elif [ "$cpu_family" == "6" ]; then
        if echo "$flags" | grep -q cmov; then
            fallback=pentiumpro
        else
            fallback=pentium #gcc incorrectly assumes i686 always has cmov
        fi
        if [ "$cpu_model" == "6" ]; then
            _CFLAGS="-march=pentium" # ? Cyrix 3 (samuel)
        elif [ "$cpu_model" == "7" ] || [ "$cpu_model" == "8" ]; then
            line="c3 winchip2 winchip-c6 $fallback"
        elif [ "$cpu_model" -ge "9" ]; then
            line="c3-2 c3 winchip2 winchip-c6 $fallback"
        fi
    fi
else #everything else "GenuineIntel"
    if [ "$cpu_family" == "3" ]; then
        _CFLAGS="-march=i386"
    elif [ "$cpu_family" == "4" ]; then
        _CFLAGS="-march=i486"
    elif [ "$cpu_family" == "5" ]; then
        if [ "$cpu_model" != "4" ]; then
            _CFLAGS="-march=pentium"
        else
            line="pentium-mmx pentium" #No overlap with other vendors
        fi
    elif [ "$cpu_family" == "6" ]; then
        if [ "$cpu_model" == "0" ] || [ "$cpu_model" == "1" ]; then
            _CFLAGS="-march=pentiumpro"
        elif [ "$cpu_model" -ge "3" ] && [ "$cpu_model" -le "6" ]; then #4=TM5600 at least
            line="pentium2 pentiumpro pentium-mmx pentium i486 i386"
        elif [ "$cpu_model" == "9" ] || [ "$cpu_model" == "13" ]; then #centrino
            line="pentium-m pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        elif [ "$cpu_model" -ge "7" ] && [ "$cpu_model" -le "11" ]; then
            line="pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        fi
    elif [ "$cpu_family" == "15" ]; then
        if [ "$cpu_model" == "3" ]; then
            line="prescott pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        else
            line="pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        fi
    fi
fi

[ -z "$_CFLAGS" ] && _CFLAGS="-march=`try_line "$line"`"

gcc_version=`$CC -dumpversion`
if expr $gcc_version '>=' 3.1 >/dev/null; then #lexographical comparison
    if echo "$flags" | grep -q sse; then
        _CFLAGS="$_CFLAGS -mfpmath=sse"
    fi
fi

echo "$_CFLAGS"
