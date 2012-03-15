
cjson = require"cjson"

post_message = (o) ->
  o = cjson.encode o if type(o) == "table"
  nacl.post_message o

printer = (channel="std_out") ->
  (...) ->
    args = {...}
    str = table.concat [tostring s for s in *args], "\t"
    post_message { channel, str }

async_print = printer"std_out"
async_print_err = printer"std_err"
async_err = (msg) -> async_print_err debug.traceback msg, 2

async_require = (thread, module_name) ->
  async_err "not yet"

async_scope = setmetatable {
    print: async_print
    require: async_require
    error: async_error
  }, {
    index: _G
  }

nacl.handle_message = (msg) ->
  error "unknown msg: " .. msg if type(msg) != "string"
  msg = cjson.decode msg
  switch msg[1]
    when "execute"
      code, err = loadstring msg[2], "execute"
      if not code
        async_print_err err
      else
        code = setfenv code, async_scope
        success, err = pcall code
        if not success
          async_print_err err
    else
      error "Don't know how to handle message: " .. msg[1]

