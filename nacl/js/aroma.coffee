
$ = (id) ->
  if typeof id == "string"
    document.getElementById id
  else
    id

debugging = false

log = ->
  console.log.apply console, arguments if debugging

listen = (elm, name, fn) -> elm.addEventListener name, fn, true

add_class_name = (elm, cls) ->
  elm.className += " #{cls}" unless elm.className.match new RegExp "\\b#{cls}\\b"

remove_class_name = (elm, cls)->
  elm.className = elm.className.replace new RegExp("\\s*\\b#{cls}\\b", "g"), ""

Function.prototype.once = ->
  fn = this
  called = false
  ->
    return if called
    called = true
    fn.apply this, arguments

insert_css = ->
  style = document.createElement "style"
  style.type = "text/css"
  style.appendChild document.createTextNode AROMA_CSS

  document.head.appendChild style

insert_css = insert_css.once()

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
    log ">> loaded image #{url} [#{img.width}, #{img.height}]"
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
  nmf_path: "aroma.nmf"

  async_handlers: {
    prefetch: (msg, callback) ->
      [_, files] = msg
      count = files.length

      msg = "Prefetching #{count} file"
      msg += "s" if count != 1
      @show_loading msg

      for tuple in files
        [loader_type, file] = tuple
        do (file) =>
          @file_loader.get_file file, loader_type,  (res) ->
            return unless callback
            if res isnt null
              count--
              callback ["success"] if count == 0
            else
              callback ["error", "Failed to get file #{file}"]
              callback = null


    require: (msg, callback) ->
      [_, module] = msg
      @show_loading "Loading module `#{module}`"
      @file_loader.get_module module, (code) ->
        if code?
          callback ["success", code]
        else
          callback ["error", "Failed to find module: tried #{url}"]

    image: (msg, callback) ->
      [_, path] = msg
      @show_loading "Loading `#{path}`"
      @file_loader.get_file path, "image", (img) ->
        if img?
          [img, w, h] = img
          callback ["success", img, w, h]
        else
          ["error", "Failed to find image: #{path}"]

    audio: (msg, callback) ->
      [_, path, type] = msg
      @show_loading "Loading `#{path}`"

      file_type = switch type
        when "static" then "sound"
        when "streaming" then "music"
        else throw "Unknown source type: #{type}"

      @file_loader.get_file path, file_type, (source_id) ->
        if source_id isnt null
          callback ["success", source_id]
        else
          callback ["error", "Failed to find audio: #{path}"]

    font: (msg, callback) ->
      [_, font_name, alphabet] = msg
      font = new Aroma.Font font_name
      callback (font.render_glyph letter for letter in alphabet.split "")
  }

  message_handlers: {
    std_out: (msg) -> @fire "std_out", msg[1]
    std_err: (msg) -> @fire "std_err", msg[1]
    request: (msg) -> # forwards to async handler
      [_, id, data] = msg
      @dispatch @async_handlers, data[0], data, (res) =>
        @hide_loading()
        @post_message ["response", id, res]

    audio: (msg) ->
      [_, source_id, fn] = msg
      s = @audio.get_source source_id
      s[fn].apply s, msg.slice 3 if s[fn]

    track_event: (msg) ->
      [_, event] = msg
      log event
      @track_event.apply this, event
  }

  constructor: (@container, @width, @height, @events={}) ->
    @container = $ @container

    @module = null
    @file_loader = new Aroma.FileLoader this
    @audio = new Aroma.Audio

    @listener = @create_dom()
    @container.appendChild @listener
    @loading_elm = @listener.querySelector ".loading_message"

    @events.std_out = @events.std_out || (msg) -> console.log msg
    @events.std_err = @events.std_err || (msg) -> console.error msg

    insert_css()

    @events.loaded = @events.loaded || =>
      @execute """
        require "main"
        if aroma and aroma.load then
          aroma.load()
        end
      """
    listen @listener, "load", =>
      log "Loaded module"
      @module = @listener.querySelector "embed"
      add_class_name @module, "loaded"
      @fire "loaded"

    listen @listener, "message", (e) =>
      @handle_message e

  create_dom: ->
    listener = document.createElement "div"
    listener.className = "aroma_listener"
    listener.innerHTML = """
      <div class="loading_message">Loading something..</div>
      <embed name="nacl_module"
        id="aroma"
        width="#{@width}" height="#{@height}"
        src="#{@nmf_path}"
        type="application/x-nacl">
        </embed>
    """
    listener

  show_loading: (msg="Loading") ->
    return unless @loading_elm

    @loading_elm.innerHTML = msg
    unless @loading_elm.className.match /visible/
      @loading_elm.className += " visible"

  hide_loading: ->
    return unless @loading_elm

    @loading_elm.className =
      @loading_elm.className.replace /visible/g, ""

  # reset any shared state
  reset: ->
    @audio.stop_all()

  fire: (name, args...) ->
    if @events[name]
      @events[name].apply null, args

  execute: (lua) ->
    @reset()
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

  track_event: (category, action, label=null, value=null, interactive=true) ->
    try
      _gaq.push ['_trackEvent', category, action, label, value, !interactive]

