--- Mathematics functions.

-- Modules --
local af = require("arrayfire")
local array = require("lib.impl.array")

-- Imports --
local CallWrap = array.CallWrap
local GetHandle = array.GetHandle
local TwoArrays = array.TwoArrays

-- Exports --
local M = {}

--
local function Binary (func)
	return function(a, b, batch)
		return TwoArrays(func, a, b, batch)
	end
end

--
local function Unary (func)
	return function(in_arr)
		return CallWrap(func, GetHandle(in_arr))
	end
end

local function LoadFuncs (into, funcs, op)
	for _, v in ipairs(funcs) do
		local func = af["af_" .. v]

		into[v] = func and op(func) -- ignore conditionally unavailable functions
	end
end

--
function M.Add (into)
	LoadFuncs(into, {
		"abs",
		"acos",
		"acosh",
		"arg",
		"asin",
		"asinh",
		"atan",
		"atanh",
		"cbrt",
		"ceil",
		"conjg",
		"cos",
		"cosh",
		"cplx",
		"erf",
		"erfc",
		"exp",
		"expm1",
		"factorial",
		"floor",
		"imag",
		"lgamma",
		"log",
		"log10",
		"log1p",
		"not",
		"real",
		"round",
		"sigmoid",
		"sign",
		"sin",
		"sinh",
		"sqrt",
		"tan",
		"tanh",
		"trunc",
		"tgamma"
	}, Unary)
	
	LoadFuncs(into, {
		"add",
		"and",
		"atan2",
		"bitand",
		"bitor",
		"bitshiftl",
		"bitshiftr",
		"bitxor",
		"cplx2",
		"div",
		"eq",
		"ge",
		"gt",
		"hypot",
		"le",
		"lt",
		"maxof",
		"minof",
		"mod",
		"mul",
		"neq",
		"or",
		"pow",
		"rem",
		"root",
		"sub"
	}, Binary)
end

-- Export the module.
return M