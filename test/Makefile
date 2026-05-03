
FILES=demo1

all: ${FILES}

${FILES}: %: %.bas
	petcat -w40 -o $@ $<

clean:
	rm -f ${FILES}

