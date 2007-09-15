
@echo off
Set ExeName=../Debug/etpc.exe
"%ExeName%"

echo Lancement des tests
@echo on
for %%a in (*.etp) do "%ExeName% %%a"

