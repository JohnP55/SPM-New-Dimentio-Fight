#pragma once

#include "evt_cmd.h"

#include <types.h>

namespace spm::evt_cam {
    extern "C" {
        EVT_DECLARE_USER_FUNC(evt_cam3d_evt_zoom_in, 9)
        EVT_DECLARE_USER_FUNC(evt_reset_camera, 2)
    }
}