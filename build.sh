#!/bin/sh
echo "g(i,x,t,o){return((3&x&(i*((3&i>>16?\"BY}6YB6%\":\"Qj}6jQ6%\")[t%8]+51)>>o))<<4);};main(i,n,s){for(i=0;;i++)putchar(g(i,1,n=i>>14,12)+g(i,s=i>>17,n^i>>13,10)+g(i,s/3,n+((i>>11)%3),10)+g(i,s/5,8+n-((i>>10)%3),9));}"|gcc -xc -

#echo "kb1_generator(int t) { return ((t / 2 * (15 & (0x234568a0 >> (t >> 8 & 28)))) | t / 2 >> (t >> 11) ^ t >> 12) + (t / 16 & t & 24); } main(i){for(i=0;;i++)putchar(kb1_generator(i));}" | gcc -xc -

#./a.out | play.exe -b 8 -c 1 -t u8 -r 8000 -
