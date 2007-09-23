# Included files
!include Sections.nsh
!include MUI.nsh
!include WinMessages.nsh
!include revision.nsh

Name "Avidemux 2.4 r${REVISION}"

SetCompressor /SOLID lzma
SetCompressorDictSize 96

# Defines
!define INTERNALNAME "Avidemux 2.4"
!define REGKEY "SOFTWARE\${INTERNALNAME}"
!define UNINST_REGKEY "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${INTERNALNAME}"
!define VERSION 2.4.0.${REVISION}
!define COMPANY "Free Software Foundation"
!define URL "http://www.avidemux.org"

!define INSTALL_OPTS_INI "InstallOptions.ini"

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-blue-full.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "PageHeader.bmp"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${REGKEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER Avidemux
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_WELCOMEFINISHPAGE_BITMAP "WelcomeFinishStrip.bmp"
!define MUI_UNICON "..\xpm\adm.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_COMPONENTSPAGE_NODESC

# Variables
Var CreateDesktopIcon
Var CreateStartMenuGroup
Var CreateQuickLaunchIcon
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE License.rtf
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE CheckSelectedUIs
!insertmacro MUI_PAGE_COMPONENTS
Page custom InstallOptionsPage
!define MUI_PAGE_CUSTOMFUNCTION_PRE IsStartMenuRequired
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_PAGE_CUSTOMFUNCTION_PRE ActivateInternalSections
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION RunAvidemux
!define MUI_FINISHPAGE_RUN_TEXT "Run ${INTERNALNAME} now"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Change Log.html"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "View Change Log now"
!define MUI_FINISHPAGE_LINK "Visit the Avidemux Win32 Builds website"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.razorbyte.com.au/avidemux/"
!define MUI_PAGE_CUSTOMFUNCTION_PRE ConfigureFinishPage
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShowFinishPage
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

#Reserve files
ReserveFile "${INSTALL_OPTS_INI}"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

# Installer attributes
OutFile avidemux_2.4_r${REVISION}_win32.exe
InstallDir "$PROGRAMFILES\Avidemux 2.4"
CRCCheck on
XPStyle on
ShowInstDetails nevershow
VIProductVersion 2.4.0.${REVISION}
VIAddVersionKey ProductName Avidemux
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey FileVersion ""
VIAddVersionKey FileDescription ""
VIAddVersionKey LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails nevershow
BrandingText "Packaged by Gruntster"
InstType Standard
InstType Full

# Installer sections
Section "Core files (required)" SecCore
    SectionIn 1 2 RO
    SetOutPath $INSTDIR
    SetOverwrite on
    File "..\..\..\avidemux_2.4_build\Build Info.txt"
    File "..\..\..\avidemux_2.4_build\Change Log.html"
    File ..\..\..\avidemux_2.4_build\zlib1.dll
    File ..\..\..\avidemux_2.4_build\freetype6.dll
    File ..\..\..\avidemux_2.4_build\iconv.dll
    File ..\..\..\avidemux_2.4_build\intl.dll
    File ..\..\..\avidemux_2.4_build\libaften.dll
    File ..\..\..\avidemux_2.4_build\libexpat.dll
    File ..\..\..\avidemux_2.4_build\libfaac.dll
    File ..\..\..\avidemux_2.4_build\libfaad2.dll
    File ..\..\..\avidemux_2.4_build\libfontconfig-1.dll
    File ..\..\..\avidemux_2.4_build\libglib-2.0-0.dll
    File ..\..\..\avidemux_2.4_build\libmp3lame-0.dll
    File ..\..\..\avidemux_2.4_build\libpng13.dll
    File ..\..\..\avidemux_2.4_build\libx264.dll
    File ..\..\..\avidemux_2.4_build\libxml2.dll
    File ..\..\..\avidemux_2.4_build\ogg.dll
    File ..\..\..\avidemux_2.4_build\pthreadGC2.dll
    File ..\..\..\avidemux_2.4_build\SDL.dll
    File ..\..\..\avidemux_2.4_build\vorbis.dll
    File ..\..\..\avidemux_2.4_build\vorbisenc.dll
    File ..\..\..\avidemux_2.4_build\xmltok.dll
    File ..\..\..\avidemux_2.4_build\xvidcore.dll
    WriteRegStr HKLM "${REGKEY}\Components" "Core files (required)" 1
SectionEnd

SectionGroup "User interfaces" SecGrpUI
    Section "Command line" SecUiCli
        SectionIn 1 2
        SetOutPath $INSTDIR
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\avidemux2_cli.exe
        WriteRegStr HKLM "${REGKEY}\Components" "Command line" 1
    SectionEnd

    Section GTK+ SecUiGtk
        SectionIn 1 2
        SetOutPath $INSTDIR\etc
        SetOverwrite on
        File /r ..\..\..\avidemux_2.4_build\etc\*
        SetOutPath $INSTDIR\lib\gtk-2.0
        File /r ..\..\..\avidemux_2.4_build\lib\gtk-2.0\*
        SetOutPath $INSTDIR\share\themes
        File /r ..\..\..\avidemux_2.4_build\share\themes\*
        SetOutPath $INSTDIR
        File ..\..\..\avidemux_2.4_build\avidemux2_gtk.exe
        File ..\..\..\avidemux_2.4_build\gtk2_prefs.exe
        File ..\..\..\avidemux_2.4_build\libatk-1.0-0.dll
        File ..\..\..\avidemux_2.4_build\libcairo-2.dll
        File ..\..\..\avidemux_2.4_build\libgdk_pixbuf-2.0-0.dll
        File ..\..\..\avidemux_2.4_build\libgdk-win32-2.0-0.dll
        File ..\..\..\avidemux_2.4_build\libgmodule-2.0-0.dll
        File ..\..\..\avidemux_2.4_build\libgobject-2.0-0.dll
        File ..\..\..\avidemux_2.4_build\libgthread-2.0-0.dll
        File ..\..\..\avidemux_2.4_build\libgtk-win32-2.0-0.dll
        File ..\..\..\avidemux_2.4_build\libpango-1.0-0.dll
        File ..\..\..\avidemux_2.4_build\libpangocairo-1.0-0.dll
        File ..\..\..\avidemux_2.4_build\libpangowin32-1.0-0.dll
        File ..\..\..\avidemux_2.4_build\jpeg62.dll
        File ..\..\..\avidemux_2.4_build\libtiff3.dll
        WriteRegStr HKLM "${REGKEY}\Components" GTK+ 1
    SectionEnd
    
    Section Qt4 SecUiQt4
        SectionIn 2
        SetOutPath $INSTDIR
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\QtGui4.dll
        File ..\..\..\avidemux_2.4_build\avidemux2_qt4.exe
        File ..\..\..\avidemux_2.4_build\mingwm10.dll
        File ..\..\..\avidemux_2.4_build\QtCore4.dll
        WriteRegStr HKLM "${REGKEY}\Components" Qt4 1
    SectionEnd    
SectionGroupEnd

SectionGroup "Additional languages" SecGrpLang
    Section Catalan SecLangCatalan
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\ca\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\ca\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Catalan 1
    SectionEnd
    
    Section Czech SecLangCzech
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\cs\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\cs\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Czech 1
    SectionEnd
    
    Section French SecLangFrench
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\fr\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\fr\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" French 1
    SectionEnd
    
    Section German SecLangGerman
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\de\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\de\LC_MESSAGES\avidemux.mo        
        WriteRegStr HKLM "${REGKEY}\Components" German 1
    SectionEnd
    
    Section Italian SecLangItalian
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\it\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\it\LC_MESSAGES\avidemux.mo        
        WriteRegStr HKLM "${REGKEY}\Components" Italian 1
    SectionEnd
    
    Section Japanese SecLangJapanese
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\ja\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\ja\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Japanese 1
    SectionEnd
    
    Section Russian SecLangRussian
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\ru\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\ru\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Russian 1
    SectionEnd
    
    Section "Serbian (Cyrillic)" SecLangSerbian
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\sr\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\sr\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Serbian 1
    SectionEnd
    
    Section "Serbian (Latin)" SecLangSerbianLatin
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\sr@Latn\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\sr@Latn\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" SerbianLatin 1
    SectionEnd
    
    Section Spanish SecLangSpanish
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\es\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\es\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Spanish 1
    SectionEnd
    
    Section Turkish SecLangTurkish
        SectionIn 2
        SetOutPath $INSTDIR\share\locale\tr\LC_MESSAGES
        SetOverwrite on
        File ..\..\..\avidemux_2.4_build\share\locale\tr\LC_MESSAGES\avidemux.mo
        WriteRegStr HKLM "${REGKEY}\Components" Turkish 1
    SectionEnd
SectionGroupEnd

Section "-Catalan GTK" SecLangCatalanGtk
    SetOutPath $INSTDIR\lib\locale\ca\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\ca\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" CatalanGtk 1
SectionEnd

Section "-Czech GTK" SecLangCzechGtk
    SetOutPath $INSTDIR\lib\locale\cs\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\cs\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" CzechGtk 1
SectionEnd

Section "-French GTK" SecLangFrenchGtk
    SetOutPath $INSTDIR\lib\locale\fr\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\fr\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" FrenchGtk 1
SectionEnd

Section "-German GTK" SecLangGermanGtk
    SetOutPath $INSTDIR\lib\locale\de\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\de\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" GermanGtk 1
SectionEnd

Section "-Italian GTK" SecLangItalianGtk
    SetOutPath $INSTDIR\lib\locale\it\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\it\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" ItalianGtk 1
SectionEnd

Section "-Japanese GTK" SecLangJapaneseGtk
    SetOutPath $INSTDIR\lib\locale\ja\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\ja\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" JapaneseGtk 1
SectionEnd

Section "-Russian GTK" SecLangRussianGtk
    SetOutPath $INSTDIR\lib\locale\ru\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\ru\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" RussianGtk 1
SectionEnd

Section "-Serbian GTK" SecLangSerbianGtk
    SetOutPath $INSTDIR\lib\locale\sr\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\sr\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" SerbianGtk 1
SectionEnd

Section "-Serbian Latin GTK" SecLangSerbianLatinGtk
    SetOutPath $INSTDIR\lib\locale\sr@Latn\LC_MESSAGES
    SetOverwrite on
    File ..\..\..\avidemux_2.4_build\lib\locale\sr@Latn\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" SerbianLatinGtk 1
SectionEnd

Section "-Spanish GTK" SecLangSpanishGtk
    SetOutPath $INSTDIR\lib\locale\es\LC_MESSAGES
    SetOverwrite on            
    File ..\..\..\avidemux_2.4_build\lib\locale\es\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" SpanishGtk 1
SectionEnd

Section "-Turkish GTK" SecLangTurkishGtk
    SetOutPath $INSTDIR\lib\locale\tr\LC_MESSAGES
    SetOverwrite on            
    File ..\..\..\avidemux_2.4_build\lib\locale\tr\LC_MESSAGES\gtk20.mo
    WriteRegStr HKLM "${REGKEY}\Components" TurkishGtk 1
SectionEnd

Section "AvsProxy" SecAvsProxy
    SectionIn 2
    SetOutPath $INSTDIR
    SetOverwrite on
    File /r ..\..\..\avidemux_2.4_build\avsproxy.exe
    File /r ..\..\..\avidemux_2.4_build\avsproxy_gui.exe
    WriteRegStr HKLM "${REGKEY}\Components" "AvsProxy" 1
SectionEnd

Section "Sample external filter" SecFilter
    SectionIn 2
    SetOutPath $INSTDIR\plugin
    SetOverwrite on
    File /r ..\..\..\avidemux_2.4_build\plugin\*
    WriteRegStr HKLM "${REGKEY}\Components" "Sample external filter" 1
SectionEnd

Section "-Start menu Change Log" SecStartMenuChangeLog
    CreateDirectory $SMPROGRAMS\$StartMenuGroup
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $INSTDIR
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Change Log.lnk" "$INSTDIR\Change Log.html"
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "${REGKEY}\Components" "Start menu" 1
SectionEnd

Section "-Start menu GTK+" SecStartMenuGtk
    CreateDirectory $SMPROGRAMS\$StartMenuGroup
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $INSTDIR
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\${INTERNALNAME} GTK+.lnk" $INSTDIR\avidemux2_gtk.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "${REGKEY}\Components" "Start menu" 1
SectionEnd

Section "-Start menu Qt4" SecStartMenuQt4
    CreateDirectory $SMPROGRAMS\$StartMenuGroup
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $INSTDIR
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\${INTERNALNAME} Qt4.lnk" $INSTDIR\avidemux2_qt4.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "${REGKEY}\Components" "Start menu" 1
SectionEnd

Section "-Start menu AVS Proxy GUI" SecStartMenuAvsProxyGui
    CreateDirectory $SMPROGRAMS\$StartMenuGroup
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $INSTDIR
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\AVS Proxy GUI.lnk" "$INSTDIR\avsproxy_gui.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "${REGKEY}\Components" "Start menu" 1
SectionEnd

Section "-Quick Launch GTK+" SecQuickLaunchGtk
    SetOutPath $INSTDIR
    CreateShortcut "$QUICKLAUNCH\${INTERNALNAME} GTK+.lnk" $INSTDIR\avidemux2_gtk.exe
    WriteRegStr HKLM "${REGKEY}\Components" "Quick Launch" 1
SectionEnd

Section "-Quick Launch Qt4" SecQuickLaunchQt4
    SetOutPath $INSTDIR
    CreateShortcut "$QUICKLAUNCH\${INTERNALNAME} Qt4.lnk" $INSTDIR\avidemux2_qt4.exe
    WriteRegStr HKLM "${REGKEY}\Components" "Quick Launch" 1
SectionEnd

Section "-Desktop GTK+" SecDesktopGtk
    SetOutPath $INSTDIR
    CreateShortcut "$DESKTOP\${INTERNALNAME} GTK+.lnk" $INSTDIR\avidemux2_gtk.exe
    WriteRegStr HKLM "${REGKEY}\Components" "Desktop" 1
SectionEnd

Section "-Desktop Qt4" SecDesktopQt4
    SetOutPath $INSTDIR
    CreateShortcut "$DESKTOP\${INTERNALNAME} Qt4.lnk" $INSTDIR\avidemux2_qt4.exe
    WriteRegStr HKLM "${REGKEY}\Components" "Desktop" 1
SectionEnd

Section -post SecUninstaller
    SectionIn 1 2
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    WriteRegStr HKLM "${UNINST_REGKEY}" DisplayName "${INTERNALNAME}"
    WriteRegStr HKLM "${UNINST_REGKEY}" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "${UNINST_REGKEY}" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "${UNINST_REGKEY}" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "${UNINST_REGKEY}" NoModify 1
    WriteRegDWORD HKLM "${UNINST_REGKEY}" NoRepair 1
SectionEnd

# Macro for selecting sections based on registry setting
!macro SELECT_SECTION SECTION_NAME SECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${SECTION_ID}
    !insertmacro SelectSection "${SECTION_ID}"
    GoTo done${SECTION_ID}
next${SECTION_ID}:
    !insertmacro UnselectSection "${SECTION_ID}"
done${SECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o "un.Sample external filter" UnSecFilter
    RmDir /r /REBOOTOK $INSTDIR\plugin
    DeleteRegValue HKLM "${REGKEY}\Components" "Sample external filter"
SectionEnd

Section /o "un.AvsProxy" UnSecAvsProxy
    Delete /REBOOTOK $INSTDIR\avsproxy.exe
    Delete /REBOOTOK $INSTDIR\avsproxy_gui.exe
    DeleteRegValue HKLM "${REGKEY}\Components" "AvsProxy"
SectionEnd

Section /o un.TurkishGtk UnSecLangTurkishGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\tr
    DeleteRegValue HKLM "${REGKEY}\Components" TurkishGtk
SectionEnd

Section /o un.SpanishGtk UnSecLangSpanishGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\es
    DeleteRegValue HKLM "${REGKEY}\Components" SpanishGtk
SectionEnd

Section /o un.SerbianGtk UnSecLangSerbianGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\sr
    DeleteRegValue HKLM "${REGKEY}\Components" SerbianGtk
SectionEnd

Section /o un.SerbianLatinGtk UnSecLangSerbianLatinGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\sr@Latn
    DeleteRegValue HKLM "${REGKEY}\Components" SerbianLatinGtk
SectionEnd

Section /o un.RussianGtk UnSecLangRussianGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\ru
    DeleteRegValue HKLM "${REGKEY}\Components" RussianGtk
SectionEnd

Section /o un.JapaneseGtk UnSecLangJapaneseGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\ja
    DeleteRegValue HKLM "${REGKEY}\Components" JapaneseGtk
SectionEnd

Section /o un.ItalianGtk UnSecLangItalianGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\it
    DeleteRegValue HKLM "${REGKEY}\Components" ItalianGtk
SectionEnd

Section /o un.GermanGtk UnSecLangGermanGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\de
    DeleteRegValue HKLM "${REGKEY}\Components" GermanGtk
SectionEnd

Section /o un.FrenchGtk UnSecLangFrenchGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\fr
    DeleteRegValue HKLM "${REGKEY}\Components" FrenchGtk
SectionEnd

Section /o un.CzechGtk UnSecLangCzechGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\cs
    DeleteRegValue HKLM "${REGKEY}\Components" CzechGtk
SectionEnd

Section /o un.CatalanGtk UnSecLangCatalanGtk
    RmDir /r /REBOOTOK $INSTDIR\lib\locale\ca
    DeleteRegValue HKLM "${REGKEY}\Components" CatalanGtk
SectionEnd

Section /o un.Spanish UnSecLangSpanish
    RmDir /r /REBOOTOK $INSTDIR\share\locale\es
    DeleteRegValue HKLM "${REGKEY}\Components" Spanish
SectionEnd

Section /o un.Turkish UnSecLangTurkish
    RmDir /r /REBOOTOK $INSTDIR\share\locale\tr
    DeleteRegValue HKLM "${REGKEY}\Components" Turkish
SectionEnd

Section /o un.Serbian UnSecLangSerbian
    RmDir /r /REBOOTOK $INSTDIR\share\locale\sr
    DeleteRegValue HKLM "${REGKEY}\Components" Serbian
SectionEnd

Section /o un.SerbianLatin UnSecLangSerbianLatin
    RmDir /r /REBOOTOK $INSTDIR\share\locale\sr@Latn
    DeleteRegValue HKLM "${REGKEY}\Components" SerbianLatin
SectionEnd

Section /o un.Russian UnSecLangRussian
    RmDir /r /REBOOTOK $INSTDIR\share\locale\ru
    DeleteRegValue HKLM "${REGKEY}\Components" Russian
SectionEnd

Section /o un.Japanese UnSecLangJapanese
    RmDir /r /REBOOTOK $INSTDIR\share\locale\ja
    DeleteRegValue HKLM "${REGKEY}\Components" Japanese
SectionEnd

Section /o un.Italian UnSecLangItalian
    RmDir /r /REBOOTOK $INSTDIR\share\locale\it
    DeleteRegValue HKLM "${REGKEY}\Components" Italian
SectionEnd

Section /o un.German UnSecLangGerman
    RmDir /r /REBOOTOK $INSTDIR\share\locale\de
    DeleteRegValue HKLM "${REGKEY}\Components" German
SectionEnd

Section /o un.French UnSecLangFrench
    RmDir /r /REBOOTOK $INSTDIR\share\locale\fr
    DeleteRegValue HKLM "${REGKEY}\Components" French
SectionEnd

Section /o un.Czech UnSecLangCzech
    RmDir /r /REBOOTOK $INSTDIR\share\locale\cs
    DeleteRegValue HKLM "${REGKEY}\Components" Czech
SectionEnd

Section /o un.Catalan UnSecLangCatalan
    RmDir /r /REBOOTOK $INSTDIR\share\locale\ca
    DeleteRegValue HKLM "${REGKEY}\Components" Catalan
SectionEnd

Section /o un.GTK+ UnSecUiGtk
    Delete /REBOOTOK $INSTDIR\jpeg62.dll
    Delete /REBOOTOK $INSTDIR\libtiff3.dll
    Delete /REBOOTOK $INSTDIR\libpangowin32-1.0-0.dll
    Delete /REBOOTOK $INSTDIR\libpangocairo-1.0-0.dll
    Delete /REBOOTOK $INSTDIR\libpango-1.0-0.dll
    Delete /REBOOTOK $INSTDIR\libgtk-win32-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libgthread-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libgobject-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libgmodule-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libgdk-win32-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libgdk_pixbuf-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libcairo-2.dll
    Delete /REBOOTOK $INSTDIR\libatk-1.0-0.dll
    Delete /REBOOTOK $INSTDIR\gtk2_prefs.exe
    Delete /REBOOTOK $INSTDIR\avidemux2_gtk.exe
    RmDir /r /REBOOTOK $INSTDIR\share\themes
    RmDir /r /REBOOTOK $INSTDIR\lib\gtk-2.0
    RmDir /r /REBOOTOK $INSTDIR\etc
    DeleteRegValue HKLM "${REGKEY}\Components" GTK+
SectionEnd

Section /o un.Qt4 UnSecUiQt4
    Delete /REBOOTOK $INSTDIR\QtCore4.dll
    Delete /REBOOTOK $INSTDIR\mingwm10.dll
    Delete /REBOOTOK $INSTDIR\avidemux2_qt4.exe
    Delete /REBOOTOK $INSTDIR\QtGui4.dll
    DeleteRegValue HKLM "${REGKEY}\Components" Qt4
SectionEnd

Section /o "un.Command line" UnSecUiCli
    Delete /REBOOTOK $INSTDIR\avidemux2_cli.exe
    DeleteRegValue HKLM "${REGKEY}\Components" "Command line"
SectionEnd

Section /o "un.Core files (required)" UnSecCore
    Delete /REBOOTOK $INSTDIR\xvidcore.dll
    Delete /REBOOTOK $INSTDIR\xmltok.dll
    Delete /REBOOTOK $INSTDIR\vorbisenc.dll
    Delete /REBOOTOK $INSTDIR\vorbis.dll
    Delete /REBOOTOK $INSTDIR\SDL.dll
    Delete /REBOOTOK $INSTDIR\pthreadGC2.dll
    Delete /REBOOTOK $INSTDIR\ogg.dll
    Delete /REBOOTOK $INSTDIR\libxml2.dll
    Delete /REBOOTOK $INSTDIR\libx264.dll
    Delete /REBOOTOK $INSTDIR\libpng13.dll
    Delete /REBOOTOK $INSTDIR\libmp3lame-0.dll
    Delete /REBOOTOK $INSTDIR\libglib-2.0-0.dll
    Delete /REBOOTOK $INSTDIR\libfontconfig-1.dll
    Delete /REBOOTOK $INSTDIR\libfaad2.dll
    Delete /REBOOTOK $INSTDIR\libfaac.dll
    Delete /REBOOTOK $INSTDIR\libexpat.dll
    Delete /REBOOTOK $INSTDIR\libaften.dll
    Delete /REBOOTOK $INSTDIR\intl.dll
    Delete /REBOOTOK $INSTDIR\iconv.dll
    Delete /REBOOTOK $INSTDIR\freetype6.dll
    Delete /REBOOTOK $INSTDIR\zlib1.dll
    Delete /REBOOTOK "$INSTDIR\Build Info.txt"
    Delete /REBOOTOK "$INSTDIR\Change Log.html"
    Delete /REBOOTOK "$INSTDIR\stdout.txt"
    Delete /REBOOTOK "$INSTDIR\stderr.txt"
    DeleteRegValue HKLM "${REGKEY}\Components" Avidemux
    
    RmDir /REBOOTOK $INSTDIR\share\gettext
    RmDir /REBOOTOK $INSTDIR\share
SectionEnd

Section /o "un.Start menu" UnSecStartMenu
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Change Log.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\${INTERNALNAME} GTK+.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\${INTERNALNAME} Qt4.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\AVS Proxy GUI.lnk"
SectionEnd

Section /o "un.Quick Launch" UnSecQuickLaunch
    Delete /REBOOTOK "$QUICKLAUNCH\${INTERNALNAME} GTK+.lnk"
    Delete /REBOOTOK "$QUICKLAUNCH\${INTERNALNAME} Qt4.lnk"
SectionEnd

Section /o "un.Desktop" UnSecDesktop
    Delete /REBOOTOK "$DESKTOP\${INTERNALNAME} GTK+.lnk"
    Delete /REBOOTOK "$DESKTOP\${INTERNALNAME} Qt4.lnk"
SectionEnd

Section un.post UnSecUninstaller
    RmDir /REBOOTOK $INSTDIR\lib\locale
    RmDir /REBOOTOK $INSTDIR\lib
    RmDir /REBOOTOK $INSTDIR\share\locale
    RmDir /REBOOTOK $INSTDIR\share
    
    DeleteRegKey HKLM "${UNINST_REGKEY}"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
    Push $R0
    StrCpy $R0 $StartMenuGroup 1
    StrCmp $R0 ">" no_smgroup
no_smgroup:
    Pop $R0
SectionEnd

# Installer functions
Function .onInit
    ReadRegStr $R0  HKLM "${UNINST_REGKEY}" "UninstallString"
    StrCmp $R0 "" startInstall
 
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "${INTERNALNAME} has already been installed. $\n$\nDo you want to remove \
      the previous version before installing $(^Name)?" IDNO startInstall
  
    # Run the uninstaller
    ClearErrors
    ExecWait '$R0 _?=$INSTDIR' ; Do not copy the uninstaller to a temp file

startInstall:
    InitPluginsDir
    
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${INSTALL_OPTS_INI}"

    # Make sure a User Interface is selected in previous install preferences
    ReadRegStr $0 HKLM "${REGKEY}\Components" "Command line"
    StrCmp $0 1 populate
    ReadRegStr $0 HKLM "${REGKEY}\Components" "GTK+"
    StrCmp $0 1 populate
    ReadRegStr $0 HKLM "${REGKEY}\Components" "Qt4"
    StrCmp $0 1 populate

    # No UI exists, so go with defaults
    Goto end

    #Select sections based on last install
populate:
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_SECTION "Command line" ${SecUiCli}
    !insertmacro SELECT_SECTION Qt4 ${SecUiQt4}
    !insertmacro SELECT_SECTION GTK+ ${SecUiGtk}
    !insertmacro SELECT_SECTION Catalan ${SecLangCatalan}
    !insertmacro SELECT_SECTION Czech ${SecLangCzech}
    !insertmacro SELECT_SECTION French ${SecLangFrench}
    !insertmacro SELECT_SECTION German ${SecLangGerman}
    !insertmacro SELECT_SECTION Italian ${SecLangItalian}
    !insertmacro SELECT_SECTION Japanese ${SecLangJapanese}
    !insertmacro SELECT_SECTION Russian ${SecLangRussian}
    !insertmacro SELECT_SECTION Serbian ${SecLangSerbian}
    !insertmacro SELECT_SECTION "Serbian (Latin)" ${SecLangSerbianLatin}
    !insertmacro SELECT_SECTION Spanish ${SecLangSpanish}
    !insertmacro SELECT_SECTION Turkish ${SecLangTurkish}
    !insertmacro SELECT_SECTION "Sample external filter" ${SecFilter}
    !insertmacro SELECT_SECTION "AvsProxy" ${SecAvsProxy}

    #startMenu:
    ReadRegStr $0 HKLM "${REGKEY}\Components" "Start menu"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTALL_OPTS_INI}" "Field 3" "State" $0
    
    #desktop:
    ReadRegStr $0 HKLM "${REGKEY}\Components" "Desktop"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTALL_OPTS_INI}" "Field 2" "State" $0
    
    #quickLaunch:
    ReadRegStr $0 HKLM "${REGKEY}\Components" "Quick Launch"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTALL_OPTS_INI}" "Field 4" "State" $0
    
