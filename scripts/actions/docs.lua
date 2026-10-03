newaction {
  trigger = "docs",
  description = "Build the website and the Doxygen C++ reference into bin/docs, needs doxygen on PATH",
  execute = function()
    local siteDir = path.join(_MAIN_SCRIPT_DIR, "docs")
    local outDir = path.join(_MAIN_SCRIPT_DIR, "bin", "docs")
    local images = {
      "editor/assets/icons/seri.png",
      "docs/images/editor.png",
      "docs/images/launcher.png",
      "docs/images/snake-editor.png",
      "docs/images/snake-game.png",
      "docs/images/knockdown-editor.png",
      "docs/images/knockdown-game.png",
    }

    if not engine_version then
      error("version not found", 0)
    end

    print("=== checking doxygen...")
    local doxygenVersion = os.outputof("doxygen --version")
    if not doxygenVersion then
      error("doxygen not found, install it and add it to PATH", 0)
    end
    print("=== doxygen " .. doxygenVersion:match("^%S+"))

    print("=== preparing " .. outDir .. "...")
    os.rmdir(outDir)
    if os.isdir(outDir) then
      error("could not clean " .. outDir .. ", is a file in it open?", 0)
    end
    os.mkdir(path.join(outDir, "images"))

    print("=== copying pages...")
    for _, file in ipairs(os.matchfiles(path.join(siteDir, "*.html"))) do
      local page = io.readfile(file):gsub("{{version}}", engine_version)
      io.writefile(path.join(outDir, path.getname(file)), page)
    end
    for _, pattern in ipairs({ "*.css", "*.js" }) do
      for _, file in ipairs(os.matchfiles(path.join(siteDir, pattern))) do
        os.copyfile(file, path.join(outDir, path.getname(file)))
      end
    end
    for _, image in ipairs(images) do
      local ok, err = os.copyfile(path.join(_MAIN_SCRIPT_DIR, image), path.join(outDir, "images", path.getname(image)))
      if not ok then
        error("could not copy " .. image .. ": " .. tostring(err), 0)
      end
    end

    print("=== generating C++ reference...")
    local config = path.join(_MAIN_SCRIPT_DIR, "bin", "Doxyfile")
    io.writefile(config, io.readfile(path.join(siteDir, "api/Doxyfile")) .. "\nPROJECT_NUMBER = " .. engine_version .. "\n")
    os.chdir(_MAIN_SCRIPT_DIR)
    local ok = os.execute(string.format('doxygen "%s"', config))
    os.remove(config)
    if not ok then
      error("doxygen failed", 0)
    end

    print("=== website built, open " .. path.join(outDir, "index.html"))
  end
}
