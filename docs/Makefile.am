EXTRA_DIST =\
	    serial.1.md\
	    telescope.1.md\
	    detector_expose.3.md\
	    telescope_get_index_by_name.3.md\
	    telescope_get_move_speed.3.md\
	    telescope_get_slew_speed.3.md\
	    telescope_get_track_rate.3.md\
	    telescope_go_home.3.md\
	    telescope_init.3.md\
	    telescope_move.3.md\
	    telescope_park.3.md\
	    telescope_park_off.3.md\
	    telescope_power_off.3.md\
	    telescope_power_on.3.md\
	    telescope_raw.3.md\
	    telescope_set_move_speed.3.md\
	    telescope_set_slew_speed.3.md\
	    telescope_set_track_rate.3.md\
	    telescope_slew.3.md\
	    telescope_status.3.md

DISTCLEANFILES =
LOCAL_DISTCLEAN =
STAMPS =

if WITH_PANDOC
man1_MANS =\
	   $(top_builddir)/man/man1/serial.1\
	   $(top_builddir)/man/man1/telescope.1

$(man1_MANS): man1.stamp

man1.stamp: 
	mkdir -p $(top_builddir)/man/man1
	PANDOC=$(PANDOC)
	$(PANDOC) -s -f markdown -t man serial.1.md -o $(top_builddir)/man/man1/serial.1
	$(PANDOC) -s -f markdown -t man telescope.1.md -o $(top_builddir)/man/man1/telescope.1
	echo > man1.stamp

EXTRA_DIST += ${man1_MANS}
STAMPS += man1.stamp

man3_MANS =\
	   $(top_builddir)/man/man3/detector_expose.3
	   $(top_builddir)/man/man3/telescope_get_index_by_name.3\
	   $(top_builddir)/man/man3/telescope_get_move_speed.3\
	   $(top_builddir)/man/man3/telescope_get_slew_speed.3\
	   $(top_builddir)/man/man3/telescope_get_track_rate.3\
	   $(top_builddir)/man/man3/telescope_go_home.3\
	   $(top_builddir)/man/man3/telescope_init.3\
	   $(top_builddir)/man/man3/telescope_move.3\
	   $(top_builddir)/man/man3/telescope_park.3\
	   $(top_builddir)/man/man3/telescope_park_off.3\
	   $(top_builddir)/man/man3/telescope_power_off.3\
	   $(top_builddir)/man/man3/telescope_power_on.3\
	   $(top_builddir)/man/man3/telescope_raw.3\
	   $(top_builddir)/man/man3/telescope_set_move_speed.3\
	   $(top_builddir)/man/man3/telescope_set_slew_speed.3\
	   $(top_builddir)/man/man3/telescope_set_track_rate.3\
	   $(top_builddir)/man/man3/telescope_slew.3\
	   $(top_builddir)/man/man3/telescope_status.3

$(man3_MANS): man3.stamp

man3.stamp:
	mkdir -p $(top_builddir)/man/man3
	PANDOC=$(PANDOC)
	$(PANDOC) -s -f markdown -t man telescope_expose.3.md -o $(top_builddir)/man/man3/expose.3
	$(PANDOC) -s -f markdown -t man telescope_get_index_by_name.3.md -o $(top_builddir)/man/man3/telescope_get_index_by_name.3
	$(PANDOC) -s -f markdown -t man telescope_get_move_speed.3.md -o $(top_builddir)/man/man3/telescope_get_move_speed.3
	$(PANDOC) -s -f markdown -t man telescope_get_slew_speed.3.md -o $(top_builddir)/man/man3/telescope_get_slew_speed.3
	$(PANDOC) -s -f markdown -t man telescope_get_track_rate.3.md -o $(top_builddir)/man/man3/telescope_get_track_rate.3
	$(PANDOC) -s -f markdown -t man telescope_go_home.3.md -o $(top_builddir)/man/man3/telescope_go_home.3
	$(PANDOC) -s -f markdown -t man telescope_init.3.md -o $(top_builddir)/man/man3/telescope_init.3
	$(PANDOC) -s -f markdown -t man telescope_move.3.md -o $(top_builddir)/man/man3/telescope_move.3
	$(PANDOC) -s -f markdown -t man telescope_park.3.md -o $(top_builddir)/man/man3/telescope_park.3
	$(PANDOC) -s -f markdown -t man telescope_park_off.3.md -o $(top_builddir)/man/man3/telescope_park_off.3
	$(PANDOC) -s -f markdown -t man telescope_power_off.3.md -o $(top_builddir)/man/man3/telescope_power_off.3
	$(PANDOC) -s -f markdown -t man telescope_power_on.3.md -o $(top_builddir)/man/man3/telescope_power_on.3
	$(PANDOC) -s -f markdown -t man telescope_raw.3.md -o $(top_builddir)/man/man3/telescope_raw.3
	$(PANDOC) -s -f markdown -t man telescope_set_move_speed.3.md -o $(top_builddir)/man/man3/telescope_set_move_speed.3
	$(PANDOC) -s -f markdown -t man telescope_set_slew_speed.3.md -o $(top_builddir)/man/man3/telescope_set_slew_speed.3
	$(PANDOC) -s -f markdown -t man telescope_set_track_rate.3.md -o $(top_builddir)/man/man3/telescope_set_track_rate.3
	$(PANDOC) -s -f markdown -t man telescope_slew.3.md -o $(top_builddir)/man/man3/telescope_slew.3
	$(PANDOC) -s -f markdown -t man telescope_status.3.md -o $(top_builddir)/man/man3/telescope_status.3

EXTRA_DIST += ${man3_MANS}
STAMPS += man3.stamp
LOCAL_DISTCLEAN += ${EXTRA_DIST}

endif

CLEANFILES = $(STAMPS) 

all-local: $(STAMPS) ${EXTRA_DIST}

distclean-local:
	rm -rf $(LOCAL_DISTCLEAN)