end:
FunctionEnd

Function CheckSelectedUIs
    SectionGetFlags ${SecGrpUI} $0
    
    IntOp $1 ${SF_SELECTED} | ${SF_PSELECTED}    
    IntOp $0 $0 & $1
    
    StrCmp $0 0 0 +3
        MessageBox MB_OK|MB_ICONSTOP "At least one User Interface must be selected."
        Abort
FunctionEnd

LangString INSTALL_OPTS_PAGE_TITLE ${LANG_ENGLISH} "Choose Install Options"
LangString INSTALL_OPTS_PAGE_SUBTITLE ${LANG_ENGLISH} "Choose where to install Avidemux icons."

Function InstallOptionsPage
    Call IsInstallOptionsRequired

    !insertmacro MUI_HEADER_TEXT "$(INSTALL_OPTS_PAGE_TITLE)" "$(INSTALL_OPTS_PAGE_SUBTITLE)"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${INSTALL_OPTS_INI}"
    
    Call ReadInstallOptions
FunctionEnd

Function IsInstallOptionsRequired
    SectionGetFlags ${SecUiGtk} $0
    SectionGetFlags ${SecUiQt4} $1
    
    IntOp $0 $0 & ${SF_SELECTED}
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $2 $0 | $1
    
    StrCmp $2 ${SF_SELECTED} end
        StrCpy $CreateDesktopIcon 0
        StrCpy $CreateStartMenuGroup 0
        StrCpy $CreateQuickLaunchIcon 0
        Abort

