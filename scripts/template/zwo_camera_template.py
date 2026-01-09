#!/usr/bin/env python3

from optparse import OptionParser
from astropy.io import fits as pyfits

def create_template_fits_file(pathname):
    phdu = pyfits.PrimaryHDU()
    phdu.header['EXTEND'] = (True, 'FITS dataset may contain extensions')
    phdu.header['DATE'] = ('2025-01-01', 'UT date this file was written (yyyy-mm-dd)')
    phdu.header['TIME'] = ('00:00:00', 'UT time this file was written (hh:mm:ss)')
    phdu.header['NEXTEND'] = (1, 'number of standard extensions')
    phdu.header['FILENAME'] =  ('xiandao_20250101000000.fits', 'name of file')
    phdu.header['CATEGORY'] = ('SCI', 'category of data found in data file')
    phdu.header['MODE'] = ('IMAGING', 'observation type - imaging or spectroscopic')
    phdu.header['TASK_ID'] = (-1, 'task ID of this observation')

    phdu.header['TELESCOP'] = ('SITIAN-PROTOTYPE', 'telescope used to acquire data')
    phdu.header['TEL_ID'] = (-1, 'telescope\'s ID')
    phdu.header['RA'] = (1.0000000E+01, 'right ascension of pointing (deg) (J2000)')
    phdu.header['DEC'] = (1.0000000E+01, 'declination of the pointing (deg) (J2000)')
    
    phdu.header['INSTRUME'] = ('ZWO CMOS', 'identifier for instrument used to acquire data')
    phdu.header['FILTER'] = ('g', 'filter in use')

    phdu.header['DETNAME'] = ('ZWO CMOS', 'detector in use')
    phdu.header['DETSIZE'] = ('6000x4000', 'detector format')
    phdu.header['COOLTEMP'] = (0.0, 'setting cooling temperature (Celsius degree)')
    phdu.header['NCHIP'] = (1, 'number of mosaic chips')
    phdu.header['X_OFFSET'] = (0, 'X offset')
    phdu.header['Y_OFFSET'] = (0, 'Y offset')
    phdu.header['X_BIN'] = (1, 'X binning')
    phdu.header['Y_BIN'] = (1, 'Y binning')
    phdu.header['GAIN'] = (5.00, 'requested gain (electrons/ADU)')
    phdu.header['COOLING'] = (False, "whether cooling enabled")
    phdu.header['SDKVER'] = ('V1.1', 'SKD version link against')
    
    phdu.header['TARGNAME'] = ('NGC2345', 'proposer\'s target name')
    phdu.header['TEL_ID'] = (-1, 'target\'s ID')
    phdu.header['RA_TARG'] = (1.0000000E+01, 'right ascension of the target (deg) (J2000)')
    phdu.header['DEC_TARG'] = (1.0000000E+01, 'declination of the target (deg) (J2000)')
    phdu.header['EQUINOX'] = (2000.0, 'equinox of celestial coord. system')
    phdu.header['RADESYS'] = ('FK5', 'celestial coord. reference system')

    phdu.header['SOURCE'] = ('EP alert', 'target source')

    phdu.header['DATE-OBS'] = ('2025-01-01', 'UT date of start of observation (yyyy-mm-dd)')
    phdu.header['TIME-OBS'] = ('00:00:00.000000', 'UT time start of observation (hh:mm:ss.ssssss)')
    phdu.header['EXPTIME'] = (20.00, 'requested exposure time (seconds)')
    phdu.header['SUN_ALT'] = (91.00, 'altitude of the Sun at the beginning of the exposure')
    phdu.header['SUN_AZ'] = (361.00, 'azumith of the Sun at the beginning of the exposure')
    phdu.header['MOON_ALT'] = (91.00, 'altitude of the Moon at the beginning of the exposure')
    phdu.header['MOON_AZ'] = (361.00, 'azumith of the Moon at the beginning of the exposure')
    phdu.header['MOONPHAS'] = (1.10, 'phase of the Moon at the beginning of the exposure')
    phdu.header['AIRMASS'] = (1.3, 'average air mass')
    
    phdu.header['SITENAME'] = ('Lenghu', 'name of site')
    phdu.header['TEL_ID'] = (-1, 'site\'s ID')
    phdu.header['SITE_LAT'] = ('+352210.33', 'latitude of the site (+/-DDMMSS.SS)')
    phdu.header['SITE_LON'] = ('+0833246.22', 'logitude of the site (+/-DDDMMSS.SS)')
    phdu.header['SITE_ALT'] = (+4100.22, 'altitude of the site (m)')
    phdu.header['SITETEMP'] = (20.0, 'temperature of the site (Celsius degree)')
    phdu.header['SITE_WS'] = (6.0, 'wind speed of the site (m/s)')
    phdu.header['SITE_WD'] = (209.0, 'wind direction of the site (deg)')
    phdu.header['SITE_RH'] = (30.0, 'relative humidity of the site (%)')
    phdu.header['SITE_DP'] = (18.2, 'dew point of the site (Celsius degree)')
    phdu.header['SITE_AP'] = (623.55, 'air pressure of the site (hPa)')
    phdu.header['SITE_SEE'] = (1.2, 'seeing of the site (arc second)')
    c = pyfits.Card.fromstring('SITE_BKG=    1.2    /sky backgroud near zenith of the site (mag/arc sec sq.)')
    phdu.header.append(c)
    

    phdu.header.add_blank('/ TELESCOPE INFORMATION', before='TELESCOP')
    phdu.header.add_blank('/ INSTRUMENT INFORMATION', before='INSTRUME')
    phdu.header.add_blank('/ DETECTOR INFORMATION', before='DETNAME')
    phdu.header.add_blank('/ TARGET INFORMATION', before='Targname')
    phdu.header.add_blank('/ EXPOSURE INFORMATION', before='DATE-OBS')
    phdu.header.add_blank('/ SITE INFORMATION', before='SITENAME')
    
    phdu.header.add_blank('', after='TASK_ID')
    phdu.header.add_blank('', before='TELESCOP')
    
    phdu.header.add_blank('', after='DEC')
    phdu.header.add_blank('', before='INSTRUME')
    
    phdu.header.add_blank('', after='FILTER')
    phdu.header.add_blank('', before='DETNAME')
    
    phdu.header.add_blank('', after='SDKVER')
    phdu.header.add_blank('', before='TARGNAME')

    phdu.header.add_blank('', after='SOURCE')
    phdu.header.add_blank('', before='DATE-OBS')
    
    phdu.header.add_blank('', after='AIRMASS')
    phdu.header.add_blank('', before='SITENAME')

    phdu.header.add_comment('and Astrophysics\', volume 376, page 359; bibcode: 2001A&A...376..359H', after='NEXTEND')
    phdu.header.add_comment('FITS (Flexible Image Transport System) format is defined in \'Astronomy', after='NEXTEND')

    phdu.writeto(pathname, overwrite=True)

def main():
    parser = OptionParser()
    parser.disable_interspersed_args()
    options,args = parser.parse_args()
    
    create_template_fits_file(args[0])

if __name__ == '__main__':
    main()
