require("Premake/Common")

require("Premake/Libs/compiler")

workspace("Frertex")
	common:setConfigsAndPlatforms()
	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("Off")
	flags("MultiProcessorCompile")

	startproject("Frertex")

	group("Dependencies")

	group("Libs")
	project("Compiler")
		location("Compiler/")
		warnings("Extra")
		libs.frertexcompiler:setup()
		common:addActions()

	group("Apps")
	project("Frertex")
		location("Frertex/")
		warnings("Extra")

		common:outDirs()
		common:debugDir()

		kind("ConsoleApp")

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		libs.frertexcompiler:setupDep()

		common:addActions()
