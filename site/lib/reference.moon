module ..., package.seeall

import insert from table
import trim_leading_white, discount from sitegen
import bind_methods, extend from require "moon"
discount = require "discount"

_html = require "sitegen.html"

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
      thing.parent = self
      insert self[t], thing

class Package extends Node
  sort_methods: =>
    table.sort @method, (a, b) ->
      a.name < b.name

class Method extends Node
  full_name: =>
    table.concat { @parent.name, ".", @name }

  highlight_code: =>
    trim_leading_white @code

  render_prototype: =>
    args = @args or {}

    fn = -> {
      @full_name!
      "("
      for i, arg in ipairs args do {
        span { arg, class: "arg_name" }
        if i == #args then "" else ", "
      }
      ")"
    }

    _html.build fn, ""

scope = {
  package: (p) =>
    insert @packages, Package p

  method: (m) => Method m

  render: =>
    buffer = {}
    html = (fn) -> table.insert buffer, _html.build fn

    for p in *@packages
      p\sort_methods!

      html -> {
        div {
          class: "package"
          h2 { p.name, class: "package_name" }
          if p.description then
            div { class: "package_description", raw p.description }

          div {
            class: "method_list"
            for m in *p.method
              div {
                class: "method"
                h3 {
                  class: "method_name"
                  a {
                    href: "#" .. m\full_name!
                    m.name
                    span { class: "para", raw "&para;" }
                  }
                }
                div { class: "prototype", raw m\render_prototype! }
                if m.description
                  div {
                    class: "method_description"
                    raw m.description
                  }
                if m.code
                  pre {
                    class: "method_code"
                    raw m\highlight_code!
                  }
              }
          }
        }
      }

    table.concat buffer, "\n"

}

export make_context = (page, ctx) ->
  bind_methods extend { packages: {} }, scope

