CC 			=	gcc -std=gnu99
CFLAGS 		=	-g -Wall
LDFlags 	=
OBJFILES 	=	main.o
TARGET 		=	main

all: $(TARGET)

$(TARGET) : $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILEs) $(TARGET) *~