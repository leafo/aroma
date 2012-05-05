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


  -- iterator for children
  each: (name) =>
    list = self[plural name] or {}

    coroutine.wrap ->
      for thing in *list
        coroutine.yield thing

class Package extends Node
  new: (...) =>
    @methods = {}
    @types = {}
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
  new: (...) =>
    @methods = {}
    super ...
    @instance_name = @instance_name or @name\lower!

class Method extends Node
  full_name: =>
    operator = @parent.invoke_operator or "."
    target = @parent.instance_name or @parent.name

    table.concat { target, operator, @name }

  anchor_name: =>
    @parent.name .. "." .. @name

  highlight_code: =>
    trim_leading_white @code

  html: =>
    ->
      div {
        class: "method"
        h3 {
          class: "method_name"
          a { name: @full_name! }
          a {
            href: "#" .. @anchor_name!
            @name
            span { class: "para", raw "&para;" }
            if @annotate then @annotate!
          }
        }
        div { class: "prototype", @prototype_html! }
        if @description
          div {
            class: "method_description"
            raw @description
          }
        if @code
          pre {
            class: "method_code"
            raw @highlight_code!
          }
      }


  prototype_html: =>
    args = @args or {}
    -> {
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

class Constructor extends Method
  full_name: =>
    "new " .. @parent.name

  annotate: => -> span { "constructor", class: "annotation" }

  anchor_name: =>
    @parent.name .. "." .. @name

  html: =>
    @returns = @returns or { @parent.instance_name }
    @name = @parent.name unless @name
    super!

scope = {
  package: (p) =>
    insert @packages, Package p

  method: (m) => Method m
  constructor: (m) => Constructor m
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
              m\html!
          }

          div {
            class: "type_list"
            for t in *p.types
              div {
                class: "type"
                h3 { class: "type_name", t.name }

                if t.description
                  div { class: "type_description", raw t.description }

                div {
                  class: "method_list"
                    for c in t\each"constructor"
                      c\html!

                    for m in *t.methods
                      m\html!
                }
              }
          }
        }
      }

    table.concat buffer, "\n"

}

export make_context = (page, ctx) ->
  bind_methods extend { packages: {} }, scope

