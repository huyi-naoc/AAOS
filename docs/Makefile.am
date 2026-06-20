EXTRA_DIST =\
	    detector.1.md\
	    detector_abort.3.md\
	    detector_disable_cooling.3.md\
	    detector_enable_cooling.3.md\
	    detector_expose.3.md\
	    detector_get_binning.3.md\
	    detector_get_capture_mode.3.md\
	    detector_get_directory.3.md\
	    detector_get_exposure_time.3.md\
	    detector_get_frame_rate.3.md\
	    detector_get_gain.3.md\
	    detector_get_image.3.md\
	    detector_get_index_by_name.3.md\
	    detector_get_option.3.md\
	    detector_get_overscan.3.md\
	    detector_get_pixel_format.3.md\
	    detector_get_prefix.3.md\
	    detector_get_readout_rate.3.md\
	    detector_get_temperature.3.md\
	    detector_get_template.3.md\
	    detector_get_trigger_mode.3.md\
	    detector_info.3.md\
	    detector_init.3.md\
	    detector_inspect.3.md\
	    detector_list_image.3.md\
	    detector_power_off.3.md\
	    detector_power_on.3.md\
	    detector_raw.3.md\
	    detector_register.3.md\
	    detector_set_binning.3.md\
	    detector_set_capture_mode.3.md\
	    detector_set_directory.3.md\
	    detector_set_exposure_time.3.md\
	    detector_set_frame_rate.3.md\
	    detector_set_gain.3.md\
	    detector_set_option.3.md\
	    detector_set_overscan.3.md\
	    detector_set_pixel_format.3.md\
	    detector_set_prefix.3.md\
	    detector_set_readout_rate.3.md\
	    detector_set_temperature.3.md\
	    detector_set_template.3.md\
	    detector_set_trigger_mode.3.md\
	    detector_status.3.md\
	    detector_stop.3.md\
	    detector_wait_for_completion.3.md\
	    serial.1.md\
	    scheduler_get_task_by_telescope_id.3.md\
	    scheduler_update_status.3.md\
	    telescope.1.md\
	    #telescope_close_cover.3.md\
	    #telescope_disable_derotator.3.md\
	    #telescope_enable_derotator.3.md\
	    #telescope_focus.3.md\
	    #telescope_get_derotator_angle.3.md\
	    #telescope_get_focus_length.3.md\
	    telescope_get_index_by_name.3.md\
	    telescope_get_move_speed.3.md\
	    telescope_get_slew_speed.3.md\
	    telescope_get_track_rate.3.md\
	    telescope_go_home.3.md\
	    telescope_info.3.md\
	    telescope_init.3.md\
	    telescope_inspect.3.md\
	    telescope_move.3.md\
	    telescope_open_cover.3.md\
	    telescope_park_off.3.md\
	    telescope_park.3.md\
	    telescope_power_off.3.md\
	    telescope_power_on.3.md\
	    telescope_raw.3.md\
	    telescope_register.3.md\
	    telescope_set_move_speed.3.md\
	    telescope_set_slew_speed.3.md\
	    telescope_set_track_rate.3.md\
	    telescope_slew.3.md\
	    telescope_status.3.md\
	    telescope_stop.3.md\
	    telescope_switch_detector.3.md\
	    telescope_switch_filter.3.md\
	    telescope_switch_instrument.3.md


DISTCLEANFILES =
LOCAL_DISTCLEAN =
STAMPS =

if WITH_PANDOC
man1_MANS =\
	   $(top_builddir)/man/man1/detector.1\
	   $(top_builddir)/man/man1/serial.1\
	   $(top_builddir)/man/man1/telescope.1

$(man1_MANS): man1.stamp

man1.stamp: 
	mkdir -p $(top_builddir)/man/man1
	PANDOC=$(PANDOC)
	$(PANDOC) -s -f markdown -t man detector.1.md -o $(top_builddir)/man/man1/detector.1
	$(PANDOC) -s -f markdown -t man serial.1.md -o $(top_builddir)/man/man1/serial.1
	$(PANDOC) -s -f markdown -t man telescope.1.md -o $(top_builddir)/man/man1/telescope.1
	echo > man1.stamp

