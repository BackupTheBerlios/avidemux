####### kdevelop will overwrite this part!!! (begin)##########

#SUBDIRS = m4  avidemux

#EXTRA_DIST = admin/config.rpath  Avidemux.kdevprj admin AUTHORS COPYING ChangeLog INSTALL README TODO History buildLavCodec.sh

####### kdevelop will overwrite this part!!! (end)############
# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
SUBDIRS =  po m4  adm_lavcodec avidemux  addons 
EXTRA_DIST = admin/config.rpath   admin AUTHORS COPYING ChangeLog INSTALL README TODO History  configure.in.in Makefile.dist

AUTOMAKE_OPTIONS = foreign

$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;

$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages

dist-hook:
	cd $(top_distdir) && perl admin/am_edit -padmin
	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs



ACLOCAL_AMFLAGS = -I m4
