module ..., package.seeall

import insert from table
import trim_leading_white, discount from sitegen
import bind_methods, extend from require "moon"
discount = require "discount"

class Node
  new: (proto) =>
    @type = @@__name\lower!
    for k, v in pairs proto
      if type(k) == "string"
        self[k] = v

    @format_desc!
    @place_children proto
  
  format_desc: =>
    if @description
      @description = discount trim_leading_white @description

  place_children: (proto) =>
    for thing in *proto
      t = thing.type
      thing.child = true
      self[t] = self[t] or {}
      insert self[t], thing

class Package extends Node
  nil

class Method extends Node
  nil

scope = {
  package: (p) =>
    insert @packages, Package p

  method: (m) => Method m

  render: =>
    packages = [p for p in *@packages when not p.child]
    "<pre>" ..  moon.dump(packages) .. "</pre>"
}

export make_context = (page, ctx) ->
  bind_methods extend { packages: {} }, scope

