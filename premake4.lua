require "Base"
require "FBX"
require "TBB"
require "WxWidgets"

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	-- Check prereqs
	Helium.CheckEnvironment()

	-- Custom action to just run the pre-build scripts.
	newaction
	{
		trigger = "prebuild",
		description = "Run the pre-build scripts without updating project or make files",
		execute = Helium.Prebuild
	}

	-- Custom option to set the global Unicode (wchar_t/UTF-16,32)) vs. ASCII (char/UTF-8)
	newoption
	{
		trigger = "no-unicode",
		description = "Disable Unicode support"
	}

	if _ACTION ~= "prebuild" then

		if _ACTION ~= "clean" then
			local bin = "Bin"
			Helium.BuildWxWidgets()
			Helium.PublishWxWidgets( bin )
			Helium.BuildTBB()
			Helium.PublishTBB( bin )
			Helium.PublishFBX( bin )
			Helium.PublishIcons( bin )
			Helium.Prebuild()
		else
			Helium.CleanWxWidgets()
			Helium.CleanTBB()
		end

		solution "Dependencies"
		Helium.DoDefaultSolutionSettings()
		dofile "Dependencies.lua"

		solution "Helium"
		Helium.DoDefaultSolutionSettings()
		dofile "Helium.lua"
	end
	
end