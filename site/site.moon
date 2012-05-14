require "sitegen"

sitegen.create_site =>
  disable "autoadd"

  @version = "0.0.2"

  deploy_to "leaf@leafo.net", "www/aroma/"

  feed "feed.moon", "feed.xml"
  add "index.html"
  add "tutorial.md"
  add "reference.moon"
  add "js_reference.moon"

