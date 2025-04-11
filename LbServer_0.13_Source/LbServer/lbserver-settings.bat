@REM $Id: lbserver-settings.bat 1171 2023-01-07 12:56:40Z sbormann71 $

REM settings for LbServer (written by LbServerConfig.exe)
REM ----------------------------------------------------------------------------

REM RS232 port where LocoBuffer is connected (1=COM1, 2=COM2, ...)
SET COM_PORT=\\.\COM1

REM TCP port number where LbServer shell listen for connections
SET TCP_PORT=1234

REM baud rate that is used on the RS232 port, may be 19200 and 57600
SET COM_SPEED=57600

REM use hardware flow control (CTS). Use true for enabled, false for disabled
SET FLOW_CNTL=true

REM ----------------------------------------------------------------------------
