CC := gcc
GCC := g++

OUTDIR := obj

OBJS := $(OUTDIR)/nsComponentManagerUtils.o $(OUTDIR)/nsCOMPtr.o $(OUTDIR)/nsID.o $(OUTDIR)/nsISupportsImpl.o $(OUTDIR)/tinyxpcom.o

TARGET := libxpcom.so

CFLAGS := -O2 -g -fPIC -Wall -DHAVE_VISIBILITY_HIDDEN_ATTRIBUTE -DHAVE_VISIBILITY_ATTRIBUTE -DMOZILLA_INTERNAL_API -D_IMPL_NS_COM `pkg-config nspr --cflags`
#XPCOM_GLUE DXP_UNIX
LFLAGS := -shared

INCS := -I../include
LIBS := `pkg-config nspr --libs`

all: makedir $(TARGET)

$(OUTDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(OUTDIR)/%.o: %.cpp
	$(GCC) $(CFLAGS) $(INCS) -c $< -o $@

$(TARGET): makedir $(OBJS)
	$(GCC) $(OBJS) $(LIBS) $(LFLAGS) -o $(TARGET)

clean:
	rm -R $(OUTDIR)

makedir:
	mkdir -p $(OUTDIR)
