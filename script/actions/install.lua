newaction {
  trigger = "install",
  description = "Install vcpkg packages into vcpkg_installed/",
  execute = function()
    local root = os.getenv("VCPKG_ROOT")
    if not root then
      error("VCPKG_ROOT is not set", 0)
    end

    local vcpkg = path.join(root, os.host() == "windows" and "vcpkg.exe" or "vcpkg")
    print("=== installing vcpkg packages for " .. vcpkg_triplet .. " with " .. vcpkg)

    if not os.execute(string.format('"%s" install --triplet=%s --no-print-usage', vcpkg, vcpkg_triplet)) then
      error("vcpkg install failed", 0)
    end

    print("=== vcpkg install succeeded")
  end
}
