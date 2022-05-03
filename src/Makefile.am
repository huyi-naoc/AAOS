bin_PROGRAMS = aws awsd serial seriald

aws_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
aws_LDADD = ../cores/libaaoscore.la  ../drivers/libaaosdriver.la
aws_SOURCES = aws_client.c

awsd_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
awsd_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
awsd_SOURCES = aws_server.c

serial_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
serial_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
serial_SOURCES = serial_client.c

seriald_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
seriald_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
seriald_SOURCES = serial_server.c
