
$ = (id) -> document.getElementById id

log = -> console.log.apply console, arguments

listen = (elm, name, fn) -> elm.addEventListener name, fn, true

get = (url, on_finish, on_fail) ->
   req = new XMLHttpRequest()
   req.onreadystatechange = ->
     if req.readyState == 4
       if req.status == 200
         on_finish req
       else
         on_fail req

   req.open "GET", url, true
   req.send null


module_to_url = (module_name) ->
  module_name = module_name.replace /\./g, '/'
  "#{module_name}.lua"

class Aroma
  async_handlers: {
    require: (msg, callback) ->
      [_, module] = msg
      url = module_to_url(module)
      pass = (req) -> callback ["success", req.responseText]
      fail = (req) -> callback ["error", "Failed to find module: tried #{url}"]
      get url, pass, fail
  }

  message_handlers: {
    std_out: (msg) -> @fire "std_out", msg[1]
    std_err: (msg) -> @fire "std_err", msg[1]
    request: (msg) -> # forwards to async handler
      [_, id, data] = msg
      @dispatch @async_handlers, data[0], data, (res) =>
        @post_message ["response", id, res]
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
    @post_message ['execute', lua]

  post_message: (o) ->
    @module.postMessage JSON.stringify o

  dispatch: (handlers, key, args...) ->
    handler = handlers[key]
    if handler
      handler.apply this, args
    else
      throw Error "missing handler for `#{key}`"

  handle_message: (msg) ->
    log ">> #{msg.data} :: #{typeof msg.data}"
    o = try JSON.parse msg.data
    if o
      @dispatch @message_handlers, o[0], o

window.Aroma = Aroma
