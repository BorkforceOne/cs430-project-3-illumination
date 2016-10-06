CC=gcc
CCFLAGS=-Wall -O3
SOURCEDIR=src
HEADERDIR=src
LDFLAGS=
OBJDIR=obj
TARGET=raycast

SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJECTS=$(patsubst $(SOURCEDIR)/%,$(OBJDIR)/%,$(SOURCES:%.c=%.o))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c $(OBJDIR)
	$(CC) $(CCFLAGS) -c $< -o $@ -I$(HEADERDIR) -I$(SOURCEDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)
