%define name	avidemux2
%define ver	2.0	
%define rel	7
%define prefix	/usr

Summary:	A graphical multiplex/demultiplex tool using GTK.
Name:		%name
Version:	%ver
Release:	%rel
License:	GPL
Group:		Applications/Multimedia
Packager:	Kubus (based on ::cipherfunk industries::)

Source:		%{name}-%{version}.tgz
URL:		http://fixounet.free.fr/avidemux/
BuildRoot:	%{_tmppath}/%{name}-%{version}-root
BuildRequires:	gcc >= 2.96
BuildRequires:	glib-devel
BuildRequires:	gtk+-devel >= 2.0.0
BuildRequires:  nasm >= 0.98.22
Requires:	glib
Requires:	gtk+ >= 2.0.0

%description
Avidemux is a graphical tool to edit AVI. It allows you to multiplex and
demultiplex audio to/from video.

It is able to cut video, import BMP, MJPEG and MPEG video, and encode them.
You can also process video with included filters. It requires a DivX 
compatible encoder and the Gimp Toolkit (GTK) libraries.

%prep
%setup

%build

LC_MESSAGES=C ; export LC_MESSAGES

if [ ! -f configure ]; then
			CC=gcc CXX=g++ \
			CFLAGS="$RPM_OPT_FLAGS" \
			CXXFLAGS="$RPM_OPT_FLAGS" \
			./autogen.sh \
			--prefix=%{_prefix} \
			--datadir=%{_datadir}/%{name} \
			--sysconfdir=%{_sysconfdir}/%{name} \
			--host=%{_target_platform} \
			--disable-warnings
else
			CC=gcc CXX=g++ \
			CFLAGS="$RPM_OPT_FLAGS" \
			CXXFLAGS="$RPM_OPT_FLAGS" \
			./configure \
			--prefix=%{_prefix} \
			--datadir=%{_datadir}/%{name} \
			--sysconfdir=%{_sysconfdir}/%{name} \
			--host=%{_target_platform} \
			--disable-warnings

fi

# if RPM_BUILD_NCPUS unset, set it
if [ -z "$RPM_BUILD_NCPUS" ] ; then
  if [ -x /usr/bin/getconf ] ; then
    RPM_BUILD_NCPUS=$(/usr/bin/getconf _NPROCESSORS_ONLN)
    if [ $RPM_BUILD_NCPUS -eq 0 ]; then
      RPM_BUILD_NCPUS=1
    fi
  else
    RPM_BUILD_NCPUS=1
  fi
fi

#make -j$RPM_BUILD_NCPUS
make

%install
rm -rf %{buildroot}
%makeinstall

%clean
rm -rf %{buildroot}

%files
%defattr(755, root, root)

%{_bindir}/%{name}

%changelog
* Tue Mar 04 2003 Jacek Pliszka <pliszka@phyun0.ucr.edu> 0.9-rc1
- update to 0.9-rc1
- .spec adjusted for rpm building from tar.gz 

* Mon Feb 10 2003 Paul Drain <pd@cipherfunk.org> 0.9pre32-2
- added GUI cleanups
- remove debugging flag from default ffmpeg build

* Sat Feb 08 2003 Paul Drain <pd@cipherfunk.org> 0.9pre32-1
- update to pre32

* Fri Jan 31 2003 Paul Drain <pd@cipherfunk.org> 0.9pre31-1
- snapshot build of 31-5 code (never officially packaged)

* Mon Jan 27 2003 Paul Drain <pd@cipherfunk.org> 0.9pre30-2
- added mean's 'recommended' pre30 patches

* Sun Jan 19 2003 Paul Drain <pd@cipherfunk.org> 0.9pre30-1
- update to pre30
- removed pending patches (merged in mainline)

* Mon Jan 13 2003 Paul Drain <pd@cipherfunk.org> 0.9pre28-4
- updated current snapshot code (against 29-3.tar.gz)
- added GUI cleanups patch

* Tue Jan 07 2003 Paul Drain <pd@cipherfunk.org> 0.9pre28-3
- updated current snapshot code (against 29-2.tar.gz)
- added nasm to the build depenancies

* Tue Jan 07 2003 Paul Drain <pd@cipherfunk.org> 0.9pre28-2
- updated current snapshot code (against 29-1.tar.gz)

