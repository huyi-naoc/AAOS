//
//  telescope_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2019/7/9.
//  Copyright © 2019 NAOC. All rights reserved.
//

#ifndef telescope_rpc_h
#define telescope_rpc_h

/**
 * @file telescope_rpc.h
 */

#include "rpc.h"
#include "telescope_def.h"
#include <stdint.h>

#define TELESCOPE_COMMAND_RAW               1
#define TELESCOPE_COMMAND_STATUS            2
#define TELESCOPE_COMMAND_POWER_ON          3
#define TELESCOPE_COMMAND_POWER_OFF         4
#define TELESCOPE_COMMAND_INIT              5
#define TELESCOPE_COMMAND_PARK              6
#define TELESCOPE_COMMAND_PARK_OFF          7
#define TELESCOPE_COMMAND_STOP              8
#define TELESCOPE_COMMAND_GO_HOME           9
#define TELESCOPE_COMMAND_MOVE              10
#define TELESCOPE_COMMAND_TRY_MOVE          11
#define TELESCOPE_COMMAND_TIMED_MOVE        12
#define TELESCOPE_COMMAND_SLEW              13
#define TELESCOPE_COMMAND_TRY_SLEW          14
#define TELESCOPE_COMMAND_TIMED_SLEW        15
#define TELESCOPE_COMMAND_RELEASE           16
#define TELESCOPE_COMMAND_SET_MOVE_SPEED    17
#define TELESCOPE_COMMAND_GET_MOVE_SPEED    18
#define TELESCOPE_COMMAND_SET_SLEW_SPEED    19
#define TELESCOPE_COMMAND_GET_SLEW_SPEED    20
#define TELESCOPE_COMMAND_SET_TRACK_RATE    21
#define TELESCOPE_COMMAND_GET_TRACK_RATE    22
#define TELESCOPE_COMMAND_GET_INDEX_BY_NAME 23
#define TELESCOPE_COMMAND_SET_OPTION        24
#define TELESCOPE_COMMAND_INSPECT           25
#define TELESCOPE_COMMAND_REGISTER          26
#define TELESCOPE_COMMAND_SWITCH_INSTRUMENT 27
#define TELESCOPE_COMMAND_SWITCH_FILTER     28
#define TELESCOPE_COMMAND_SWITCH_DETECTOR   29
#define TELESCOPE_COMMAND_FOCUS             30

