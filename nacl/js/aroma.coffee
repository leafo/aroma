

$ = (id) -> document.getElementById id

log = -> console.log.apply console, arguments

listen = (elm, name, fn) -> elm.addEventListener name, fn, true

get = (url, on_finish, on_fail, send=yes) ->
  req = new XMLHttpRequest()
  req.onreadystatechange = ->
    if req.readyState == 4
      if req.status == 200
        on_finish req
      else
        on_fail req

  req.open "GET", url, true
  if send
    req.send null
  else
    req

module_to_url = (module_name) ->
  module_name = module_name.replace /\./g, '/'
  "#{module_name}.lua"


# bytes can be array like
encode_byte_array = (bytes, size=null) ->
  count = size || bytes.length
  chunk_size = 1024
  parts = Math.floor(count / chunk_size) + 1

  strings = new Array parts
  slice = Function.prototype.call.bind Array.prototype.slice
  char_encode = Function.prototype.apply.bind String.fromCharCode, null

  pi = 0
  while pi < parts
    strings[pi] = char_encode slice bytes, pi * chunk_size, (pi + 1) * chunk_size
    pi++

  strings.join ""

get_image_data = (url, callback) ->
  img = new Image()
  img.src = url

  img.onload = ->
    canvas = document.createElement "canvas"
    console.log "loaded image #{url} [#{img.width}, #{img.height}]"
    canvas.width = img.width
    canvas.height = img.height

    ctx = canvas.getContext "2d"
    ctx.drawImage img, 0, 0

    image_data = ctx.getImageData 0, 0, img.width, img.height
    callback encode_byte_array(image_data.data), img.width, img.height

  img.onerror = ->
    callback null

  null

window.get_image_data = get_image_data
window.encode_byte_array = encode_byte_array

class Aroma
  async_handlers: {
    require: (msg, callback) ->
      [_, module] = msg
      url = module_to_url(module)
      pass = (req) -> callback ["success", req.responseText]
      fail = (req) -> callback ["error", "Failed to find module: tried #{url}"]
      get url, pass, fail

    image: (msg, callback) ->
      [_, path] = msg
      get_image_data path, (image_bytes, width, height) ->
        if !image_bytes
          callback ["error", "Failed to find image: #{path}"]
        else
          callback ["success", image_bytes, width, height]
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


class StreamingSource
  @from_url: (url, callback) ->
    elm = document.createElement "audio"
    elm.src = url
    elm.autoplay = false
    source = new StreamingSource elm

    listen elm, "loadedmetadata", -> callback source
    listen elm, "error", -> callback null

  constructor: (@audio_elm) ->

  play: ->
    log "playing streaming source"
    @audio_elm.play()
  rewind: -> @audio_elm.currentTime = 0

class StaticSource
  @from_url: (context, url, callback) ->
    pass = (req) =>
      context.decodeAudioData req.response, (buffer) =>
        callback new StaticSource context, buffer

    fail = (req) -> callback null

    req = get url, pass, fail, false
    req.responseType = "arraybuffer"
    req.send()

  constructor: (@context, @buffer) ->

  play: ->
    source = @context.createBufferSource()
    source.buffer = @buffer
    source.connect @context.destination
    source.noteOn 0

class Aroma.Audio
  constructor: ->
    @sources = []
    @url_to_source = {}
    @context = new webkitAudioContext

  play_source: (id) ->
    @sources[id].play()

  new_source: (url, type, callback) ->
    existing_id = @url_to_source[url]
    return callback existing_id if existing_id

    handle_source = (source) =>
      callback null unless source
      id = @sources.length
      @sources.push source
      @url_to_source[url] = id
      callback id

    switch type
      when "static"
        StaticSource.from_url @context, url, handle_source
      when "streaming"
        StreamingSource.from_url url, handle_source
      else
        throw "Unknown source type: #{type}"

window.Aroma = Aroma

# # audio test
# a = new Aroma.Audio
# a.new_source "game/theme.ogg", "streaming", (source_id) ->
#   a.play_source source_id
# 
# a.new_source "game/start.wav", "static", (source_id) ->
#   a.play_source source_id
# 
# window.a = a
