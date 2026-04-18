
100 gosub 9000:rem read data
110 rem parameters (cmd, param)
120 c=1:p=5
130 rem colours
140 r1=0:g1=0:b1=255
150 r2=255:g2=0:b2=0

1000 open 1,16,16
1100 gosub 8000
1200 close 1
1999 end

8000 rem loop 1
8005 c1$=chr$(r1)+chr$(g1)+chr$(b1)+chr$(p)
8007 c2$=chr$(r2)+chr$(g2)+chr$(b2)+chr$(p)

8010 j1 =0: j3 =0: j2 =i2-1: j4 =i4-1
8020 f=0
8100 if j1 < i1 then print#1,chr$(c);chr$(r1(j1));c1$;:j1=j1+1:f=1
8110 if j2 >= 0 then print#1,chr$(c);chr$(r2(j2));c2$;:j2=j2-1:f=1
8120 if j3 < i3 then print#1,chr$(c);chr$(r3(j3));c1$;:j3=j3+1:f=1 
8130 if j4 >= 0 then print#1,chr$(c);chr$(r4(j4));c2$;:j4=j4-1:f=1
8140 if f then 8020
8199 return

9000 rem read array
9010 dim r1(20), r2(20), r3(20), r4(20)
9015 i1=0
9020 read a:if a >= 0 then r1(i1) = a: i1=i1 + 1: goto 9020
9030 i2=0
9040 read a:if a >= 0 then r2(i2) = a: i2=i2 + 1: goto 9040
9050 i3=0
9060 read a:if a >= 0 then r3(i3) = a: i3=i3 + 1: goto 9060
9070 i4=0
9080 read a:if a >= 0 then r4(i4) = a: i4=i4 + 1: goto 9080
9090 return

10000 rem top kbd row
10010 data 65, 0, 8, 1, 9, 2, 10, 3, 11, 4, 12
10020 data 5, 73, 66, 6, 14, 7, 15
10030 data -1
10100 rem 2nd kbd row
10110 data 72, 16, 24, 17, 25, 18, 26, 19, 27
10120 data 20, 28, 21, 75, 68, 22, 30, 23, 31
10130 data -1
10200 rem 3rd kbd row
10210 data 67, 32, 40, 33, 41, 34, 42, 35, 43
10220 data 36, 44, 76, 53, 38, 46, 39, 47
10230 data -1
10300 rem 4th kbd row
10310 data 64, 48, 56, 49, 57, 50, 58, 51, 59
10320 data 52, 60, 69, 54, 62, 55, 63
10330 data -1