#ifdef __cplusplus
extern "C" {
#endif

int telescope_get_index_by_name(void *_self, const char *name);

/**
 * Staus method of telescope object.
 * @param[in,out] _self telescope object.
 * @param[in] res a pointer to restore the result.
 * @param[in] res_size size of \b res.
 * @param[in] res_len data length of \b res. If \b res_len is \b NULL, do nothing.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 */
int telescope_status(void *_self, char *res, size_t res_size, size_t *res_len);

/**
 * Set options.
 * @param[in,out] _self telescope object.
 * @param[in] option options.
 */
int telescope_set_option(void *_self, uint16_t option);

/**
 * Raw method of telescope object.
 * @details Send a raw command to a telescope. The command is not necessarily a string. The intepretion of command are depend on the type of the underline telescope. Raw method may change the state of the telescope silently, as a consequence, it will make the state of the telescope inconsistently. Use it with caution.
 * @param[in,out] _self telescope object.
 * @param[in] command a pointer to restore the raw commannd to send to the telescope.
 * @param[in] command_size size of \b command.
 * @param[in,out] results  a pointer to restore the results.
 * @param[in] results_size sizeof \b results.
 * @param[out] return_size data length of \b results. If \b return_size is \b NULL, do nothing.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EBADCMD
 * Bad command is sent to the telescope.
 * @retval AAOS_ECMDNOSUPPORT
 * Raw command is not implemented.
 */
int telescope_raw(void *_self, const void *command, size_t command_size, void *results, size_t results_size, size_t *return_size);

/**
 * Power on method of telescope object.
 * @details If power on method is supported by the underline telescope, it will change the state of the telescope to UNINITIALIZED if the telescope is in PWROFF state, otherwise, it does nothing.
 * @param[in,out] _self telescope object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Power on method is not supported by the underline telescope.
 */
int telescope_power_on(void *_self);

/**
 * Power off method of telescope object.
 * @details If power off method is supported by the underline telescope, it will change the state of the telescope to PWROFF. Approriate cleaning up methods should be called before calling power off.
 * @param[in,out] _self telescope object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Power off method is not supported by the underline telescope.
 */
int telescope_power_off(void *_self);

/**
 * Init method of telescope object.
 * @details Init method will initialize the underline telescope. If the telescope is in UNINITIALIZED state, all the parameters of telescope, such as location, time, PID, etc, should be set properly in this method. After init call, the state of the telescope is PARKED.
 * @param[in,out] _self telescope object.
 */
int telescope_init(void *_self);

/**
 * Init method of telescope object.
 * @details Park method will disable the movement of both axeses of  the telescope, which means the telescope parks at the current position without any tracking.
 * @param[in,out] _self telescope object.
 */
int telescope_park(void *_self);

/**
 * Init method of telescope object.
 * @details Park method will disable the movement of both axeses of  the telescope, which means the telescope parks at the current position without any tracking.
 * @param[in,out] _self telescope object.
 */
int telescope_park_off(void *_self);

/**
 * Stop method of telescope object.
 * @details Stop the movements of both axes, after it returns successfully, the telescope is in TRACKING state. If the telescope is not in MOVING or SLEWING state, it does nothing.
 * @param[in,out] _self telescope object.
 */
int telescope_stop(void *_self);

/**
 * Go home method of telesciope
 * @details Make the telescope point to a predefined position and then park there. After it returns successfully, the state of the telescope will change to PARKED. Go home may be implemented by slew or move method, therefore, during the executing period, the state of the telescope can be either SLEWING or MOVING.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b direction is not TELESCOPE_MOVE_EAST, TELESCOPE_MOVE_WEST, TELESCOPE_MOVE_NORTH or TELESCOPE_MOVE_SOUTH.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_ETIMEDOUT
 * Executing slew timed out. Usually means failure.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_go_home(void *_self);

/**
 * Move method of telescope
 * @details Move the single axis of the telescope towards a direction for a period. The telescope should be in TRACKING, PARKED, SLEWING or MOVING state before executing move method. If an earlier call of slew or move does not complete (i.e., telescope is in state of SLEWING or MOVING), the earlier call will be aborted by the current call, and return AAOS_ECANCELED immediately. Thus move can be aborted by move, park, stop, go home. During move is executing, the state of telescope is changed to MOVING, When the calling of slew returns successfully, the telescope is in TRACKING state.
 * @param[in,out] _self telescope object.
 * @param[in] direction direction to move, possible value, TELESCOPE_MOVE_EAST, TELESCOPE_MOVE_WEST, TELESCOPE_MOVE_NORTH, TELESCOPE_MOVE_SOUTH.
 * @param[in] duration moving duration in seconds.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b direction is not TELESCOPE_MOVE_EAST, TELESCOPE_MOVE_WEST, TELESCOPE_MOVE_NORTH or TELESCOPE_MOVE_SOUTH.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_move(void *_self, unsigned int direction, double duration);

/**
 * Move method of telescope
 * @details Move the single axis of the telescope towards a direction for a period. The telescope should be in TRACKING or PARKED state before executing move method. If an earlier call of slew or move does not complete (i.e., telescope is in state of SLEWING or MOVING), the earlier call will be aborted by the current call, and return AAOS_ECANCELED immediately. Thus move can be aborted by move, park, stop, go home. During move is executing, the state of telescope is changed to MOVING, When the calling of slew returns successfully, the telescope is in TRACKING state.
 * @param[in,out] _self telescope object.
 * @param[in] direction direction to move, possible value, TELESCOPE_MOVE_EAST, TELESCOPE_MOVE_WEST, TELESCOPE_MOVE_NORTH, TELESCOPE_MOVE_SOUTH.
 * @param[in] duration moving duration in seconds.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EBUSY
 * A earlier call of slew or move does not complete.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b direction is not TELESCOPE_MOVE_EAST, TELESCOPE_MOVE_WEST, TELESCOPE_MOVE_NORTH or TELESCOPE_MOVE_SOUTH.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_try_move(void *_self, unsigned int direction, double duration);

/**
 * Move method of telescope
 * @details Move the single axis of the telescope towards a direction for a period. The telescope should be in TRACKING, PARKED, SLEWING or MOVING  state before executing move method. If an earlier call of slew or move does not complete (i.e., telescope is in state of SLEWING or MOVING), the earlier call will be aborted by the current call, and return AAOS_ECANCELED immediately. Thus move can be aborted by move, park, stop, go home. During move is executing, the state of telescope is changed to MOVING, When the calling of slew returns successfully, the telescope is in TRACKING state.
 * @param[in,out] _self telescope object.
 * @param[in] direction direction to move, possible value, TELESCOPE_MOVE_EAST, TELESCOPE_MOVE_WEST, TELESCOPE_MOVE_NORTH, TELESCOPE_MOVE_SOUTH.
 * @param[in] duration moving duration in seconds.
 * @param[in] timeout timeout in seconds it wait for the telescope is in TRACKING or PARKED state.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EBUSY
 * A earlier call of slew or move does not complete.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b ra and/or \b dec is out of range or the limiation of the telescope.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_timed_move(void *_self, unsigned int direction, double duration, double timeout);

/**
 * Slew method of telescope object.
 * @details Slew the telescope to a object identied by its coordinate. The telescope should be in TRACKING, PARKED, SLEWING or MOVING state before executing slew method. If an earlier call of slew or move does not complete (i.e., telescope is in state of slewing or moving), the earlier call will be aborted by the current call, and return AAOS_ECANCELED immediately. Thus slew can be aborted by move, park, stop, go_home. When the calling of slew returns successfully, the telescope should have begun to track the object, which means successive operation of exposing can be issued. During slew is executing, the state of telescope is changed to SLEWING, and after it returns, it is then changed to TRACKING.
 * @param[in,out] _self telescope object.
 * @param[in] ra Right ascension of the object, in degree, max range [0, 360).
 * @param[in] dec Declination of the object, in degree, max range [-90, 90].
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b ra and/or \b dec is out of range or the limiation of the telescope.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_ETIMEDOUT
 * Executing slew timed out. Usually means failure.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_slew(void *_self, double ra, double dec);

/**
 * Slew method of telescope object.
 * @details Slew the telescope to a object identied by its coordinate. The telescope should be in TRACKING or PARKED, state before executing slew method. If an earlier call of slew or move does not complete (i.e., telescope is in state of slewing or moving), the earlier call will be aborted by the current call, and return AAOS_ECANCELED immediately. Thus slew can be aborted by move, park, stop, go_home. When the calling of slew returns successfully, the telescope should have begun to track the object, which means successive operation of exposing can be issued. During slew is executing, the state of telescope is changed to SLEWING, and after it returns, it is then changed to TRACKING.
 * @param[in,out] _self telescope object.
 * @param[in] ra Right ascension of the object, in degree, max range [0, 360).
 * @param[in] dec Declination of the object, in degree, max range [-90, 90].
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EBUSY
 * A earlier call of slew or move does not complete.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b ra and/or \b dec is out of range or the limiation of the telescope.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_ETIMEDOUT
 * Executing slew timed out. Usually means failure.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_try_slew(void *_self, double ra, double dec);

/**
 * Slew method of telescope object.
 * @details Slew the telescope to a object identied by its coordinate. The telescope should be in TRACKING, PARKED, SLEWING or MOVING state before executing slew method. If an earlier call of slew or move does not complete (i.e., telescope is in state of slewing or moving), the earlier call will be aborted by the current call, and return AAOS_ECANCELED immediately. Thus slew can be aborted by move, park, stop, go_home. When the calling of slew returns successfully, the telescope should have begun to track the object, which means successive operation of exposing can be issued. During slew is executing, the state of telescope is changed to SLEWING, and after it returns, it is then changed to TRACKING.
 * @param[in,out] _self telescope object.
 * @param[in] ra Right ascension of the object, in degree, max range [0, 360).
 * @param[in] dec Declination of the object, in degree, max range [-90, 90].
 * @param[in] timeout timeout in seconds it wait for the telescope is in TRACKING or PARKED state.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EBUSY
 * A earlier call of slew or move does not complete.
 * @retval AAOS_ECANCELED
 * Slew is canceled by a successive call of stop, park, move or slew.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The input \b ra and/or \b dec is out of range or the limiation of the telescope.
 * @retval AAOS_EPWROFF
 * The underline telescope has not been powered on.
 * @retval AAOS_ETIMEDOUT
 * Executing slew timed out. Usually means failure.
 * @retval AAOS_EUNINT
 * The underline telescope has not been initialized yet.
 */
int telescope_timed_slew(void *_self, double ra, double dec, double timeout);
int telescope_release(void *_self);

/**
 * Set move speed of telescope object.
 * @param[in,out] _self telescope object.
 * @paramp[in] move_speed move speed in arcsecond per second.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The \b move_speed is out of range.
 */
int telescope_set_move_speed(void *_self, double move_speed);

/**
 * Get move speed of telescope object.
 * @param[in,out] _self telescope object.
 * @paramp[in,out] move_speed move speed in arcsecond per second.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 */
int telescope_get_move_speed(void *_self, double *move_speed);

/**
 * Set move speed of telescope object.
 * @param[in,out] _self telescope object.
 * @param[in] slew_speed_x slew speed of primary axis in arcsecond per second.
 * @param[in] slew_speed_y slew speed of secondary axis in in arcsecond per second.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The \b slew_speed_x or \b slew_speed_y is out of range.
 */
int telescope_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y);

