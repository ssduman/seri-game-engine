local GameManager = {
    fields = {
        cell_size = 22,
        columns = 20,
        rows = 15,
        start_interval = 0.18,
        min_interval = 0.06,
        interval_step = 0.015,
        foods_per_level = 5,
    },
}

local HEAD_COLOR = Vec4(0.45, 0.9, 0.5, 1.0)
local BODY_COLOR = Vec4(0.25, 0.65, 0.35, 1.0)
local FOOD_COLOR = Vec4(0.95, 0.35, 0.35, 1.0)
local CELL_GAP = 2

local DIRECTIONS = {
    { keys = { Key.w, Key.up }, x = 0, y = 1 },
    { keys = { Key.s, Key.down }, x = 0, y = -1 },
    { keys = { Key.a, Key.left }, x = -1, y = 0 },
    { keys = { Key.d, Key.right }, x = 1, y = 0 },
}

function GameManager:OnCreate()
    self.state = "menu"
    self.segments = {}
    self.occupied = {}
    self.queued = {}
    self.best_score = 0
end

function GameManager:OnStart()
    self.main_menu = Scene.Find("MainMenu")
    self.game = Scene.Find("Game")
    self.game_over = Scene.Find("GameOver")
    self.board = Scene.Find("Board")
    self.score_text = Scene.Find("ScoreText")
    self.level_text = Scene.Find("LevelText")
    self.final_score_text = Scene.Find("FinalScoreText")
    self.best_score_text = Scene.Find("BestScoreText")

    self:ShowMenu()
end

function GameManager:OnUpdate(dt)
    if self.state ~= "playing" then
        if Input.GetKeyDown(Key.enter) or Input.GetKeyDown(Key.space) then
            self:StartGame()
        end
        return
    end

    self:ReadInput()

    self.timer = self.timer + dt
    while self.state == "playing" and self.timer >= self.interval do
        self.timer = self.timer - self.interval
        self:Step()
    end
end

function GameManager:ShowMenu()
    self:ClearBoard()
    self.state = "menu"
    self:SetPanels(true, false, false)
end

function GameManager:StartGame()
    self:ClearBoard()
    self:SetPanels(false, true, false)

    self.board.rect.size_delta = Vec2(self.columns * self.cell_size, self.rows * self.cell_size)

    self.score = 0
    self.level = 1
    self.eaten = 0
    self.interval = self.start_interval
    self.timer = 0
    self.direction = { x = 1, y = 0 }
    self.queued = {}

    local x = self.columns // 2
    local y = self.rows // 2
    for i = 0, 2 do
        local segment = { x = x - i, y = y, entity = self:CreateCell(x - i, y, i == 0 and HEAD_COLOR or BODY_COLOR) }
        table.insert(self.segments, segment)
        self.occupied[self:Key(segment.x, segment.y)] = true
    end

    self:SpawnFood()
    self:UpdateHud()

    self.state = "playing"
end

function GameManager:EndGame()
    self.state = "over"
    self.best_score = math.max(self.best_score, self.score)

    self.final_score_text.text.text = string.format("Score %d   Level %d", self.score, self.level)
    self.best_score_text.text.text = string.format("Best %d", self.best_score)

    self:SetPanels(false, true, true)
end

function GameManager:Step()
    if #self.queued > 0 then
        self.direction = table.remove(self.queued, 1)
    end

    local head = self.segments[1]
    local tail = self.segments[#self.segments]
    local x = head.x + self.direction.x
    local y = head.y + self.direction.y

    if x < 0 or x >= self.columns or y < 0 or y >= self.rows then
        self:EndGame()
        return
    end

    local key = self:Key(x, y)
    local tail_key = self:Key(tail.x, tail.y)
    local eating = key == self.food_key

    if self.occupied[key] and (eating or key ~= tail_key) then
        self:EndGame()
        return
    end

    head.entity.sprite.color = BODY_COLOR

    if eating then
        table.insert(self.segments, 1, { x = x, y = y, entity = self:CreateCell(x, y, HEAD_COLOR) })
        self.occupied[key] = true
        self:Eat()
        return
    end

    table.remove(self.segments)
    self.occupied[tail_key] = nil

    tail.x = x
    tail.y = y
    tail.entity.sprite.color = HEAD_COLOR
    self:PlaceCell(tail.entity, x, y)

    table.insert(self.segments, 1, tail)
    self.occupied[key] = true
end

function GameManager:Eat()
    self.eaten = self.eaten + 1
    self.score = self.score + 10 * self.level

    if self.eaten % self.foods_per_level == 0 then
        self.level = self.level + 1
        self.interval = math.max(self.min_interval, self.interval - self.interval_step)
    end

    self:UpdateHud()
    self:SpawnFood()
end

function GameManager:SpawnFood()
    local free = {}
    for y = 0, self.rows - 1 do
        for x = 0, self.columns - 1 do
            if not self.occupied[self:Key(x, y)] then
                free[#free + 1] = { x = x, y = y }
            end
        end
    end

    if #free == 0 then
        self:EndGame()
        return
    end

    local cell = free[math.random(#free)]

    if self.food then
        self:PlaceCell(self.food, cell.x, cell.y)
    else
        self.food = self:CreateCell(cell.x, cell.y, FOOD_COLOR)
    end

    self.food_key = self:Key(cell.x, cell.y)
end

function GameManager:ReadInput()
    for _, direction in ipairs(DIRECTIONS) do
        for _, key in ipairs(direction.keys) do
            if Input.GetKeyDown(key) then
                self:QueueDirection(direction)
            end
        end
    end
end

function GameManager:QueueDirection(direction)
    if #self.queued >= 2 then
        return
    end

    local last = self.queued[#self.queued] or self.direction
    if direction.x == -last.x and direction.y == -last.y then
        return
    end
    if direction.x == last.x and direction.y == last.y then
        return
    end

    table.insert(self.queued, direction)
end

function GameManager:ClearBoard()
    for _, segment in ipairs(self.segments) do
        Scene.Destroy(segment.entity)
    end

    if self.food then
        Scene.Destroy(self.food)
    end

    self.segments = {}
    self.occupied = {}
    self.food = nil
    self.food_key = nil
end

function GameManager:CreateCell(x, y, color)
    local entity = Scene.Instantiate("Cell", self.board)
    entity.rect.size_delta = Vec2(self.cell_size - CELL_GAP, self.cell_size - CELL_GAP)
    entity.sprite.color = color
    self:PlaceCell(entity, x, y)
    return entity
end

function GameManager:PlaceCell(entity, x, y)
    entity.rect.anchored_position = Vec2(x * self.cell_size + CELL_GAP / 2, y * self.cell_size + CELL_GAP / 2)
end

function GameManager:SetPanels(menu, game, over)
    self.main_menu.active = menu
    self.game.active = game
    self.game_over.active = over
end

function GameManager:UpdateHud()
    self.score_text.text.text = "Score " .. self.score
    self.level_text.text.text = "Level " .. self.level
end

function GameManager:Key(x, y)
    return y * self.columns + x
end

return GameManager
