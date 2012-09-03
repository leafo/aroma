
discount = require "discount"
feed = require "feed"

set "title", "Changelog"
set "link_to_home", true

html ->
  div {
    class: "changelog"

    h2 "Changelog"
    for entry in *feed
      div {
        class: "release"
        id: "v" .. entry._release.version
        raw discount trim_leading_white entry.description
      }
  }
