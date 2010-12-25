-- todo: mutable mesh

require "aroma"

local canvas = aroma(800, 800, "shadows") {
	viewport = 2,
	clear_color = {34,34,34}
}

-- local text = canvas.font("lib/font.bmp", 16, 16)

local m = canvas.mesh({
	140,130,
	170,132,
	180,160,
	160,210
}, 2, "polygon")
local base = m:clone()

local mouse = false

function point(x,y, r,g,b) 
	if not r then r,g,b = 0,255,0 end
	local w = 2
	canvas:rect(x-w,y-w,x+w,y+w, r,g,b)
end


function edges(vertices) 
	return coroutine.wrap(function()
		local first = nil
		local prev = nil
		for i,x,y in vertices do
			if not prev then
				first = {x,y}
				prev = {x,y}
			else
				coroutine.yield(prev[1], prev[2], x, y)
				prev[1], prev[2] = x,y
			end
		end
		coroutine.yield(prev[1], prev[2], first[1], first[2])
	end)
end


-- get two at at time
function two(what)
	local i = 1
	return function()
		local a, b = what[i], what[i+1]
		i = i + 2
		return a,b
	end
end

-- generate shadow poygon for polygon mesh
function shadowPolygon(mesh, light, drawNormals) 
	local startShadow, endShadow = 0,0

	local prevEdge = 0
	local firstEdge = 0
	function checkEdgeChange(i, edgeType)
		if prevEdge == 0 then
			firstEdge = edgeType 
		elseif prevEdge ~= edgeType then
			if edgeType == 1 then -- light	
				endShadow = i
			elseif edgeType == 2 then -- shadow
				startShadow = i
			end
		end
		prevEdge = edgeType
	end

	local i = 1
	for x1,y1, x2,y2 in edges(mesh:vertices()) do
		-- find edge normal
		local nx = (y2 - y1)
		local ny = -(x2 - x1)

		-- midpoint
		local mx = (x2 + x1) / 2
		local my = (y2 + y1) / 2

		-- to the light
		local lx,ly = 
			light.x - mx,
			light.y - my

		
		local dot = lx*nx + ly*ny
		
		local edgeType
		if dot > 0 then -- facing the light
			edgeType = 1
			if drawNormals then
				canvas:line(mx, my, mx + nx, my + ny, 255,255,255)
			end
		else
			edgeType = 2
			if drawNormals then
				canvas:line(mx, my, mx + nx, my + ny, 255,0,0)
			end
		end

		checkEdgeChange(i, edgeType)

		i = i + 1
	end

	-- check the last edge
	checkEdgeChange(1, firstEdge)

	if drawNormals then -- light lines
		for i,x,y in mesh:vertices() do
			-- text(x,y, tostring(i))
			if i == startShadow then
				point(x, y, 255,0,255)
			elseif i == endShadow then
				point(x, y, 0,0,255)
			end
		end

		canvas:line(light.x, light.y, {mesh:get(startShadow)}, 255,0,255) -- light vector
		canvas:line(light.x, light.y, {mesh:get(endShadow)}, 0,0,255) -- light vector
	end


	local shadowVerts = {}
	local current = startShadow

	local done = false
	while not done do
		if current == endShadow then done = true end
		local x,y = mesh:get(current)


		local lx, ly =
			x - light.x, y - light.y


		table.insert(shadowVerts, x)
		table.insert(shadowVerts, y)

		table.insert(shadowVerts, x+lx*100)
		table.insert(shadowVerts, y+ly*100)

		if drawNormals then -- extrapolation dots
			point(x,y)
			point(x+lx,y+ly, 0,128,255)
		end

		current = current + 1
		if current > mesh:count() then current = 1 end
	end

	return canvas.mesh(shadowVerts, 2, "triangle_strip")
end

local points = {}
local time = 0
while true do
	time = time + canvas.dt
	-- canvas.rect(10,10, 100, 100, {200,10,10})
	-- canvas.rect(50,50, 150, 150, {10,200,10, 128})
	-- canvas.rect(100,100, 200, 200, {10,10,200, 64})

	-- print(canvas.mouse.x, canvas.mouse.y)

	local light = {
		x = canvas.mouse.x,
		y = canvas.mouse.y,
	}

	-- for i,x,y in base:vertices() do
	-- 	m:set(i, x+math.sin(time*5+y)*10, y)
	-- end

	point(light.x, light.y, 255,255,0)

	if canvas.mouse.left then mouse = true end
	if mouse and not canvas.mouse.left then
		print"click"
		table.insert(points, canvas.mouse.x)
		table.insert(points, canvas.mouse.y)
		
		mouse = false
	end


	m:render(244,123,0)

	if #points >= 3*2 then
		local msub = canvas.mesh(points, 2, "polygon")
		msub:render(255,128,0)
		shadowPolygon(msub, light):render(0,0,0)
	end

	for x,y in two(points) do
		point(x,y)
	end

	shadowPolygon(m, light):render(0,0,0)

	if not canvas:flush() or canvas:key"esc" then break end
end
