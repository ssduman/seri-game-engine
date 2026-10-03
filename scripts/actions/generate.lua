newaction {
  trigger = "generate",
  description = "Generate project files for this platform (vs2026 on Windows, gmake on Linux)",
  execute = function()
    local generator = os.host() == "windows" and "vs2026" or "gmake"
    print("=== generating projects with " .. generator)

    if not os.execute(string.format('"%s" %s', _PREMAKE_COMMAND, generator)) then
      error("premake " .. generator .. " failed", 0)
    end

    print("=== projects generated")
  end
}
