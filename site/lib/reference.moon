module ..., package.seeall

import insert from table
import trim_leading_white, discount from sitegen
import bind_methods, extend from require "moon"
discount = require "discount"

_html = require "sitegen.html"

plural = (str) -> str .. "s"

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
      t = plural thing.type
      thing.child = true
      self[t] = self[t] or {}
      thing.parent = self
      insert self[t], thing

class Package extends Node
  new: (...) =>
    @methods = {}
    super ...

  tag_methods: (tag_order) =>
    order = tag_order or @tag_order or {}
    tags = {}
    for m in *@methods
      tag = m.tag or "misc"
      tags[tag] = {} if not tags[tag]
      table.insert tags[tag], m

    tags_list = {}
    insert_group = (name) ->
      methods = tags[name]
      if methods
        table.insert tags_list, methods
        methods.name = name
        tags[name] = nil

    insert_group name for name in *order
    insert_group name for name in pairs tags

    tags_list

  sort_methods: (methods=@methods)=>
    normalize_name = (name) ->
      name = name\gsub "^set", "setget"
      name = name\gsub "^get", "setget"
      name

    table.sort methods, (a, b) ->
      normalize_name(a.name) < normalize_name(b.name)

class Type extends Node
  nil

class Method extends Node
  full_name: =>
    table.concat { @parent.name, ".", @name }

  highlight_code: =>
    trim_leading_white @code

  render_prototype: =>
    args = @args or {}

    fn = -> {
      if @returns
        {
          for i, ret in ipairs @returns do {
            span { ret, class: "ret_name" }
            if i == #@returns then "" else ", "
          }
          " = "
        }

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
  type: (t) => Type t

  render: =>
    buffer = {}
    html = (fn) -> table.insert buffer, _html.build fn

    for p in *@packages
      p\sort_methods!

      html -> {
        div {
          class: "package"
          h2 { p.name, class: "package_name" }

          if p.description
            div { class: "package_description", raw p.description }

          if p.show_tags
            div {
              class: "tagged_methods"
              for methods in *p\tag_methods!
                div {
                  class: "tag_group"
                  div { methods.name, class: "group_title"}
                  ul {
                    for m in *methods
                      li {
                        a { m.name, href: "#" .. m\full_name! }
                      }
                  }
                }
            }

          div {
            class: "method_list"
            for m in *p.methods
              div {
                class: "method"
                h3 {
                  class: "method_name"
                  a { name: m\full_name! }
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

