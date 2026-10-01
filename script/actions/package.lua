local function copyFile(source, destination)
  os.mkdir(path.getdirectory(destination))
  local ok, err = os.copyfile(source, destination)
  if not ok then
    error("could not copy " .. source .. ": " .. tostring(err), 0)
  end
end

newaction {
  trigger = "package",
  description = "Zip the release Editor build into package/",
  execute = function()
    if os.host() ~= "windows" then
      error("package is only supported on windows for now", 0)
    end

    local releaseDir = path.join(_MAIN_SCRIPT_DIR, "bin/Release-x86_64/Editor")
    local packageDir = path.join(_MAIN_SCRIPT_DIR, "package")

    print("=== checking release build...")
    if not os.isfile(path.join(releaseDir, "Editor.exe")) then
      error(path.join(releaseDir, "Editor.exe") .. " not found, build release first", 0)
    end

    print("=== reading version...")
    local literals = io.readfile(path.join(_MAIN_SCRIPT_DIR, "seri/seri/core/Literals.h")) or ""
    local version = literals:match('kVersion%s*=%s*"([^"]+)"')
    if not version then
      error("version not found", 0)
    end
    local stageDir = path.join(packageDir, "seri-game-engine-x64-v" .. version)
    local zipFile = stageDir .. ".zip"
    print("=== version is " .. version)

    print("=== finding visual studio runtimes...")
    local vswhere = path.join(os.getenv("ProgramFiles(x86)"), "Microsoft Visual Studio/Installer/vswhere.exe")
    if not os.isfile(vswhere) then
      error("vswhere not found", 0)
    end
    local vsDir = os.outputof('"' .. vswhere .. '" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath')
    if not vsDir or vsDir == "" then
      error("visual studio with c++ tools not found", 0)
    end
    local redistVersion = (io.readfile(path.join(vsDir, "VC/Auxiliary/Build/Microsoft.VCRedistVersion.default.txt")) or ""):match("^%s*(.-)%s*$")
    local crtDir = os.matchdirs(path.join(vsDir, "VC/Redist/MSVC", redistVersion, "x64/Microsoft.VC*.CRT"))[1]
    if not crtDir then
      error("runtime folder not found for redist version " .. redistVersion, 0)
    end
    print("=== runtimes found at " .. crtDir)

    print("=== preparing package folder...")
    os.rmdir(stageDir)
    if os.isdir(stageDir) then
      error("could not clean " .. stageDir .. ", is a file in it open?", 0)
    end
    os.remove(zipFile)
    if os.isfile(zipFile) then
      error("could not replace " .. zipFile .. ", is it open?", 0)
    end

    print("=== copying release files...")
    copyFile(path.join(releaseDir, "Editor.exe"), path.join(stageDir, "Editor.exe"))
    local dlls = os.matchfiles(path.join(releaseDir, "*.dll"))
    if #dlls == 0 then
      error("no dlls found in " .. releaseDir, 0)
    end
    for _, file in ipairs(dlls) do
      copyFile(file, path.join(stageDir, path.getname(file)))
    end
    for _, file in ipairs(os.matchfiles(path.join(releaseDir, "assets/**"))) do
      copyFile(file, path.join(stageDir, path.getrelative(releaseDir, file)))
    end

    print("=== copying runtimes...")
    for _, name in ipairs({ "msvcp140.dll", "vcruntime140.dll", "vcruntime140_1.dll" }) do
      copyFile(path.join(crtDir, name), path.join(stageDir, name))
    end

    print("=== zipping...")
    if not os.execute(string.format('tar -a -c -f "%s" -C "%s" *', zipFile, stageDir)) then
      error("zip failed", 0)
    end
    print("=== zipped to " .. zipFile)

    print("=== cleaning copied files...")
    os.rmdir(stageDir)
    if os.isdir(stageDir) then
      error("could not delete " .. stageDir, 0)
    end

    print("=== done")
  end
}
