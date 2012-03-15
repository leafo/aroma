
$ = (id) -> document.getElementById id

log = -> console.log.apply console, arguments

listen = (elm, name, fn) -> elm.addEventListener name, fn, true

class Aroma
  message_handlers: {
    std_out: (msg) -> @fire "std_out", msg[1]
    std_err: (msg) -> @fire "std_err", msg[1]
  }

  constructor: (@container, @events) ->
    @module = null
    listen @container, "load", =>
      log "Loaded module"
      @fire "loaded"
      @module = @container.querySelectorAll("embed")[0]

    listen @container, "message", (e) =>
      @handle_message e

  fire: (name, args...) ->
    if @events[name]
      @events[name].apply null, args

  execute: (lua) ->
    @post_msg ['execute', lua]

  post_msg: (o) ->
    @module.postMessage JSON.stringify o

  handle_message: (msg) ->
    log ">> #{msg.data} :: #{typeof msg.data}"
    o = try JSON.parse msg.data
    if o
      handler = @message_handlers[o[0]]
      handler.call this, o if handler

window.Aroma = Aroma
