/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#ifndef STATE_CONSTS_H
#define STATE_CONSTS_H

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#define F_SEP "\\"
#elif defined(__linux__) || defined(__GNUC__)
#define F_SEP "/"
#else
#error "Unrecognized system."
#endif

enum COLORS {
    BAR_COLOR = 1,
    BSC_COLOR,
    HLT_COLOR,
    OFF_COLOR,
    DIM_COLOR,
    LIT_COLOR,
    GUI_COLOR,
    EMP_COLOR,
    DW_COLOR,
    SW_COLOR,
    SN_COLOR,
    GR_COLOR,
    FR_COLOR,
    HL_COLOR,
    MN_COLOR
};

#endif