EXTRA_DIST += ${man1_MANS}
STAMPS += man1.stamp

man3_MANS =\
	   $(top_builddir)/man/man3/detector_abort.3
	   $(top_builddir)/man/man3/detector_disable_cooling.3
	   $(top_builddir)/man/man3/detector_enable_cooling.3
	   $(top_builddir)/man/man3/detector_expose.3
	   $(top_builddir)/man/man3/detector_get_binning.3
	   $(top_builddir)/man/man3/detector_get_capture_mode.3
	   $(top_builddir)/man/man3/detector_get_directory.3
	   $(top_builddir)/man/man3/detector_get_exposure_time.3
	   $(top_builddir)/man/man3/detector_get_frame_rate.3
	   $(top_builddir)/man/man3/detector_get_gain.3
	   $(top_builddir)/man/man3/detector_get_image.3
	   $(top_builddir)/man/man3/detector_get_index_by_name.3
	   $(top_builddir)/man/man3/detector_get_option.3
	   $(top_builddir)/man/man3/detector_get_overscan.3
	   $(top_builddir)/man/man3/detector_get_pixel_format.3
	   $(top_builddir)/man/man3/detector_get_prefix.3
	   $(top_builddir)/man/man3/detector_get_readout_rate.3
	   $(top_builddir)/man/man3/detector_get_region.3
	   $(top_builddir)/man/man3/detector_get_temperature.3
	   $(top_builddir)/man/man3/detector_get_template.3
	   $(top_builddir)/man/man3/detector_get_trigger_mode.3
	   $(top_builddir)/man/man3/detector_info.3
	   $(top_builddir)/man/man3/detector_init.3
	   $(top_builddir)/man/man3/detector_inspect.3
	   $(top_builddir)/man/man3/detector_list_image.3
	   $(top_builddir)/man/man3/detector_power_off.3
	   $(top_builddir)/man/man3/detector_power_on.3
	   $(top_builddir)/man/man3/detector_raw.3
	   $(top_builddir)/man/man3/detector_register.3
	   $(top_builddir)/man/man3/detector_set_binning.3
	   $(top_builddir)/man/man3/detector_set_capture_mode.3
	   $(top_builddir)/man/man3/detector_set_directory.3
	   $(top_builddir)/man/man3/detector_set_exposure_time.3
	   $(top_builddir)/man/man3/detector_set_frame_rate.3
	   $(top_builddir)/man/man3/detector_set_gain.3
	   $(top_builddir)/man/man3/detector_set_option.3
	   $(top_builddir)/man/man3/detector_set_overscan.3
	   $(top_builddir)/man/man3/detector_set_pixel_format.3
	   $(top_builddir)/man/man3/detector_set_prefix.3
	   $(top_builddir)/man/man3/detector_set_readout_rate.3
	   $(top_builddir)/man/man3/detector_set_region.3
	   $(top_builddir)/man/man3/detector_set_temperature.3
	   $(top_builddir)/man/man3/detector_set_template.3
	   $(top_builddir)/man/man3/detector_set_trigger_mode.3
	   $(top_builddir)/man/man3/detector_status.3
	   $(top_builddir)/man/man3/detector_stop.3
	   $(top_builddir)/man/man3/detector_wait_for_completion.3
	   $(top_builddir)/man/man3/scheduler_get_task_by_telescope_name.3
	   $(top_builddir)/man/man3/scheduler_update_status.3
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
	   $(top_builddir)/man/man3/telescope_status.3\
	   $(top_builddir)/man/man3/telescope_switch_detector.3\
	   $(top_builddir)/man/man3/telescope_switch_filter.3\
	   $(top_builddir)/man/man3/telescope_switch_instrument.3
       

$(man3_MANS): man3.stamp

