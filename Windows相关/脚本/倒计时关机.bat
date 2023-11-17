@echo off
mode con: cols=40 lines=10
title 倒计时关机

setlocal EnableDelayedExpansion
:: 启用延迟变量扩充

:input
SET /p DelayTime=请输入倒计时(S)：

SET "var=" & for /f "delims=0123456789" %%i in ("!DelayTime!") do SET var=%%i
IF DEFINED var (
ECHO 输入不符合要求，请重新输入
CALL :input
)

set /a time=!DelayTime!

:countdown
cls
echo 退出程序将停止计时...
echo 关机倒计时：%time% 秒
set /a time-=1

::等待1秒（方法1）
::ping 127.0.0.1 -n 2 >nul

::等待1秒（方法2）
timeout /t 1 >nul

if %time% gtr 0 (
	goto countdown
) else (
	goto close
)

:close
echo 执行关机操作...
::shutdown -s -t 0