* Sun Jan 05 2003 Paul Drain <pd@cipherfunk.org> 0.9pre28-1
- update to pre28
- removed freetype and alignment bugfixes (merged in mainline)

* Mon Dec 30 2002 Paul Drain <pd@cipherfunk.org> 0.9pre26-2
- added freetype 2.1.3 build patch
- added MP3 and PCM alignment patch

* Sat Dec 28 2002 Paul Drain <pd@cipherfunk.org> 0.9pre26-1
- update to pre26
- removed ffmpeg bugfixes (merged in mainline)

* Fri Dec 20 2002 Paul Drain <pd@cipherfunk.org> 0.9pre24-5
- updated current snapshot code (against 25-3.tar.gz)

* Thu Dec 12 2002 Paul Drain <pd@cipherfunk.org> 0.9pre24-4
- added current snapshot code (against 25-1.tar.gz)
- removed extended mp3 header patch (merged in mainline)
- removed the __without_divx flag in this specfile
  (no longer required)
  
* Wed Dec 11 2002 Paul Drain <pd@cipherfunk.org> 0.9pre24-3
- added H263 based XViD playback patch to fix padding on badly
  encoded XViD or emulated DivX films
- added various small compile time bugfixes from ffmpeg CVS
- added RPM version tag

* Tue Dec 10 2002 Paul Drain <pd@cipherfunk.org> 0.9pre24-2
- added extended mp3 header patch (exports header info to CBR and VBR MP3s)
- added stablization filter patch
- rebuilt against LAME 3.93.1

* Mon Dec 09 2002 Paul Drain <pd@cipherfunk.org> 0.9pre24
- update to pre24

* Fri Dec 06 2002 Paul Drain <pd@cipherfunk.org> 0.9pre22-6
- update pre23 snapshot patch (patch vs. 23-9.tar.gz)
- update XViD defaults (keyframe lengths)

* Tue Dec 03 2002 Paul Drain <pd@cipherfunk.org> 0.9pre22-5
- update pre23 snapshot patch (patch vs. 23-6.tar.gz)

* Fri Nov 29 2002 Paul Drain <pd@cipherfunk.org> 0.9pre22-4
- update pre23 snapshot patch (patch vs. 23-4.tar.gz)
- fixed fastchroma bug

* Tue Nov 25 2002 Paul Drain <pd@cipherfunk.org> 0.9pre22-2
- add pre23 snapshot patch (patch vs. 23-1.tar.gz)

* Thu Nov 21 2002 Paul Drain <pd@cipherfunk.org> 0.9pre22-1
- update to pre22

* Mon Oct 07 2002 Paul Drain <pd@cipherfunk.org>
- update to pre18
- removed 0.9pre16 patches, merged in mainline

* Fri Oct 04 2002 Paul Drain <pd@cipherfunk.org>
- automated rebuild under gcc 3.2

* Mon Sep 23 2002 Paul Drain <pd@cipherfunk.org>
- update to pre16
- removed 0.9pre14 patches, merged in mainline
- added patch for unusual GTK+ paths in automake

* Sun Sep 22 2002 Paul Drain <pd@cipherfunk.org>
- add patch to fix 2pass CBR encoding bug
- add patch to fix ffmpeg single pass VBR demuxing bug

* Sat Sep 14 2002 Paul Drain <pd@cipherfunk.org>
- update to pre14

* Fri Aug 30 2002 Paul Drain <pd@cipherfunk.org>
- update to pre13

* Sun Aug 25 2002 Paul Drain <pd@cipherfunk.org>
- added XviD encoder fixes from current avidemux snapshot

* Thu Aug 15 2002 Paul Drain <pd@cipherfunk.org>
- update to pre12
- removed patch0, fixed in mainline

* Wed Aug 14 2002 Paul Drain <pd@cipherfunk.org>
- mark divx4linux as an i386 dependant library
- remove %attr and make the defaults 0755 (no need to write to the binaries)

* Mon Aug 12 2002 Paul Drain <pd@cipherfunk.org>
- update to pre11
- added cosmetic patch for spelling error :) 

* Mon Aug 05 2002 Paul Drain <pd@cipherfunk.org>
- Completely rebuilt the specfile

