# Microsoft Developer Studio Project File - Name="mp3checker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mp3checker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mp3checker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp3checker.mak" CFG="mp3checker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp3checker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mp3checker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp3checker - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /I "..\..\mpck" ,..\..\mpck" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "mp3checker - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\mpck" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mp3checker - Win32 Release"
# Name "mp3checker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\mpck\bufio.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\checkarguments.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\checkfile.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\checkframe.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\crc.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\file.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\fileinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\filename.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\frame.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\id3.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\listview.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\mp3checker.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\mp3checker.rc
# End Source File
# Begin Source File

SOURCE=..\..\mpck\options.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\print.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\progress.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\rebar.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\sort.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\total.c
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\vector.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\mpck\bufio.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\checkarguments.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\checkfile.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\checkframe.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\crc.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\file.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\fileinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\filename.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\frame.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\id3.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\listview.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\matrices.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\mp3checker.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\mp3errors.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\mpck.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\options.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\print.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\progress.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\rebar.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\sort.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\synchsafe.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\total.h
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\vector.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\mp3checker\windows\dir.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\dirbad.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\dirgood.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\dirunknown.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\file.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\filebad.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\filegood.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\fileunknown.ico
# End Source File
# Begin Source File

SOURCE=..\..\mp3checker\windows\mp3checker.ico
# End Source File
# End Group
# End Target
# End Project
