//
//  log_rpc.h
//  AAOS
//
//  Created by huyi on 2025/12/23.
//

#ifndef log_rpc_h
#define log_rpc_h

#define LOG_COMMAND_SUBMIT                  1
#define LPG_COMMAND_GET_INDEX_BY_FACILITY   2

#ifdef __cpluscplus
extern "C" {
#endif

int log_submit(void *_self, const char *message);

extern const void *Log(void);
extern const void *logClass(void);

extern const void *LogClient(void);
extern const void *LogClientClass(void);

extern const void *LogServer(void);
extern const void *LogServerClass(void);

#ifdef __cplusplus
}
#endif

#endif /* log_rpc_h */