class StreamingSource
  @from_url: (url, callback) ->
    elm = document.createElement "audio"
    elm.src = url
    elm.autoplay = false
    source = new StreamingSource elm

    listen elm, "loadedmetadata", -> callback source

    listen elm, "error", -> callback null

    # listen elm, "canceled", ->
    #   alert "canceled loading audio"

    # listen elm, "stalled", ->
    #   alert "stalled loading audio"

    listen elm, "ended", =>
      if source.looping
        source.rewind()
        source.play()

  constructor: (@audio_elm) ->

  play: -> @audio_elm.play()
  pause: -> @audio_elm.pause()

  set_looping: (@looping) ->

  stop: ->
    @pause()
    @rewind()

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
    @context = new webkitAudioContext

  stop_all: ->
    for s in @sources
      s.stop() if s.stop

  get_source: (id) ->
    s = @sources[id]
    throw "Invalid source id #{id}" if s is undefined
    s

  add_source: (source, url) ->
    return null unless source
    id = @sources.length
    @sources.push source
    id

class Aroma.Font
  constructor: (@font_string="16pt sans-serif") ->
    @canvas = document.createElement "canvas"
    @ctx = @canvas.getContext "2d"
    @height = @calc_height()

  reset_canvas: (w, h)->
    @canvas.width = w
    @canvas.height = h

    @ctx.font = @font_string
    @ctx.fillStyle = "white"
    @ctx.textBaseline = "top"

  calc_height: ->
    e = document.createElement "div"
    e.innerHTML = "Mg" # highest and lowest
    e.style.font = @font_string
    document.body.appendChild e

    height = e.offsetHeight
    document.body.removeChild e
    height

  render_glyph: (str) ->
    default_width = 80
    @reset_canvas default_width, @height
    real_width = @ctx.measureText(str).width
    @reset_canvas real_width, @height if real_width > default_width

    @ctx.fillText str, 0, 0

    bytestring = encode_byte_array @ctx.getImageData(0, 0, real_width, @height).data
    [str, bytestring, real_width, @height]


