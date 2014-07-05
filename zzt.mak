#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc +BccDos.cfg 
TLINK   = TLink
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -Lc:\dev\BC5\LIB
IDE_BFLAGS = 
LLATDOS_zztdexe =  -c -Tde
RLATDOS_zztdexe = 
BLATDOS_zztdexe = 
CNIEAT_zztdexe = -Ic:\dev\BC5\INCLUDE -D
LNIEAT_zztdexe = -x
LEAT_zztdexe = $(LLATDOS_zztdexe)
REAT_zztdexe = $(RLATDOS_zztdexe)
BEAT_zztdexe = $(BLATDOS_zztdexe)

#
# Dependency List
#
Dep_zzt = \
   zzt.exe

zzt : BccDos.cfg $(Dep_zzt)
  echo MakeNode 

Dep_zztdexe = \
   ope.obj\
   sm_adlib.obj\
   sm.obj\
   timerisr.obj\
   mm.obj\
   ge.obj\
   bm.obj\
   zzt.obj

zzt.exe : $(Dep_zztdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_zztdexe) $(LNIEAT_zztdexe) +
c:\dev\BC5\LIB\c0l.obj+
ope.obj+
sm_adlib.obj+
sm.obj+
timerisr.obj+
mm.obj+
ge.obj+
bm.obj+
zzt.obj
$<,$*
c:\dev\BC5\LIB\bidsl.lib+
c:\dev\BC5\LIB\emu.lib+
c:\dev\BC5\LIB\mathl.lib+
c:\dev\BC5\LIB\cl.lib

|

ope.obj :  ope.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ ope.cpp
|

sm_adlib.obj :  sm_adlib.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ sm_adlib.cpp
|

sm.obj :  sm.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ sm.cpp
|

timerisr.obj :  timerisr.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ timerisr.cpp
|

mm.obj :  mm.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ mm.cpp
|

ge.obj :  ge.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ ge.cpp
|

bm.obj :  bm.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ bm.cpp
|

zzt.obj :  zzt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_zztdexe) $(CNIEAT_zztdexe) -o$@ zzt.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=zzt.csm
-ml
-f
-1-
-H-
-Z
-O
-Oe
-Ol
-Ob
-O-W
| $@


