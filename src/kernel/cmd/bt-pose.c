//
// Created by 0xDC on 4/15/19.
//

#include <zephyr.h>
#include <shell/shell.h>
#include "bt-pose.h"


static int
bb_pose(const struct shell * shell, size_t argc, char ** argv)
{
    return 0;
}


// TODO: bb_scan list is not using my passed in args
SHELL_STATIC_SUBCMD_SET_CREATE(subcmd_bb_pose,
        SHELL_CMD_ARG(headset, NULL, "Connectable headset", bb_pose, 1, 0),
        SHELL_CMD_ARG(le_device, NULL, "Device @BT-Address. (ex. bb_pose a1:c2:b3:d4:e5:f6)", bb_pose, 2, 0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(bb_pose, &subcmd_bb_pose, "BB Pose - Impersonate a profile:", NULL);