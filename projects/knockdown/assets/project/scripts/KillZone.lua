local KillZone = {}

function KillZone:OnCreate()
    self.knocked = 0
end

function KillZone:OnStart()
    self.knocked_text = Scene.Find("KnockedText")
end

function KillZone:OnTriggerEnter(other)
    if other.name == "Block" then
        self.knocked = self.knocked + 1
        self.knocked_text.text.text = "Knocked " .. self.knocked
    end

    Scene.Destroy(other)
end

return KillZone
