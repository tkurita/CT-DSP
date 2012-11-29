rem 実行前に C:\sBox\sboxset を実行して、環境をセットアップする。

set product=CT-DSP-flash
set builddir=flash
cl6x -c -q -mv6710 -o2 -eo.o67 -fr %builddir% *.c
cd %builddir%
lnk6x *.o67 -x -o %product%.out -m %product%.map -l sboxdevf.lib ../flashsdramboot.cmd
cd ..