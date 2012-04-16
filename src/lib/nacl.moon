
cjson = require"cjson"

ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=[];',./!@#$%^&*()_+{}|:\"<>?\\`~ "
DEFAULT_FONT = "12px sans-serif"

-- disable invalid functions
os.remove = -> error "os.remove is disabled"
os.rename = -> error "os.rename is disabled"
os.execute = -> error "os.execute is disabled"
os.exit = -> error "os.exit is disabled"
io.tmpfile = -> error "io.tmpfile is disabled"

post_message = (o) ->
  o = cjson.encode o if type(o) == "table"
  nacl.post_message o

printer = (channel="std_out") ->
  (...) ->
    args = {...}
    str = table.concat [tostring s for s in *args], "\t"
    post_message { channel, str }

async_print = printer"std_out"
async_err = printer"std_err"

msg_id = 0
msg_responses = {}

request_response = (msg) ->
  id = msg_id
  msg_id += 1
  post_message { "request", id, msg }
  while true
    msg = coroutine.yield!
    if msg == "response" and msg_responses[id]
      return with msg_responses[id]
        msg_responses[id] = nil

safe_resume = (co, ...) ->
  success, error_msg = coroutine.resume co, ...
  if not success
    async_err debug.traceback co, error_msg
    nacl.set_game_thread nil
    false
  else
    true

async_require = (module_name) ->
  mod = package.loaded[module_name]
  return mod if mod

  loader = if package.preload[module_name]
    package.preload[module_name]
  else
    msg = request_response { "require", module_name }
    status, code = unpack msg
    return async_err code if status == "error"
    assert loadstring code, module_name

  setfenv loader, getfenv 2
  mod = loader module_name
  mod = true if mod == nil
  package.loaded[module_name] = mod
  mod

-- scope of the game thread
async_scope = setmetatable {
    print: async_print
    require: async_require
  }, {
    __index: _G
  }

game_thread = nil -- to prevent it from being garbage collected

nacl.show_error = async_err

-- these are client functions, need a way to separate them
nacl.prefetch = (files) ->
  tuples = {}

  add_files = (list, t="text") ->
    for k, file_or_list in pairs list
      if type(file_or_list) == "table"
        add_files file_or_list, k
      else
        table.insert tuples, { t, file_or_list }

  add_files files

  return if #tuples == 0
  status, msg = unpack request_response { "prefetch", tuples }
  error "prefetch: " .. msg if status != "success"

nacl.track_event = (category, action, label=nil, value=nil, interactive=true) ->
  post_message { "track_event", { category, action, label, value, interactive } }

---

nacl.handle_message = (msg) ->
  error "unknown msg: " .. tostring(msg) if type(msg) != "string"
  print ">>", msg\sub 1, 120

  msg = cjson.decode msg

  switch msg[1]
    -- create a new execution thread
    -- throws out the old one
    when "execute"
      fn, err = loadstring msg[2], "execute"
      if not fn
        async_err err
      else
        setfenv fn, async_scope
        run = -> aroma.run fn

        game_thread = coroutine.create run
        nacl.set_game_thread game_thread
        safe_resume game_thread

    when "response"
      _, id, data = unpack msg
      msg_responses[id] = data
      safe_resume game_thread, "response"
    else
      error "Don't know how to handle message: " .. (msg[1] or msg)

img_cache = {}

image_data_from_url = (url) ->
  msg = request_response { "image", url }
  status, bytes, width, height = unpack msg
  if status == "success"
    nacl.image_data_from_byte_string bytes, width, height
  else
    error bytes


class AudioSource
  new: (@source_id, @url) =>
  msg: (...) => post_message { "audio", @source_id, ... }

  play: => @msg "play"
  stop: => @msg "stop"
  rewind: => @msg "rewind"
  setLooping: (should_loop) => @msg "set_looping", should_loop

  __tostring: => "HTMLAudioSource<" .. @url .. ">"

nacl.init = {
  graphics: =>
    old_new_image = @newImage

    @newImage = (url, ...) ->
      return old_new_image url, ... if type(url) != "string"
      return img_cache[url] if img_cache[url]

      img_data = image_data_from_url url
      img = old_new_image img_data
      img_cache[url] = img
      img

    @newFont = (font_str, alphabet=ALPHABET) ->
      glyphs = request_response { "font", font_str, alphabet }
      cache = aroma.font.newGlyphCache!
      for g in *glyphs
        letter, data_string, w, h = unpack g
        data = nacl.image_data_from_byte_string data_string, w, h
        cache\add_glyph letter\byte!, data
      cache\to_font!

    aroma.boot = ->
      if not self.getFont!
        self.setFont self.newFont DEFAULT_FONT

  audio: =>
    @newSource = (url, kind="static") ->
      msg = request_response { "audio", url, kind }
      status, source_id, err_msg = unpack msg
      if status == "success"
        AudioSource source_id, url
      else
        error error_msg

  image: =>
    old_new_image_data = @newImageData
    @newImageData = (...) ->
      args = {...}
      old_new_image_data ... if type(args[1]) != "string"
      image_data_from_url args[1]
}

functions_to_reset = {"draw", "update", "keypressed", "keyreleased"}

nacl.init_all = (aroma) ->
  aroma.run = (setup_fn) ->
    blank = ->
    aroma[key] = blank for key in *functions_to_reset
    aroma.graphics.reset!
    aroma.boot!

    setup_fn!

    while true -- listen for game messages
      msg = {coroutine.yield!}
      if aroma[msg[1]]
        name = table.remove msg, 1
        aroma[name] unpack msg
      else
        print "unknown message"
        print unpack msg