end:
FunctionEnd

Function ReadInstallOptions
    !insertmacro MUI_INSTALLOPTIONS_READ $CreateDesktopIcon "${INSTALL_OPTS_INI}" "Field 2" "State"
    !insertmacro MUI_INSTALLOPTIONS_READ $CreateStartMenuGroup "${INSTALL_OPTS_INI}" "Field 3" "State"
    !insertmacro MUI_INSTALLOPTIONS_READ $CreateQuickLaunchIcon "${INSTALL_OPTS_INI}" "Field 4" "State"
FunctionEnd

Function IsStartMenuRequired
    StrCmp $CreateStartMenuGroup 1 +2
        Abort    
FunctionEnd

Function ActivateInternalSections
    #AVS Proxy GUI shortcut:
    SectionGetFlags ${SecAvsProxy} $0
    IntOp $0 $0 & ${SF_SELECTED}
    IntOp $0 $0 & $CreateStartMenuGroup
    SectionSetFlags ${SecStartMenuAvsProxyGui} $0
    
    #Change Log shortcut:
    SectionSetFlags ${SecStartMenuChangeLog} $CreateStartMenuGroup

    #GTK shortcuts:
    SectionGetFlags ${SecUiGtk} $0
    IntOp $0 $0 & ${SF_SELECTED}

    IntOp $1 $0 & $CreateDesktopIcon
    SectionSetFlags ${SecDesktopGtk} $1
    
    IntOp $1 $0 & $CreateQuickLaunchIcon
    SectionSetFlags ${SecQuickLaunchGtk} $1
    
    IntOp $1 $0 & $CreateStartMenuGroup
    SectionSetFlags ${SecStartMenuGtk} $1
    
    #GTK languages:
    SectionGetFlags ${SecLangCatalan} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangCatalanGtk} $1
    
    SectionGetFlags ${SecLangCzech} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangCzechGtk} $1
    
    SectionGetFlags ${SecLangFrench} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangFrenchGtk} $1
    
    SectionGetFlags ${SecLangGerman} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangGermanGtk} $1
    
    SectionGetFlags ${SecLangItalian} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangItalianGtk} $1
    
    SectionGetFlags ${SecLangJapanese} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangJapaneseGtk} $1
    
    SectionGetFlags ${SecLangRussian} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangRussianGtk} $1

    SectionGetFlags ${SecLangSerbian} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangSerbianGtk} $1
    
    SectionGetFlags ${SecLangSerbianLatin} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangSerbianLatinGtk} $1

    SectionGetFlags ${SecLangSpanish} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangSpanishGtk} $1
    
    SectionGetFlags ${SecLangTurkish} $1
    IntOp $1 $1 & ${SF_SELECTED}
    IntOp $1 $0 & $1
    SectionSetFlags ${SecLangTurkishGtk} $1

    #Qt4 shortcuts:
    SectionGetFlags ${SecUiQt4} $0
    IntOp $0 $0 & ${SF_SELECTED}
    
    IntOp $1 $0 & $CreateDesktopIcon
    SectionSetFlags ${SecDesktopQt4} $1
    
    IntOp $1 $0 & $CreateQuickLaunchIcon
    SectionSetFlags ${SecQuickLaunchQt4} $1
    
    IntOp $1 $0 & $CreateStartMenuGroup
    SectionSetFlags ${SecStartMenuQt4} $1
