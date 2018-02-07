@echo off
IF [%1] == [] (GOTO end)
netsh advfirewall firewall add rule name="Open M1 gameserver port" dir=in action=allow program=%~1 protocol=UDP localport=any
netsh advfirewall firewall add rule name="Open M1 gameserver port" dir=out action=allow program=%~1 protocol=UDP localport=any
ECHO Sucessfully opened firewall to %1
pause
exit

:end
ECHO Missing parameter ( parameter 1 = file to open ports to )
