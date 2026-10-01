newaction {
  trigger = "help",
  description = "Print available actions",
  execute = function()
    local wrapper = os.host() == "windows" and "seri" or "./seri.sh"

    local names = {}
    for _, file in ipairs(os.matchfiles(path.join(_MAIN_SCRIPT_DIR, "script/actions/*.lua"))) do
      table.insert(names, path.getbasename(file))
    end
    table.sort(names)

    print("usage: " .. wrapper .. " <action>")
    print("")
    print("actions:")
    for _, name in ipairs(names) do
      local action = premake.action.get(name)
      if action then
        print(string.format("  %-10s %s", name, action.description))
      end
    end
    print("")
    print("premake actions and options: " .. wrapper .. " --help")
  end
}
