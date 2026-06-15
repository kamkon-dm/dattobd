#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-only

set -u

#
# Copyright (C) 2019 Datto, Inc.
#

DBD_VER=$(grep -i "Version:" ../dist/dattobd.spec | awk '{print $2}')

echo
echo "dattobd: $DBD_VER $(git rev-parse --short HEAD)"
echo "kernel: $(uname -r)"
echo "gcc: $(gcc --version | awk 'NR==1 {print $3}')"
echo "bash: ${BASH_VERSION}"
echo "python: $(python3 --version)"
echo

dmesg -c &> /dev/null
>| dmesg.log
python3 -m unittest -v
ret=$?
dmesg > dmesg.log

exit ${ret}
