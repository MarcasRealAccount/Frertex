require("Premake/Common")

require("Premake/ThirdParty/fmt")
require("Premake/ThirdParty/jsoncpp")
require("Premake/Libs/frertex")

workspace("Frertex")
	common:setConfigsAndPlatforms()
	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("On")
	flags("MultiProcessorCompile")

	startproject("Frertex")

	group("Dependencies")
	project("FMT")
		location("ThirdParty/FMT/")
		warnings("Off")
		libs.fmt:setup()
		location("ThirdParty/")

	project("jsoncpp")
		location("ThirdParty/jsoncpp/")
		warnings("Off")
		libs.jsoncpp:setup()
		location("ThirdParty/")

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
		removefiles({
			"%{prj.location}/Src/OldMain.cpp",
			"*.DS_Store"
		})

		libs.frertex:setupDep()

		common:addActions()

	project("Generator")
		location("Generator/")
		warnings("Extra")

		common:outDirs()
		common:debugDir()

		kind("ConsoleApp")

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		libs.jsoncpp:setupDep()

		common:addActions()
