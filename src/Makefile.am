bin_PROGRAMS = aws awsd pdu pdud scheduler schedulerd serial seriald telescope telescoped thermal thermald

aws_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
aws_LDADD = ../cores/libaaoscore.la  ../drivers/libaaosdriver.la
aws_SOURCES = aws_client.c

awsd_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
awsd_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
awsd_SOURCES = aws_server.c

pdu_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
pdu_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
pdu_SOURCES = pdu_client.c

pdud_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
pdud_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
pdud_SOURCES = pdu_server.c

schedulerd_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
schedulerd_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
schedulerd_SOURCES = scheduler_server.c

scheduler_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
scheduler_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
scheduler_SOURCES = scheduler_client.c

serial_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
serial_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
serial_SOURCES = serial_client.c

seriald_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
seriald_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
seriald_SOURCES = serial_server.c

telescope_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
telescope_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
telescope_SOURCES = telescope_client.c

telescoped_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
telescoped_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
telescoped_SOURCES = telescope_server.c

thermal_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
thermal_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
thermal_SOURCES = thermal_client.c

thermald_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
thermald_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
thermald_SOURCES = thermal_server.c
