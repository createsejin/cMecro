//
// Created by creat on 2024-01-27.
//

#ifndef ACTIONS_H
#define ACTIONS_H
namespace action_operator {
    enum class Actions {
        XC,
        XC_BLOCK_TOGGLE,
        XC_HOLD,
        XC_V,
        XC_V_BLOCK,
        XC_V_HOLD,
        XC_V_B_HOLD,
        XC_V_B,
        XC_B_HOLD,
        X, // 9
        X_TOGGLE, // X랑 conflict
        X_BLOCK_TOGGLE,
        X_V_BLOCK, // 12
        X_V_HOLD,
        V_X,
        V,
        V_HOLD, // 16
        V_BLOCK_HOLD,

        PROGRAM_EXIT,
        COMMAND_MODE,
        MAX // Actions의 end를 가르킨다.
    };
}
#endif //ACTIONS_H
