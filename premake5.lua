workspace("Frertex")
	common:addConfigs()
	common:addBuildDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("On")
	flags("MultiProcessorCompile")

	startproject("CLI")

	group("Libs")
	project("Frertex")
		location("Frertex/")
		warnings("Extra")

		kind("StaticLib")
		common:outDirs(true)

		includedirs({ "%{prj.location}/Inc/" })
		files({
			"%{prj.location}/Inc/**",
			"%{prj.location}/Src/**"
		})
		removefiles({ "*.DS_Store" })

		pkgdeps({ "commonbuild", "backtrace", "fmt" })

		common:addActions()

	project("Docs")
		location("Docs/")
		kind("Utility")
		files("%{prj.location}/**")

	group("Apps")
	project("CLI")
		location("CLI/")
		warnings("Extra")

		kind("ConsoleApp")
		common:outDirs()
		common:debugDir()

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		links({ "Frertex" })
		externalincludedirs({ "%{wks.location}/Frertex/Inc/" })

		pkgdeps({ "commonbuild", "backtrace", "fmt" })

		common:addActions()

	project("Generator")
		location("Generator/")
		warnings("Extra")

		kind("ConsoleApp")
		common:outDirs()
		common:debugDir()

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		pkgdeps({ "commonbuild", "backtrace", "simdjson" })

		common:addActions()
