local MenuButton = {
    fields = {
        action = "play",
    },
}

function MenuButton:OnClick()
    if self.action == "exit" then
        Application.Quit()
        return
    end

    local manager = Scene.Find("GameManager")
    local game = manager and manager:GetScript("GameManager")
    if not game then
        Log.Warning("GameManager script not found")
        return
    end

    if self.action == "play" then
        game:StartGame()
    elseif self.action == "menu" then
        game:ShowMenu()
    end
end

return MenuButton
