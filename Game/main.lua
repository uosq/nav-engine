---@class InitContext
---@field width integer
---@field height integer
---@field title string

local red = Color(255, 0, 0, 255)

--- Called before the game starts anything
---@param ctx InitContext
function Init(ctx)
    ctx.width = 200
    ctx.height = 200
    ctx.title = "Jogo daora"
end

--- Called every new frame
function Draw()
    draw.SetColor(255, 255, 255, 255)

    draw.SetColor(red)
    draw.Rectangle(0, 0, 200, 200)
end

--- Called before the engine is about to quit
--- Use this to unitialize stuff
function Quit()
end