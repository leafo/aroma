
-- basic ply file parser

module("ply", package.seeall)

local function makeInserter(rows)
	return function(tbl, source) 
		for _,c in ipairs(rows) do
			table.insert(tbl, source[c])
		end
	end
end

local mesh_meta = {
	__index = {
		-- return array of vertices for all faces
		getVertices = function(self)
			local out = {}
			local insert = makeInserter{"x", "y", "z"}
			for _,f in ipairs(self.face) do
				for i = 2,4 do
					insert(out, self.vertex[f[i]+1])
				end
			end
			return out
		end,
		-- return array of normals for face vertices
		getNormals = function(self)
			local out = {}
			local insert = makeInserter{"nx", "ny", "nz"}
			for _,f in ipairs(self.face) do
				for i = 2,4 do
					local v = self.vertex[f[i]+1]
					insert(out, v)
				end
			end
			return out
		end,
	}
}

function parse(fname)
	local lineNo = 0

	local elements = {}
	local element = nil
	local function header(line)
		if element then
			local ptype, pname = line:match("^property (%w+) (%w+)")
			if ptype then
				table.insert(element.properties, {type = ptype, name = pname})
			else 
				-- end of properties	
				table.insert(elements, element)
				element = nil
			end
		end
		
		-- try to find a new element
		local etype, count = line:match("^element (%w+) (%d+)")
		if etype then
			element = {
				type = etype,
				count = tonumber(count),
				properties = {}
			}
		end

		if line == "end_header" then return true
		else return false end
	end

	local current = nil
	local final = {}
	local function data(line)
		local count = 0
		local set = {}
		
		local islist
		if #current.properties then
			islist = current.properties[1].type == "list"
		end

		for value in line:gmatch("(-?%d+%.?%d*)") do
			count = count + 1
			local fv = tonumber(value)
			
			if islist then table.insert(set, fv)
			else
				if not current.properties[count] then
					error("Too many values on line: "..lineNo)
				end

				set[current.properties[count].name] = fv
			end
		end

		-- not enough values
		if not islist and count < #current.properties then
			error("Too few values for property on line: "..lineNo)
		end

		table.insert(final[current.type], set)

		if #final[current.type] == current.count then
			current = table.remove(elements, nil)
		end

		return current == nil
	end

	local state = 0 -- 
	for line in io.lines(fname) do
		lineNo = lineNo + 1
		if state == 0 then -- opening
			assert(line == "ply")
			state = 1
		elseif state == 1 then -- header
			local done = header(line)
			if done then
				-- create storage for final set
				for _,v in ipairs(elements) do
					final[v.type] = {}
				end

				current = table.remove(elements, 1)
				state = 2
			end
		elseif state == 2 then -- data
			data(line)
		end
	end

	setmetatable(final, mesh_meta)
	return final
end


