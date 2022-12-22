libs         = libs         or {}
libs.frertex = libs.frertex or {
	name     = "",
	location = ""
}

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

	pkgdeps({ "fmt" })
end

function frertex:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/Inc/" })

	pkgdeps({ "fmt" })
end
