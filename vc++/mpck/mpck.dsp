# Microsoft Developer Studio Project File - Name="mpck" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=mpck - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mpck.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpck.mak" CFG="mpck - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpck - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mpck - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mpck - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\libgnugetopt-1.2" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "mpck - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\libgnugetopt-1.2" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mpck - Win32 Release"
# Name "mpck - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\mpck\bitfile.c
# End Source File
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

SOURCE=..\..\mpck\filename.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\frame.c
# End Source File
# Begin Source File

SOURCE="..\..\libgnugetopt-1.2\getopt.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgnugetopt-1.2\getopt_long.c"
# End Source File
# Begin Source File

SOURCE=..\..\mpck\metatag.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\layer2.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\main.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\options.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\print.c
# End Source File
# Begin Source File

SOURCE=..\..\mpck\total.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\mpck\bitfile.h
# End Source File
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

SOURCE=..\..\mpck\filename.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\frame.h
# End Source File
# Begin Source File

SOURCE="..\..\libgnugetopt-1.2\getopt.h"
# End Source File
# Begin Source File

SOURCE=..\..\mpck\metatag.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\layer2.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\matrices.h
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

SOURCE=..\..\mpck\synchsafe.h
# End Source File
# Begin Source File

SOURCE=..\..\mpck\total.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
