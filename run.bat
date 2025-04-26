@echo off
start cmd /k "cd build && GameOfLife.exe"
timeout /t 5 >nul
start cmd /k "python client_gui.py"
