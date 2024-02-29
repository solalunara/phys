export
all:
	+$(MAKE) -C entities -f zENTITIES.mak
	+$(MAKE) -C maths -f zMATHS.mak
	+$(MAKE) -C physics -f zPHYSICS.mak
	+$(MAKE) -C render -f zRENDER.mak
	+$(MAKE) -C UI -f zUI.mak
