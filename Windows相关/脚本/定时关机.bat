@echo off
title ��ʱ�ػ�
setlocal EnableDelayedExpansion
:: �����ӳٱ�������

:input
SET /p DelayTime=�����뵹��ʱ(S)��

SET "var=" & for /f "delims=0123456789" %%i in ("!DelayTime!") do SET var=%%i
IF DEFINED var (
ECHO ���벻����Ҫ������������
CALL :input
)

::��ʱ�ػ�����
SHUTDOWN -S -T !DelayTime! 
PAUSE