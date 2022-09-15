libs         = libs         or {}
libs.jsoncpp = libs.jsoncpp or {
	name     = "",
	location = ""
}

local jsoncpp = libs.jsoncpp

function jsoncpp:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/include/" })

	files({
		self.location .. "/include/json/**",
		self.location .. "/src/lib_json/**"
	})
end

function jsoncpp:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/include/" })
end