FunctionEnd

Function ConfigureFinishPage
    SectionGetFlags ${SecUiGtk} $0
    IntOp $0 $0 & ${SF_SELECTED}
    StrCmp $0 ${SF_SELECTED} end
    
    SectionGetFlags ${SecUiQt4} $0
    IntOp $0 $0 & ${SF_SELECTED}
    StrCmp $0 ${SF_SELECTED} end

    DeleteINISec "$PLUGINSDIR\ioSpecial.ini" "Field 4"
        
end:
FunctionEnd

Function OnShowFinishPage
    # Make hyperlink underlined to keep it consistent with web browsers
    GetDlgItem $0 $MUI_HWND 1205
    CreateFont $1 "$(^Font)" 9 500 /UNDERLINE
    SendMessage $0 ${WM_SETFONT} $1 0    
FunctionEnd

Function RunAvidemux
    SetOutPath $INSTDIR

#GTK:
    SectionGetFlags ${SecUiGtk} $0
    IntOp $0 $0 & ${SF_SELECTED}
    
    StrCmp $0 ${SF_SELECTED} 0 Qt4
        Exec "$INSTDIR\avidemux2_gtk.exe"

    Goto end
    
Qt4:
    SectionGetFlags ${SecUiQt4} $0
    IntOp $0 $0 & ${SF_SELECTED}
    
    StrCmp $0 ${SF_SELECTED} 0 End
        Exec "$INSTDIR\avidemux2_qt4.exe"    
    
