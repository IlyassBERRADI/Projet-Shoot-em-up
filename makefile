CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lMLV
OBJDIR = bin

SRCDIR = src
HDRDIR = header

SRCS = $(wildcard $(SRCDIR)/*.c)
HDRS = $(wildcard $(HDRDIR)/*.h)
OBJFILES = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(HDRDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
	rm -rf $(OBJDIR)