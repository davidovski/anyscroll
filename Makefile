
PROG	= anyscroll
FLAGS	= -lX11 -lXtst -lXi -lpthread



${PROG}: ${PROG}.o 
	gcc ${PROG}.c -o ${PROG} ${FLAGS}
