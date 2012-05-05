require "sitegen"
site = sitegen.create_site =>
  @title = "Aroma"
  deploy_to "leaf@leafo.net", "www/aroma/"

  add "index.md"
  add "reference.moon"
  add "js_reference.moon"

site\write!
