#!/bin/bash
# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# Used to create the test archive file for openage.cabextract.test

cd `mktemp -d`

# get cabarc.exe tool
echo "downloading CABARC.EXE"
curl http://download.microsoft.com/download/d/3/8/d38066aa-4e37-4ae8-bce3-a4ce662b2024/WindowsXP-KB838079-SupportTools-ENU.exe > tmp.exe
7z e tmp.exe support.cab
7z e support.cab cabarc.exe

# create test files
dd if=/dev/urandom bs=512k count=1 > testfilea
dd if=/dev/zero bs=64M count=1 > testfileb
touch testfilec
mkdir -p testdir
echo "testfiled" > testdir/testfiled
cp /bin/bash testdir/testfilee

# compress test files
testfiles="testfilea testfileb testfilec testdir/testfiled testdir/testfilee"
echo
wine cabarc.exe -m LZX:21 n openage_testarc.cab $testfiles
echo
echo "creating openage_testarc.tar.xz"
tar cJvf openage_testarc.tar.xz $testfiles

# show info
echo
echo "md5sums"
md5sum $testfiles
echo
echo "sizes"
wc -c $testfiles
echo
echo "archive sizes"
wc -c openage_testarc.cab openage_testarc.tar.xz
echo
echo "folder: `pwd`"

# clean up
rm cabarc.exe
rm tmp.exe
rm support.cab
