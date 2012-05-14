
log = (msg) -> console?.log msg

track_event = (cat, action, label, value=0, interactive=true) ->
  log ["track event:", cat, action, label].join " "
  try
    _gaq.push ['_trackEvent', cat, action, label, value, interactive]
  catch e

document.onclick = (e=window.event) ->
  elm = e.target
  while elm
    if match = elm.className?.match /event_(\w+)/
      track_event "aroma", "click", match[1]
      break
    elm = elm.parentNode

