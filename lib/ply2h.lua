
require "ply"
require "cosmo"

function ply2h(fname)
	local mesh = ply.parse(fname)
	local vs = mesh:getVertices();
	local ns = mesh:getNormals();

	local count = #vs

	local name, ext = fname:match"^(.*)%.(%w*)$"

	local function list_formatter(list)
		local count = #list
		local row_length = 6
		return function()
			for i,entry in ipairs(list) do
				cosmo.yield{
					v = string.format("% f", entry),
					newline = i % row_length == 0 and "\n\t" or "",
					_template = i == count and 2 or 1
				}
			end
		end
	end



	return cosmo.f[==[
#ifndef $name
#define $name

double $vname[] = {
	$vertices[[$v, $newline]],[[$v]]
};

double $vname_normal[] = {
	$normals[[$v, $newline]],[[$v]]
};


int $vname_len = $count;  

#endif /* $name */
	]==]{
		name = string.upper(name).."_PLY_H_",
		count = count,

		vname = name.."_"..ext,
		vname_len = name.."_"..ext.."_len",
		vertices = list_formatter(vs),


		vname_normal = name.."_normals_"..ext,
		normals = list_formatter(ns)
	}
end

print(ply2h(...))

