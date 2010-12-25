
require "aroma"

-- 
-- hold space to spawn cells
--

local speed = .1 -- time in seconds for one iteration

function Board(width, height, cell_width, cell_height) 
	local array = {}
	for i = 1,width*height do table.insert(array, 0) end

	cell_width, cell_height = cell_width or 10, cell_height or 10

	local changes = {}

	local function i(x,y) return x % width + 1 + y % height * width end
	local function xy(i) return (i - 1) % width, math.floor((i - 1) / height) end
	local function get(x, y) return array[i(x,y)] end
	local function set(x, y, status) 
		table.insert(changes, {i(x,y), status})
	end

	local function apply()
		for _,change in ipairs(changes) do
			local i, status = unpack(change)
			array[i] = status
		end
		changes = {}
	end

	-- run a single generation
	local function generation()
		for i = 1,#array do
			local x, y = xy(i)
			local count = get(x+1,y) + get(x+1,y+1) +
				get(x,y+1) + get(x-1,y+1) + get(x-1,y) +
				get(x,y-1) + get(x+1,y-1)

			if array[i] > 0 then
				if count < 2 or count > 3 then set(x,y, 0) end
			else 
				if count == 3 then set(x,y, 1) end
			end

		end

		apply()
	end

	local time = 0
	return {
		paused = false,
		render = function(self, canvas)
			if not self.paused then
				time = time + canvas.dt
				if time >= speed then
					generation()
					time = time - speed
				end
			end


			for i = 1,#array do
				local x, y = cell_width* ((i - 1) % width),
					cell_height*math.floor((i - 1) / height)
				-- canvas:rect(x,y, x+cell_width, y+cell_width, {x, y, 128})
				canvas:rect(x,y, x+cell_width, y+cell_width,
					array[i] == 1 and {255,255,255} or {16,16,16})
			end
		end,
		flood = function() 
			for i = 0,100 do
				set(math.random(0, width), math.random(0, height), 1)
			end
			apply()
		end
	}
end

math.randomseed(os.time())

aroma(300, 300, "life") {
	clear_color = {34,34,34},
	ondraw = function(self)
		local width = 4
		local b = Board(self.width/width, self.height/width, width, width)

		self.ondraw = function() 
			if self:key"space" then
				b.paused = true
				b:flood();
			else
				b.paused = false
			end

			b:render(self)
		end
	end
}

