libs         = libs         or {}
libs.frertex = libs.frertex or {
	name     = "",
	location = ""
}

require("../ThirdParty/fmt")

local frertex = libs.frertex

function frertex:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/Inc/" })

	files({
		self.location .. "/Inc/**",
		self.location .. "/Src/**"
	})
	removefiles({
		self.location .. "/Inc/Old/**",
		self.location .. "/Src/Old/**"
	})

	libs.fmt:setupDep()
end

function frertex:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/Inc/" })

	libs.fmt:setupDep()
end
