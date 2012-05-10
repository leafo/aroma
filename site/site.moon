require "sitegen"

sitegen.create_site =>
  disable "autoadd"

  @title = "Aroma"
  @version = "0.0.1"

  deploy_to "leaf@leafo.net", "www/aroma/"

  add "index.html"
  add "tutorial.md"
  add "reference.moon"
  add "js_reference.moon"


