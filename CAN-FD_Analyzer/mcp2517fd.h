#ifndef MCP2517FD_H
#define MCP2517FD_H

typedef enum {
    NORMAL_MODE = 0, SLEEP_MODE = 1, INT_LOOPBACK_MODE = 2,
    LISTEN_ONLY_MODE = 3, CONFIG_MODE = 4, EXT_LOOPBACK_MODE = 5, CAN_2_0_MODE = 6,
    RESTRICTED_MODE = 7
} OperatingMode;

typedef enum {
    N_50K = 0, N_100K, N_125K, N_250K, N_500K, N_800K, N_1M
} NominalBaudRates;

typedef enum {
    D_500K = 0, D_1M, D_2M, D_5M, D_8M
} DataBaudRates;

#define SET_HARDWARE_CONFIG         0xA1
#define SET_FILTER                  0xA2
#define GET_STATUS                  0xA3
#define CLEAR_ERRORS                0xA4
#define GET_HARDWARE_INFO           0xA5
#define SET_FIFO_LENGTHS            0xA6
#define CLEAR_FILTER                0xA7
#define GET_VERSION                 0xA9

#endif // MCP2517FD_H
