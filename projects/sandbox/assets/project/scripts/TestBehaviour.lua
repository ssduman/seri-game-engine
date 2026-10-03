local TestBehaviour = {
    fields = {
        speed = 2.0,
        label = "test",
    },
}

function TestBehaviour:OnCreate()
    print("on create", self.entity.name, self.label)
end

function TestBehaviour:OnUpdate(dt)
    local direction = Vec3(0, 0, 0)

    if Input.GetKey(Key.w) then direction.z = direction.z - 1 end
    if Input.GetKey(Key.s) then direction.z = direction.z + 1 end
    if Input.GetKey(Key.a) then direction.x = direction.x - 1 end
    if Input.GetKey(Key.d) then direction.x = direction.x + 1 end

    local transform = self.entity.transform
    transform.position = transform.position + direction:Normalized() * (self.speed * dt)
end

return TestBehaviour
