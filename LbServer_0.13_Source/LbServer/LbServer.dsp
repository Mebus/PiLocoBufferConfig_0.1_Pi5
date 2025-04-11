# Microsoft Developer Studio Project File - Name="LbServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=LbServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LbServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LbServer.mak" CFG="LbServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LbServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "LbServer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LocoNet/LocoBuffer/LbServer", LDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LbServer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Shared" /I "..\..\Shared\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "LbServer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\Shared\win32" /I "..\..\Shared" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LbServer - Win32 Release"
# Name "LbServer - Win32 Debug"
# Begin Group "Sources"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbstractLoconetDevice.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ClientConnection.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LbServer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LocoBuffer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LoconetServer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Recourses.rc
# End Source File
# Begin Source File

SOURCE=.\TcpServer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbstractLoconetDevice.h
# End Source File
# Begin Source File

SOURCE=.\ClientConnection.h
# End Source File
# Begin Source File

SOURCE=.\LocoBuffer.h
# End Source File
# Begin Source File

SOURCE=.\LoconetServer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\TcpServer.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# End Group
# Begin Group "Shared Source"

# PROP Default_Filter "CPP"
# Begin Group "win32 Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\shared\win32\AbstractThread.cpp

!IF  "$(CFG)" == "LbServer - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "LbServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\SerialPort.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\SyncObjects.cpp

!IF  "$(CFG)" == "LbServer - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "LbServer - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "linux Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\shared\linux\AbstractThread.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\shared\linux\SerialPort.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\shared\linux\SyncObjects.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\shared\LoconetMessage.cpp

!IF  "$(CFG)" == "LbServer - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "LbServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\ReceiveLineBuffer.cpp

!IF  "$(CFG)" == "LbServer - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "LbServer - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Shared Header"

# PROP Default_Filter ""
# Begin Group "win32 Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\shared\win32\AbstractThread.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\OsSpecific.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\SerialPort.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\SyncObjects.h
# End Source File
# End Group
# Begin Group "linux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\shared\linux\AbstractThread.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\linux\OsSpecific.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\linux\SerialPort.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\linux\SyncObjects.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\shared\LoconetMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\ReceiveLineBuffer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\LoconetOverTcp.ico
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
