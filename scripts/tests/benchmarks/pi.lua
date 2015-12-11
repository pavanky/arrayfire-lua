--[[
/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

   monte-carlo estimation of PI
   algorithm:
   - generate random (x,y) samples uniformly
   - count what percent fell inside (top quarter) of unit circle
]]

-- Standard library imports --
local random = math.random
local sqrt = math.sqrt

-- Modules --
local af = require("arrayfire")
local lib = require("lib.af_lib")

-- generate millions of random samples
local samples = 20e6

-- Shorthands --
local Comp, K = lib.CompareResult, lib.NewConstant

--[[ Self-contained code to run host and device estimates of PI.  Note that
   each is generating its own random values, so the estimates of PI
   will differ. ]]
local function pi_device ()
    local x, y = lib.randu(samples, "f32"), lib.randu(samples, "f32")
    return 4.0 * lib.sum("f32", Comp(lib.sqrt(x * x + y * y) < K(1))) / samples
end

local function pi_host ()
	local count = 0
    for _ = 1, samples do
        local x = random()
        local y = random()
        if sqrt(x * x + y * y) < 1 then
            count = count + 1
		end
    end
    return 4.0 * count / samples
end

lib.main(function()
    lib.printf("device:  %.5f seconds to estimate  pi = %.5f", lib.timeit(pi_device), pi_device()) -- Lua fine with original signature
    lib.printf("  host:  %.5f seconds to estimate  pi = %.5f", lib.timeit(pi_host), pi_host())
end)