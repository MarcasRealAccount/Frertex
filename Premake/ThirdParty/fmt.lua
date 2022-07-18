libs     = libs     or {}
libs.fmt = libs.fmt or {
	location = ""
}

local fmt = libs.fmt

function fmt:setup()
	self.location = common:projectLocation()

	kind("Utility")
	common:outDirs(true)

	defines({ "FMT_HEADER_ONLY" })

	includedirs({ self.location .. "/include/" })

	files({ self.location .. "/inclue/**" })
end

function fmt:setupDep()
	defines({ "FMT_HEADER_ONLY" })

	externalincludedirs({ self.location .. "/include/" })
end
