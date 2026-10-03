local Launcher = {
    fields = {
        ball_speed = 22.0,
        turn_speed = 60.0,
        spawn_distance = 1.0,
        min_pitch = -45.0,
        max_pitch = 20.0,
    },
}

function Launcher:OnCreate()
    self.pending = {}
    self.balls = 0
end

function Launcher:OnStart()
    self.balls_text = Scene.Find("BallsText")
end

function Launcher:OnUpdate(dt)
    self:LaunchPending()

    if Input.GetKeyDown(Key.r) then
        Scene.Reload()
        return
    end

    self:Aim(dt)

    if Input.GetKeyDown(Key.space) then
        self:Fire()
    end
end

function Launcher:Aim(dt)
    local yaw = 0
    local pitch = 0

    if Input.GetKey(Key.a) or Input.GetKey(Key.left) then yaw = yaw + 1 end
    if Input.GetKey(Key.d) or Input.GetKey(Key.right) then yaw = yaw - 1 end
    if Input.GetKey(Key.w) or Input.GetKey(Key.up) then pitch = pitch + 1 end
    if Input.GetKey(Key.s) or Input.GetKey(Key.down) then pitch = pitch - 1 end

    local transform = self.entity.transform
    local rotation = transform.rotation
    local step = self.turn_speed * dt

    transform.rotation = Vec3(
        math.max(self.min_pitch, math.min(self.max_pitch, rotation.x + pitch * step)),
        rotation.y + yaw * step,
        rotation.z
    )
end

function Launcher:Fire()
    local transform = self.entity.transform
    local forward = self:Forward(transform.rotation)

    local ball = Scene.Instantiate("ball")
    if not ball then
        return
    end

    ball.transform.position = transform.position + forward * self.spawn_distance

    -- the physics body is created on the next frame, so the velocity is applied then
    table.insert(self.pending, { ball = ball, velocity = forward * self.ball_speed })

    self.balls = self.balls + 1
    self.balls_text.text.text = "Balls " .. self.balls
end

function Launcher:LaunchPending()
    for _, shot in ipairs(self.pending) do
        if shot.ball:IsValid() then
            shot.ball.rigidbody.velocity = shot.velocity
        end
    end

    self.pending = {}
end

function Launcher:Forward(rotation)
    local pitch = math.rad(rotation.x)
    local yaw = math.rad(rotation.y)

    return Vec3(-math.sin(yaw) * math.cos(pitch), math.sin(pitch), -math.cos(yaw) * math.cos(pitch))
end

return Launcher
