ACLOCAL_AMFLAGS = -I m4

SUBDIRS = 	po \
		src \
		files

AM_CFLAGS = @CFLAGS@ -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\"
export AM_CFLAGS

EXTRA_DIST = config.rpath

cppcheck :
	$(MAKE) cppcheck -C src &> cppcheck.log
