require("Premake/frertex")

workspace("Frertex")
	common:setConfigsAndPlatforms()
	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("Off")
	flags("MultiProcessorCompile")

	startproject("Frertex")

	group("Libs")
	project("Frertex")
		location("Frertex/")
		warnings("Extra")
		libs.frertex:setup()
		common:addActions()

	project("Docs")
		location("Docs/")
		kind("Utility")
		files("%{prj.location}/**")

	group("Apps")
	project("CLI")
		location("CLI/")
		warnings("Extra")

		common:outDirs()
		common:debugDir()

		kind("ConsoleApp")

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		libs.frertex:setupDep()

		common:addActions()
