
cjson = require"cjson"

nacl.post_message cjson.encode {
  hello: "world"
}

nacl.handle_message = (msg) ->
  print "got a msg:", msg, type msg
  nacl.post_message msg

