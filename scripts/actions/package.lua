local function copyFile(source, destination)
  os.mkdir(path.getdirectory(destination))
  local ok, err = os.copyfile(source, destination)
  if not ok then
    error("could not copy " .. source .. ": " .. tostring(err), 0)
  end
end

local function removeDir(dir)
  if os.host() == "windows" then
    os.rmdir(dir)
  else
    os.execute(string.format('rm -rf "%s"', dir))
  end
end

newaction {
  trigger = "package",
  description = "Archive the release Editor build into package/",
  execute = function()
    local isWindows = os.host() == "windows"
    local executable = isWindows and "Editor.exe" or "Editor"

    local releaseDir = path.join(_MAIN_SCRIPT_DIR, "bin", platform_name .. "-release-x64", "Editor")
    local packageDir = path.join(_MAIN_SCRIPT_DIR, "package")
    
    if not engine_version then
      error("version not found", 0)
    end
    print("=== version is " .. engine_version)

    print("=== checking release build...")
    if not os.isfile(path.join(releaseDir, executable)) then
      error(path.join(releaseDir, executable) .. " not found, build release first", 0)
    end

    local stageDir = path.join(packageDir, "seri-game-engine-" .. platform_name .. "-x64-v" .. engine_version)
    local archiveFile = stageDir .. (isWindows and ".zip" or ".tar.gz")

    local crtDir
    if isWindows then
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
      crtDir = os.matchdirs(path.join(vsDir, "VC/Redist/MSVC", redistVersion, "x64/Microsoft.VC*.CRT"))[1]
      if not crtDir then
        error("runtime folder not found for redist version " .. redistVersion, 0)
      end
      print("=== runtimes found at " .. crtDir)
    end

    print("=== preparing package folder...")
    removeDir(stageDir)
    if os.isdir(stageDir) then
      error("could not clean " .. stageDir .. ", is a file in it open?", 0)
    end
    os.remove(archiveFile)
    if os.isfile(archiveFile) then
      error("could not replace " .. archiveFile .. ", is it open?", 0)
    end

    print("=== copying release files...")
    copyFile(path.join(releaseDir, executable), path.join(stageDir, executable))
    if isWindows then
      local dlls = os.matchfiles(path.join(releaseDir, "*.dll"))
      if #dlls == 0 then
        error("no dlls found in " .. releaseDir, 0)
      end
      for _, file in ipairs(dlls) do
        copyFile(file, path.join(stageDir, path.getname(file)))
      end
    else
      if not os.execute(string.format('strip --strip-debug "%s"', path.join(stageDir, executable))) then
        error("could not strip " .. executable, 0)
      end
      if #os.matchfiles(path.join(releaseDir, "*.so*")) == 0 then
        error("no shared libraries found in " .. releaseDir, 0)
      end
      if not os.execute(string.format('cp -a "%s"/*.so* "%s/"', releaseDir, stageDir)) then
        error("could not copy shared libraries", 0)
      end
    end
    for _, file in ipairs(os.matchfiles(path.join(releaseDir, "assets/**"))) do
      copyFile(file, path.join(stageDir, path.getrelative(releaseDir, file)))
    end

    if isWindows then
      print("=== copying runtimes...")
      for _, name in ipairs({ "msvcp140.dll", "vcruntime140.dll", "vcruntime140_1.dll" }) do
        copyFile(path.join(crtDir, name), path.join(stageDir, name))
      end
    end

    print("=== archiving...")
    local archiveCommand = isWindows and '""%s" -a -c -f "%s" -C "%s" *"' or '%s -czf "%s" -C "%s" .'
    local tar = isWindows and path.join(os.getenv("SystemRoot"), "System32/tar.exe") or "tar"
    if not os.execute(string.format(archiveCommand, tar, archiveFile, stageDir)) then
      error("archive failed", 0)
    end
    print("=== archived to " .. archiveFile)

    print("=== cleaning copied files...")
    removeDir(stageDir)
    if os.isdir(stageDir) then
      error("could not delete " .. stageDir, 0)
    end

    print("=== done")
  end
}