end:
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_SECTION "Core files (required)" ${UnSecCore}
    !insertmacro SELECT_SECTION "Command line" ${UnSecUiCli}
    !insertmacro SELECT_SECTION Qt4 ${UnSecUiQt4}
    !insertmacro SELECT_SECTION GTK+ ${UnSecUiGtk}
    !insertmacro SELECT_SECTION Catalan ${UnSecLangCatalan}
    !insertmacro SELECT_SECTION Czech ${UnSecLangCzech}
    !insertmacro SELECT_SECTION French ${UnSecLangFrench}
    !insertmacro SELECT_SECTION German ${UnSecLangGerman}
    !insertmacro SELECT_SECTION Italian ${UnSecLangItalian}
    !insertmacro SELECT_SECTION Japanese ${UnSecLangJapanese}
    !insertmacro SELECT_SECTION Russian ${UnSecLangRussian}
    !insertmacro SELECT_SECTION Serbian ${UnSecLangSerbian}
    !insertmacro SELECT_SECTION SerbianLatin ${UnSecLangSerbianLatin}
    !insertmacro SELECT_SECTION Spanish ${UnSecLangSpanish}
    !insertmacro SELECT_SECTION Turkish ${UnSecLangTurkish}
    !insertmacro SELECT_SECTION CatalanGtk ${UnSecLangCatalanGtk}
    !insertmacro SELECT_SECTION CzechGtk ${UnSecLangCzechGtk}
    !insertmacro SELECT_SECTION FrenchGtk ${UnSecLangFrenchGtk}
    !insertmacro SELECT_SECTION GermanGtk ${UnSecLangGermanGtk}
    !insertmacro SELECT_SECTION ItalianGtk ${UnSecLangItalianGtk}
    !insertmacro SELECT_SECTION JapaneseGtk ${UnSecLangJapaneseGtk}
    !insertmacro SELECT_SECTION RussianGtk ${UnSecLangRussianGtk}
    !insertmacro SELECT_SECTION SerbianGtk ${UnSecLangSerbianGtk}
    !insertmacro SELECT_SECTION SerbianLatinGtk ${UnSecLangSerbianLatinGtk}
    !insertmacro SELECT_SECTION SpanishGtk ${UnSecLangSpanishGtk}
    !insertmacro SELECT_SECTION TurkishGtk ${UnSecLangTurkishGtk}
    !insertmacro SELECT_SECTION "AvsProxy" ${UnSecAvsProxy}
    !insertmacro SELECT_SECTION "Sample external filter" ${UnSecFilter}
    !insertmacro SELECT_SECTION "Start menu" ${UnSecStartMenu}
    !insertmacro SELECT_SECTION "Quick Launch" ${UnSecQuickLaunch}
    !insertmacro SELECT_SECTION "Desktop" ${UnSecDesktop}
FunctionEnd