# Loads and caches files
class Aroma.FileLoader
  loaders: {
    sound: (path, callback) ->
      audio = @aroma.audio
      StaticSource.from_url audio.context, path, (source) =>
        callback audio.add_source source

    music: (path, callback) ->
      # this is ugly, but should fix the failed loading
      load_it = =>
        return setTimeout load_it, 60 if @pending > 5
        StreamingSource.from_url path, (source) =>
          callback @aroma.audio.add_source source

      load_it()

    image: (path, callback) ->
      get_image_data path, (image_bytes, width, height) ->
        return callback [image_bytes, width, height] if image_bytes?
        callback null
  }

  default_loader: (path, callback) ->
    @_get path, (req) ->
      callback req.responseText

  _get: (url, callback) ->
    log ">> getting: #{url}"
    get url, callback, @fail_fn || ->

  real_path: (path) ->
    if @root?
      "#{@root}/#{path}"
    else
      path

  constructor: (@aroma, @root) ->
    @pending = 0
    @file_cache = {}

  get_module: (module_name, callback) ->
    path = module_name.replace /\./g, '/'
    path = "#{path}.lua"
    @get_file path, null, callback

  get_file: (path, loader, callback) ->
    path = @real_path path

    return callback @file_cache[path] if @file_cache[path]?

    loader = @loaders[loader] if loader
    loader = @default_loader unless loader

    @on_fail = -> callback null

    @pending++
    loader.call this, path, (result) =>
      @pending--
      @file_cache[path] = result
      callback result

window.Aroma = Aroma

LOADING_2 = "data:image/gif;base64,R0lGODlhEAALAPQAAAAAAP///yQkJC4uLhQUFPj4+P///9DQ0Hx8fJ6enkRERNzc3LS0tHR0dJqamkBAQNjY2Pr6+rCwsBgYGCYmJgoKCsbGxiIiIgwMDEhISF5eXjQ0NBAQEAAAAAAAAAAAACH/C05FVFNDQVBFMi4wAwEAAAAh/hpDcmVhdGVkIHdpdGggYWpheGxvYWQuaW5mbwAh+QQJCwAAACwAAAAAEAALAAAFLSAgjmRpnqSgCuLKAq5AEIM4zDVw03ve27ifDgfkEYe04kDIDC5zrtYKRa2WQgAh+QQJCwAAACwAAAAAEAALAAAFJGBhGAVgnqhpHIeRvsDawqns0qeN5+y967tYLyicBYE7EYkYAgAh+QQJCwAAACwAAAAAEAALAAAFNiAgjothLOOIJAkiGgxjpGKiKMkbz7SN6zIawJcDwIK9W/HISxGBzdHTuBNOmcJVCyoUlk7CEAAh+QQJCwAAACwAAAAAEAALAAAFNSAgjqQIRRFUAo3jNGIkSdHqPI8Tz3V55zuaDacDyIQ+YrBH+hWPzJFzOQQaeavWi7oqnVIhACH5BAkLAAAALAAAAAAQAAsAAAUyICCOZGme1rJY5kRRk7hI0mJSVUXJtF3iOl7tltsBZsNfUegjAY3I5sgFY55KqdX1GgIAIfkECQsAAAAsAAAAABAACwAABTcgII5kaZ4kcV2EqLJipmnZhWGXaOOitm2aXQ4g7P2Ct2ER4AMul00kj5g0Al8tADY2y6C+4FIIACH5BAkLAAAALAAAAAAQAAsAAAUvICCOZGme5ERRk6iy7qpyHCVStA3gNa/7txxwlwv2isSacYUc+l4tADQGQ1mvpBAAIfkECQsAAAAsAAAAABAACwAABS8gII5kaZ7kRFGTqLLuqnIcJVK0DeA1r/u3HHCXC/aKxJpxhRz6Xi0ANAZDWa+kEAA7AAAAAAAAAAAA"


LOADING_1 = "data:image/gif;base64,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"


AROMA_CSS = """
  embed {
    background: black url(#{LOADING_1}) 50% 50% no-repeat;
  }

  embed.loaded {
    background: black;
  }

  .loading_message {
    position: absolute;
    font-size: 12px;
    background: rgba(0,0,0, 0.5) url(#{LOADING_2}) 8px 50% no-repeat;
    color: white;
    padding: 8px 12px;
    padding-left: 30px;
    border-radius: 0px 0px 4px 0px;
    box-shadow: 1px 1px 0px rgba(255,255,255, 0.4),
      1px 1px 8px rgba(0,0,0, 0.8);
  }

  .loading_message:not(.visible) {
    display: none;
  }
"""

# font test
# c = new Aroma.Font
# console.log c.render_glyph "H"

