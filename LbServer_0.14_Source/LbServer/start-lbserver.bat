@REM $Id: start-lbserver.bat 1171 2023-01-07 12:56:40Z sbormann71 $
@ECHO OFF
SETLOCAL
PROMPT LBSERVER$G

SET CUR_DIR=%~dp0
CHDIR /D "%CUR_DIR%"

SET CFG_FILE=%APPDATA%\LoconetOverTcp.sf.net\LbServer\lbserver-settings.bat
IF EXIST "%CFG_FILE%" (
	CALL "%CFG_FILE%"
) ELSE (
	@REM try local file
	IF EXIST lbserver-settings.bat CALL lbserver-settings.bat
)
TITLE LbServer %COM_PORT% %TCP_PORT%

"%CUR_DIR%LbServer.exe" "%COM_PORT%" %TCP_PORT% %COM_SPEED% %FLOW_CNTL%

IF ERRORLEVEL == 1 PAUSE