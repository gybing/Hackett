@echo off 
if "%1"=="h" goto begin 
start mshta vbscript:createobject("wscript.shell").run("""%~nx0"" h",0)(window.close)&&exit 
:begin
::以下为正常批处理命令，不可含有pause set/p等交互命令
.\electron.exe .\resources\LXChat