CC := gcc

OUTDIR := obj

OBJS := $(OUTDIR)/xpidl.o $(OUTDIR)/xpidl_header.o $(OUTDIR)/xpidl_idl.o $(OUTDIR)/xpidl_util.o

TARGET := xpidl

CFLAGS := -O2 -g -Wall `pkg-config libIDL-2.0 --cflags` `pkg-config glib-2.0 --cflags`
LFLAGS :=

INCS := -I.
LIBS := `pkg-config libIDL-2.0 --libs` `pkg-config glib-2.0 --libs`

all: makedir $(TARGET)

$(OUTDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBS) $(LFLAGS) -o $(TARGET)

clean:
	rm -R $(OUTDIR)

makedir:
	mkdir -p $(OUTDIR)
