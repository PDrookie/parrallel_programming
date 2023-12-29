CC := gcc
CFLAGS := -Wall
TARGET := final2
v := 1

all: $(TARGET)

$(TARGET):f74101254_final2.c
	$(CC) -o $@ $^ -fopenmp

judge: all
	@judge -v ${v} || printf "or \`make judge v=1\`"

clean:
	rm -f $(TARGET)
