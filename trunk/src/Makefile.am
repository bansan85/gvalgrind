DEFAULT_INCLUDES	=	-I..

bin_PROGRAMS		= gvalgrind
gvalgrind_SOURCES	= main.c \
			  texte.c \
			  options.c \
			  fonctions.c

gvalgrind_LDADD		= @XML_LIBS@ @GTK_LIBS@
gvalgrind_CPPFLAGS	= @XML_CPPFLAGS@ @GTK_CFLAGS@
