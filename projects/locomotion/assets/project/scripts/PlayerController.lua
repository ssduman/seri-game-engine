local PlayerController = {
    fields = {
        walk_speed = 1.5,
        run_speed = 5.0,
        turn_speed = 540.0,
        jump_speed = 4.0,
        gravity = 12.0,
    },
}

function PlayerController:OnStart()
    self.ground_y = self.entity.transform.position.y
    self.vertical_speed = 0
    self.airborne = false
end

function PlayerController:OnUpdate(dt)
    local transform = self.entity.transform
    local animator = self.entity.animator

    local direction = self:ReadInput()
    local speed = 0

    if direction:Length() > 0 then
        speed = Input.GetKey(Key.left_shift) and self.run_speed or self.walk_speed
        local target = math.deg(math.atan(-direction.x, -direction.z))
        transform.rotation = Vec3(0, self:TurnTowards(transform.rotation.y, target, self.turn_speed * dt), 0)
        transform.position = transform.position + direction * (speed * dt)
    end

    animator:SetFloat("speed", speed)

    if Input.GetKeyDown(Key.space) then
        self:Jump()
    end

    if self.airborne then
        self:UpdateJump(dt)
    end
end

function PlayerController:Jump()
    if self.airborne then
        return
    end

    self.airborne = true
    self.vertical_speed = self.jump_speed
    self.entity.animator:SetTrigger("jump")
end

function PlayerController:UpdateJump(dt)
    local transform = self.entity.transform
    local position = transform.position

    self.vertical_speed = self.vertical_speed - self.gravity * dt
    position.y = position.y + self.vertical_speed * dt

    if position.y <= self.ground_y then
        position.y = self.ground_y
        self.vertical_speed = 0
        self.airborne = false
        self.entity.animator:SetTrigger("land")
    end

    transform.position = position
end

function PlayerController:ReadInput()
    local direction = Vec3(0, 0, 0)

    if Input.GetKey(Key.w) then direction.z = direction.z - 1 end
    if Input.GetKey(Key.s) then direction.z = direction.z + 1 end
    if Input.GetKey(Key.a) then direction.x = direction.x - 1 end
    if Input.GetKey(Key.d) then direction.x = direction.x + 1 end

    return direction:Normalized()
end

function PlayerController:TurnTowards(current, target, max_step)
    local delta = (target - current + 540) % 360 - 180

    if math.abs(delta) <= max_step then
        return target
    end

    return current + (delta > 0 and max_step or -max_step)
end

return PlayerController