man3.stamp:
	mkdir -p $(top_builddir)/man/man3
	PANDOC=$(PANDOC)
	$(PANDOC) -s -f markdown -t man detector_abort.3.md -o $(top_builddir)/man/man3/detector_abort.3
	$(PANDOC) -s -f markdown -t man detector_disable_cooling.3.md -o $(top_builddir)/man/man3/detector_disable_cooling.3
	$(PANDOC) -s -f markdown -t man detector_enable_cooling.3.md -o $(top_builddir)/man/man3/detector_enable_cooling.3
	$(PANDOC) -s -f markdown -t man detector_expose.3.md -o $(top_builddir)/man/man3/detector_expose.3
	$(PANDOC) -s -f markdown -t man detector_get_binning.3.md -o $(top_builddir)/man/man3/detector_get_binning.3
	$(PANDOC) -s -f markdown -t man detector_get_capture_mode.3.md -o $(top_builddir)/man/man3/detector_get_capture_mode.3
	$(PANDOC) -s -f markdown -t man detector_get_directory.3.md -o $(top_builddir)/man/man3/detector_get_directory.3
	$(PANDOC) -s -f markdown -t man detector_get_exposure_time.3.md -o $(top_builddir)/man/man3/detector_get_exposure_time.3
	$(PANDOC) -s -f markdown -t man detector_get_frame_rate.3.md -o $(top_builddir)/man/man3/detector_get_frame_rate.3
	$(PANDOC) -s -f markdown -t man detector_get_gain.3.md -o $(top_builddir)/man/man3/detector_get_gain.3
	$(PANDOC) -s -f markdown -t man detector_get_image.3.md -o $(top_builddir)/man/man3/detector_get_image.3
	$(PANDOC) -s -f markdown -t man detector_get_index_by_name.3.md -o $(top_builddir)/man/man3/detector_get_index_by_name.3
	$(PANDOC) -s -f markdown -t man detector_get_option.3.md -o $(top_builddir)/man/man3/detector_get_option.3
	$(PANDOC) -s -f markdown -t man detector_get_overscan.3.md -o $(top_builddir)/man/man3/detector_get_overscan.3
	$(PANDOC) -s -f markdown -t man detector_get_pixel_format.3.md -o $(top_builddir)/man/man3/detector_get_pixel_format.3
	$(PANDOC) -s -f markdown -t man detector_get_prefix.3.md -o $(top_builddir)/man/man3/detector_get_prefix.3
	$(PANDOC) -s -f markdown -t man detector_get_readout_rate.3.md -o $(top_builddir)/man/man3/detector_get_readout_rate.3
	$(PANDOC) -s -f markdown -t man detector_get_region.3.md -o $(top_builddir)/man/man3/detector_get_region.3
	$(PANDOC) -s -f markdown -t man detector_get_temperature.3.md -o $(top_builddir)/man/man3/detector_get_temperature.3
	$(PANDOC) -s -f markdown -t man detector_get_template.3.md -o $(top_builddir)/man/man3/detector_get_template.3
	$(PANDOC) -s -f markdown -t man detector_get_trigger_mode.3.md -o $(top_builddir)/man/man3/detector_get_trigger_mode.3
	$(PANDOC) -s -f markdown -t man detector_info.3.md -o $(top_builddir)/man/man3/detector_info.3
	$(PANDOC) -s -f markdown -t man detector_init.3.md -o $(top_builddir)/man/man3/detector_init.3
	$(PANDOC) -s -f markdown -t man detector_inspect.3.md -o $(top_builddir)/man/man3/detector_inspect.3
	$(PANDOC) -s -f markdown -t man detector_power_off.3.md -o $(top_builddir)/man/man3/detector_power_off.3
	$(PANDOC) -s -f markdown -t man detector_power_on.3.md -o $(top_builddir)/man/man3/detector_power_on.3
	$(PANDOC) -s -f markdown -t man detector_raw.3.md -o $(top_builddir)/man/man3/detector_raw.3
	$(PANDOC) -s -f markdown -t man detector_register.3.md -o $(top_builddir)/man/man3/detector_register.3
	$(PANDOC) -s -f markdown -t man detector_set_binning.3.md -o $(top_builddir)/man/man3/detector_set_binning.3
	$(PANDOC) -s -f markdown -t man detector_set_capture_mode.3.md -o $(top_builddir)/man/man3/detector_set_capture_mode.3
	$(PANDOC) -s -f markdown -t man detector_set_directory.3.md -o $(top_builddir)/man/man3/detector_set_directory.3
	$(PANDOC) -s -f markdown -t man detector_set_exposure_time.3.md -o $(top_builddir)/man/man3/detector_set_exposure_time.3
	$(PANDOC) -s -f markdown -t man detector_set_frame_rate.3.md -o $(top_builddir)/man/man3/detector_set_frame_rate.3
	$(PANDOC) -s -f markdown -t man detector_set_gain.3.md -o $(top_builddir)/man/man3/detector_set_gain.3
	$(PANDOC) -s -f markdown -t man detector_set_image.3.md -o $(top_builddir)/man/man3/detector_set_image.3
	$(PANDOC) -s -f markdown -t man detector_set_index_by_name.3.md -o $(top_builddir)/man/man3/detector_set_index_by_name.3
	$(PANDOC) -s -f markdown -t man detector_set_option.3.md -o $(top_builddir)/man/man3/detector_set_option.3
	$(PANDOC) -s -f markdown -t man detector_set_overscan.3.md -o $(top_builddir)/man/man3/detector_set_overscan.3
	$(PANDOC) -s -f markdown -t man detector_set_pixel_format.3.md -o $(top_builddir)/man/man3/detector_set_pixel_format.3
	$(PANDOC) -s -f markdown -t man detector_set_prefix.3.md -o $(top_builddir)/man/man3/detector_set_prefix.3
	$(PANDOC) -s -f markdown -t man detector_set_readout_rate.3.md -o $(top_builddir)/man/man3/detector_set_readout_rate.3
	$(PANDOC) -s -f markdown -t man detector_set_region.3.md -o $(top_builddir)/man/man3/detector_set_region.3
	$(PANDOC) -s -f markdown -t man detector_set_temperature.3.md -o $(top_builddir)/man/man3/detector_set_temperature.3
	$(PANDOC) -s -f markdown -t man detector_set_template.3.md -o $(top_builddir)/man/man3/detector_set_template.3
	$(PANDOC) -s -f markdown -t man detector_set_trigger_mode.3.md -o $(top_builddir)/man/man3/detector_set_trigger_mode.3
	$(PANDOC) -s -f markdown -t man detector_status.3.md -o $(top_builddir)/man/man3/detector_status.3
	$(PANDOC) -s -f markdown -t man detector_stop.3.md -o $(top_builddir)/man/man3/detector_stop.3
	$(PANDOC) -s -f markdown -t man detector_wait_for_completion.3.md -o $(top_builddir)/man/man3/detector_wait_for_completion.3
	$(PANDOC) -s -f markdown -t man scheduler_get_task_by_telescope_id.3.md -o $(top_builddir)/man/man3/scheduler_get_task_by_telescope_id.3
	$(PANDOC) -s -f markdown -t man scheduler_update_status.3.md -o $(top_builddir)/man/man3/scheduler_update_status.3
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
	$(PANDOC) -s -f markdown -t man telescope_switch_detector.3.md  -o $(top_builddir)/man/man3/telescope_switch_detector.3
	$(PANDOC) -s -f markdown -t man telescope_switch_filter.3.md  -o $(top_builddir)/man/man3/telescope_switch_filter.3
	$(PANDOC) -s -f markdown -t man telescope_switch_instrument.3.md  -o $(top_builddir)/man/man3/telescope_switch_instrument.3

EXTRA_DIST += ${man3_MANS}
STAMPS += man3.stamp
LOCAL_DISTCLEAN += ${EXTRA_DIST}

endif

CLEANFILES = $(STAMPS) 

all-local: $(STAMPS) ${EXTRA_DIST}

distclean-local:
	rm -rf $(LOCAL_DISTCLEAN)
