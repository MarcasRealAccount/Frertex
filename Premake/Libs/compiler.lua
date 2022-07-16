libs                 = libs                 or {}
libs.frertexcompiler = libs.frertexcompiler or {
	name     = "",
	location = ""
}

local frertexcompiler = libs.frertexcompiler

function frertexcompiler:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/Inc/" })

	files({
		self.location .. "/Inc/**",
		self.location .. "/Src/**"
	})
end

function frertexcompiler:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/Inc/" })
end