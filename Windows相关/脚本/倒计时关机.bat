@echo off
mode con: cols=40 lines=10
title ����ʱ�ػ�

setlocal EnableDelayedExpansion
:: �����ӳٱ�������

:input
SET /p DelayTime=�����뵹��ʱ(S)��

SET "var=" & for /f "delims=0123456789" %%i in ("!DelayTime!") do SET var=%%i
IF DEFINED var (
ECHO ���벻����Ҫ������������
CALL :input
)

set /a time=!DelayTime!

:countdown
cls
echo �˳�����ֹͣ��ʱ...
echo �ػ�����ʱ��%time% ��
set /a time-=1

::�ȴ�1�루����1��
::ping 127.0.0.1 -n 2 >nul

::�ȴ�1�루����2��
timeout /t 1 >nul

if %time% gtr 0 (
	goto countdown
) else (
	goto close
)

:close
echo ִ�йػ�����...
::shutdown -s -t 0