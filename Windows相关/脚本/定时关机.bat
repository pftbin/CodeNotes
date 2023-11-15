@echo off
title 定时关机
setlocal EnableDelayedExpansion
:: 启用延迟变量扩充

:input
SET /p DelayTime=请输入倒计时(S)：

SET "var=" & for /f "delims=0123456789" %%i in ("!DelayTime!") do SET var=%%i
IF DEFINED var (
ECHO 输入不符合要求，请重新输入
CALL :input
)

::定时关机命令
SHUTDOWN -S -T !DelayTime! 
PAUSE