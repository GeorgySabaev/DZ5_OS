target: echo terminal
echo: echo.c
	gcc echo.c -o echo
terminal: terminal.c
	gcc terminal.c -o terminal