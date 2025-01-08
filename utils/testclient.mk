bin-y += testclient

testclient_CFLAGS += -I $(srcdir)/src
testclient_LDFLAGS += -L $(srcdir)/src
testclient_LIBS += devicesharing