/**
 * Get move speed of telescope object.
 * @param[in,out] _self telescope object.
 * @param[out] slew_speed_x slew speed of primary axis in arcsecond per second.
 * @param[out] slew_speed_y slew speed of secondary axis in in arcsecond per second.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 */
int telescope_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y);

/**
 * Set move speed of telescope object.
 * @param[in,out] _self telescope object.
 * @param[in] track_rate_x track_rate of primary axis in arcsecond per second.
 * @param[in] track_rate_y track_rate of secondary axis in in arcsecond per second.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_EINVAL
 * The \b track_rate_x or \b track_rate_y is out of range.
 */
int telescope_set_track_rate(void *_self, double track_rate_x, double track_rate_y);

/**
 * Set move speed of telescope object.
 * @param[in,out] _self telescope object.
 * @param[in] track_rate_x track_rate of primary axis in arcsecond per second.
 * @param[in] track_rate_y track_rate of secondary axis in in arcsecond per second.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Telescope is in MALFUNCTION state.
 */
int telescope_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y);

/**
 * Inspect whether the telescope is working properly.
 * @param[in,out] _self telescope object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ERROR
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 *  Inspect is not supported.
 */
int telescope_inspect(void *_self);

/**
 * Wait until the telescope is recovered. 
 * @param[in,out] _self telescope object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ERROR
 * Telescope is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 *  Inspect is not supported.
 */
int telescope_register(void *_self, double timeout);

int telescope_switch_instrument(void *_self, const char *name);
int telescope_switch_filter(void *_self, const char *name);
int telescope_switch_detector(void *_self, const char *name);
int telescope_focus(void *_self, unsigned int absolute, double step);


extern const void *Telescope(void);
extern const void *TelescopeClass(void);

extern const void *TelescopeClient(void);
extern const void *TelescopeClientClass(void);

extern const void *TelescopeServer(void);
extern const void *TelescopeServerClass(void);

#ifdef __cplusplus
}
#endif

#endif /* telescope_rpc_h */
