
PROG	= anyscroll
FLAGS	= -lX11 -lXtst


${PROG}: ${PROG}.o 
	gcc ${PROG}.c -o ${PROG} ${FLAGS}
