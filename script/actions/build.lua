newoption {
  trigger = "config",
  value = "CONFIG",
  description = "Configuration for the build action",
  default = "debug",
  allowed = {
    { "debug", "Debug" },
    { "release", "Release" },
  },
}

newoption {
  trigger = "rebuild",
  description = "Clean before building, for the build action",
}

newaction {
  trigger = "build",
  description = "Build all projects, --config=debug (default) or --config=release, --rebuild to clean first",
  execute = function()
    local config = _OPTIONS["config"]
    local rebuild = _OPTIONS["rebuild"] ~= nil
    print("=== " .. (rebuild and "rebuilding " or "building ") .. config)

    local command
    if os.host() == "windows" then
      local solution = path.join(_MAIN_SCRIPT_DIR, "Seri Game Engine.slnx")
      if not os.isfile(solution) then
        error(solution .. " not found, run generate first", 0)
      end
      local vswhere = path.join(os.getenv("ProgramFiles(x86)"), "Microsoft Visual Studio/Installer/vswhere.exe")
      if not os.isfile(vswhere) then
        error("vswhere not found", 0)
      end
      local msbuild = (os.outputof('"' .. vswhere .. '" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe') or ""):match("[^\r\n]+")
      if not msbuild then
        error("msbuild not found", 0)
      end
      command = string.format('""%s" "%s" /t:%s /p:Configuration=%s /p:Platform=x64 /m /nologo /v:minimal"', msbuild, solution, rebuild and "Rebuild" or "Build", config:sub(1, 1):upper() .. config:sub(2))
    else
      if not os.isfile(path.join(_MAIN_SCRIPT_DIR, "Makefile")) then
        error("Makefile not found, run generate first", 0)
      end
      command = string.format('make -C "%s" config=%s -j$(nproc)', _MAIN_SCRIPT_DIR, config)
      if rebuild then
        command = string.format('make -C "%s" config=%s clean && ', _MAIN_SCRIPT_DIR, config) .. command
      end
    end

    if not os.execute(command) then
      error("build failed", 0)
    end

    print("=== build succeeded")
  end
}
