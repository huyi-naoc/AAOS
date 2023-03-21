include_HEADERS = aws.h aws_r.h aws_def.h aws_rpc.h aws_rpc_r.h device.h device_r.h serial.h serial_r.h serial_rpc.h serial_rpc_r.h pdu_def.h pdu.h pdu_r.h pdu.c pdu_rpc.h pdu_rpc_r.h pdu_rpc.c telescope.h telescope_r.h telescope_def.h telescope_rpc.h telescope_rpc_r.h thermal.h thermal_r.h 
lib_LTLIBRARIES = libaaosdriver.la
libaaosdriver_la_SOURCES = device.h device_r.h device.c serial.h serial_r.h serial.c serial_rpc.h serial_rpc_r.h serial_rpc.c aws_def.h aws.h aws_r.h aws.c aws_rpc.h aws_rpc_r.h aws_rpc.c pdu_def.h  pdu.h pdu_r.h pdu.c pdu_rpc.h pdu_rpc_r.h pdu_rpc.c telescope_def.h telescope.h telescope_r.h telescope.c telescope_rpc.h telescope_rpc.h telescope_rpc.c thermal.h thermal_r.h  thermal.c
libaaosdriver_la_CFLAGS = -I$(top_srcdir)/cores -fPIC
libaaosdriver_la_LDFLAGS = -version-info 0:1:0
