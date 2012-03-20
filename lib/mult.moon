#!/usr/bin/env moon

to_i = (size, x,y) -> y * size + x

value = (left, right, x, y, size) ->
  idx = (...) -> to_i size, ...
  parts = for i = 0, size - 1
    table.concat {
      left, "[", idx(i, y), "] * ", right, "[", idx(x, i), "]"
    }

  table.concat parts, " + "

generate = (left, right, size=4) ->
  parts = {}
  for y = 0, size - 1
    for x = 0, size - 1
      table.insert parts, value left, right, x, y, size
  table.concat parts, ",\n"


left, right, size = ...
if not left or not right or not size or not tonumber size
  print "usage: ./mult.moon left right size"
  return

print generate left, right, tonumber size
